@echo off
REM Go to the directory of this script
cd /d "%~dp0"

REM Set build directory absolute path
set "BUILD_DIR=%~dp0..\\build"

REM Create build directory if it doesn't exist
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

REM Change to build directory
cd /d "%BUILD_DIR%"

REM Run CMake
cmake -S .. -B . -G "Visual Studio 17 2022" -A x64