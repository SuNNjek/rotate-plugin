{
  description = "AviSynth+ rotate plugin";

  inputs = {
    nixpkgs.url = "https://channels.nixos.org/nixpkgs-unstable/nixexprs.tar.xz";
  };

  outputs = {
    self,
    nixpkgs,
    ...
  }:
    let
      allSystems = builtins.attrNames nixpkgs.legacyPackages;

      forAllSystems = (f:
        nixpkgs.lib.genAttrs allSystems (system:
          f (import nixpkgs { inherit system; })
        )
      );
    in {
      packages = forAllSystems (pkgs: rec {
        rotate-plugin = pkgs.stdenv.mkDerivation {
          pname = "rotate-plugin";
          version = self.rev or self.dirtyRev;
          src = ./.;

          nativeBuildInputs = with pkgs; [
            cmake
          ];
        };

        default = rotate-plugin;
      });

      devShells = forAllSystems (pkgs: {
        default = pkgs.mkShell {
          packages = with pkgs; [
            cmake
          ];
        };
      });
    };
}