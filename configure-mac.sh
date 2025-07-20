#!/bin/bash
# macOS Configuration Script

set -e

# Find Qt installation - works with any installation method
find_qt() {
    echo "🔍 Searching for Qt6 installation..." >&2
    
    # Method 1: Check common Qt installer paths
    for qt_base in "$HOME/Qt"/*; do
        if [ -d "$qt_base" ]; then
            for kit in "$qt_base"/{macos,clang_64,gcc_64}; do
                if [ -f "$kit/lib/cmake/Qt6/Qt6Config.cmake" ]; then
                    echo "✅ Found Qt at: $kit" >&2
                    echo "$kit"
                    return 0
                fi
            done
        fi
    done
    
    # Method 2: Check Homebrew
    if command -v brew >/dev/null 2>&1; then
        local brew_qt=$(brew --prefix qt6 2>/dev/null || brew --prefix qt@6 2>/dev/null || brew --prefix qt 2>/dev/null)
        if [ -n "$brew_qt" ] && [ -f "$brew_qt/lib/cmake/Qt6/Qt6Config.cmake" ]; then
            echo "✅ Found Qt via Homebrew: $brew_qt" >&2
            echo "$brew_qt"
            return 0
        fi
    fi
    
    # Method 3: Check system paths
    for sys_path in /usr/local/qt6 /opt/qt6 /usr/lib/qt6; do
        if [ -f "$sys_path/lib/cmake/Qt6/Qt6Config.cmake" ]; then
            echo "✅ Found Qt at: $sys_path" >&2
            echo "$sys_path"
            return 0
        fi
    done
    
    # Method 4: Check if qt6-config exists
    if command -v qt6-config >/dev/null 2>&1; then
        local qt_prefix=$(qt6-config --prefix)
        if [ -f "$qt_prefix/lib/cmake/Qt6/Qt6Config.cmake" ]; then
            echo "✅ Found Qt via qt6-config: $qt_prefix" >&2
            echo "$qt_prefix"
            return 0
        fi
    fi
    
    return 1
}

# Get Qt path
echo "🔍 Looking for Qt6..."
qt_path=$(find_qt)

if [ -z "$qt_path" ]; then
    echo "❌ Qt6 not found! Install with:"
    echo "  • brew install qt@6"
    echo "  • Or download from https://www.qt.io/download"
    echo "  • Or install via Qt Online Installer"
    exit 1
fi

echo "✅ Using Qt6 at: $qt_path"

# Handle command line arguments
case "$1" in
    dev|--dev)
        echo "🔧 Configuring DEBUG build..."
        cmake . -B build-dev \
            -DCMAKE_PREFIX_PATH="$qt_path" \
            -DQt6_DIR="$qt_path/lib/cmake/Qt6" \
            -DCMAKE_BUILD_TYPE=Debug \
            -DBUILD_TESTS=ON \
            -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
        echo "✅ Run: ./build-mac.sh dev"
        ;;
    release|--release)
        echo "🚀 Configuring RELEASE build..."
        cmake . -B build-release \
            -DCMAKE_PREFIX_PATH="$qt_path" \
            -DQt6_DIR="$qt_path/lib/cmake/Qt6" \
            -DCMAKE_BUILD_TYPE=Release \
            -DBUILD_TESTS=OFF
        echo "✅ Run: ./build-mac.sh release"
        ;;
    analysis|--analysis)
        echo "🔬 Configuring ANALYSIS build..."
        cmake . -B build-analysis \
            -DCMAKE_PREFIX_PATH="$qt_path" \
            -DQt6_DIR="$qt_path/lib/cmake/Qt6" \
            -DCMAKE_BUILD_TYPE=Debug \
            -DBUILD_TESTS=ON \
            -DENABLE_CLANG_TIDY=ON \
            -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
        echo "✅ Run: ./build-mac.sh analysis"
        ;;
    clean|--clean)
        echo "🧹 Cleaning configuration directories..."
        rm -rf build-dev build-release build-analysis
        echo "✅ Cleaned"
        ;;
    *)
        echo "Usage: $0 [dev|release|analysis|clean]"
        echo ""
        echo "Build types:"
        echo "  dev      - Development build with tests and debug symbols"
        echo "  release  - Optimized release build"
        echo "  analysis - Debug build with static analysis enabled"
        echo "  clean    - Remove all build directories"
        ;;
esac
