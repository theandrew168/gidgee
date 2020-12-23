# gidgee
Experiments in real-time rendering and rigid body physics

## Dependencies
Gidgee depends on two other open-source projects: [GLFW](https://www.glfw.org/) and [Vulkan](https://www.khronos.org/vulkan/).

### GLFW
On Windows and macOS, this dependency is bundled into the project and therefore requires no special installation steps.
On Linux, however, this must be installed (or built) manually.
The package name on Debian-based distributions is `libglfw3-dev`.

### Vulkan
Gidgee depends on having a recent version of the [Vulkan SDK](https://vulkan.lunarg.com/sdk/home) installed.
Consult the install guide for your preferred platform ([Windows](https://vulkan.lunarg.com/doc/sdk/latest/windows/getting_started.html), [macOS](https://vulkan.lunarg.com/doc/sdk/latest/mac/getting_started.html), [Linux](https://vulkan.lunarg.com/doc/sdk/latest/linux/getting_started.html))

## Building
This project is built using POSIX-compatible [make](https://pubs.opengroup.org/onlinepubs/009695399/utilities/make.html).
For unix-like systems, it can be built natively.
For Windows builds, [mingw-w64](http://mingw-w64.org/doku.php) is used to cross-compile the project from a unix-like system.

### Linux
```
# debian-based
sudo apt install gcc make libglfw3-dev

make
```

### macOS
```
brew install make

make LDLIBS='-Lvendor/lib64/macos/ -lglfw3  \
  -framework Cocoa -framework IOKit'
```

### Windows
Windows binaries can be built natively or cross-compiled from Linux or macOS.

#### Native
With a recent version of [Visual Studio](https://visualstudio.microsoft.com/downloads/) installed.

#### Cross-Compile From Linux
```
# debian-based
sudo apt install make mingw-w64

make  \
  AR=x86_64-w64-mingw32-ar  \
  CC=x86_64-w64-mingw32-gcc  \
  LDFLAGS=-mwindows  \
  LDLIBS='-Lvendor/lib64/windows/ -lglfw3  \
    -lgdi32 -lkernel32 -lshell32 -luser32'
```

#### Cross-Compile From macOS
```
brew install make mingw-w64

make  \
  AR=x86_64-w64-mingw32-ar  \
  CC=x86_64-w64-mingw32-gcc  \
  LDFLAGS=-mwindows  \
  LDLIBS='-Lvendor/lib64/windows/ -lglfw3  \
    -lgdi32 -lkernel32 -lshell32 -luser32'
```
