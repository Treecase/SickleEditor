# Sickle
Open-source editor for GoldSrc.


## Usage

Pass the game directory.
```shell
$ sickle ~/.steam/steam/steamapps/common/Half-Life
```


## Building

Install the required libraries:
- [SDL2](https://libsdl.org)
- [GLEW](http://glew.sourceforge.net/)
- [glm](https://github.com/g-truc/glm)
- [imgui](https://github.com/ocornut/imgui) (Included as a git submodule)

Note that if you are building on Windows, the dependencies should be installed into the `externals` directory.

Build the program:

```shell
$ mkdir build
$ cd build
$ cmake ..
$ cmake --build .
```
