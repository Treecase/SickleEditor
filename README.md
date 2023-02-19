
# Sickle Editor

An open-source editor for GoldSrc maps. Currently, only .MAP files are supported. Sickle Editor is released under the [GPLv3](https://www.gnu.org/licenses/gpl-3.0.en.html) license.


## Building

Note that on Windows, only building with [mingw](https://www.mingw-w64.org) is supported. Sickle depends on [gtkmm-3](https://gtkmm.org/en/index.html) and [Lua 5.4](https://www.lua.org).

To build the program:

```shell
$ mkdir build
$ cd build
$ cmake ..
$ cmake --build .
```

## Installing

```shell
$ cmake --install .
```
