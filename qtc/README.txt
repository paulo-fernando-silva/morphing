ffd:

- Dependencies:
・ffd: usually a lib implementing some functionality.
・ffdTest: the test for that lib functionality.

- To build the any project just:
・Set the project build directory to the ./build.XXX where XXX is the project type.
・If there are dependencies, add the environment variable PKG_CONFIG_PATH ${HOME}/usr/lib/pkgconfig
・Also add to the PATH :/opt/local/bin
・Run QMake.
・Rebuild.
・Set "Working directory" in "Build & Run" project options to $$PWD/..
