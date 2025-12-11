{ self
, lib
}:
{
  default = self.overlays.learn-opengl;

  learn-opengl = final: prev: {
    learn-opengl = prev.callPackage ./default.nix { inherit (prev.myLib) version; };
  };

  libs = final: prev: {
    myLib = import ./lib.nix { inherit self lib; };
  };
}
