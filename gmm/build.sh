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
# The .apk is signed (Alpine refuses unsigned packages). The first run creates an
# RSA key pair in ./keys/ (keep gmm.rsa private, do NOT commit it; build/ is wiped
# on every run, so the key lives outside it). The public key is copied to
# build/gmm.rsa.pub: install it once with
#   sudo cp gmm.rsa.pub /etc/apk/keys/
# Use your own key with GMM_APK_KEY=/path/key.rsa and GMM_APK_KEY_NAME=<name>
# (the public key is then /etc/apk/keys/<name>.rsa.pub). Encrypted keys: set
# NFPM_APK_PASSPHRASE. Needs openssl; without it the apk is built unsigned.

set -euo pipefail

cd "$(dirname "${BASH_SOURCE[0]}")"

NAME=gmm
SRC=main.go
MAN=gmm.1
OUT=build
STAGE="$OUT/.stage"
LINUX_ARCHES="amd64 arm64"
APK_KEY="${GMM_APK_KEY:-keys/gmm.rsa}"
APK_KEY_NAME="${GMM_APK_KEY_NAME:-gmm}"
APK_SIGNED=0
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

setup_apk_key() {
    if ! command -v openssl >/dev/null 2>&1; then
        warn "openssl not found: the .apk will be unsigned (apk add --allow-untrusted)"
        return 0
    fi
    if [ ! -f "$APK_KEY" ]; then
        log "creating apk signing key: $APK_KEY"
        mkdir -p "$(dirname "$APK_KEY")"
        openssl genrsa -traditional -out "$APK_KEY" 4096 2>/dev/null \
            || openssl genrsa -out "$APK_KEY" 4096 2>/dev/null \
            || { warn "could not create $APK_KEY: the .apk will be unsigned"; rm -f "$APK_KEY"; return 0; }
        chmod 600 "$APK_KEY"
        warn "keep $APK_KEY private and out of git"
    fi
    local pass=()
    if [ -n "${NFPM_APK_PASSPHRASE:-}" ]; then
        pass=(-passin env:NFPM_APK_PASSPHRASE)
    fi
    if openssl rsa -in "$APK_KEY" "${pass[@]}" -pubout -out "$OUT/$APK_KEY_NAME.rsa.pub" 2>/dev/null; then
        APK_KEY_ABS="$(cd "$(dirname "$APK_KEY")" && pwd)/$(basename "$APK_KEY")"
        APK_SIGNED=1
        log "Alpine signing key successfully configured."
    else
        warn "could not read $APK_KEY: the .apk will be unsigned"
        rm -f "$OUT/$APK_KEY_NAME.rsa.pub"
    fi
}

make_pkg() { # arch format
    local arch="$1" fmt="$2" cfg="$STAGE/nfpm-$1.yaml" license="" apksig=""
    if [ "$APK_SIGNED" = 1 ] && [ "$fmt" = "apk" ]; then
        apksig="apk:
  signature:
    key_file: \"$APK_KEY_ABS\"
    key_name: \"$APK_KEY_NAME\""
    fi
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
$apksig
overrides:
  deb:
    recommends: [gcc]
  rpm:
    recommends: [gcc]
  apk:
    depends: [gcc, musl-dev, binutils, ncurses-libs, gmp, libffi]
EOF
    "$NFPM" package --config "$cfg" --packager "$fmt" --target "$OUT/" >/dev/null
}

SKIPPED=""
if find_nfpm; then
    setup_apk_key
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
if [ "$APK_SIGNED" = 1 ]; then
    echo "  sudo cp $OUT/$APK_KEY_NAME.rsa.pub /etc/apk/keys/   # once"
    echo "  sudo apk add $OUT/${NAME}_${VERSION}_x86_64.apk"
else
    echo "  sudo apk add --allow-untrusted $OUT/${NAME}_${VERSION}_x86_64.apk"
fi
echo "  tar -xJf $OUT/${NAME}-${VERSION}-linux-amd64.tar.xz   # then copy gmm to /usr/bin, gmm.1 to /usr/share/man/man1"
[ -z "$SKIPPED" ] || exit 1
