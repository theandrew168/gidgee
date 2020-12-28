# Request POSIX Make behavior and disable any default suffix-based rules
.POSIX:
.SUFFIXES:

# CFLAGS breakout by category
CFLAGS_VERSION = -std=c99
CFLAGS_OPTIMIZATIONS = -g -Og
CFLAGS_WARNINGS = -Wall -Wextra -Wpedantic
CFLAGS_DEFINITIONS = -DGLFW_INCLUDE_NONE -DVK_NO_PROTOTYPES
CFLAGS_INCLUDE_DIRS = -Ires/ -Isrc/ -Ivendor/include/

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
libgidgee_sources =      \
  src/opengl_buffer.c    \
  src/opengl_loader.c    \
  src/opengl_renderer.c  \
  src/opengl_shader.c    \
  src/opengl_texture.c   \
  src/pixel.c            \
  src/vertex.c           \
  src/vulkan_loader.c    \
  src/vulkan_renderer.c
libgidgee_objects = $(libgidgee_sources:.c=.o)

# Express dependencies between object and source files
src/opengl_buffer.o: src/opengl_buffer.c src/opengl_buffer.h src/opengl_loader.h src/vertex.h
src/opengl_loader.o: src/opengl_loader.c src/opengl_loader.h
src/opengl_renderer.o: src/opengl_renderer.c src/opengl_renderer.h src/opengl_loader.h
src/opengl_shader.o: src/opengl_shader.c src/opengl_shader.h src/opengl_loader.h
src/opengl_texture.o: src/opengl_texture.c src/opengl_texture.h src/opengl_loader.h
src/pixel.o: src/pixel.c src/pixel.h
src/vertex.o: src/vertex.c src/vertex.h
src/vulkan_loader.o: src/vulkan_loader.c src/vulkan_loader.h
src/vulkan_renderer.o: src/vulkan_renderer.c src/vulkan_renderer.h src/vulkan_loader.h

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

# Declare required resource headers
resource_headers =        \
  res/models/cube.h       \
  res/models/square.h     \
  res/models/triangle.h   \
  res/shaders/dev_frag.h  \
  res/shaders/dev_vert.h  \
  res/textures/wall.h

# Express dependencies between header and resource files
res/models/cube.h: res/models/cube.obj
res/models/square.h: res/models/square.obj
res/models/triangle.h: res/models/triangle.obj
res/shaders/dev_frag.h: res/shaders/dev_frag.glsl
res/shaders/dev_vert.h: res/shaders/dev_vert.glsl
res/textures/wall.h: res/textures/wall.jpg

# Resource conversion requires some Python packages
$(resource_headers): venv

# Compile and link the main executable
gidgee: src/main.c libgidgee.a $(resource_headers)
	@echo "EXE     $@"
	@$(CC) $(CFLAGS) $(LDFLAGS) -o $@ src/main.c libgidgee.a $(LDLIBS)

# Create the virtualenv for pre/post build scripts
venv:
	@echo "VENV    venv/"
	@python3 -m venv venv/
	@./venv/bin/pip install wheel
	@echo "DEPS    scripts/requirements.txt"
	@./venv/bin/pip install -r scripts/requirements.txt

# Double suffix rules for convertion resource files to header files
.SUFFIXES: .obj .h
.obj.h:
	@echo "MODEL   $@"
	@./venv/bin/python3 scripts/res2header.py $< $@

.SUFFIXES: .glsl .h
.glsl.h:
	@echo "SHADER  $@"
	@./venv/bin/python3 scripts/res2header.py $< $@

.SUFFIXES: .jpg .h
.jpg.h:
	@echo "TEXTURE $@"
	@./venv/bin/python3 scripts/res2header.py $< $@

.SUFFIXES: .png .h
.png.h:
	@echo "TEXTURE $@"
	@./venv/bin/python3 scripts/res2header.py $< $@

# Helper target that cleans up build artifacts
.PHONY: clean
clean:
	rm -fr gidgee *.exe *.a *.so *.dll src/*.o res/models/*.h res/shaders/*.h res/textures/*.h
