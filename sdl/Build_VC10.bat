@echo off

call "%ProgramFiles(x86)%\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" x86
cl.exe pmaze2.cpp