{ stdenv
, lib
, meson
, ninja
, pkg-config
, glfw
, glm
, version ? "git"
}:
stdenv.mkDerivation {
  pname = "learn-opengl";
  inherit version;
  src = lib.cleanSource ./..;

  nativeBuildInputs = [
    meson
    ninja
    pkg-config
  ];

  buildInputs = [
    glfw
    glm
  ];

  mesonBuildType = "release";

  meta = {
    description = "Perilous Trials of OpenGL";
    mainProgram = "learn-opengl";
    homepage = "https://github.com/spitulax/learn-opengl";
    license = lib.licenses.mit;
    platforms = lib.platforms.all;
  };
}
