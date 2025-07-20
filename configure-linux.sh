#!/bin/bash
# Linux Configuration Script

set -e

# Find Qt installation
find_qt() {
    local qt_paths=(
        "$HOME/Qt/*/gcc_64"
        "/usr/lib/qt6"
        "/usr/lib/x86_64-linux-gnu/qt6"
        "/opt/Qt/*/gcc_64"
        "$(pkg-config --variable=prefix Qt6Core 2>/dev/null)"
    )
    
    for path in "${qt_paths[@]}"; do
        if [ -d "$path" ] && [ -f "$path/lib/cmake/Qt6/Qt6Config.cmake" ]; then
            echo "$path"
            return 0
        fi
    done
    return 1
}

QT_PATH=$(find_qt)
if [ -z "$QT_PATH" ]; then
    echo "❌ Qt6 not found! Install with:"
    echo "  • sudo apt install qt6-base-dev qt6-tools-dev (Ubuntu/Debian)"
    echo "  • sudo dnf install qt6-qtbase-devel qt6-qttools-devel (Fedora)"
    echo "  • Or download from https://www.qt.io/download"
    exit 1
fi

case "$1" in
    dev|--dev)
        echo "🔧 Configuring DEBUG build..."
        cmake . -B build -DCMAKE_PREFIX_PATH="$QT_PATH" -DCMAKE_BUILD_TYPE=Debug
        echo "✅ Run: ./build-linux.sh dev"
        ;;
    release|--release)
        echo "🚀 Configuring RELEASE build..."
        cmake . -B build-release -DCMAKE_PREFIX_PATH="$QT_PATH" -DCMAKE_BUILD_TYPE=Release
        echo "✅ Run: ./build-linux.sh release"
        ;;
    clean|--clean)
        rm -rf build build-release
        echo "✅ Cleaned"
        ;;
    *)
        echo "Usage: $0 [dev|release|clean]"
        ;;
esac