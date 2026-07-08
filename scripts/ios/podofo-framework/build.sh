#!/bin/bash
# Compiles the Objective-C++ bridging layer (src/wrapper/ios/*.mm) for all 3
# iOS slices, combines each slice's bridge objects + PoDoFo + every
# dependency's static lib into one PoDoFo.a per slice (mirroring
# scripts/android/podofo-aar/build.sh's whole-archive .so link, but as a
# static lib instead of a shared one — no equivalent of Android's separate
# libc++_shared.so copy step is needed here since libc++ on Apple platforms
# is a system library, not something bundled), lipos the two simulator
# slices together, and assembles the final PoDoFo.xcframework.
#
# Usage: build.sh <ORGANIZED_LIBS_DIR> <BRIDGE_SRC_DIR> <OUTPUT_DIR>
#   ORGANIZED_LIBS_DIR: output of organize_libs.sh (per-slice lib/+include/)
#   BRIDGE_SRC_DIR: src/wrapper/ios
#   OUTPUT_DIR: where PoDoFo.xcframework (and intermediate per-slice
#               artifacts) are written

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/../common.sh"

ORGANIZED_LIBS_DIR="$1"
BRIDGE_SRC_DIR="$2"
OUTPUT_DIR="$3"

if [ -z "$ORGANIZED_LIBS_DIR" ] || [ ! -d "$ORGANIZED_LIBS_DIR" ]; then
    echo "Error: ORGANIZED_LIBS_DIR argument missing or not found"
    echo "Usage: $0 <ORGANIZED_LIBS_DIR> <BRIDGE_SRC_DIR> <OUTPUT_DIR>"
    exit 1
fi
if [ -z "$BRIDGE_SRC_DIR" ] || [ ! -d "$BRIDGE_SRC_DIR" ]; then
    echo "Error: BRIDGE_SRC_DIR argument missing or not found"
    echo "Usage: $0 <ORGANIZED_LIBS_DIR> <BRIDGE_SRC_DIR> <OUTPUT_DIR>"
    exit 1
fi
if [ -z "$OUTPUT_DIR" ]; then
    echo "Error: OUTPUT_DIR argument missing"
    echo "Usage: $0 <ORGANIZED_LIBS_DIR> <BRIDGE_SRC_DIR> <OUTPUT_DIR>"
    exit 1
fi

rm -rf "$OUTPUT_DIR"
mkdir -p "$OUTPUT_DIR"

# Bridge headers exposed to consumers — everything except the internal-only
# ones (PoDoFoInternal.h pulls in <podofo/podofo.h>/C++ types and must never
# reach a plain Objective-C or Swift consumer).
PUBLIC_HEADERS=()
for h in "$BRIDGE_SRC_DIR"/*.h; do
    base="$(basename "$h")"
    if [ "$base" != "PoDoFoInternal.h" ]; then
        PUBLIC_HEADERS+=("$h")
    fi
done

# 1. Compile the bridge for each slice, then combine with PoDoFo + every
#    dependency's static lib into one PoDoFo.a per slice.
for SLICE in "${IOS_SLICES[@]}"; do
    echo "=== Compiling bridge for $SLICE ==="

    ios_slice_env "$SLICE"

    OBJ_DIR="$OUTPUT_DIR/obj/$SLICE"
    mkdir -p "$OBJ_DIR"

    OBJS=()
    for src in "$BRIDGE_SRC_DIR"/*.mm; do
        obj="$OBJ_DIR/$(basename "${src%.mm}").o"
        # $CXX (not a literal clang++) — already ccache-wrapped by
        # ios_slice_env above.
        $CXX -c -fobjc-arc -fmodules -std=c++17 -stdlib=libc++ \
            $CFLAGS_ARCH -O2 \
            -I "$ORGANIZED_LIBS_DIR/$SLICE/include" \
            -I "$BRIDGE_SRC_DIR" \
            -o "$obj" "$src"
        OBJS+=("$obj")
    done

    echo "=== Combining static libs for $SLICE ==="
    SLICE_LIB_DIR="$OUTPUT_DIR/slice/$SLICE"
    mkdir -p "$SLICE_LIB_DIR"
    # /usr/bin/libtool explicitly, not a bare `libtool`: common.sh prepends
    # Homebrew's GNU libtool (needed for libxml2's autogen.sh) onto PATH,
    # and GNU libtool has no `-static` mode at all ("unrecognised option")
    # — only Apple's own libtool does, so this must bypass the PATH lookup.
    /usr/bin/libtool -static -o "$SLICE_LIB_DIR/PoDoFo.a" \
        "$ORGANIZED_LIBS_DIR/$SLICE/lib/"*.a \
        "${OBJS[@]}"
done

# 2. lipo the two simulator slices together into one fat library; the
#    device slice stays single-arch.
echo "=== Assembling platform libraries ==="
mkdir -p "$OUTPUT_DIR/platform/ios-arm64/include" "$OUTPUT_DIR/platform/ios-simulator/include"

cp "$OUTPUT_DIR/slice/ios-arm64/PoDoFo.a" "$OUTPUT_DIR/platform/ios-arm64/PoDoFo.a"
lipo -create \
    "$OUTPUT_DIR/slice/ios-arm64-simulator/PoDoFo.a" \
    "$OUTPUT_DIR/slice/ios-x86_64-simulator/PoDoFo.a" \
    -output "$OUTPUT_DIR/platform/ios-simulator/PoDoFo.a"

# 3. Headers: PoDoFo's own C++ headers (only needed so the bridge headers'
#    own #includes resolve — a pure ObjC/Swift consumer only ever imports
#    the umbrella PoDoFo.h) plus every public bridge header, identical for
#    both platforms.
for PLATFORM_DIR in "$OUTPUT_DIR/platform/ios-arm64" "$OUTPUT_DIR/platform/ios-simulator"; do
    cp -r "$ORGANIZED_LIBS_DIR/ios-arm64/include/"* "$PLATFORM_DIR/include/"
    cp "${PUBLIC_HEADERS[@]}" "$PLATFORM_DIR/include/"

    cat > "$PLATFORM_DIR/include/module.modulemap" << EOF
module PoDoFo {
    umbrella header "PoDoFo.h"
    export *
    link "c++"
}
EOF
done

# 4. Assemble the XCFramework.
echo "=== Creating XCFramework ==="
rm -rf "$OUTPUT_DIR/PoDoFo.xcframework"
xcodebuild -create-xcframework \
    -library "$OUTPUT_DIR/platform/ios-arm64/PoDoFo.a" -headers "$OUTPUT_DIR/platform/ios-arm64/include" \
    -library "$OUTPUT_DIR/platform/ios-simulator/PoDoFo.a" -headers "$OUTPUT_DIR/platform/ios-simulator/include" \
    -output "$OUTPUT_DIR/PoDoFo.xcframework"

rm -rf "$OUTPUT_DIR/obj" "$OUTPUT_DIR/slice"

echo "PoDoFo.xcframework created at $OUTPUT_DIR/PoDoFo.xcframework"
