#!/bin/bash

# Exit on error
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/../common.sh"

parse_skip_prepare "$@"
set -- "${POSITIONAL[@]}"

TARGET_DIR="$1"
LIBPNG_VERSION="$2"
ZLIB_DIR="$3"

# Convert TARGET_DIR to absolute path
TARGET_DIR="$(cd "$(dirname "$TARGET_DIR")" && pwd)/$(basename "$TARGET_DIR")"

# Define directories
BUILD_DIR="$TARGET_DIR/build"
DOWNLOAD_DIR="$TARGET_DIR/download"
INSTALL_DIR="$TARGET_DIR/install"

function check() {
    if [ -z "$TARGET_DIR" ]; then
        echo "Error: TARGET_DIR argument not provided."
        echo "Usage: $0 <TARGET_DIR> <LIBPNG_VERSION>"
        exit 1
    fi

    if [ -z "$LIBPNG_VERSION" ]; then
        echo "Error: LIBPNG_VERSION argument not provided."
        echo "Usage: $0 <TARGET_DIR> <LIBPNG_VERSION> <ZLIB_DIR>"
        exit 1
    fi

    # Unlike freetype's bzip2/brotli (optional, pkg-config-detected), libpng
    # hard-requires zlib.h at configure/build time. The Android build gets
    # this for free from the NDK sysroot's own bundled zlib; iOS has no such
    # system zlib available for a static cross build, so it must be passed
    # in explicitly.
    if [ -z "$ZLIB_DIR" ]; then
        echo "Error: ZLIB_DIR argument not provided."
        echo "Usage: $0 <TARGET_DIR> <LIBPNG_VERSION> <ZLIB_DIR>"
        exit 1
    elif [ ! -d "$ZLIB_DIR" ]; then
        echo "Error: ZLIB_DIR directory not found at $ZLIB_DIR"
        exit 1
    fi
}

function prepare() {
    mkdir -p "$BUILD_DIR" "$DOWNLOAD_DIR"

    if [ ! -f "$DOWNLOAD_DIR/libpng-$LIBPNG_VERSION.tar.gz" ]; then
        echo "Downloading libpng..."
        curl -L "https://downloads.sourceforge.net/project/libpng/libpng16/$LIBPNG_VERSION/libpng-$LIBPNG_VERSION.tar.gz" -o "$DOWNLOAD_DIR/libpng-$LIBPNG_VERSION.tar.gz"
    fi

    if [ ! -d "$BUILD_DIR/libpng-$LIBPNG_VERSION" ]; then
        echo "Extracting libpng..."
        tar xzf "$DOWNLOAD_DIR/libpng-$LIBPNG_VERSION.tar.gz" -C "$BUILD_DIR"
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

        "$BUILD_DIR/libpng-$LIBPNG_VERSION/configure" \
            --host="$HOST" \
            --prefix="$INSTALL_DIR/$SLICE" \
            --enable-static \
            --disable-shared \
            --with-pic \
            CC="$CC" \
            CXX="$CXX" \
            AR="$AR" \
            RANLIB="$RANLIB" \
            STRIP="$STRIP" \
            CPPFLAGS="$CFLAGS_ARCH -I$ZLIB_DIR/$SLICE/include" \
            CFLAGS="$CFLAGS_ARCH -O3 -fvisibility=hidden -DPNG_INTEL_SSE_OPT=0" \
            CXXFLAGS="$CFLAGS_ARCH -O3 -fvisibility=hidden -DPNG_INTEL_SSE_OPT=0" \
            LDFLAGS="$CFLAGS_ARCH -L$ZLIB_DIR/$SLICE/lib"

        make clean
        make libpng16.la
        make install-libLTLIBRARIES install-data-am

        # Create symlink for libpng.a
        cd "$INSTALL_DIR/$SLICE/lib"
        ln -sf libpng16.a libpng.a

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
