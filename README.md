# Sickle Editor

![Build Status Badge](https://img.shields.io/github/actions/workflow/status/Treecase/SickleEditor/cmake-multi-platform.yml)
![License Badge](https://img.shields.io/github/license/Treecase/SickleEditor)

An open-source editor for GoldSrc maps. Both `.MAP` and `.RMF` formats are supported. Sickle Editor is released under the [GPLv3](https://www.gnu.org/licenses/gpl-3.0.en.html) license.


## Getting Started

There are a few things that must be set up to get started. Firstly, click `Edit>Preferences` in the menu to open the preferences window. There are four areas here: Game Definition, Game Root Path, Sprite Root Path, and Texture WADs.

The Game Definition is a file that tells the editor which Entity types are available in the game. Sickle needs to be pointed to this file in order to properly edit maps; for Half-Life, this file is called 'halflife.fgd'.

The game root path tells Sickle where to find the game data files like sprites and sounds.

The sprite root path tells Sickle where to find the sprite files used for entity icons (eg. lights).

GoldSrc maps use external data files called WADs to store texture data. Sickle doesn't know where these files are, so you'll have to point it to them.


## Building

### Linux

Make sure you install the required dependencies. For example, Debian/Ubuntu users can run:

```bash
sudo apt install build-essential cmake bison flex git libglew-dev libglm-dev libgtkmm-3.0-dev liblua5.4-dev
```

And then build the program:

```bash
git clone https://github.com/Treecase/SickleEditor
cd SickleEditor
cmake -B build .
cmake --build build
```

### Windows

Building on Windows requires [MSYS2](https://msys2.org). Note that this process is only tested with the UCRT64 environment (which is the default).

To start, install MSYS2 and open the UCRT64 shell. Run these commands to install the required dependencies:

```bash
pacman -S mingw-w64-ucrt-x86_64-{toolchain,cmake,glew,glm,gtkmm3,lua}
pacman -S bison flex git
```

To build the program:

```bash
git clone https://github.com/Treecase/SickleEditor
cd SickleEditor
cmake -B build .
cmake --build build
```


## Installing

To install the Sickle Editor, use:

```bash
cmake --install build
```

By default, the program will be installed to `/usr/local` on Linux and `C:/Program Files/Sickle Editor` on Windows. If you want the installation to go somewhere else, use:

```bash
cmake --install build --prefix=<Install Prefix>
```

Linux users will also have to compile the GSettings schema with the following command.

```bash
glib-compile-schemas <Install Prefix>/share/glib-2.0/schemas
```
