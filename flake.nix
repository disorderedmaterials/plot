{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-22.11";
    future.url = "github:NixOS/nixpkgs/nixos-unstable";
    outdated.url = "github:NixOS/nixpkgs/nixos-21.05";
    qt-idaaas.url = "github:disorderedmaterials/qt-idaaas";
  };
  outputs =
    { self, nixpkgs, future, outdated, flake-utils, bundlers, qt-idaaas }:
    let

      version = "0.1";
      base_libs = pkgs: with pkgs; [ cmake ninja ];
      gui_libs = { pkgs, q }:
        with pkgs; [
          glib
          libGL.dev
          libglvnd
          libglvnd.dev
          q.qtbase
          q.qt3d
          q.qtsvg
          q.wrapQtAppsHook
        ];
      check_libs = pkgs: with pkgs; [ gtest ];

    in
    flake-utils.lib.eachSystem [ "x86_64-linux" "aarch64-linux" ] (system:

    let
      pkgs = import nixpkgs { inherit system; };
      next = import future { inherit system; };
      qt = qt-idaaas.packages.${system};
    in
    {
      devShells.default = pkgs.stdenv.mkDerivation {
        name = "mildred-shell";
        buildInputs = base_libs pkgs ++ gui_libs {inherit pkgs; q=qt;} ++ check_libs pkgs
          ++ (with pkgs; [
          (pkgs.clang-tools.override {
            llvmPackages = pkgs.llvmPackages_7;
          })
          ccache
          ccls
          cmake-format
          cmake-language-server
          distcc
          gdb
          openmpi
          tbb
          valgrind
        ]);
        CMAKE_CXX_COMPILER_LAUNCHER = "${pkgs.ccache}/bin/ccache";
        CMAKE_CXX_FLAGS_DEBUG = "-g -O0";
        CXXL = "${pkgs.stdenv.cc.cc.lib}";
        my_proxy = "socks5://localhost:8888";
      };

      apps = {
        default =
          flake-utils.lib.mkApp { drv = self.packages.${system}.library; };
      };

      packages = {
        library = pkgs.stdenv.mkDerivation ({
          inherit version;
          pname = "mildred";
          src = ./.;
          buildInputs = base_libs pkgs ++ (gui_libs {inherit pkgs; q=qt;});
          nativeBuildInputs = [ pkgs.wrapGAppsHook ];

          cmakeFlags = [ "-G Ninja" "-DJV2_USE_CONAN=OFF" ];
          installPhase = ''
            mkdir -p $out/bin
            mv bin/* $out/bin/
          '';

          meta = with pkgs.lib; {
            description = "Plot lib for 2D/3D";
            homepage = "https://github.com/disorderedmaterials/plot";
            license = licenses.gpl3;
            maintainers = with maintainers; [ rprospero ];
          };
        });

        singularity =
          nixpkgs.legacyPackages.${system}.singularity-tools.buildImage {
            name = "mildred-${version}";
            diskSize = 1024 * 250;
            contents = [
              self.packages.${system}.library
            ];
            runScript = "${self.packages.${system}.library}/bin/mildred $@";
          };
      };
    });
}
