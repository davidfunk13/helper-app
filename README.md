# dave.

A cross-platform cryptographic helper application built with Qt6 and C++. **dave.** provides three essential tools for security analysis and development:

## Features

### 🔓 Decoder Tool
- **Base64** decoding
- **Hexadecimal** decoding  
- **ROT/Caesar cipher** decoding with adjustable shift values (1-25)

### 🔍 JavaScript Unpacker
- **Dean Edwards packer** deobfuscation
- **Hex/Unicode escape** sequence decoding
- **String.fromCharCode** unpacking
- **JavaScript beautification** with proper formatting

### 🌐 Curl Request Builder
- Visual HTTP request builder with live curl command generation
- Support for all HTTP methods (GET, POST, PUT, DELETE, PATCH, HEAD, OPTIONS)
- Header management with autocomplete suggestions
- JSON body formatting and validation
- Advanced options: verbose levels, redirects, SSL verification
- One-click copy to clipboard

## Screenshots

The application features a clean, intuitive interface with:
- Home screen with tool selection
- Dedicated screens for each tool
- Real-time processing and output
- Copy-to-clipboard functionality

## Prerequisites

### All Platforms
- **Qt6** (6.0 or later)
- **CMake** (3.16 or later)
- **C++17** compatible compiler

### Platform-Specific Requirements

#### macOS
- **Xcode Command Line Tools**: `xcode-select --install`
- **Qt6**: Download from [Qt.io](https://www.qt.io/download) or install via Homebrew: `brew install qt6`

#### Linux (Debian/Ubuntu/Tails/Parrot)
```bash
sudo apt update
sudo apt install build-essential cmake qt6-base-dev qt6-tools-dev
```

#### Linux (Kali)
```bash
sudo apt update
sudo apt install build-essential cmake qt6-base-dev qt6-tools-dev
# Kali often has Qt6 in repositories
```

#### Linux (Arch/BlackArch)
```bash
sudo pacman -S base-devel cmake qt6-base qt6-tools
```

#### Linux (Fedora Security Lab)
```bash
sudo dnf install gcc-c++ cmake qt6-qtbase-devel qt6-qttools-devel
```

#### Windows
- **Visual Studio 2019/2022** with C++ support
- **Qt6**: Download from [Qt.io](https://www.qt.io/download)
- **CMake**: Download from [cmake.org](https://cmake.org/download/)

## Building from Source

### 1. Clone the Repository
```bash
git clone <repository-url>
cd helper-app
```

### 2. Configure Build

#### macOS (Qt Installer)
```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/Users/$USER/Qt/6.9.1/macos ..
```

#### macOS (Homebrew)
```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/opt/homebrew/opt/qt6 ..
```

#### Linux
```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
```

#### Windows (Qt Installer)
```cmd
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:\Qt\6.9.1\msvc2022_64" ..
```

### 3. Build the Application
```bash
cmake --build build --config Release
```

### Build Types

CMake supports different build configurations for different use cases:

#### Release (Production)
- **Optimization**: Maximum (`-O3`)
- **Debug symbols**: None
- **Size**: Smallest optimized executable
- **Speed**: Fastest runtime
- **Use case**: Production deployment, distribution

```bash
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=<qt-path> ..
cmake --build build --config Release
```

#### Debug (Development)
- **Optimization**: None (`-O0`)
- **Debug symbols**: Full (`-g`)
- **Size**: Larger executable
- **Speed**: Slower runtime
- **Use case**: Development, debugging, stepping through code

```bash
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=<qt-path> ..
cmake --build build --config Debug
```

#### RelWithDebInfo (Production Debugging)
- **Optimization**: Medium (`-O2`)
- **Debug symbols**: Full (`-g`)
- **Speed**: Fast but debuggable
- **Use case**: Production debugging, profiling

```bash
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_PREFIX_PATH=<qt-path> ..
cmake --build build --config RelWithDebInfo
```

#### MinSizeRel (Embedded/Minimal)
- **Optimization**: Size (`-Os`)
- **Debug symbols**: None
- **Size**: Smallest possible executable
- **Use case**: Embedded systems, minimal deployments

```bash
cmake -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_PREFIX_PATH=<qt-path> ..
cmake --build build --config MinSizeRel
```

### Quick Build Commands

For convenience, here are complete build sequences for each type:

#### macOS (Qt Installer)
```bash
# Release (for distribution/production)
rm -rf build && mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/Users/$USER/Qt/6.9.1/macos ..
cmake --build . --config Release

# Debug (for development)
rm -rf build && mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=/Users/$USER/Qt/6.9.1/macos ..
cmake --build . --config Debug

# RelWithDebInfo (fast but debuggable)
rm -rf build && mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_PREFIX_PATH=/Users/$USER/Qt/6.9.1/macos ..
cmake --build . --config RelWithDebInfo

# MinSizeRel (smallest size)
rm -rf build && mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_PREFIX_PATH=/Users/$USER/Qt/6.9.1/macos ..
cmake --build . --config MinSizeRel
```

#### Linux
```bash
# Release
rm -rf build && mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release

# Debug
rm -rf build && mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . --config Debug

# RelWithDebInfo
rm -rf build && mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
cmake --build . --config RelWithDebInfo

# MinSizeRel
rm -rf build && mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=MinSizeRel ..
cmake --build . --config MinSizeRel
```

#### Windows
```cmd
# Release
rmdir /s build & mkdir build & cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:\Qt\6.9.1\msvc2022_64" ..
cmake --build . --config Release

# Debug
rmdir /s build & mkdir build & cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH="C:\Qt\6.9.1\msvc2022_64" ..
cmake --build . --config Debug

# RelWithDebInfo
rmdir /s build & mkdir build & cd build
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_PREFIX_PATH="C:\Qt\6.9.1\msvc2022_64" ..
cmake --build . --config RelWithDebInfo

# MinSizeRel
rmdir /s build & mkdir build & cd build
cmake -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_PREFIX_PATH="C:\Qt\6.9.1\msvc2022_64" ..
cmake --build . --config MinSizeRel
```

### 4. Run the Application

#### macOS
```bash
./build/dave.app/Contents/MacOS/dave
```

#### Linux
```bash
./build/dave
```

#### Windows
```cmd
.\build\dave.exe
```

## Project Structure

```
helper-app/
├── src/
│   ├── main.cpp           # Main UI application
│   ├── decoder.h/.cpp     # Base64/Hex/ROT decoding logic
│   ├── unpacker.h/.cpp    # JavaScript deobfuscation logic
│   └── curl_builder.h/.cpp # Curl command generation logic
├── CMakeLists.txt         # Build configuration
├── .gitignore            # Git ignore patterns
└── README.md             # This file
```

## Development

### IDE Setup (VS Code)
The project includes VS Code configuration for IntelliSense support. The `.vscode/c_cpp_properties.json` file is configured for macOS with Qt installer paths.

For other platforms, update the `includePath` in `.vscode/c_cpp_properties.json`:

**Linux:**
```json
"includePath": [
    "${workspaceFolder}/**",
    "/usr/include/qt6",
    "/usr/include/qt6/QtCore",
    "/usr/include/qt6/QtWidgets",
    "/usr/include/qt6/QtGui"
]
```

**Windows:**
```json
"includePath": [
    "${workspaceFolder}/**",
    "C:/Qt/6.9.1/msvc2022_64/include",
    "C:/Qt/6.9.1/msvc2022_64/include/QtCore",
    "C:/Qt/6.9.1/msvc2022_64/include/QtWidgets",
    "C:/Qt/6.9.1/msvc2022_64/include/QtGui"
]
```

### Testing
The modular architecture separates business logic from UI components, making unit testing straightforward. Each module (`Decoder`, `Unpacker`, `CurlBuilder`) provides static methods that can be tested independently.

## Security Notice

This tool is designed for **defensive security purposes only**:
- Analyzing encoded/obfuscated content
- Security research and education
- Penetration testing and vulnerability assessment
- Reverse engineering for defensive purposes

## License

[Add your license information here]

## Contributing

[Add contribution guidelines here]

## Support

For issues and bug reports, please [create an issue](link-to-issues) in the project repository.
