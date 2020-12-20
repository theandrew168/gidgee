@echo off
SetLocal EnableDelayedExpansion

SET sources=
FOR /R %%f in (*.c) do (
    if "%%~xf"==".c" SET sources=!sources! %%f
)
echo Files: %sources%

SET cflags=/FC /MT /WL /Zl /Fo.\src\ /I %cd%\src\ /I %cd%\vendor\include\
SET ldflags=/SUBSYSTEM:CONSOLE /INCREMENTAL:NO /OPT:REF /OUT:.\main.exe
SET ldlibs=/LIBPATH:"%cd%\vendor\lib64\windows\" glfw3.lib gdi32.lib kernel32.lib shell32.lib user32.lib

cl %cflags% %sources% /link %ldflags% %ldlibs%

echo Error: %ERRORLEVEL%
