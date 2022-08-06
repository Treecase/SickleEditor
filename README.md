# Sickle
Open-source editor for GoldSrc.


## Usage

Pass in a list of models to view the texture of.
```shell
$ sickle valve/models/barney.mdl valve/models/gman.mdl
```


## Building

Install the required libraries:
- [SDL2](https://libsdl.org)
- [GLEW](http://glew.sourceforge.net/)
- [glm](https://github.com/g-truc/glm)
- [imgui](https://github.com/ocornut/imgui)

Note that imgui must be installed into a folder named `externals`, and if you are building on Windows, so should all the other dependencies.

Build the program:

```shell
$ mkdir build
$ cd build
$ cmake ..
$ cmake --build .
```
