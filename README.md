# rrun

A cli that detects whether a project is using CMake or Meson, then
creates the build folder if needed, builds the executable, and runs it.

## Usage
```bash
cd ~/Projects/myproject
```
```bash
rrun
```

## Build and install
```bash
meson setup build
```
```bash
meson compile -C build
```
```bash
sudo meson install -C build
```
