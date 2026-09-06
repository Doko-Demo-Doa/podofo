#!/bin/bash

# Exit on error
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/../common.sh"

parse_skip_prepare "$@"
set -- "${POSITIONAL[@]}"

TARGET_DIR="$1"
BZIP2_VERSION="$2"

# Convert TARGET_DIR to absolute path
TARGET_DIR="$(cd "$(dirname "$TARGET_DIR")" && pwd)/$(basename "$TARGET_DIR")"

# Define directories
BUILD_DIR="$TARGET_DIR/build"
DOWNLOAD_DIR="$TARGET_DIR/download"
INSTALL_DIR="$TARGET_DIR/install"

function check() {
    if [ -z "$TARGET_DIR" ]; then
        echo "Error: TARGET_DIR argument not provided."
        echo "Usage: $0 <TARGET_DIR> <BZIP2_VERSION>"
        exit 1
    fi

    if [ -z "$BZIP2_VERSION" ]; then
        echo "Error: BZIP2_VERSION argument not provided."
        echo "Usage: $0 <TARGET_DIR> <BZIP2_VERSION>"
        exit 1
    fi
}

function prepare() {
    mkdir -p "$BUILD_DIR" "$DOWNLOAD_DIR"

    download_verified "https://sourceware.org/pub/bzip2/bzip2-$BZIP2_VERSION.tar.gz" \
        "$DOWNLOAD_DIR/bzip2-$BZIP2_VERSION.tar.gz"

    if [ ! -d "$BUILD_DIR/bzip2-$BZIP2_VERSION" ]; then
        echo "Extracting bzip2..."
        tar xzf "$DOWNLOAD_DIR/bzip2-$BZIP2_VERSION.tar.gz" -C "$BUILD_DIR"
    fi
}

function build() {
    mkdir -p "$BUILD_DIR" "$INSTALL_DIR"

    for SLICE in "${IOS_SLICES[@]}"; do
        echo "Building for $SLICE..."

        ios_slice_env "$SLICE"

        ARCH_BUILD_DIR="$BUILD_DIR/$SLICE"
        mkdir -p "$ARCH_BUILD_DIR"
        cd "$ARCH_BUILD_DIR"

        # Copy source files
        cp -r "$BUILD_DIR/bzip2-$BZIP2_VERSION"/* .

        # Modify Makefile for cross-compilation
        sed_i 's/CC=gcc/CC=$(CC)/' Makefile
        sed_i 's/CFLAGS=-Wall -Winline -O2 -g/CFLAGS=-Wall -Winline -O2 -g -fPIC/' Makefile
        sed_i 's/AR=ar/AR=$(AR)/' Makefile
        sed_i 's/RANLIB=ranlib/RANLIB=$(RANLIB)/' Makefile

        make clean
        make libbz2.a CC="$CC" AR="$AR" RANLIB="$RANLIB" CFLAGS="$CFLAGS_ARCH -O3 -fPIC"

        mkdir -p "$INSTALL_DIR/$SLICE/lib" "$INSTALL_DIR/$SLICE/include"
        cp libbz2.a "$INSTALL_DIR/$SLICE/lib/"
        cp bzlib.h "$INSTALL_DIR/$SLICE/include/"

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
