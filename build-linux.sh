#!/bin/bash

# Build script for Linux
# Usage: ./build-linux.sh [dev|release|analysis|clean] [deb|rpm|appimage|app]

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
        deb|rpm|appimage)
            BUILD_TYPE="release"
            TARGET="$1"
            ;;
        *)
            echo "Usage: $0 [dev|release|analysis|clean] [deb|rpm|appimage|app]"
            echo ""
            echo "Build Types:"
            echo "  dev      - Debug build with tests and sanitizers"
            echo "  release  - Optimized build for distribution"
            echo "  analysis - Debug build with static analysis tools"
            echo "  clean    - Remove all build directories"
            echo ""
            echo "Package Types:"
            echo "  app      - Build the application only (default)"
            echo "  deb      - Build and create Debian package"
            echo "  rpm      - Build and create RPM package"
            echo "  appimage - Build and create AppImage"
            exit 1
            ;;
    esac
fi

# Parse second argument
if [ $# -ge 2 ]; then
    case "$2" in
        deb|rpm|appimage|app)
            TARGET="$2"
            ;;
    esac
fi

# Set build directory
BUILD_DIR="build-${BUILD_TYPE}"

# Check if configuration exists
if [ ! -d "$BUILD_DIR" ]; then
    print_warning "Build directory doesn't exist. Running configuration first..."
    ./configure-linux.sh "$BUILD_TYPE"
fi

print_header "🔨 Building $BUILD_TYPE configuration..."

# Build the project
cd "$BUILD_DIR"

# Get number of CPU cores
if command -v nproc > /dev/null; then
    CORES=$(nproc)
else
    CORES=4
fi

print_header "Building with $CORES cores..."
make -j"$CORES"

if [ "$TARGET" != "app" ]; then
    print_header "📦 Creating $TARGET package..."
    
    case "$TARGET" in
        deb)
            cpack -G DEB --config CPackConfig.cmake
            ;;
        rpm)
            cpack -G RPM --config CPackConfig.cmake
            ;;
        appimage)
            cpack -G External --config CPackConfig.cmake
            ;;
    esac
    
    PACKAGE_FILE=$(find . -name "dave-1.0.0-*.$TARGET" -o -name "dave-1.0.0-*.AppImage" | head -1)
    if [ -n "$PACKAGE_FILE" ]; then
        print_success "Package created: ./$BUILD_DIR/$(basename $PACKAGE_FILE)"
    else
        print_error "Failed to create $TARGET package"
        exit 1
    fi
else
    print_success "Build completed: ./$BUILD_DIR/dave"
fi

cd ..
print_success "Build finished successfully!"
