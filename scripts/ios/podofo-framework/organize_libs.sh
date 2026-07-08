#!/bin/bash
# Consolidates every dependency's + PoDoFo's per-slice static libs and
# headers into one directory tree, mirroring
# scripts/android/podofo-aar/organize_libs.sh.
#
# Usage: organize_libs.sh <brotli> <bzip2> <freetype> <harfbuzz> <libpng> <libxml2> <openssl> <podofo> <zlib> <target>
# Each of the first 9 args is a dependency's install root (containing
# ios-arm64/ios-arm64-simulator/ios-x86_64-simulator subdirs).

set -e

if [ $# -ne 10 ]; then
    echo "Error: Incorrect number of arguments"
    echo "Usage: $0 <brotli> <bzip2> <freetype> <harfbuzz> <libpng> <libxml2> <openssl> <podofo> <zlib> <target>"
    exit 1
fi

BROTLI_DIR="$1"
BZIP2_DIR="$2"
FREETYPE_DIR="$3"
HARFBUZZ_DIR="$4"
LIBPNG_DIR="$5"
LIBXML2_DIR="$6"
OPENSSL_DIR="$7"
PODOFO_DIR="$8"
ZLIB_DIR="$9"
TARGET_DIR="${10}"

SLICES=("ios-arm64" "ios-arm64-simulator" "ios-x86_64-simulator")

for SLICE in "${SLICES[@]}"; do
    mkdir -p "$TARGET_DIR/$SLICE/lib" "$TARGET_DIR/$SLICE/include"

    cp "$PODOFO_DIR/$SLICE/lib/libpodofo.a" "$TARGET_DIR/$SLICE/lib/"
    cp "$PODOFO_DIR/$SLICE/lib/libpodofo_private.a" "$TARGET_DIR/$SLICE/lib/"
    cp "$PODOFO_DIR/$SLICE/lib/libpodofo_3rdparty.a" "$TARGET_DIR/$SLICE/lib/"

    cp "$BROTLI_DIR/$SLICE/lib/libbrotlicommon-static.a" "$TARGET_DIR/$SLICE/lib/libbrotli.a"
    cp "$BROTLI_DIR/$SLICE/lib/libbrotlidec-static.a" "$TARGET_DIR/$SLICE/lib/libbrotlidec.a"
    cp "$BROTLI_DIR/$SLICE/lib/libbrotlienc-static.a" "$TARGET_DIR/$SLICE/lib/libbrotlienc.a"

    cp "$BZIP2_DIR/$SLICE/lib/libbz2.a" "$TARGET_DIR/$SLICE/lib/"
    cp "$FREETYPE_DIR/$SLICE/lib/libfreetype.a" "$TARGET_DIR/$SLICE/lib/"
    cp "$HARFBUZZ_DIR/$SLICE/lib/libharfbuzz.a" "$TARGET_DIR/$SLICE/lib/"
    cp "$LIBPNG_DIR/$SLICE/lib/libpng.a" "$TARGET_DIR/$SLICE/lib/"
    cp "$LIBXML2_DIR/$SLICE/lib/libxml2.a" "$TARGET_DIR/$SLICE/lib/"
    cp "$OPENSSL_DIR/$SLICE/lib/libssl.a" "$TARGET_DIR/$SLICE/lib/"
    cp "$OPENSSL_DIR/$SLICE/lib/libcrypto.a" "$TARGET_DIR/$SLICE/lib/"
    cp "$ZLIB_DIR/$SLICE/lib/libz.a" "$TARGET_DIR/$SLICE/lib/"

    cp -r "$PODOFO_DIR/$SLICE/include/"* "$TARGET_DIR/$SLICE/include/"
done

echo "Libraries copied and organized successfully into $TARGET_DIR"
