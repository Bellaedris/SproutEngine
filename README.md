
# Build and run
The build process uses cmake and is an adaptation of [LearnOpenGL](https://github.com/JoeyDeVries/LearnOpenGL) work. 
## Windows build

As all the required libs and headers are already in the project, just run cmake and open the generated solution (Cmake gui for windows was used in my case). Cmake may require admin privileges.

## Linux build
First make sure you have CMake, Git, and GCC by typing as root (sudo) `apt-get install g++ cmake git` and then get the required packages: Using root (sudo) and type `apt-get install libsoil-dev libglm-dev libassimp-dev libglew-dev libglfw3-dev libxinerama-dev libxcursor-dev  libxi-dev libfreetype-dev libgl1-mesa-dev xorg-dev` .

Build through CMake-gui: The source directory is SproutEngine and specify the build directory as SproutEngine/build. Creating the build directory within SproutEngine is important for linking to the resource files (it also will be ignored by Git). Hit configure and specify your compiler files (Unix Makefiles are recommended), resolve any missing directories or libraries, and then hit generate. Navigate to the build directory (`cd SproutEngine/build`) and type make in the terminal. This should generate the executables in the respective chapter folders.

Build through Cmake command line:

```
cd /path/to/SproutEngine
mkdir build && cd build
cmake ..
cmake --build .
```

Note that CodeBlocks or other IDEs may have issues running the programs due to problems finding the shader and resource files, however it should still be able to generate the executables. To work around this problem it is possible to set an environment variable to tell the tutorials where the resource files can be found. The environment variable is named LOGL_ROOT_PATH and may be set to the path to the root of the SproutEngine directory tree. For example:

`export LOGL_ROOT_PATH=/home/user/tutorials/SproutEngine`

Running ls $LOGL_ROOT_PATH should list, among other things, this README file and the resources directory.
Mac OS X building

## MacOS build
Building on Mac OS X is fairly simple:

```
brew install cmake assimp glm glfw freetype
cmake -S . -B build
cmake --build build -j$(sysctl -n hw.logicalcpu)
```
