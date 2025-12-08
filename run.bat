@echo off
chcp 65001 >nul

REM Setup Visual Studio environment for ml and link
call "%ProgramFiles%\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul 2>&1
if errorlevel 1 (
    call "%ProgramFiles(x86)%\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul 2>&1
)
if errorlevel 1 (
    call "%ProgramFiles%\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul 2>&1
)
if errorlevel 1 (
    call "%ProgramFiles(x86)%\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul 2>&1
)

echo Running GAS-2025...
cd Debug
GAS-2025.exe -in:in.txt -out:..//Assembler/out.asm
if errorlevel 1 (
    echo Error generating assembly!
    cd ..
    pause
    exit /b 1
)
cd ..

if not exist "Assembler\out.asm" (
    echo Error: Assembler\out.asm was not created!
    pause
    exit /b 1
)

echo.
echo Compiling assembly with ml...
cd Assembler
ml /c /coff /Zi out.asm
if errorlevel 1 (
    echo Error compiling assembly!
    cd ..
    pause
    exit /b 1
)

echo.
echo Linking with link...
link /SUBSYSTEM:CONSOLE /ENTRY:main out.obj libucrt.lib kernel32.lib ..\Debug\StaticLibrary.lib
if errorlevel 1 (
    echo Error linking!
    cd ..
    pause
    exit /b 1
)

echo.
echo Running generated program...
out.exe

cd ..
pause
