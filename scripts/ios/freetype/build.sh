#!/bin/bash

# Exit on error
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/../common.sh"

parse_skip_prepare "$@"
set -- "${POSITIONAL[@]}"

TARGET_DIR="$1"
FREETYPE_VERSION="$2"
BZIP2_DIR="$3"
BROTLI_DIR="$4"

# Convert TARGET_DIR to absolute path
TARGET_DIR="$(cd "$(dirname "$TARGET_DIR")" && pwd)/$(basename "$TARGET_DIR")"

# Define directories
BUILD_DIR="$TARGET_DIR/build"
DOWNLOAD_DIR="$TARGET_DIR/download"
INSTALL_DIR="$TARGET_DIR/install"

function check() {
    if [ -z "$TARGET_DIR" ]; then
        echo "Error: TARGET_DIR argument not provided."
        echo "Usage: $0 <TARGET_DIR> <FREETYPE_VERSION> <BZIP2_DIR> <BROTLI_DIR>"
        exit 1
    fi

    if [ -z "$FREETYPE_VERSION" ]; then
        echo "Error: FREETYPE_VERSION argument not provided."
        echo "Usage: $0 <TARGET_DIR> <FREETYPE_VERSION> <BZIP2_DIR> <BROTLI_DIR>"
        exit 1
    fi

    if [ -z "$BZIP2_DIR" ]; then
        echo "Error: BZIP2_DIR argument not provided."
        echo "Usage: $0 <TARGET_DIR> <FREETYPE_VERSION> <BZIP2_DIR> <BROTLI_DIR>"
        exit 1
    elif [ ! -d "$BZIP2_DIR" ]; then
        echo "Error: BZIP2_DIR directory not found at $BZIP2_DIR"
        exit 1
    fi

    if [ -z "$BROTLI_DIR" ]; then
        echo "Error: BROTLI_DIR argument not provided."
        echo "Usage: $0 <TARGET_DIR> <FREETYPE_VERSION> <BZIP2_DIR> <BROTLI_DIR>"
        exit 1
    elif [ ! -d "$BROTLI_DIR" ]; then
        echo "Error: BROTLI_DIR directory not found at $BROTLI_DIR"
        exit 1
    fi
}

# GitHub mirror: download.savannah.gnu.org was unresponsive (502/504).
FREETYPE_TAG="VER-$(echo "$FREETYPE_VERSION" | tr '.' '-')"

# subprojects/dlg is a git submodule a GitHub tag archive doesn't carry, and
# freetype always compiles it in regardless of FT_DEBUG_LOGGING. Pin the
# exact commit freetype's .gitmodules points to for $FREETYPE_TAG and fetch
# its 3 needed files directly instead.
DLG_COMMIT="0f5aa8d7b67d2c6b543fcdbe099bac18842caa09"

function prepare() {
    mkdir -p "$BUILD_DIR" "$DOWNLOAD_DIR"

    download_verified "https://github.com/freetype/freetype/archive/refs/tags/${FREETYPE_TAG}.tar.gz" \
        "$DOWNLOAD_DIR/freetype-$FREETYPE_VERSION.tar.gz"

    if [ ! -d "$BUILD_DIR/freetype-$FREETYPE_VERSION" ]; then
        echo "Extracting freetype..."
        tar xzf "$DOWNLOAD_DIR/freetype-$FREETYPE_VERSION.tar.gz" -C "$BUILD_DIR"
        mv "$BUILD_DIR/freetype-${FREETYPE_TAG}" "$BUILD_DIR/freetype-$FREETYPE_VERSION"

        mkdir -p "$BUILD_DIR/freetype-$FREETYPE_VERSION/include/dlg" "$BUILD_DIR/freetype-$FREETYPE_VERSION/src/dlg"
        for f in include/dlg/dlg.h include/dlg/output.h src/dlg/dlg.c; do
            curl -fL --retry 3 --retry-delay 5 --retry-all-errors \
                -o "$BUILD_DIR/freetype-$FREETYPE_VERSION/$f" \
                "https://raw.githubusercontent.com/nyorain/dlg/$DLG_COMMIT/$f"
        done
    fi
}

function build() {
    mkdir -p "$BUILD_DIR" "$INSTALL_DIR"

    # A GitHub tag archive lacks the autotools-generated builds/unix/configure.
    if [ ! -f "$BUILD_DIR/freetype-$FREETYPE_VERSION/builds/unix/configure" ]; then
        echo "Generating freetype configure script..."
        (cd "$BUILD_DIR/freetype-$FREETYPE_VERSION" && ./autogen.sh)
    fi

    for SLICE in "${IOS_SLICES[@]}"; do
        echo "Building for $SLICE..."

        ios_slice_env "$SLICE"

        ARCH_BUILD_DIR="$BUILD_DIR/$SLICE"
        mkdir -p "$ARCH_BUILD_DIR"
        cd "$ARCH_BUILD_DIR"

        # NOTE: BZIP2_DIR/BROTLI_DIR are install roots (containing one subdir
        # per slice, eg. "$BZIP2_DIR/$SLICE/include") produced by the
        # bzip2/brotli build.sh scripts — indexed by $SLICE here, since each
        # slice has its own cross-compiled bzip2/brotli static libs.
        export CC
        export CXX
        export AR
        export RANLIB
        export STRIP
        export CFLAGS="$CFLAGS_ARCH -O3 -fvisibility=hidden -I$BZIP2_DIR/$SLICE/include -I$BROTLI_DIR/$SLICE/include"
        export CXXFLAGS="$CFLAGS"
        export LDFLAGS="$CFLAGS_ARCH -L$BZIP2_DIR/$SLICE/lib -L$BROTLI_DIR/$SLICE/lib"

        "$BUILD_DIR/freetype-$FREETYPE_VERSION/configure" \
            --host="$HOST" \
            --prefix="$INSTALL_DIR/$SLICE" \
            --enable-static \
            --disable-shared \
            --with-pic \
            --with-bzip2="$BZIP2_DIR/$SLICE"

        make clean
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
