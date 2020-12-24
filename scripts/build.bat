@echo off

:: list of source files to be compiled
set SOURCES=                  ^
  %cd%\src\opengl_loader.c    ^
  %cd%\src\opengl_renderer.c  ^
  %cd%\src\opengl_shader.c    ^
  %cd%\src\vulkan_loader.c    ^
  %cd%\src\vulkan_renderer.c  ^
  %cd%\src\main.c

:: References:
:: https://docs.microsoft.com/en-us/cpp/build/reference/compiler-options
:: https://docs.microsoft.com/en-us/cpp/build/reference/linker-options

:: /FC  use full pathnames in diagnostics
:: /MT  link with LIBCMT.LIB
:: /WL  enable one line diagnostics
:: /Zi  generate debugging info
:: /Zl  omit default C runtime library
:: /Fo  name object file
:: /I   add directory to header search path
:: /D   defines a preprocessing symbol
set CFLAGS=/FC /MT /WL /Zi /Zl /Fo.\src\
set CFLAGS=%CFLAGS% /I %cd%\src\ /I %cd%\vendor\include\
set CFLAGS=%CFLAGS% /D GLFW_INCLUDE_NONE /D VK_NO_PROTOTYPES

:: /SUBSYSTEM    type of application (CONSOLE for development, WINDOWS for deployment)
:: /ENTRY        program entry point (always mainCRTStartup)
:: /INCREMENTAL  control incremental linking
set LDFLAGS=/SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup /INCREMENTAL:NO

:: /LIBPATH      extra directories to search for libraries
:: glfw3.lib     open source library for multi-platform OpenGL and Vulkan development
:: gdi32.lib     primitive drawing functions for display output
:: kernel32.lib  exposes most of the Win32 base APIs
:: shell32.lib   misc windows utilties and assorted things
:: user32.lib    creates and manipulates standard elements of the Windows UI
set LDLIBS=/LIBPATH:"%cd%\vendor\lib64\windows\" glfw3.lib gdi32.lib kernel32.lib shell32.lib user32.lib

:: cl.exe   MSVC compiler command-line interface
:: CFLAGS   options for compilation
:: SOURCES  source files to be compiled
:: /link    options to be passed to the linker
:: /OUT     name of output file
:: LDFLAGS  options for linking
:: LDLIBS   libraries to link against
cl.exe %CFLAGS% %SOURCES% /link /OUT:.\gidgee.exe %LDFLAGS% %LDLIBS%

echo Error: %ERRORLEVEL%
