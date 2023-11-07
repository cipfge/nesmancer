# NES Emulator
Work in progress.

## Build instructions
### Linux
Install dependencies

Debian based distributions
```bash
sudo apt install build-essential cmake ninja-build sdl2-devel
```
Fedora
```bash
sudo dnf install gcc-c++ cmake ninja-build SDL2-devel
```

Build
```bash
mkdir build
cd build
cmake -G Ninja ..
ninja
```
