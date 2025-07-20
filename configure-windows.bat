@echo off
REM Windows Configuration Script

REM Find Qt installation
set QT_PATH=
for /d %%i in ("C:\Qt\*") do (
    if exist "%%i\msvc*\lib\cmake\Qt6\Qt6Config.cmake" (
        for /d %%j in ("%%i\msvc*") do set QT_PATH=%%j
    )
)

if "%QT_PATH%"=="" (
    echo ❌ Qt6 not found! Install from:
    echo   • https://www.qt.io/download
    echo   • Select MSVC compiler version
    exit /b 1
)

if "%1"=="dev" goto dev
if "%1"=="--dev" goto dev
if "%1"=="release" goto release
if "%1"=="--release" goto release
if "%1"=="clean" goto clean
if "%1"=="--clean" goto clean
goto usage

:dev
echo 🔧 Configuring DEBUG build...
cmake . -B build -DCMAKE_PREFIX_PATH="%QT_PATH%" -DCMAKE_BUILD_TYPE=Debug
echo ✅ Run: build-windows.bat dev
goto end

:release
echo 🚀 Configuring RELEASE build...
cmake . -B build-release -DCMAKE_PREFIX_PATH="%QT_PATH%" -DCMAKE_BUILD_TYPE=Release
echo ✅ Run: build-windows.bat release
goto end

:clean
rd /s /q build build-release 2>nul
echo ✅ Cleaned
goto end

:usage
echo Usage: %0 [dev^|release^|clean]

:end