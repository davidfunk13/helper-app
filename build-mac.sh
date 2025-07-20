#!/bin/bash

# Build script for macOS
# Usage: ./build-mac.sh [dev|release|analysis|clean] [dmg|app]

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

print_header() {
    echo -e "${BLUE}$1${NC}"
}

print_success() {
    echo -e "${GREEN}✅ $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠️  $1${NC}"
}

print_error() {
    echo -e "${RED}❌ $1${NC}"
}

# Default values
BUILD_TYPE="release"
TARGET="app"

# Parse arguments
if [ $# -ge 1 ]; then
    case "$1" in
        dev|debug)
            BUILD_TYPE="dev"
            ;;
        release)
            BUILD_TYPE="release"
            ;;
        analysis)
            BUILD_TYPE="analysis"
            ;;
        clean)
            print_header "🧹 Cleaning all build directories..."
            rm -rf build-dev build-release build-analysis
            print_success "All build directories cleaned"
            exit 0
            ;;
        dmg)
            BUILD_TYPE="release"
            TARGET="dmg"
            ;;
        *)
            echo "Usage: $0 [dev|release|analysis|clean] [dmg|app]"
            echo ""
            echo "Build Types:"
            echo "  dev      - Debug build with tests and sanitizers"
            echo "  release  - Optimized build for distribution"
            echo "  analysis - Debug build with static analysis tools"
            echo "  clean    - Remove all build directories"
            echo ""
            echo "Targets:"
            echo "  app      - Build the application only (default)"
            echo "  dmg      - Build and create DMG installer"
            exit 1
            ;;
    esac
fi

# Parse second argument
if [ $# -ge 2 ]; then
    case "$2" in
        dmg)
            TARGET="dmg"
            ;;
        app)
            TARGET="app"
            ;;
    esac
fi

# Set build directory
BUILD_DIR="build-${BUILD_TYPE}"

# Check if configuration exists
if [ ! -d "$BUILD_DIR" ]; then
    print_warning "Build directory doesn't exist. Running configuration first..."
    
    # Configure the project
    case "$BUILD_TYPE" in
        dev)
            print_header "🔧 Configuring DEBUG build..."
            cmake . -B "$BUILD_DIR" \
                -DCMAKE_PREFIX_PATH="/Users/dave/Qt/6.9.1/macos" \
                -DQt6_DIR="/Users/dave/Qt/6.9.1/macos/lib/cmake/Qt6" \
                -DCMAKE_BUILD_TYPE=Debug \
                -DBUILD_TESTS=ON \
                -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
            ;;
        release)
            print_header "🚀 Configuring RELEASE build..."
            cmake . -B "$BUILD_DIR" \
                -DCMAKE_PREFIX_PATH="/Users/dave/Qt/6.9.1/macos" \
                -DQt6_DIR="/Users/dave/Qt/6.9.1/macos/lib/cmake/Qt6" \
                -DCMAKE_BUILD_TYPE=Release \
                -DBUILD_TESTS=OFF
            ;;
        analysis)
            print_header "🔍 Configuring ANALYSIS build..."
            cmake . -B "$BUILD_DIR" \
                -DCMAKE_PREFIX_PATH="/Users/dave/Qt/6.9.1/macos" \
                -DQt6_DIR="/Users/dave/Qt/6.9.1/macos/lib/cmake/Qt6" \
                -DCMAKE_BUILD_TYPE=Debug \
                -DBUILD_TESTS=ON \
                -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
                -DCMAKE_CXX_CLANG_TIDY="clang-tidy"
            ;;
    esac
fi

print_header "🔨 Building $BUILD_TYPE configuration..."

# Build the project
cd "$BUILD_DIR"

if command -v nproc > /dev/null; then
    CORES=$(nproc)
elif command -v sysctl > /dev/null; then
    CORES=$(sysctl -n hw.ncpu)
else
    CORES=4
fi

print_header "Building with $CORES cores..."
make -j"$CORES" || { print_error "Build failed"; exit 1; }

if [ "$TARGET" = "dmg" ]; then
    print_header "📦 Creating DMG installer..."
    
    # Use CPack to create the DMG
    cpack --config CPackConfig.cmake || { print_error "Failed to create DMG"; exit 1; }
    
    if [ -f "dave-1.0.0-Darwin.dmg" ]; then
        print_success "DMG created: ./$BUILD_DIR/dave-1.0.0-Darwin.dmg"
    else
        print_error "DMG file not found after CPack"
        exit 1
    fi
else
    print_success "Build completed: ./$BUILD_DIR/dave.app"
fi

cd ..
print_success "Build finished successfully!"
