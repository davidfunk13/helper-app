# Dave - Security Helper Tool

A cryptographic helper tool for security analysis and penetration testing, built with Qt6 and C++.

## 🚀 Quick Start

### 1. Install Qt6 and CMake

**macOS:**
```bash
brew install qt6 cmake
```

**Windows:**
- Download Qt6 from https://www.qt.io/download
- Install CMake and Visual Studio 2019+

**Linux:**
```bash
# Ubuntu/Debian:
sudo apt install qt6-base-dev qt6-tools-dev cmake build-essential

# Fedora:
sudo dnf install qt6-qtbase-devel qt6-qttools-devel cmake gcc-c++
```

### 2. Build Your App

**Fresh start (no build directories exist):**

```bash
# macOS:
./configure-mac.sh dev      # Setup debug build
./build-mac.sh dev          # Build the app
./build/dave.app/Contents/MacOS/dave  # Run it

# Windows:
configure-windows.bat dev
build-windows.bat dev
.\build\dave.exe

# Linux:
./configure-linux.sh dev
./build-linux.sh dev
./build/dave
```

**For distribution:**
```bash
# macOS - Creates professional DMG:
./configure-mac.sh release
./build-mac.sh dmg         # → dave-1.0.0-Darwin.dmg

# Windows - Creates installer:
configure-windows.bat release
build-windows.bat installer

# Linux - Creates packages:
./configure-linux.sh release  
./build-linux.sh package  # → .deb, .rpm, .tar.gz
```

### 3. Build Options

| Command | Purpose | Features |
|---------|---------|----------|
| `dev` | Daily development | Debug + tests + fast builds |
| `release` | Distribution | Optimized + installers |
| `analysis` | Code quality | Static analysis + sanitizers |
| `clean` | Start fresh | Removes all build dirs |

## 📁 Project Structure

```
dave/
├── src/
│   ├── core/           # Core cryptographic functions
│   ├── ui/             # Qt GUI components
│   ├── tests/          # Unit tests
│   └── main.cpp        # Application entry point
├── configure-*.sh/bat  # Configuration scripts
├── build-*.sh/bat      # Build scripts
└── CMakeLists.txt      # Build configuration
```

## 🔧 Development Workflow

1. **First time setup:**
   ```bash
   ./configure-mac.sh dev  # (or windows/linux equivalent)
   ```

2. **Daily development:**
   ```bash
   ./build-mac.sh dev      # Quick builds for testing
   ```

3. **Before release:**
   ```bash
   ./configure-mac.sh release
   ./build-mac.sh dmg      # Creates professional installer
   ```

## 📦 What Gets Built

**macOS:** `dave.app` + professional DMG installer with drag-to-Applications
**Windows:** `dave.exe` + NSIS installer with Start Menu shortcuts  
**Linux:** `dave` binary + .deb/.rpm packages for easy installation

## 🧠 How It Works (Technical)

**The Problem:** Your app uses Qt libraries, but users don't have Qt installed.

**The Solution:** 
1. **macdeployqt/windeployqt/linuxdeploy** - Bundles Qt libraries into your app
2. **CPack** - Creates professional installers for distribution

**Build Process:**
1. CMake compiles your C++ code
2. Qt's deploy tool bundles dependencies 
3. CPack packages everything into installers

## 🆘 Troubleshooting

**"Qt not found"**
- Make sure Qt6 is installed
- Scripts auto-detect common Qt locations
- Manual path: Set `CMAKE_PREFIX_PATH` to your Qt installation

**"Build failed"**
- Run `./configure-*.sh clean` and try again
- Check you have CMake 3.16+ and C++17 compiler

**"App won't run"**
- Use debug build for development: `./build-*.sh dev`
- Release builds are for distribution only

## 📜 Commands Reference

| Task | macOS | Windows | Linux |
|------|-------|---------|-------|
| Setup dev | `./configure-mac.sh dev` | `configure-windows.bat dev` | `./configure-linux.sh dev` |
| Build dev | `./build-mac.sh dev` | `build-windows.bat dev` | `./build-linux.sh dev` |
| Setup release | `./configure-mac.sh release` | `configure-windows.bat release` | `./configure-linux.sh release` |
| Create installer | `./build-mac.sh dmg` | `build-windows.bat installer` | `./build-linux.sh package` |
| Run tests | `./build-mac.sh test` | `build-windows.bat test` | `./build-linux.sh test` |
| Clean all | `./configure-mac.sh clean` | `configure-windows.bat clean` | `./configure-linux.sh clean` |

---

**That's it!** No need to remember complex CMake commands. The scripts handle Qt detection, dependency bundling, and installer creation automatically.
