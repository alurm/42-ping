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
        default = packages.ft_ping;
        ft_ping = pkgs.callPackage ./packages/ft_ping.nix;
        inetutils-2-0 = pkgs.callPackage ./packages/inetutils-2.0.nix {};
      };

      devShells.default = pkgs.mkShell {
        packages = [
          pkgs.clang
          pkgs.valgrind
          packages.inetutils-2-0
        ];
      };
    });
}
