# gidgee
Experiments in real-time rendering and rigid body physics


## Dependencies
Building Gidgee depends on three other projects: [GLFW](https://www.glfw.org/), [Python](https://www.python.org/), and [Vulkan](https://www.khronos.org/vulkan/).

### GLFW
On Windows and macOS, this dependency is bundled into the project and therefore requires no special installation steps.
On Linux, however, this must be installed (or built) manually.
The package name on Debian-based distributions is `libglfw3-dev`.

### Python
Gidgee's build process utilizes Python for various scripting tasks.
Download and install a recent version of Python 3 from the [download page](https://www.python.org/downloads/) for your corresponding operating system.
At the moment, `scripts/res2header.py` is used to convert runtime resources (models, shaders, textures, etc) from their native format into embeddable C headers.
This shift in complexity from runtime to build time allows the final artifact to be completely standalone.

### Vulkan
Gidgee depends on having a recent version of the [Vulkan SDK](https://vulkan.lunarg.com/sdk/home) installed.
Consult the install guide for your preferred platform ([Windows](https://vulkan.lunarg.com/doc/sdk/latest/windows/getting_started.html), [macOS](https://vulkan.lunarg.com/doc/sdk/latest/mac/getting_started.html), [Linux](https://vulkan.lunarg.com/doc/sdk/latest/linux/getting_started.html)) to get this up and running correctly.

## Building
This project is built using POSIX-compatible [make](https://pubs.opengroup.org/onlinepubs/009695399/utilities/make.html) on unix-like systems and a simple batch file on Windows.
Windows binaries can also be cross-compiled from a unix-like system via [mingw-w64](http://mingw-w64.org/doku.php).

### Linux
```
# debian-based
sudo apt install gcc make libglfw3-dev python3 python3-venv

make
```

### macOS
```
brew install make python3

make LDLIBS='-Lvendor/lib64/macos/ -lglfw3  \
  -framework Cocoa -framework CoreFoundation -framework IOKit'
```

### Windows
Windows binaries can be built natively or cross-compiled from Linux or macOS.

#### Native
With a recent version of [Visual Studio](https://visualstudio.microsoft.com/downloads/) installed:
```
scripts\shell.bat
scripts\build.bat
```

The `scripts\shell.bat` script assumes that Visual Studio 2019 Community Edition is installed.
If you are using a different version then you'll need to update this script accordingly.

#### Cross-Compile from Linux
```
# debian-based
sudo apt install make mingw-w64 python3 python3-venv

make  \
  AR=x86_64-w64-mingw32-ar  \
  CC=x86_64-w64-mingw32-gcc  \
  LDFLAGS=-mwindows  \
  LDLIBS='-Lvendor/lib64/windows/ -lglfw3  \
    -lgdi32 -lkernel32 -lshell32 -luser32'
```

#### Cross-Compile from macOS
```
brew install make mingw-w64 python3

make  \
  AR=x86_64-w64-mingw32-ar  \
  CC=x86_64-w64-mingw32-gcc  \
  LDFLAGS=-mwindows  \
  LDLIBS='-Lvendor/lib64/windows/ -lglfw3  \
    -lgdi32 -lkernel32 -lshell32 -luser32'
```
