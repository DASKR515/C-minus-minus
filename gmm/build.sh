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
#   gmm-<ver>-windows-amd64.zip          gmm.exe + man page + Readme (needs zip)
#   gmm-<ver>-linux-<arch>.tar.xz        binary + man page + Readme
#   gmm_<ver>_<arch>.deb                 Debian / Ubuntu
#   gmm-<ver>-1.<arch>.rpm               Fedora / RHEL / openSUSE
#   gmm_<ver>_<arch>.apk                 Alpine Linux
#   package.nix default.nix flake.nix    NixOS / Nix (uses the tar.xz above)
#   SHA256SUMS                           checksums of every file above
#
# Runtime dependencies (installed automatically by the packages):
#   ghc, clang 15 (13-20 accepted), llvm 15 (13-20 accepted), gcc, binutils,
#   libc headers, gmp, libffi and ncurses development files.
#   GMM_MINIMAL_DEPS=1 makes ghc/clang/llvm only "recommended", for people who
#   let gmm download its own GHC or use ghcup.
#
# The .apk is signed (Alpine refuses unsigned packages). The first run creates an
# RSA key pair in ./keys/ (keep gmm.rsa private, do NOT commit it; build/ is wiped
# on every run, so the key lives outside it). The public key is copied to
# build/gmm.rsa.pub: install it once with
#   sudo cp gmm.rsa.pub /etc/apk/keys/
# Use your own key with GMM_APK_KEY=/path/key.rsa and GMM_APK_KEY_NAME=<name>
# (the public key is then /etc/apk/keys/<name>.rsa.pub). Encrypted keys: set
# NFPM_APK_PASSPHRASE. Needs openssl; without it the apk is built unsigned.
#
# Other variables: GMM_LICENSE, NFPM_VERSION (default: latest),
# SOURCE_DATE_EPOCH (reproducible archives).

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
NFPM_VERSION="${NFPM_VERSION:-latest}"
HOMEPAGE="https://github.com/DASKR515/C-minus-minus"
MAINTAINER="DASKR"
DESCRIPTION="GHC C-- Manager: build tool and compiler driver for C-- projects"

# GHC 9.12 accepts LLVM 13-20 (llvmVersionRange in main.go); 15 is preferred.
DEB_CLANG="clang-15 | clang-16 | clang-17 | clang-18 | clang-19 | clang-20 | clang-14 | clang-13"
DEB_LLVM="llvm-15 | llvm-16 | llvm-17 | llvm-18 | llvm-19 | llvm-20 | llvm-14 | llvm-13"

log()  { printf '\033[1;34m==>\033[0m %s\n' "$*"; }
warn() { printf '\033[1;33mwarning:\033[0m %s\n' "$*" >&2; }
die()  { printf '\033[1;31merror:\033[0m %s\n' "$*" >&2; exit 1; }

if [ "${1:-}" = "clean" ]; then
    rm -rf "$OUT"
    echo "removed $OUT"
    exit 0
fi
if [ "${1:-}" = "-h" ] || [ "${1:-}" = "--help" ]; then
    # Print the leading comment block (everything up to the first non-comment line).
    awk 'NR == 1 { next } /^#/ { sub(/^# ?/, ""); print; next } { exit }' "$0"
    exit 0
fi

# ---------------------------------------------------------------- checks ----
for tool in go tar xz gzip; do
    command -v "$tool" >/dev/null 2>&1 || die "'$tool' is required but not installed"
done
[ -f "$SRC" ]    || die "$SRC not found (run build.sh next to main.go)"
[ -f go.mod ]    || die "go.mod not found (run 'go mod init' / 'go mod tidy' first)"
[ -f "$MAN" ]    || die "$MAN not found (put the man page next to main.go)"
[ -d libgmm ]    || die "libgmm/ not found (main.go embeds it)"

VERSION="$(sed -n 's/^[[:space:]]*Version[[:space:]]*=[[:space:]]*"\([^"]*\)".*/\1/p' "$SRC" | head -n1)"
[ -n "$VERSION" ] || die "could not read Version from $SRC"
log "gmm $VERSION"

# Fixed timestamps make the archives reproducible.
EPOCH="${SOURCE_DATE_EPOCH:-$(git log -1 --format=%ct 2>/dev/null || date +%s)}"

# GNU tar options; bsdtar (macOS) does not know them, so they stay empty there.
# The "+" form keeps an empty array legal under `set -u` on bash < 4.4.
TAR_OPTS=()
if tar --version 2>/dev/null | grep -q 'GNU tar'; then
    TAR_OPTS=(--owner=0 --group=0 --numeric-owner --sort=name --mtime="@$EPOCH")
fi

sha256() {
    if command -v sha256sum >/dev/null 2>&1; then
        sha256sum "$@"
    else
        shasum -a 256 "$@"
    fi
}

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

install_docs() { # destination dir
    install -m 0644 "$MAN" "$1/$MAN"
    for extra in Readme.md README.md LICENSE; do
        if [ -f "$extra" ]; then
            install -m 0644 "$extra" "$1/$extra"
        fi
    done
}

# ---------------------------------------------------------------- tar.xz ----
for arch in $LINUX_ARCHES; do
    dir="$NAME-$VERSION-linux-$arch"
    log "packing $dir.tar.xz"
    mkdir -p "$STAGE/$dir"
    install -m 0755 "$STAGE/linux-$arch/$NAME" "$STAGE/$dir/$NAME"
    install_docs "$STAGE/$dir"
    XZ_OPT="-9e" tar ${TAR_OPTS[@]+"${TAR_OPTS[@]}"} -C "$STAGE" -cJf "$OUT/$dir.tar.xz" "$dir"
done

# ------------------------------------------------------------ windows zip ----
if command -v zip >/dev/null 2>&1; then
    dir="$NAME-$VERSION-windows-amd64"
    log "packing $dir.zip"
    mkdir -p "$STAGE/$dir"
    install -m 0755 "$OUT/$NAME.exe" "$STAGE/$dir/$NAME.exe"
    install_docs "$STAGE/$dir"
    ( cd "$STAGE" && zip -qr -X "$OLDPWD/$OUT/$dir.zip" "$dir" )
else
    warn "zip not found: skipping the Windows zip (gmm.exe is still in $OUT/)"
fi

# ------------------------------------------------------------------- nix ----
# NixOS cannot run the GHC bindist gmm normally downloads (its binaries expect
# a standard dynamic loader), so the Nix package wires gmm to GHC/clang/LLVM 15
# from nixpkgs through GMM_GHC / GMM_CC / GMM_LLVM and PATH.
make_nix() {
    cat > "$OUT/package.nix" <<'EOF'
{ lib
, stdenvNoCC
, makeWrapper
, haskell
, llvmPackages_15
, gcc
, binutils
, ghc ? haskell.compiler.ghc912
}:

let
  version = "@VERSION@";
  arch = if stdenvNoCC.hostPlatform.isAarch64 then "arm64" else "amd64";
  toolchain = [ ghc gcc binutils llvmPackages_15.clang llvmPackages_15.llvm ];
in
stdenvNoCC.mkDerivation {
  pname = "gmm";
  inherit version;

  # Release tarball produced by build.sh (a static Go binary + man page).
  src = ./. + "/gmm-${version}-linux-${arch}.tar.xz";

  nativeBuildInputs = [ makeWrapper ];

  dontConfigure = true;
  dontBuild = true;
  dontStrip = true;

  installPhase = ''
    runHook preInstall
    install -Dm755 gmm "$out/bin/gmm"
    install -Dm644 gmm.1 "$out/share/man/man1/gmm.1"
    wrapProgram "$out/bin/gmm" \
      --set-default GMM_GHC "${ghc}/bin/ghc" \
      --set-default GMM_CC "${gcc}/bin/gcc" \
      --set-default GMM_LLVM "${llvmPackages_15.llvm}/bin/opt" \
      --prefix PATH : "${lib.makeBinPath toolchain}"
    runHook postInstall
  '';

  meta = {
    description = "GHC C-- Manager: build tool and compiler driver for C-- projects";
    homepage = "@HOMEPAGE@";
    mainProgram = "gmm";
    platforms = [ "x86_64-linux" "aarch64-linux" ];
  };
}
EOF

    cat > "$OUT/default.nix" <<'EOF'
# nix-build            -> ./result/bin/gmm
# nix-env -if default.nix
{ pkgs ? import <nixpkgs> { } }:
pkgs.callPackage ./package.nix { }
EOF

    cat > "$OUT/flake.nix" <<'EOF'
{
  description = "gmm - GHC C-- Manager";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = { self, nixpkgs }:
    let
      systems = [ "x86_64-linux" "aarch64-linux" ];
      forAll = f: nixpkgs.lib.genAttrs systems (system: f nixpkgs.legacyPackages.${system});
    in
    {
      packages = forAll (pkgs: rec {
        gmm = pkgs.callPackage ./package.nix { };
        default = gmm;
      });

      overlays.default = final: _prev: {
        gmm = final.callPackage ./package.nix { };
      };

      devShells = forAll (pkgs: {
        default = pkgs.mkShell {
          packages = [
            pkgs.haskell.compiler.ghc912
            pkgs.gcc
            pkgs.binutils
            pkgs.llvmPackages_15.clang
            pkgs.llvmPackages_15.llvm
          ];
        };
      });
    };
}
EOF

    sed -i.bak -e "s|@VERSION@|$VERSION|g" -e "s|@HOMEPAGE@|$HOMEPAGE|g" "$OUT/package.nix"
    rm -f "$OUT/package.nix.bak"

    if command -v nix-instantiate >/dev/null 2>&1; then
        for f in package.nix default.nix flake.nix; do
            nix-instantiate --parse "$OUT/$f" >/dev/null || die "invalid Nix syntax in $OUT/$f"
        done
    fi
}

log "writing Nix files"
make_nix

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
    log "installing nfpm $NFPM_VERSION (needs network)"
    if go install "github.com/goreleaser/nfpm/v2/cmd/nfpm@$NFPM_VERSION" \
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
    if openssl rsa -in "$APK_KEY" ${pass[@]+"${pass[@]}"} -pubout -out "$OUT/$APK_KEY_NAME.rsa.pub" 2>/dev/null; then
        APK_KEY_ABS="$(cd "$(dirname "$APK_KEY")" && pwd)/$(basename "$APK_KEY")"
        APK_SIGNED=1
        log "Alpine signing key successfully configured."
    else
        warn "could not read $APK_KEY: the .apk will be unsigned"
        rm -f "$OUT/$APK_KEY_NAME.rsa.pub"
    fi
}

# Dependency lists per package format. Names that differ between distributions
# are listed per format; deb can express "any of these versions" with "|".
#   base  = what linking a C-- program needs (C compiler, linker, libc headers,
#           gmp/libffi/ncurses for the GHC runtime)
#   tools = the compiler toolchain (ghc, clang, llvm)
DEB_BASE='"gcc", "libc6-dev", "binutils", "libgmp-dev", "libffi-dev", "libncurses-dev"'
DEB_TOOLS="\"ghc\", \"$DEB_CLANG\", \"$DEB_LLVM\""
RPM_BASE='"gcc", "glibc-devel", "binutils", "gmp-devel", "libffi-devel", "ncurses-devel"'
RPM_TOOLS='"ghc", "clang", "llvm"'
APK_BASE='"gcc", "musl-dev", "binutils", "gmp", "gmp-dev", "libffi", "libffi-dev", "ncurses-libs", "ncurses-dev"'
APK_TOOLS='"ghc", "clang15", "llvm15"'

overrides_yaml() {
    local fmt base tools
    for fmt in deb rpm apk; do
        case "$fmt" in
            deb) base=$DEB_BASE; tools=$DEB_TOOLS ;;
            rpm) base=$RPM_BASE; tools=$RPM_TOOLS ;;
            apk) base=$APK_BASE; tools=$APK_TOOLS ;;
        esac
        echo "  $fmt:"
        if [ "${GMM_MINIMAL_DEPS:-0}" = 1 ]; then
            echo "    depends: [$base]"
            echo "    recommends: [$tools]"
        else
            echo "    depends: [$base, $tools]"
        fi
    done
}

make_pkg() { # arch format
    local arch="$1" fmt="$2" cfg="$STAGE/nfpm-$1-$2.yaml" license="" apksig=""
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
$(overrides_yaml)
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

# -------------------------------------------------------------- checksums ----
log "writing SHA256SUMS"
(
    cd "$OUT"
    # shellcheck disable=SC2035
    sha256 $(ls -1 | grep -v '^SHA256SUMS$' | grep -v '^\.stage$') > SHA256SUMS
)

# ---------------------------------------------------------------- summary ----
echo
log "done: $OUT/"
ls -lh "$OUT" | sed '1d'
echo
echo "install examples:"
echo "  sudo dpkg -i $OUT/${NAME}_${VERSION}_amd64.deb      # or: sudo apt install ./$OUT/${NAME}_${VERSION}_amd64.deb"
echo "  sudo rpm -i $OUT/${NAME}-${VERSION}-1.x86_64.rpm    # or: sudo dnf install ./$OUT/..."
if [ "$APK_SIGNED" = 1 ]; then
    echo "  sudo cp $OUT/$APK_KEY_NAME.rsa.pub /etc/apk/keys/   # once"
    echo "  sudo apk add $OUT/${NAME}_${VERSION}_x86_64.apk"
else
    echo "  sudo apk add --allow-untrusted $OUT/${NAME}_${VERSION}_x86_64.apk"
fi
echo "  nix-env -if $OUT/default.nix                        # NixOS / Nix"
echo "  tar -xJf $OUT/${NAME}-${VERSION}-linux-amd64.tar.xz   # then copy gmm to /usr/bin, gmm.1 to /usr/share/man/man1"
if [ -n "$SKIPPED" ]; then
    warn "incomplete build: $SKIPPED were skipped"
    exit 1
fi
