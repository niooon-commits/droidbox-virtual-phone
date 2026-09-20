# Quick Start & Compilation Guide

This guide describes how to build and deploy the DroidBox C++ Virtual Android OS Engine.

## Option A: Build with Android NDK for Real Device (ARM64 / x86_64)

```bash
# 1. Clone or navigate to the virtual engine folder
cd virtual_android_os

# 2. Build shared library for Android APK
./scripts/build_ndk.sh

# Or using CMake with NDK toolchain:
cmake -B build \
    -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_PLATFORM=android-29
cmake --build build -j8
```

## Option B: Run Standalone Daemon in Termux / Linux Shell

If testing on an Android device via Termux or ADB:

```bash
# Compile natively
make

# Run the virtual OS daemon
./bin/vphone_daemon
```

Expected Output:
```
===============================================================
       DROIDBOX - VIRTUAL ANDROID PHONE ENGINE (C++ NATIVE)    
       Isolated Second Space, GMS Bridge & Sandbox Runtime     
===============================================================

[1/6] Initializing Virtual Phone Engine...
      Status: INITIALIZING

[2/6] Booting Virtual OS Subsystem...
      Status: ACTIVE & RUNNING

[3/6] Verifying Google Play Services & Play Store Bridge...
      Virtual GSF ID:        a8f2d9c0e4b1a7f3
      Virtual Android ID:    3c91d8e0a4f5b2c7
      Play Store Version:    39.4.24-29 [0] [PR] 608123984
      Play Store Ready:      YES [OK]
      YouTube Compatible:    CERTIFIED [OK]
```
