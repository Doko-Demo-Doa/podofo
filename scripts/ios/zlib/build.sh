#!/bin/bash

# Exit on error
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/../common.sh"

parse_skip_prepare "$@"
set -- "${POSITIONAL[@]}"

TARGET_DIR="$1"
ZLIB_VERSION="$2"

# Convert TARGET_DIR to absolute path
TARGET_DIR="$(cd "$(dirname "$TARGET_DIR")" && pwd)/$(basename "$TARGET_DIR")"

# Define directories
BUILD_DIR="$TARGET_DIR/build"
DOWNLOAD_DIR="$TARGET_DIR/download"
INSTALL_DIR="$TARGET_DIR/install"

function check() {
    # Check that TARGET_DIR argument has been passed
    if [ -z "$TARGET_DIR" ]; then
        echo "Error: TARGET_DIR argument not provided."
        echo "Usage: $0 <TARGET_DIR> <ZLIB_VERSION>"
        exit 1
    fi

    # Check that ZLIB_VERSION argument has been passed
    if [ -z "$ZLIB_VERSION" ]; then
        echo "Error: ZLIB_VERSION argument not provided."
        echo "Usage: $0 <TARGET_DIR> <ZLIB_VERSION>"
        exit 1
    fi
}

function prepare() {
    # Create directories if they don't exist
    mkdir -p "$BUILD_DIR" "$DOWNLOAD_DIR"

    # Download zlib if not already present
    if [ ! -f "$DOWNLOAD_DIR/zlib-$ZLIB_VERSION.tar.gz" ]; then
        echo "Downloading zlib..."
        curl -L "https://github.com/madler/zlib/archive/refs/tags/v$ZLIB_VERSION.tar.gz" -o "$DOWNLOAD_DIR/zlib-$ZLIB_VERSION.tar.gz"
    fi

    # Extract zlib if not already extracted
    if [ ! -d "$BUILD_DIR/zlib-$ZLIB_VERSION" ]; then
        echo "Extracting zlib..."
        tar xzf "$DOWNLOAD_DIR/zlib-$ZLIB_VERSION.tar.gz" -C "$BUILD_DIR"
    fi
}

function build() {
    # Create install directory
    mkdir -p "$INSTALL_DIR"

    # Build for each slice
    for SLICE in "${IOS_SLICES[@]}"; do
        echo "Building for $SLICE..."

        ios_slice_env "$SLICE"

        # Create build directory for this slice
        ARCH_BUILD_DIR="$BUILD_DIR/$SLICE"
        mkdir -p "$ARCH_BUILD_DIR"
        cd "$ARCH_BUILD_DIR"

        # Copy source files
        cp -r "$BUILD_DIR/zlib-$ZLIB_VERSION"/* .

        # Configure zlib for cross-compilation. AR/RANLIB must be passed
        # explicitly — zlib's configure defaults them to the host's plain
        # "ar"/"ranlib" otherwise, which archives the cross-compiled objects
        # into a member-less libz.a (no error, just an empty archive),
        # leaving deflate/inflate unresolved at link time.
        # -Dfdopen=fdopen: zlib 1.2.13's zutil.h has a decades-old bug where
        # it treats Apple's TARGET_OS_MAC (defined on every Apple platform,
        # including iOS — not just classic Mac OS, which is what this check
        # actually predates) as reason to macro-redefine `fdopen` to NULL,
        # which then collides with the real prototyped `fdopen` declared in
        # the SDK's own <stdio.h> ("expected ')'"). The redefinition is
        # guarded by `#ifndef fdopen`, so pre-defining fdopen as a harmless
        # self-referential macro (the preprocessor doesn't recurse on
        # self-reference, so this is a no-op everywhere fdopen is actually
        # called) skips the broken branch entirely.
        CHOST="$HOST" \
        CC="$CC" \
        AR="$AR" \
        RANLIB="$RANLIB" \
        CFLAGS="$CFLAGS_ARCH -O3 -Dfdopen=fdopen" \
        ./configure --static --prefix="$INSTALL_DIR/$SLICE"

        # Build and install. Build only the static lib target: plain `make`
        # (== "all: static shared") also links the "example"/"minigzip" test
        # programs, which fail to link in this cross-compilation setup and
        # aren't needed anyway. `install` only depends on `install-libs` (and
        # copies headers itself), so it doesn't need those targets built.
        make clean
        make libz.a
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
