# Nesmancer
![GitHub Actions](https://github.com/cipfge/nesmancer/actions/workflows/build.yml/badge.svg)

Nintendo Entertainment System emulator.

This is a work in progress hobby cross-platform C++ project tested on Windows and Linux, the emulator is not cycle accurate.

![screenshot](docs/screenshot.png)

## Features
* Sound: Development in progress
* Supported mappers: NROM(0), MMC1(1), UxROM(2), CNROM(3)
* Gamepad: Support for Player 1 and Player 2

## Build instructions
### Windows
* Install dependencies
  * Install Visual Studio Community with C++ desktop development and **CMake**
  * Download latest SDL-VS binary from [SDL Github Releases](https://github.com/libsdl-org/SDL/) and extract it to **thirdparty** folder or any other directory and set the **SDL2_DIR** environment variable
* Build
  * Clone the repository and open **nesmancer** folder in **Visual Studio** or **Visual Studio Code**
  * Build solution and run

### Linux
* Install dependencies
  * Debian/Ubuntu: ```sudo apt install build-essential cmake ninja-build libsdl2-dev libdbus-1-dev```
  * Fedora: ```sudo dnf install gcc-c++ cmake ninja-build SDL2-devel dbus-devel```
  * Arch: ```sudo pacman -S gcc cmake ninja sdl2 dbus```

* Build
  * Clone the repository and open the terminal application in the project directory.
  * Run the following commands: ```mkdir build && cd build && cmake -G Ninja .. && ninja```
  * Run the emulator: ```./nesmancer [path to ROM file]```

## Usage
To open a ROM file use the **File->Open (Ctr+O)** menu or pass the ROM file as the first argument when launching the program.

To pause or resume the emulatation use the **System->Pause (Esc)** menu.

## Controller configuration
Controller and keyboard mapping:

 Button     | Keyboard 1      | Keyboard 2      | XBox Controller  |
 -----------|-----------------|-----------------|------------------|
 A          | H key           | Numpad 2        | X                |
 B          | J key           | Numpad 3        | B                |
 Start      | U key           | Numpad 6        | Menu             |
 Select     | Y key           | Numpad 5        | Y                |
 Up         | W key           | Up key          | D-Pad/Left stick |
 Down       | S key           | Down key        | D-Pad/Left stick |
 Left       | A key           | Left key        | D-Pad/Left stick |
 Right      | D key           | Right key       | D-Pad/Left stick |
