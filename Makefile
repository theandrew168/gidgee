# Request POSIX Make behavior and disable any default suffix-based rules
.POSIX:
.SUFFIXES:

# CFLAGS breakout by category
CFLAGS_VERSION = -std=c99
CFLAGS_OPTIMIZATIONS = -g -Og
CFLAGS_WARNINGS = -Wall -Wextra -Wpedantic
CFLAGS_DEFINITIONS = -DGLFW_INCLUDE_NONE
CFLAGS_INCLUDE_DIRS = -Isrc/ -Ivendor/include/

# Declare compiler tools and flags
AR      = ar
CC      = cc
CFLAGS  = -fPIC
CFLAGS += $(CFLAGS_VERSION)
CFLAGS += $(CFLAGS_OPTIMIZATIONS)
CFLAGS += $(CFLAGS_WARNINGS)
CFLAGS += $(CFLAGS_DEFINITIONS)
CFLAGS += $(CFLAGS_INCLUDE_DIRS)
LDFLAGS =
LDLIBS  = -ldl -lglfw -lm

# Declare which targets should be built by default
default: gidgee
all: libgidgee.a libgidgee.so gidgee

# Declare library sources
libgidgee_sources =  \
  src/opengl_loader.c
libgidgee_objects = $(libgidgee_sources:.c=.o)

# Express dependencies between object and source files
src/opengl_loader.o: src/opengl_loader.c src/opengl_loader.h

# Build the static library
libgidgee.a: $(libgidgee_objects)
	@echo "STATIC  $@"
	@$(AR) rcs $@ $(libgidgee_objects)

# Build the shared library
libgidgee.so: $(libgidgee_objects)
	@echo "SHARED  $@"
	@$(CC) $(LDFLAGS) -shared -o $@ $(libgidgee_objects) $(LDLIBS)

# Double suffix rule for compiling .c files to .o object files
.SUFFIXES: .c .o
.c.o:
	@echo "CC      $@"
	@$(CC) $(CFLAGS) -c -o $@ $<

# Compile and link the main executable
gidgee: src/main.c libgidgee.a
	@echo "EXE     $@"
	@$(CC) $(CFLAGS) $(LDFLAGS) -o $@ src/main.c libgidgee.a $(LDLIBS)

# Helper target that cleans up build artifacts
.PHONY: clean
clean:
	rm -fr gidgee *.exe *.a *.so *.dll src/*.o
