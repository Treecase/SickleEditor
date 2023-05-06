
# Sickle Editor

An open-source editor for GoldSrc maps. Both `.MAP` and `.RMF` formats are supported. Sickle Editor is released under the [GPLv3](https://www.gnu.org/licenses/gpl-3.0.en.html) license.


## Getting Started

GoldSrc maps use external data files called WADs to store texture data. Sickle doesn't know where these files are, so you'll have to point it to them. To do this, click `Edit` in the menu and select `WAD Paths`. This will open a dialog where you can add or remove WADs to Sickle. Add the WAD paths, then click `Done`.


## Building

Sickle depends on [gtkmm-3](https://gtkmm.org/en/index.html) and [Lua 5.4](https://www.lua.org). You'll also need to have [flex](https://github.com/westes/flex) and [bison](https://www.gnu.org/software/bison) installed.

Before building, you must initialize the git submodules for the project:

```shell
$ git submodule update --init --recursive
```

To build the program:

```shell
$ mkdir build
$ cd build
$ cmake ..
$ cmake --build .
```

### Windows

Currently, only building with [mingw](https://www.mingw-w64.org) is supported. You may have to tell CMake to use the MinGW generator:

```shell
$ mkdir build
$ cd build
$ cmake -G 'MinGW Makefiles' ..
$ cmake --build .
```

I also had some problems getting CMake to find Flex and Bison, and had to manually add the paths to `CMakeCache.txt`:

```
BISON_EXECUTABLE:FILEPATH=C:/msys64/usr/bin/bison.exe
FLEX_EXECUTABLE:FILEPATH=C:/msys64/usr/bin/flex.exe
FL_LIBRARY:FILEPATH=C:/msys64/usr/lib/libfl.a
```


## Installing

```shell
$ cmake --install .
```

or,

```shell
$ cmake --install . --prefix=<install path>
```
