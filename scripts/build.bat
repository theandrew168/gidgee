@echo off
SetLocal EnableDelayedExpansion

SET sources=
FOR /R %%f in (*.c) do (
    if "%%~xf"==".c" SET sources=!sources! %%f
)
echo Files: %sources%

SET cflags=-MT -WL -FC /Fo.\src\ /I %cd%\src\ /I %cd%\vendor\include\
SET ldflags=/SUBSYSTEM:CONSOLE /INCREMENTAL:NO /OPT:REF /OUT:.\main.exe
SET ldlibs=/LIBPATH:"%cd%\vendor\lib64\windows\" libglfw3.a gdi32.lib kernel32.lib user32.lib

cl %cflags% %sources% /link %ldflags% %ldlibs%

echo Error: %ERRORLEVEL%
