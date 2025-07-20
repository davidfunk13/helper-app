# 🚀 First Time Setup Guide

This guide will walk you through setting up and building the Dave security helper tool from scratch.

## 📋 Prerequisites

### macOS Requirements
- **macOS 10.15+** (Catalina or newer)
- **Xcode Command Line Tools**: `xcode-select --install`
- **CMake 3.20+**: `brew install cmake`
- **Qt6**: Download from [Qt.io](https://www.qt.io/download) or install via Homebrew
  ```bash
  brew install qt@6
  ```

### Additional macOS Tools (Optional but Recommended)
- **Homebrew**: `/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"`
- **vcpkg** (for additional dependencies): Will be handled automatically by CMake

## 🏗️ Quick Start (5 minutes)

### 1. Clone and Enter Directory
```bash
git clone <your-repo-url>
cd helper-app
```

### 2. First Build (Debug/Development)
```bash
./configure-mac.sh dev
./build-mac.sh dev
```

### 3. Release Build with DMG
```bash
./configure-mac.sh release
./build-mac.sh dmg
```

## 📁 Build System Overview

### Configuration Scripts
- `./configure-mac.sh [dev|release|analysis]` - Configure build environment
- `./configure-linux.sh [dev|release|analysis]` - Linux configuration
- `./configure-windows.bat [dev|release|analysis]` - Windows configuration

### Build Scripts  
- `./build-mac.sh [dev|release|analysis|clean] [app|dmg]` - Build on macOS
- `./build-linux.sh [dev|release|analysis|clean] [app|deb|rpm|appimage]` - Build on Linux
- `./build-windows.bat [dev|release|analysis|clean] [app|installer]` - Build on Windows

### Build Types
- **dev** - Debug build with tests and sanitizers (fast compile, debug symbols)
- **release** - Optimized build for distribution (slower compile, optimized)
- **analysis** - Debug build with static analysis tools (clang-tidy, etc.)

## 🎯 Common Workflows

### Development Workflow
```bash
# Initial setup
./configure-mac.sh dev
./build-mac.sh dev

# After code changes
./build-mac.sh dev        # Quick rebuild
./build-mac.sh clean dev  # Clean rebuild if needed
```

### Release Workflow
```bash
# Build release version
./configure-mac.sh release
./build-mac.sh release

# Create professional DMG installer
./build-mac.sh dmg
```

### Testing Workflow  
```bash
# Run with sanitizers and debug info
./configure-mac.sh dev
./build-mac.sh dev

# Run the application
./build-dev/dave.app/Contents/MacOS/dave
```

## 📦 Output Locations

- **Development builds**: `./build-dev/dave.app`
- **Release builds**: `./build-release/dave.app`  
- **DMG installer**: `./build-release/dave-1.0.0-Darwin.dmg`
- **Analysis builds**: `./build-analysis/dave.app`

## 🔧 Advanced Configuration

### Custom Qt Installation
If Qt is installed in a custom location:
```bash
export Qt6_DIR="/path/to/Qt/6.x.x/macos/lib/cmake/Qt6"
./configure-mac.sh release
```

### Clean Everything
```bash
./build-mac.sh clean        # Remove all build directories
rm -rf vcpkg_installed/     # Remove vcpkg dependencies (optional)
```

### Build with Specific Features
The build system automatically detects and configures:
- Qt6 Widgets, Core, Gui, DBus, Svg
- vcpkg dependencies (curl, etc.)
- macOS code signing preparation
- DMG installer with background image and custom layout

## 🎨 DMG Installer Features

The release DMG includes:
- **Professional appearance** with custom background image
- **Large application icons** (128x128 pixels)
- **Drag-to-Applications layout** similar to Firefox installer
- **Automatic styling** applied during build process

## 🐛 Troubleshooting

### "Qt6 not found"
```bash
# Install Qt6 via Homebrew
brew install qt@6

# Or set custom path
export Qt6_DIR="/Users/$(whoami)/Qt/6.9.1/macos/lib/cmake/Qt6"
```

### "Command not found"
```bash
# Make scripts executable
chmod +x configure-mac.sh build-mac.sh
```

### "CMake version too old"  
```bash
# Update CMake
brew upgrade cmake
# or
pip install --upgrade cmake
```

### "vcpkg dependencies failing"
```bash
# Clean and reconfigure
./build-mac.sh clean
rm -rf vcpkg_installed/
./configure-mac.sh release
```

## 🚢 Ready to Ship Checklist

- [ ] `./build-mac.sh dmg` creates professional DMG
- [ ] DMG opens with background image and large icons
- [ ] Application launches without security warnings (after first right-click → Open)
- [ ] All core features (DECODER, UNPACKER, CURL BUILDER) work correctly
- [ ] No console errors or debug output in release build

## 📞 Need Help?

1. Check build output for specific error messages
2. Try `./build-mac.sh clean` followed by reconfiguration
3. Verify all prerequisites are installed
4. Check Qt installation path and version

---

**Next Steps**: After successful build, see `INSTALL.md` for deployment and distribution instructions.
