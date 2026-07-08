#!/bin/bash
# Shared helpers sourced by every scripts/ios/<dep>/build.sh and
# scripts/ios/podofo/build.sh. Not meant to be executed directly.
#
# This pipeline produces 3 build slices: one device slice and two simulator
# slices. The two simulator slices are lipo'd together into a single fat
# simulator library only at final XCFramework packaging time (in
# scripts/ios/podofo-framework/build.sh) — every intermediate
# dependency/PoDoFo build stays a plain single-arch cross-compile, same
# mental model as the Android pipeline's per-ABI loop.
IOS_SLICES=("ios-arm64" "ios-arm64-simulator" "ios-x86_64-simulator")

# 16.3, not the more obvious 14.0 the reference repo uses: PoDoFo's own
# PdfDeclarationsPrivate.cpp calls the floating-point overload of
# std::to_chars, which Apple's libc++ marks unavailable before iOS 16.3 (the
# integer overloads are fine on 14; the float one shipped later). Building
# with a lower deployment target fails outright at compile time ("'to_chars'
# is unavailable: introduced in iOS 16.3") rather than misbehaving at
# runtime, so this is the actual minimum this fork's code supports on iOS,
# not just a preference — found by attempting a real cross-compile at 14.0
# first.
MIN_IOS_VERSION="${MIN_IOS_VERSION:-16.3}"

if command -v nproc >/dev/null 2>&1; then
    NPROC="$(nproc)"
else
    NPROC="$(sysctl -n hw.ncpu)"
fi

# Several vendored dependency CMakeLists.txt files (eg. brotli, harfbuzz) declare
# an old cmake_minimum_required() that CMake >= 4.0 refuses to configure at all
# ("Compatibility with CMake < 3.5 has been removed"). This env var is CMake's
# own escape hatch for exactly this, and applies to every `cmake` invocation
# below without having to patch each dependency's CMakeLists.txt individually.
export CMAKE_POLICY_VERSION_MINIMUM=3.5

# In-place sed: only macOS/BSD sed is relevant here (this pipeline is
# macOS-only, since it needs Xcode), which requires an (empty) backup suffix
# argument after -i.
sed_i() {
    sed -i '' "$@"
}

if command -v ccache >/dev/null 2>&1; then
    CCACHE_BIN="ccache"
else
    CCACHE_BIN=""
fi

with_ccache() {
    if [ -n "$CCACHE_BIN" ]; then
        echo "$CCACHE_BIN $1"
    else
        echo "$1"
    fi
}

# Populates CC/CXX/AR/RANLIB/STRIP/SYSROOT/ARCH/HOST/CFLAGS_ARCH for the given
# build slice, for Makefile/autotools-based dependency scripts that need a
# concrete compiler invocation rather than a CMake toolchain file. Also sets
# IOS_CMAKE_PLATFORM, the corresponding PLATFORM value for
# scripts/ios/ios.toolchain.cmake (vendored from leetal/ios-cmake), used by
# the CMake-based dependency scripts (brotli, harfbuzz) and
# scripts/ios/podofo/build.sh.
ios_slice_env() {
    local slice="$1"
    case "$slice" in
        ios-arm64)
            SDK="iphoneos"
            ARCH="arm64"
            HOST="arm64-apple-ios"
            MIN_FLAG="-mios-version-min=$MIN_IOS_VERSION"
            IOS_CMAKE_PLATFORM="OS64"
            ;;
        ios-arm64-simulator)
            SDK="iphonesimulator"
            ARCH="arm64"
            # NOT "arm64-apple-ios-simulator": the config.sub shipped with
            # freetype/libpng/libxml2's vendored autotools predates GNU
            # config's "-simulator" suffix and rejects it outright
            # ("Kernel 'ios' not known to work with OS 'simulator'"),
            # aborting configure before it even gets to compiling anything.
            # --host is only used by autoconf to decide "yes this is a cross
            # build" (any host/build mismatch triggers that) and to name
            # Makefile variables — it has no effect on the actual compiler
            # invocation, which is fully pinned below via CC/CFLAGS_ARCH
            # (-arch/-isysroot/-mios-simulator-version-min) regardless of
            # what this string says. A plain, already-valid Darwin triplet
            # is enough to satisfy config.sub while every real target detail
            # still comes from the explicit flags.
            HOST="aarch64-apple-darwin"
            MIN_FLAG="-mios-simulator-version-min=$MIN_IOS_VERSION"
            IOS_CMAKE_PLATFORM="SIMULATORARM64"
            ;;
        ios-x86_64-simulator)
            SDK="iphonesimulator"
            ARCH="x86_64"
            HOST="x86_64-apple-darwin"
            MIN_FLAG="-mios-simulator-version-min=$MIN_IOS_VERSION"
            IOS_CMAKE_PLATFORM="SIMULATOR64"
            ;;
        *)
            echo "Error: unknown iOS slice '$slice' (expected one of: ${IOS_SLICES[*]})" >&2
            exit 1
            ;;
    esac

    SYSROOT="$(xcrun --sdk "$SDK" --show-sdk-path)"
    CC="$(with_ccache "$(xcrun --sdk "$SDK" -f clang)")"
    CXX="$(with_ccache "$(xcrun --sdk "$SDK" -f clang++)")"
    AR="$(xcrun --sdk "$SDK" -f ar)"
    RANLIB="$(xcrun --sdk "$SDK" -f ranlib)"
    STRIP="$(xcrun --sdk "$SDK" -f strip)"
    CFLAGS_ARCH="-arch $ARCH -isysroot $SYSROOT $MIN_FLAG"

    # Force every pkg-config lookup in these cross builds to find nothing,
    # rather than silently falling back to the host's default search paths
    # (eg. Homebrew's /opt/homebrew/lib/pkgconfig). Without this, freetype's
    # own ./configure happily reports "checking for LIBPNG... yes" using the
    # *host* macOS libpng.pc/headers — a real cross-contamination bug found
    # while first running this pipeline (freetype ended up compiling against
    # /opt/homebrew/opt/libpng's headers instead of our cross-compiled iOS
    # one). An empty PKG_CONFIG_LIBDIR overrides pkg-config's compiled-in
    # default search dirs entirely (rather than appending to them), so every
    # dependency lookup correctly reports "not found" and each script falls
    # back to the explicit --with-<dep>=DIR/-I/-L flags it's given instead —
    # matching how the Android pipeline behaves (no host pkg-config files
    # there to accidentally match against in the first place).
    export PKG_CONFIG_LIBDIR=""
    unset PKG_CONFIG_PATH

    # Same contamination class as PKG_CONFIG_LIBDIR above, different vector:
    # the user's shell environment sets CPPFLAGS/LDFLAGS (eg. pointing at a
    # Homebrew LLVM install) for everyday host development. autoconf bakes
    # whatever CPPFLAGS is set at configure time straight into the
    # generated Makefile's own CPPFLAGS variable, so every dependency's
    # cross-compiled build would silently pick up host include/lib paths
    # unless this is cleared first. Each build.sh still explicitly passes
    # its own CFLAGS/CXXFLAGS/LDFLAGS per ./configure invocation (which
    # replaces the environment for that command already), but nothing
    # previously touched CPPFLAGS specifically — found while debugging why
    # libpng's ./configure baked in an unrelated /opt/homebrew include path.
    unset CPPFLAGS LDFLAGS CFLAGS CXXFLAGS
}

# Strips a "--skip-prepare" flag out of "$@" and sets SKIP_PREPARE=1 if found,
# leaving the remaining positional arguments correctly shifted. Every
# scripts/ios/<dep>/build.sh calls this instead of hand-rolling
# `set -- "${@/"$arg"}"` (the idiom the Android scripts use, ported from the
# reference repo): that idiom replaces a matched argument with an empty
# string rather than removing it, which silently leaves a stray empty
# positional argument in its place instead of shifting later args down.
parse_skip_prepare() {
    SKIP_PREPARE=0
    POSITIONAL=()
    for arg in "$@"; do
        if [ "$arg" == "--skip-prepare" ]; then
            SKIP_PREPARE=1
        else
            POSITIONAL+=("$arg")
        fi
    done
}
