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
  * Download [SDL2-devel-2.28.5-VC.zip](https://github.com/libsdl-org/SDL/releases/download/release-2.28.5/SDL2-devel-2.28.5-VC.zip) and extract it to **thirdparty** folder or set the **SDL2_DIR** environment variable
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
Currently only Player 1 is mapped to the keyboard

 Button     | Keyboard        | XBox Controller  |
 -----------|-----------------|------------------|
 A          | A key           | X                |
 B          | S key           | B                |
 Start      | Enter           | Menu             |
 Select     | Space           | Y                |
 Up         | Up arrow Key    | D-Pad/Left stick |
 Down       | Down arrow key  | D-Pad/Left stick |
 Left       | Left arrow key  | D-Pad/Left stick |
 Right      | Right arrow key | D-Pad/Left stick |
