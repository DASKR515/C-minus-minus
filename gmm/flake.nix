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
