#!/usr/bin/env bash
#
# build.sh - build gmm from main.go and package it.
#
# Run it from the folder that holds main.go, go.mod, libgmm/ and gmm.1:
#
#   ./build.sh          build everything into ./build
#   ./build.sh clean    delete ./build
#
# Produces in ./build:
#   gmm                                  executable for this machine
#   gmm.exe                              Windows x86_64 executable
#   gmm-<ver>-linux-<arch>.tar.xz        binary + man page + Readme
#   gmm_<ver>_<arch>.deb                 Debian / Ubuntu
#   gmm-<ver>-1.<arch>.rpm               Fedora / RHEL / openSUSE
#   gmm_<ver>_<arch>.apk                 Alpine Linux
#
# Packages install the binary as /usr/bin/gmm and the man page as
# /usr/share/man/man1/gmm.1.gz. (Not /usr/local/bin/gmm: gmm itself uses that
# path as a directory for its downloaded GHC toolchain.)
#
# deb, rpm and apk are made with nfpm. If it is missing, the script installs it
# with "go install". Set GMM_LICENSE=<name> to put a license in the packages.

set -euo pipefail

cd "$(dirname "${BASH_SOURCE[0]}")"

NAME=gmm
SRC=main.go
MAN=gmm.1
OUT=build
STAGE="$OUT/.stage"
LINUX_ARCHES="amd64 arm64"
HOMEPAGE="https://github.com/DASKR515/C-minus-minus"
MAINTAINER="DASKR"
DESCRIPTION="GHC C-- Manager: build tool and compiler driver for C-- projects"

log()  { printf '\033[1;34m==>\033[0m %s\n' "$*"; }
warn() { printf '\033[1;33mwarning:\033[0m %s\n' "$*" >&2; }
die()  { printf '\033[1;31merror:\033[0m %s\n' "$*" >&2; exit 1; }

if [ "${1:-}" = "clean" ]; then
    rm -rf "$OUT"
    echo "removed $OUT"
    exit 0
fi
if [ "${1:-}" = "-h" ] || [ "${1:-}" = "--help" ]; then
    sed -n '3,24p' "$0" | sed 's/^# \{0,1\}//'
    exit 0
fi

# ---------------------------------------------------------------- checks ----
for tool in go tar xz gzip; do
    command -v "$tool" >/dev/null 2>&1 || die "'$tool' is required but not installed"
done
[ -f "$SRC" ]   || die "$SRC not found (run build.sh next to main.go)"
[ -f "$MAN" ]   || die "$MAN not found (put the man page next to main.go)"
[ -d libgmm ]   || die "libgmm/ not found (main.go embeds it)"

VERSION="$(sed -n 's/^[[:space:]]*Version[[:space:]]*=[[:space:]]*"\([^"]*\)".*/\1/p' "$SRC" | head -n1)"
[ -n "$VERSION" ] || die "could not read Version from $SRC"
log "gmm $VERSION"

TAR_OWNER=()
if tar --version 2>/dev/null | grep -q 'GNU tar'; then
    TAR_OWNER=(--owner=0 --group=0 --numeric-owner)
fi

rm -rf "$OUT"
mkdir -p "$STAGE"
trap 'rm -rf "$STAGE"' EXIT

# --------------------------------------------------------------- binaries ----
build_bin() { # goos goarch output
    CGO_ENABLED=0 GOOS="$1" GOARCH="$2" \
        go build -trimpath -ldflags "-s -w" -o "$3" "$SRC"
}

log "building gmm for this machine"
CGO_ENABLED=0 go build -trimpath -ldflags "-s -w" -o "$OUT/$NAME" "$SRC"

log "building gmm.exe (windows/amd64)"
build_bin windows amd64 "$OUT/$NAME.exe"

for arch in $LINUX_ARCHES; do
    log "building linux/$arch"
    mkdir -p "$STAGE/linux-$arch"
    build_bin linux "$arch" "$STAGE/linux-$arch/$NAME"
done

# --------------------------------------------------------------- man page ----
gzip -9nc "$MAN" > "$STAGE/$MAN.gz"

# ---------------------------------------------------------------- tar.xz ----
for arch in $LINUX_ARCHES; do
    dir="$NAME-$VERSION-linux-$arch"
    log "packing $dir.tar.xz"
    mkdir -p "$STAGE/$dir"
    install -m 0755 "$STAGE/linux-$arch/$NAME" "$STAGE/$dir/$NAME"
    install -m 0644 "$MAN" "$STAGE/$dir/$MAN"
    for extra in Readme.md README.md LICENSE; do
        [ -f "$extra" ] && install -m 0644 "$extra" "$STAGE/$dir/$extra"
    done
    XZ_OPT="-9e" tar "${TAR_OWNER[@]}" -C "$STAGE" -cJf "$OUT/$dir.tar.xz" "$dir"
done

# ------------------------------------------------------------ deb/rpm/apk ----
find_nfpm() {
    if command -v nfpm >/dev/null 2>&1; then
        NFPM="$(command -v nfpm)"
        return 0
    fi
    local bindir
    bindir="$(go env GOBIN)"
    [ -n "$bindir" ] || bindir="$(go env GOPATH | cut -d: -f1)/bin"
    if [ -x "$bindir/nfpm" ]; then
        NFPM="$bindir/nfpm"
        return 0
    fi
    log "installing nfpm (needs network)"
    if go install github.com/goreleaser/nfpm/v2/cmd/nfpm@latest 2>/dev/null \
        && [ -x "$bindir/nfpm" ]; then
        NFPM="$bindir/nfpm"
        return 0
    fi
    return 1
}

make_pkg() { # arch format
    local arch="$1" fmt="$2" cfg="$STAGE/nfpm-$1.yaml" license=""
    if [ -n "${GMM_LICENSE:-}" ]; then
        license="license: \"${GMM_LICENSE}\""
    fi
    cat > "$cfg" <<EOF
name: $NAME
arch: $arch
platform: linux
version: "$VERSION"
section: devel
priority: optional
maintainer: "$MAINTAINER"
description: "$DESCRIPTION"
homepage: "$HOMEPAGE"
$license
contents:
  - src: $STAGE/linux-$arch/$NAME
    dst: /usr/bin/$NAME
    file_info:
      mode: 0755
  - src: $STAGE/$MAN.gz
    dst: /usr/share/man/man1/$MAN.gz
    file_info:
      mode: 0644
overrides:
  deb:
    recommends: [gcc]
  rpm:
    recommends: [gcc]
  apk:
    depends: [gcc, musl-dev]
EOF
    "$NFPM" package --config "$cfg" --packager "$fmt" --target "$OUT/" >/dev/null
}

SKIPPED=""
if find_nfpm; then
    for arch in $LINUX_ARCHES; do
        for fmt in deb rpm apk; do
            log "packing $fmt ($arch)"
            make_pkg "$arch" "$fmt"
        done
    done
    if command -v dpkg-deb >/dev/null 2>&1; then
        for deb in "$OUT"/*.deb; do
            [ -e "$deb" ] || continue
            dpkg-deb -c "$deb" | grep -q "usr/share/man/man1/$MAN.gz" \
                || warn "man page missing in $deb"
        done
    fi
else
    SKIPPED="deb rpm apk"
    warn "nfpm is not available and could not be installed: skipping deb, rpm and apk"
    warn "install it from https://nfpm.goreleaser.com and run ./build.sh again"
fi

# ---------------------------------------------------------------- summary ----
echo
log "done: $OUT/"
ls -lh "$OUT" | sed '1d'
echo
echo "install examples:"
echo "  sudo dpkg -i $OUT/${NAME}_${VERSION}_amd64.deb"
echo "  sudo rpm -i $OUT/${NAME}-${VERSION}-1.x86_64.rpm"
echo "  sudo apk add --allow-untrusted $OUT/${NAME}_${VERSION}_x86_64.apk"
echo "  tar -xJf $OUT/${NAME}-${VERSION}-linux-amd64.tar.xz   # then copy gmm to /usr/bin, gmm.1 to /usr/share/man/man1"
