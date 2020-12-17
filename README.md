# gidgee
Experiments in real-time rendering and rigid body physics

## Building
This project is built using POSIX-compatible [make](https://pubs.opengroup.org/onlinepubs/009695399/utilities/make.html).
For unix-like systems, it can be built natively.
For Windows builds, [mingw-w64](http://mingw-w64.org/doku.php) is used to cross-compile the project from a unix-like system.

### Linux
```
# debian-based
sudo apt install gcc make

make
```

### macOS
```
brew install make

make
```

### Windows
Windows binaries are cross-compiled from Linux or macOS.

From Linux:
```
# debian-based
sudo apt install make mingw-w64

make  \
  AR=x86_64-w64-mingw32-ar  \
  CC=x86_64-w64-mingw32-gcc  \
  LDLIBS=
```

From macOS:
```
brew install make mingw-w64

make  \
  AR=x86_64-w64-mingw32-ar  \
  CC=x86_64-w64-mingw32-gcc  \
  LDLIBS=
```
