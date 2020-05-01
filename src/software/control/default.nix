let
  sources = import ./nix/sources.nix;
  pkgs = import sources.nixpkgs {};
  generatedBuild = pkgs.callPackage ./Cargo.nix {
  defaultCrateOverrides = pkgs.defaultCrateOverrides // {
      expat-sys = attrs: {
        buildInputs = [pkgs.cmake]; 
      };
      servo-freetype-sys = attrs: {
        rename = "freetype-sys";
        buildInputs = [pkgs.cmake]; 
      };
      "wayland-client" = attrs: {
        patches = if attrs.version == "0.21.13" then
            [ ./nix/wayland-client-0.12.13-force-native_lib.patch ]
          else
            [];
      };
    };
  };
in generatedBuild.rootCrate.build
