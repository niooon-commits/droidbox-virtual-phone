#!/usr/bin/env bash
# Build script for Android NDK Clang++ Cross-compilation

set -e

echo "=== Building DroidBox C++ Engine with Android NDK ==="

if [ -z "$ANDROID_NDK_HOME" ]; then
    echo "Warning: ANDROID_NDK_HOME environment variable not set."
    echo "Using default ndk-build if in PATH..."
fi

NDK_BUILD_BIN="${ANDROID_NDK_HOME}/ndk-build"
if ! command -v "$NDK_BUILD_BIN" &> /dev/null; then
    NDK_BUILD_BIN="ndk-build"
fi

if command -v "$NDK_BUILD_BIN" &> /dev/null; then
    "$NDK_BUILD_BIN" NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=Android.mk NDK_APPLICATION_MK=Application.mk
    echo "[SUCCESS] Shared libraries generated in libs/arm64-v8a/ and libs/x86_64/"
else
    echo "[INFO] NDK not detected in local CLI. Use CMake with Android toolchain:"
    echo "  cmake -B build -DCMAKE_TOOLCHAIN_FILE=\$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake \\"
    echo "        -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-29"
    echo "  cmake --build build"
fi
