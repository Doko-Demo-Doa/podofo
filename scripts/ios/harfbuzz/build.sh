#!/bin/bash

# Exit on error
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/../common.sh"

parse_skip_prepare "$@"
set -- "${POSITIONAL[@]}"

TARGET_DIR="$1"
HARFBUZZ_VERSION="$2"

# Convert TARGET_DIR to absolute path
TARGET_DIR="$(cd "$(dirname "$TARGET_DIR")" && pwd)/$(basename "$TARGET_DIR")"

# Define directories
BUILD_DIR="$TARGET_DIR/build"
DOWNLOAD_DIR="$TARGET_DIR/download"
INSTALL_DIR="$TARGET_DIR/install"

function check() {
    if [ -z "$TARGET_DIR" ]; then
        echo "Error: TARGET_DIR argument not provided."
        echo "Usage: $0 <TARGET_DIR> <HARFBUZZ_VERSION>"
        exit 1
    fi

    if [ -z "$HARFBUZZ_VERSION" ]; then
        echo "Error: HARFBUZZ_VERSION argument not provided."
        echo "Usage: $0 <TARGET_DIR> <HARFBUZZ_VERSION>"
        exit 1
    fi
}

function prepare() {
    mkdir -p "$BUILD_DIR" "$DOWNLOAD_DIR"

    if [ ! -d "$BUILD_DIR/harfbuzz" ]; then
        echo "Cloning Harfbuzz..."
        git clone --branch "$HARFBUZZ_VERSION" --depth 1 https://github.com/harfbuzz/harfbuzz.git "$BUILD_DIR/harfbuzz"
    fi
}

function build() {
    mkdir -p "$BUILD_DIR" "$INSTALL_DIR"

    CMAKE_CCACHE_ARGS=()
    if [ -n "$CCACHE_BIN" ]; then
        CMAKE_CCACHE_ARGS=(-DCMAKE_C_COMPILER_LAUNCHER="$CCACHE_BIN" -DCMAKE_CXX_COMPILER_LAUNCHER="$CCACHE_BIN")
    fi

    for SLICE in "${IOS_SLICES[@]}"; do
        echo "Building for $SLICE..."

        ios_slice_env "$SLICE"

        ARCH_BUILD_DIR="$BUILD_DIR/harfbuzz-$SLICE"
        mkdir -p "$ARCH_BUILD_DIR"
        cd "$ARCH_BUILD_DIR"

        cmake "$BUILD_DIR/harfbuzz" \
            -DCMAKE_TOOLCHAIN_FILE="$SCRIPT_DIR/../ios.toolchain.cmake" \
            -DPLATFORM="$IOS_CMAKE_PLATFORM" \
            -DDEPLOYMENT_TARGET="$MIN_IOS_VERSION" \
            -DENABLE_BITCODE=OFF \
            -DENABLE_ARC=ON \
            -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR/$SLICE" \
            -DHB_HAVE_FREETYPE=OFF \
            -DHB_HAVE_GLIB=OFF \
            -DHB_HAVE_ICU=OFF \
            -DHB_BUILD_TESTS=OFF \
            -DHB_BUILD_UTILS=OFF \
            -DHB_BUILD_SUBSET=OFF \
            "${CMAKE_CCACHE_ARGS[@]}"

        make -j"$NPROC"
        make install

        echo "Build completed for $SLICE"
    done

    echo "Build completed successfully!"
    echo "Libraries are installed in: $INSTALL_DIR"
}

check

if [ "$SKIP_PREPARE" -eq 0 ]; then
    prepare
fi

build
