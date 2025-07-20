@echo off
REM Windows Build Script for Dave Security Helper

setlocal enabledelayedexpansion

REM Color support for Windows
set "GREEN=[92m"
set "RED=[91m"
set "BLUE=[94m"
set "YELLOW=[93m"
set "RESET=[0m"

REM Parse command line arguments
set "BUILD_TYPE="
set "ACTION="

if "%~1"=="dev" set "BUILD_TYPE=dev"
if "%~1"=="--dev" set "BUILD_TYPE=dev"
if "%~1"=="release" set "BUILD_TYPE=release"
if "%~1"=="--release" set "BUILD_TYPE=release"
if "%~1"=="analysis" set "BUILD_TYPE=analysis"
if "%~1"=="--analysis" set "BUILD_TYPE=analysis"
if "%~1"=="clean" set "ACTION=clean"
if "%~1"=="--clean" set "ACTION=clean"

if "%~2"=="app" set "ACTION=app"
if "%~2"=="installer" set "ACTION=installer"
if "%~2"=="test" set "ACTION=test"
if "%~2"=="package" set "ACTION=package"

REM Handle clean command
if "%ACTION%"=="clean" (
    echo %BLUE%🧹 Cleaning build directories...%RESET%
    if exist build rmdir /s /q build
    if exist build-release rmdir /s /q build-release
    if exist build-analysis rmdir /s /q build-analysis
    echo %GREEN%✅ Cleaned%RESET%
    exit /b 0
)

REM Default to dev if no build type specified
if "%BUILD_TYPE%"=="" set "BUILD_TYPE=dev"

REM Set build directory based on type
if "%BUILD_TYPE%"=="dev" set "BUILD_DIR=build"
if "%BUILD_TYPE%"=="release" set "BUILD_DIR=build-release"
if "%BUILD_TYPE%"=="analysis" set "BUILD_DIR=build-analysis"

REM Check if configuration exists
if not exist "%BUILD_DIR%" (
    echo %RED%❌ Build not configured! Run configure-windows.bat %BUILD_TYPE% first%RESET%
    exit /b 1
)

echo %BLUE%🔨 Building %BUILD_TYPE% configuration...%RESET%

REM Build the project
cmake --build %BUILD_DIR% --config %BUILD_TYPE% --parallel

if %ERRORLEVEL% neq 0 (
    echo %RED%❌ Build failed!%RESET%
    exit /b 1
)

echo %GREEN%✅ Build completed successfully!%RESET%

REM Handle post-build actions
if "%ACTION%"=="test" (
    echo %BLUE%🧪 Running tests...%RESET%
    cd %BUILD_DIR%
    ctest --output-on-failure
    cd ..
)

if "%ACTION%"=="installer" (
    echo %BLUE%📦 Creating installer...%RESET%
    cd %BUILD_DIR%
    cpack
    cd ..
    echo %GREEN%✅ Installer created in %BUILD_DIR%/%RESET%
)

if "%ACTION%"=="package" (
    echo %BLUE%📦 Creating package...%RESET%
    cd %BUILD_DIR%
    cpack
    cd ..
    echo %GREEN%✅ Package created in %BUILD_DIR%/%RESET%
)

if "%ACTION%"=="app" (
    echo %BLUE%🚀 Running application...%RESET%
    if "%BUILD_TYPE%"=="release" (
        %BUILD_DIR%\Release\dave.exe
    ) else (
        %BUILD_DIR%\Debug\dave.exe
    )
)

REM Default action: show what was built
if "%ACTION%"=="" (
    echo.
    echo %GREEN%📁 Build artifacts in: %BUILD_DIR%\%RESET%
    echo %YELLOW%💡 Next steps:%RESET%
    echo   • Run app: %0 %BUILD_TYPE% app
    echo   • Run tests: %0 %BUILD_TYPE% test
    echo   • Create installer: %0 %BUILD_TYPE% installer
)
