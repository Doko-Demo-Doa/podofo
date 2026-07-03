#!/bin/bash

# Exit on error
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/../common.sh"

NDK_DIR="$1"
PODOFO_WRAPPER_DIR="$2"
TARGET_DIR="$3"

# Convert TARGET_DIR to absolute path
TARGET_DIR="$(cd "$(dirname "$TARGET_DIR")" && pwd)/$(basename "$TARGET_DIR")"

# Define directories
INSTALL_DIR="$TARGET_DIR/install"
BUILD_DIR="$TARGET_DIR/build"

# Check for required inputs
if [ -z "$NDK_DIR" ] || [ ! -d "$NDK_DIR" ]; then
    echo "Error: NDK_DIR argument missing or not found"
    echo "Usage: $0 <NDK_DIR> <PODOFO_WRAPPER_DIR> <TARGET_DIR>"
    exit 1
fi
if [ -z "$PODOFO_WRAPPER_DIR" ] || [ ! -d "$PODOFO_WRAPPER_DIR" ]; then
    echo "Error: PODOFO_WRAPPER_DIR argument missing or not found"
    echo "Usage: $0 <NDK_DIR> <PODOFO_WRAPPER_DIR> <TARGET_DIR>"
    exit 1
fi
if [ -z "$JAVA_HOME" ] || [ ! -d "$JAVA_HOME/include" ]; then
    echo "Error: could not determine JAVA_HOME (needed for jni.h). Set it explicitly."
    exit 1
fi

# Set up NDK toolchain
TOOLCHAIN="$NDK_DIR/toolchains/llvm/prebuilt/$HOST_TAG"
API_LEVEL=21

mkdir -p "$BUILD_DIR"

# Build the JNI shared library for each architecture
ANDROID_ARCHS=("arm64-v8a" "armeabi-v7a" "x86" "x86_64")

for ABI in "${ANDROID_ARCHS[@]}"; do
    echo "Processing $ABI..."

    # Create architecture-specific directory
    mkdir -p "$BUILD_DIR/jni/$ABI"

    # Set up compiler for this architecture
    case $ABI in
        "arm64-v8a")
            CXX="$TOOLCHAIN/bin/aarch64-linux-android$API_LEVEL-clang++"
            ;;
        "armeabi-v7a")
            CXX="$TOOLCHAIN/bin/armv7a-linux-androideabi$API_LEVEL-clang++"
            ;;
        "x86")
            CXX="$TOOLCHAIN/bin/i686-linux-android$API_LEVEL-clang++"
            ;;
        "x86_64")
            CXX="$TOOLCHAIN/bin/x86_64-linux-android$API_LEVEL-clang++"
            ;;
    esac

    # Create shared library from static libraries
    echo "Creating shared library for $ABI..."
    "$CXX" -shared -fPIC -fvisibility=hidden -fvisibility-inlines-hidden -o "$BUILD_DIR/jni/$ABI/libpodofo.so" \
        -I"$JAVA_HOME/include" \
        -I"$JAVA_HOME/include/$JNI_PLATFORM_DIR" \
        -I"$PODOFO_WRAPPER_DIR" \
        -I"$INSTALL_DIR/$ABI/include" \
        -I"$INSTALL_DIR/$ABI/include/podofo" \
        -I"$INSTALL_DIR/$ABI/include/podofo/main" \
        -I"$INSTALL_DIR/$ABI/include/podofo/auxiliary" \
        -I"$INSTALL_DIR/$ABI/include/podofo/3rdparty" \
        -I"$INSTALL_DIR/$ABI/include/podofo/optional" \
        "$PODOFO_WRAPPER_DIR/podofo_jni.cpp" \
        -Wl,--whole-archive \
        "$INSTALL_DIR/$ABI/lib/libpodofo.a" \
        "$INSTALL_DIR/$ABI/lib/libpodofo_private.a" \
        "$INSTALL_DIR/$ABI/lib/libpodofo_3rdparty.a" \
        "$INSTALL_DIR/$ABI/lib/libbrotli.a" \
        "$INSTALL_DIR/$ABI/lib/libbrotlidec.a" \
        "$INSTALL_DIR/$ABI/lib/libbrotlienc.a" \
        "$INSTALL_DIR/$ABI/lib/libbz2.a" \
        "$INSTALL_DIR/$ABI/lib/libfreetype.a" \
        "$INSTALL_DIR/$ABI/lib/libharfbuzz.a" \
        "$INSTALL_DIR/$ABI/lib/libpng.a" \
        "$INSTALL_DIR/$ABI/lib/libxml2.a" \
        "$INSTALL_DIR/$ABI/lib/libssl.a" \
        "$INSTALL_DIR/$ABI/lib/libcrypto.a" \
        "$INSTALL_DIR/$ABI/lib/libz.a" \
        -Wl,--no-whole-archive \
        -Wl,-z,max-page-size=16384 \
        -llog

    # Copy C++ runtime library
    case $ABI in
        "arm64-v8a")
            cp "$TOOLCHAIN/sysroot/usr/lib/aarch64-linux-android/libc++_shared.so" "$BUILD_DIR/jni/$ABI/"
            ;;
        "armeabi-v7a")
            cp "$TOOLCHAIN/sysroot/usr/lib/arm-linux-androideabi/libc++_shared.so" "$BUILD_DIR/jni/$ABI/"
            ;;
        "x86")
            cp "$TOOLCHAIN/sysroot/usr/lib/i686-linux-android/libc++_shared.so" "$BUILD_DIR/jni/$ABI/"
            ;;
        "x86_64")
            cp "$TOOLCHAIN/sysroot/usr/lib/x86_64-linux-android/libc++_shared.so" "$BUILD_DIR/jni/$ABI/"
            ;;
    esac

    # Copy headers (only once, since they're the same for all architectures)
    if [ "$ABI" = "arm64-v8a" ]; then
        cp -r "$INSTALL_DIR/$ABI/include" "$BUILD_DIR/jni/"
    fi
done

echo "JNI shared libraries built successfully in: $BUILD_DIR/jni"
