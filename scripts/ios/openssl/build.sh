#!/bin/bash

# Exit on error
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/../common.sh"

parse_skip_prepare "$@"
set -- "${POSITIONAL[@]}"

TARGET_DIR="$1"
OPENSSL_VERSION="$2"

# Convert TARGET_DIR to absolute path
TARGET_DIR="$(cd "$(dirname "$TARGET_DIR")" && pwd)/$(basename "$TARGET_DIR")"

BUILD_DIR="$TARGET_DIR/build"
DOWNLOAD_DIR="$TARGET_DIR/download"
INSTALL_DIR="$TARGET_DIR/install"

# OpenSSL ships its own named Configure targets for iOS (see
# Configurations/15-ios.conf in its source tree) that already wrap
# `xcrun -sdk <sdk> cc` internally, so no explicit CC override is needed here
# — just the target name for each slice. Not a `declare -A` associative
# array: macOS ships bash 3.2 (GPLv3 avoidance), which doesn't have them —
# a plain function keeps this working under the system /bin/bash.
openssl_target_for_slice() {
    case "$1" in
        ios-arm64) echo "ios64-xcrun" ;;
        ios-arm64-simulator) echo "iossimulator-arm64-xcrun" ;;
        ios-x86_64-simulator) echo "iossimulator-x86_64-xcrun" ;;
        *) echo "Error: unknown iOS slice '$1'" >&2; exit 1 ;;
    esac
}

function check() {
    if [ -z "$TARGET_DIR" ]; then
        echo "Error: TARGET_DIR argument not provided."
        echo "Usage: $0 <TARGET_DIR> <OPENSSL_VERSION>"
        exit 1
    fi

    if [ -z "$OPENSSL_VERSION" ]; then
        echo "Error: OPENSSL_VERSION argument not provided."
        echo "Usage: $0 <TARGET_DIR> <OPENSSL_VERSION>"
        exit 1
    fi
}

function prepare() {
    mkdir -p "$BUILD_DIR" "$DOWNLOAD_DIR"

    if [ ! -f "$DOWNLOAD_DIR/openssl-$OPENSSL_VERSION.tar.gz" ]; then
        curl -L "https://www.openssl.org/source/openssl-$OPENSSL_VERSION.tar.gz" -o "$DOWNLOAD_DIR/openssl-$OPENSSL_VERSION.tar.gz"
    fi

    if [ ! -d "$BUILD_DIR/openssl-$OPENSSL_VERSION" ]; then
        tar xzf "$DOWNLOAD_DIR/openssl-$OPENSSL_VERSION.tar.gz" -C "$BUILD_DIR"
    fi
}

function build() {
    mkdir -p "$BUILD_DIR" "$INSTALL_DIR"

    for SLICE in "${IOS_SLICES[@]}"; do
        echo "Building for $SLICE..."

        TARGET="$(openssl_target_for_slice "$SLICE")"

        # OpenSSL's own build system does its own in-place object build per
        # Configure invocation, so each slice gets a fresh copy of the source
        # tree rather than sharing $BUILD_DIR/openssl-$OPENSSL_VERSION
        # (mirrors how the other autotools-based scripts copy/build per-slice).
        ARCH_BUILD_DIR="$BUILD_DIR/$SLICE"
        rm -rf "$ARCH_BUILD_DIR"
        cp -r "$BUILD_DIR/openssl-$OPENSSL_VERSION" "$ARCH_BUILD_DIR"
        cd "$ARCH_BUILD_DIR"

        ./Configure "$TARGET" \
            --prefix="$INSTALL_DIR/$SLICE" \
            --openssldir="$INSTALL_DIR/$SLICE" \
            no-shared \
            no-tests \
            threads

        make clean
        make -j"$NPROC"
        make install_sw

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
