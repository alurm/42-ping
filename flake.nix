{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = {
    nixpkgs,
    flake-utils,
    ...
  }:
    flake-utils.lib.eachDefaultSystem (system: let
      pkgs = nixpkgs.legacyPackages.${system};
    in rec {
      packages = {
        ft_ping = pkgs.callPackage ./packages/ft_ping.nix {};
        # The subject: "You will take as reference the ping implementation from inetutils-2.0".
        inetutils = pkgs.callPackage ./packages/inetutils-2.0.nix {};
      };

      devShells.default = pkgs.mkShell {
        # With pkgs.clangStdenv, <stdint.h> is missing somehow.
        # https://nixos.wiki/wiki/Using_Clang_instead_of_GCC.
        packages = [
          packages.inetutils
          pkgs.valgrind
        ];
      };
    });
}
