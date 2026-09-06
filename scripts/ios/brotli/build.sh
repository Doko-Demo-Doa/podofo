#!/bin/bash

# Exit on error
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/../common.sh"

parse_skip_prepare "$@"
set -- "${POSITIONAL[@]}"

TARGET_DIR="$1"
BROTLI_VERSION="$2"

# Convert TARGET_DIR to absolute path
TARGET_DIR="$(cd "$(dirname "$TARGET_DIR")" && pwd)/$(basename "$TARGET_DIR")"

# Define directories
BUILD_DIR="$TARGET_DIR/build"
DOWNLOAD_DIR="$TARGET_DIR/download"
INSTALL_DIR="$TARGET_DIR/install"

function check() {
    if [ -z "$TARGET_DIR" ]; then
        echo "Error: TARGET_DIR argument not provided."
        echo "Usage: $0 <TARGET_DIR> <BROTLI_VERSION>"
        exit 1
    fi

    if [ -z "$BROTLI_VERSION" ]; then
        echo "Error: BROTLI_VERSION argument not provided."
        echo "Usage: $0 <TARGET_DIR> <BROTLI_VERSION>"
        exit 1
    fi
}

function prepare() {
    mkdir -p "$BUILD_DIR" "$DOWNLOAD_DIR"

    download_verified "https://github.com/google/brotli/archive/refs/tags/v$BROTLI_VERSION.tar.gz" \
        "$DOWNLOAD_DIR/brotli-$BROTLI_VERSION.tar.gz"

    if [ ! -d "$BUILD_DIR/brotli-$BROTLI_VERSION" ]; then
        echo "Extracting brotli..."
        tar xzf "$DOWNLOAD_DIR/brotli-$BROTLI_VERSION.tar.gz" -C "$BUILD_DIR"
    fi
}

function build() {
    mkdir -p "$BUILD_DIR" "$INSTALL_DIR"

    for SLICE in "${IOS_SLICES[@]}"; do
        echo "Building for $SLICE..."

        ios_slice_env "$SLICE"

        ARCH_BUILD_DIR="$BUILD_DIR/$SLICE"
        mkdir -p "$ARCH_BUILD_DIR/out"
        cd "$ARCH_BUILD_DIR/out"

        CMAKE_CCACHE_ARGS=()
        if [ -n "$CCACHE_BIN" ]; then
            CMAKE_CCACHE_ARGS=(-DCMAKE_C_COMPILER_LAUNCHER="$CCACHE_BIN" -DCMAKE_CXX_COMPILER_LAUNCHER="$CCACHE_BIN")
        fi

        cmake "$BUILD_DIR/brotli-$BROTLI_VERSION" \
            -DCMAKE_TOOLCHAIN_FILE="$SCRIPT_DIR/../ios.toolchain.cmake" \
            -DPLATFORM="$IOS_CMAKE_PLATFORM" \
            -DDEPLOYMENT_TARGET="$MIN_IOS_VERSION" \
            -DENABLE_BITCODE=OFF \
            -DENABLE_ARC=ON \
            -DCMAKE_BUILD_TYPE=Release \
            -DBUILD_SHARED_LIBS=OFF \
            -DBROTLI_BUNDLED_MODE=ON \
            -DBROTLI_DISABLE_TESTS=ON \
            -DCMAKE_POLICY_DEFAULT_CMP0057=NEW \
            "${CMAKE_CCACHE_ARGS[@]}"

        make -j"$NPROC"

        mkdir -p "$INSTALL_DIR/$SLICE/lib" "$INSTALL_DIR/$SLICE/include"
        cp libbrotli*.a "$INSTALL_DIR/$SLICE/lib/"
        cp -r "$BUILD_DIR/brotli-$BROTLI_VERSION/c/include/"* "$INSTALL_DIR/$SLICE/include/"

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
