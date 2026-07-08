#!/bin/bash

# Exit on error
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/../common.sh"

parse_skip_prepare "$@"
set -- "${POSITIONAL[@]}"

TARGET_DIR="$1"
LIBXML_VERSION="$2"

# Convert TARGET_DIR to absolute path
TARGET_DIR="$(cd "$(dirname "$TARGET_DIR")" && pwd)/$(basename "$TARGET_DIR")"

# Define directories
BUILD_DIR="$TARGET_DIR/build"
DOWNLOAD_DIR="$TARGET_DIR/download"
INSTALL_DIR="$TARGET_DIR/install"

function check() {
    if [ -z "$TARGET_DIR" ]; then
        echo "Error: TARGET_DIR argument not provided."
        echo "Usage: $0 <TARGET_DIR> <LIBXML_VERSION>"
        exit 1
    fi

    if [ -z "$LIBXML_VERSION" ]; then
        echo "Error: LIBXML_VERSION argument not provided."
        echo "Usage: $0 <TARGET_DIR> <LIBXML_VERSION>"
        exit 1
    fi
}

function prepare() {
    mkdir -p "$BUILD_DIR" "$DOWNLOAD_DIR"

    if [ ! -f "$DOWNLOAD_DIR/libxml2-$LIBXML_VERSION.tar.gz" ]; then
        echo "Downloading libxml2..."
        curl -L "https://github.com/GNOME/libxml2/archive/refs/tags/v$LIBXML_VERSION.tar.gz" -o "$DOWNLOAD_DIR/libxml2-$LIBXML_VERSION.tar.gz"
    fi

    if [ ! -d "$BUILD_DIR/libxml2-$LIBXML_VERSION" ]; then
        echo "Extracting libxml2..."
        rm -rf "$BUILD_DIR/libxml2-$LIBXML_VERSION"
        tar xzf "$DOWNLOAD_DIR/libxml2-$LIBXML_VERSION.tar.gz" -C "$BUILD_DIR"
    fi
}

function build() {
    mkdir -p "$BUILD_DIR" "$INSTALL_DIR"

    # Generate configure script (this is a git tag archive, so it ships no
    # pre-generated configure script)
    if [ ! -f "$BUILD_DIR/libxml2-$LIBXML_VERSION/configure" ]; then
        echo "Generating configure script..."
        cd "$BUILD_DIR/libxml2-$LIBXML_VERSION"
        ./autogen.sh
        make distclean
        cd "$TARGET_DIR"
    fi

    for SLICE in "${IOS_SLICES[@]}"; do
        echo "Building for $SLICE..."

        ios_slice_env "$SLICE"

        ARCH_BUILD_DIR="$BUILD_DIR/$SLICE"
        mkdir -p "$ARCH_BUILD_DIR"
        cd "$ARCH_BUILD_DIR"

        # See the equivalent Android script (scripts/android/libxml2/build.sh)
        # for why this specific --without-* set was chosen: this fork's
        # PdfXMPPacket/XMPUtils/PdfSigningContext need RelaxNG (xmlRelaxNG*)
        # and XPath (xmlXPath*), which are libxml2 defaults kept enabled here;
        # only features needing an extra cross-compiled dependency we don't
        # otherwise need (python, liblzma, iconv, ICU), legacy network
        # transports, dynamic module loading, and debug builds are disabled.
        "$BUILD_DIR/libxml2-$LIBXML_VERSION/configure" \
            --host="$HOST" \
            --prefix="$INSTALL_DIR/$SLICE" \
            --enable-static \
            --disable-shared \
            --with-pic \
            --without-python \
            --without-lzma \
            --without-zlib \
            --without-iconv \
            --without-icu \
            --without-iso8859x \
            --without-ftp \
            --without-http \
            --without-catalog \
            --without-docbook \
            --without-modules \
            --without-debug \
            --without-mem-debug \
            --without-run-debug \
            --without-coverage \
            --without-history \
            --without-readline \
            --without-legacy \
            CC="$CC" \
            CXX="$CXX" \
            AR="$AR" \
            RANLIB="$RANLIB" \
            STRIP="$STRIP" \
            CFLAGS="$CFLAGS_ARCH -O3 -fvisibility=hidden" \
            CXXFLAGS="$CFLAGS_ARCH -O3 -fvisibility=hidden" \
            LDFLAGS="$CFLAGS_ARCH"

        make clean
        make libxml2.la
        make install

        # Create symlink for libxml2.a
        cd "$INSTALL_DIR/$SLICE/lib"
        ln -sf libxml2.a libxml.a

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
