#!/bin/bash

# Exit on error
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/../common.sh"

parse_skip_prepare "$@"
set -- "${POSITIONAL[@]}"

TARGET_DIR="$1"
FREETYPE_DIR="$2"
LIBPNG_DIR="$3"
LIBXML2_DIR="$4"
OPENSSL_DIR="$5"
TARGET_SLICE="$6"

# Convert TARGET_DIR and the dependency install dirs to absolute paths — same
# reasoning as the Android script: a relative dependency path would silently
# make find_package(OpenSSL/Freetype/...) skip creating the imported targets
# it needs, with no clear error until the final link.
TARGET_DIR="$(cd "$(dirname "$TARGET_DIR")" && pwd)/$(basename "$TARGET_DIR")"
FREETYPE_DIR="$(cd "$FREETYPE_DIR" && pwd)"
LIBPNG_DIR="$(cd "$LIBPNG_DIR" && pwd)"
LIBXML2_DIR="$(cd "$LIBXML2_DIR" && pwd)"
OPENSSL_DIR="$(cd "$OPENSSL_DIR" && pwd)"

# Define directories
BUILD_DIR="$TARGET_DIR/build"
DOWNLOAD_DIR="$TARGET_DIR/download"
INSTALL_DIR="$TARGET_DIR/install"

# The podofo source tree this build is running from (repo root, three levels
# up from this script's directory)
PODOFO_SRC_DIR="$(cd "$SCRIPT_DIR/../../.." && pwd)"

# If TARGET_SLICE is specified, only build for that slice
SLICES=("${IOS_SLICES[@]}")
if [ -n "$TARGET_SLICE" ]; then
    SLICES=("$TARGET_SLICE")
fi

function check() {
    if [ -z "$TARGET_DIR" ]; then
        echo "Error: TARGET_DIR argument not provided."
        echo "Usage: $0 <TARGET_DIR> <FREETYPE_DIR> <LIBPNG_DIR> <LIBXML2_DIR> <OPENSSL_DIR>"
        exit 1
    fi
}

function prepare() {
    # Clean up existing build directory
    rm -rf "$BUILD_DIR"
    mkdir -p "$BUILD_DIR"/podofo/

    # Exclude the build directory itself, and other unrelated build scratch,
    # from being copied
    rsync -a --exclude 'build' --exclude 'build-host' --exclude 'src/android' "$PODOFO_SRC_DIR"/ "$BUILD_DIR"/podofo/
}

function build() {
    mkdir -p "$BUILD_DIR" "$INSTALL_DIR"

    CCACHE_CMAKE_ARGS=()
    if [ -n "$CCACHE_BIN" ]; then
        CCACHE_CMAKE_ARGS=(-DCMAKE_C_COMPILER_LAUNCHER="$CCACHE_BIN" -DCMAKE_CXX_COMPILER_LAUNCHER="$CCACHE_BIN")
    fi

    echo "Configuring PoDoFo..."

    for SLICE in "${SLICES[@]}"; do
        echo "Building for $SLICE..."

        ios_slice_env "$SLICE"

        ARCH_BUILD_DIR="$BUILD_DIR/$SLICE"
        mkdir -p "$ARCH_BUILD_DIR"
        cd "$ARCH_BUILD_DIR"

        # Same fork-specific flags as scripts/android/podofo/build.sh:
        #  - PODOFO_BUILD_LIB_ONLY replaces the old PODOFO_BUILD_TOOLS/EXAMPLES/DOC/TEST=OFF
        #    quartet (this fork doesn't have those individual options).
        #  - PODOFO_WITH_FONTMANAGER=OFF replaces PODOFO_NO_FONTCONFIG/NO_WIN32GRAPHICS/NO_X11
        #    (also gone). Fontconfig has no sensible meaning on iOS anyway, and turning off
        #    the font manager means we don't need to cross-compile it at all.
        #  - CMAKE_DISABLE_FIND_PACKAGE_{JPEG,TIFF}=TRUE: root CMakeLists.txt does a plain
        #    find_package(JPEG)/find_package(TIFF) with no option to opt out otherwise, and
        #    (unlike Android's NDK sysroot) the iOS SDK sysroot sits alongside a normal
        #    Homebrew install on this host — CMake's find_package happily found Homebrew's
        #    host libjpeg/libtiff despite cross-compiling, silently enabling
        #    PdfImage's JPEG/TIFF codepaths against a dylib that isn't part of this
        #    XCFramework, which then fails at final link with "symbol(s) not found" for
        #    every _jpeg_*/_TIFF* symbol. Forcing both find_package calls to report
        #    "not found" restores the same (unsupported) codec surface Android already has.
        cmake "$BUILD_DIR/podofo" \
            -DCMAKE_TOOLCHAIN_FILE="$SCRIPT_DIR/../ios.toolchain.cmake" \
            -DPLATFORM="$IOS_CMAKE_PLATFORM" \
            -DDEPLOYMENT_TARGET="$MIN_IOS_VERSION" \
            -DENABLE_BITCODE=OFF \
            -DENABLE_ARC=ON \
            -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR/$SLICE" \
            -DCMAKE_DISABLE_FIND_PACKAGE_JPEG=TRUE \
            -DCMAKE_DISABLE_FIND_PACKAGE_TIFF=TRUE \
            -DFREETYPE_INCLUDE_DIRS="$FREETYPE_DIR/$SLICE/include/freetype2" \
            -DFREETYPE_LIBRARY="$FREETYPE_DIR/$SLICE/lib/libfreetype.a" \
            -DPNG_PNG_INCLUDE_DIR="$LIBPNG_DIR/$SLICE/include" \
            -DPNG_LIBRARY="$LIBPNG_DIR/$SLICE/lib/libpng.a" \
            -DLIBXML2_INCLUDE_DIR="$LIBXML2_DIR/$SLICE/include/libxml2" \
            -DLIBXML2_LIBRARY="$LIBXML2_DIR/$SLICE/lib/libxml2.a" \
            -DOPENSSL_ROOT_DIR="$OPENSSL_DIR/$SLICE" \
            -DOPENSSL_INCLUDE_DIR="$OPENSSL_DIR/$SLICE/include" \
            -DOPENSSL_CRYPTO_LIBRARY="$OPENSSL_DIR/$SLICE/lib/libcrypto.a" \
            -DOPENSSL_SSL_LIBRARY="$OPENSSL_DIR/$SLICE/lib/libssl.a" \
            -DPODOFO_BUILD_STATIC=ON \
            -DPODOFO_BUILD_LIB_ONLY=TRUE \
            -DPODOFO_WITH_FONTMANAGER=OFF \
            -DCMAKE_CXX_FLAGS="-I$LIBXML2_DIR/$SLICE/include -I$FREETYPE_DIR/$SLICE/include -I$LIBPNG_DIR/$SLICE/include -I$OPENSSL_DIR/$SLICE/include" \
            "${CCACHE_CMAKE_ARGS[@]}"

        cmake --build . --target install -- -j"$NPROC"

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
