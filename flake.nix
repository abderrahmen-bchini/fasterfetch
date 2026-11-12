{
  description = "fasterfetch - a retro distro fetch utility";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = { self, nixpkgs }:
    let
      forAllSystems = nixpkgs.lib.genAttrs [
        "x86_64-linux"
        "aarch64-linux"
        "x86_64-darwin"
        "aarch64-darwin"
      ];
    in {
      packages = forAllSystems (system:
        let pkgs = nixpkgs.legacyPackages.${system};
        in {
          default = pkgs.stdenv.mkDerivation {
            pname = "fasterfetch";
            version = "1.0.0";
            src = ./.;

            nativeBuildInputs = [ pkgs.gnumake ];

            buildPhase = "make";
            installPhase = "make install PREFIX=$out";

            meta = with pkgs.lib; {
              description = "A retro distro fetch utility that reads system info directly from kernel interfaces";
              homepage = "https://github.com/abderrahmen-bchini/fasterfetch";
              license = licenses.mit;
              platforms = platforms.unix;
              mainProgram = "fasterfetch";
            };
          };
        });

      apps = forAllSystems (system: {
        default = {
          type = "app";
          program = "${self.packages.${system}.default}/bin/fasterfetch";
        };
      });
    };
}
