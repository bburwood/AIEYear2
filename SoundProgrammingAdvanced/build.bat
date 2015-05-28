@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat"

msbuild /t:clean
msbuild SoundProgramming.sln

copy .\Debug\SoundProgramming.exe .\data
copy .\Debug\*.dll .\data
cd data
SoundProgramming.exe
cd ..
