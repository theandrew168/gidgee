@echo off

:: list of source files to be compiled
set SOURCES=                ^
  %cd%\src\opengl_loader.c  ^
  %cd%\src\main.c

:: References:
:: https://docs.microsoft.com/en-us/cpp/build/reference/compiler-options-listed-alphabetically
:: https://docs.microsoft.com/en-us/cpp/build/reference/linker-options

:: /FC  use full pathnames in diagnostics
:: /MT  link with LIBCMT.LIB
:: /WL  enable one line diagnostics
:: /Fo  name object file
:: /I   add to include search path
set CFLAGS=/FC /MT /WL /Fo.\src\ /I%cd%\src\ /I%cd%\vendor\include\

:: /OUT          name of output file
:: /SUBSYSTEM    type of application (usually CONSOLE or WINDOWS)
:: /ENTRY        program entry point (usually mainCRTStartup)
:: /INCREMENTAL  control incremental linking
set LDFLAGS=/OUT:.\main.exe /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup /INCREMENTAL:NO

:: /LIBPATH      extra directories to search for libraries
:: glfw3.lib     open source library for multi-platform OpenGL and Vulkan development
:: gdi32.lib     primitive drawing functions for display output
:: kernel32.lib  exposes most of the Win32 base APIs
:: shell32.lib   misc windows utilties?
:: user32.lib    creates and manipulates standard elements of the Windows UI
set LDLIBS=/LIBPATH:"%cd%\vendor\lib64\windows\" glfw3.lib gdi32.lib kernel32.lib shell32.lib user32.lib

:: cl.exe  MSVC compiler command-line interface
:: /link   options to be passed to the linker
cl.exe %CFLAGS% %SOURCES% /link %LDFLAGS% %LDLIBS%

echo Error: %ERRORLEVEL%
