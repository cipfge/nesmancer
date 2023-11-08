# NES Emulator
Work in progress.

## Build instructions
### Linux
* Install dependencies

Debian based distributions
```bash
sudo apt install build-essential cmake ninja-build sdl2-devel
```
Fedora
```bash
sudo dnf install gcc-c++ cmake ninja-build SDL2-devel
```
Arch Linux
```bash
sudo pacman -S gcc cmake ninja sdl2
```

* Build

Open the terminal application in the project directory and run the following commands
```bash
mkdir build
cd build
cmake -G Ninja ..
ninja
```
