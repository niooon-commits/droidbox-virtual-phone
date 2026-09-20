# DroidBox — Virtual Android Phone Engine

[![Android Build & Release](https://github.com/niooon-commits/droidbox-virtual-phone/actions/workflows/android-build-release.yml/badge.svg)](https://github.com/niooon-commits/droidbox-virtual-phone/actions/workflows/android-build-release.yml)
[![Download Latest APK](https://img.shields.io/badge/Download-Latest%20APK-brightgreen.svg?logo=android)](https://github.com/niooon-commits/droidbox-virtual-phone/releases/download/latest/DroidBox-VirtualPhone-Latest.apk)
[![NIOOON Verified](https://img.shields.io/badge/NIOOON-Vault%20Verified-blue.svg)](https://nioon.lovable.app/)

**DroidBox** is an isolated Android-inside-Android virtualization engine written in modern **C++20** and embedded into an Android runtime. It allows running a secondary, completely private Android OS container inside any standard Android device without requiring root privileges.

---

## 📥 Download the Latest APK
- 👉 **[Download DroidBox-VirtualPhone-Latest.apk](https://github.com/niooon-commits/droidbox-virtual-phone/releases/download/latest/DroidBox-VirtualPhone-Latest.apk)**
- 📦 **[Persistent Release Page](https://github.com/niooon-commits/droidbox-virtual-phone/releases/tag/latest)**

> 🔄 **Single Rolling Release**: Every commit pushed to this repository triggers an automated GitHub Actions build that updates the exact same release with the latest APK file.

---

## 🌟 Core Architecture

```
+-------------------------------------------------------------+
|                 DroidBox Android App (Kotlin)               |
|  - Mini Floating Window (Picture-in-Picture)                |
|  - Virtual App Drawer & Sandboxed Launcher                  |
+-------------------------------------------------------------+
                               | JNI (native-lib.cpp)
+-------------------------------------------------------------+
|                 VirtualOS C++20 Core Engine                 |
|  - Namespace Isolator (PID, Mount, IPC, Net)                |
|  - Binder Hook Engine (/dev/binder ioctl interception)      |
|  - Virtual Filesystem (Sandboxed /data and /sdcard)         |
|  - GMS Core Bridge (Google Play Store & YouTube spoofing)   |
|  - Virtual Surface Compositor & Touch Event Injector        |
+-------------------------------------------------------------+
```

## 🛠️ Building Locally

```bash
# Clone the repository
git clone https://github.com/niooon-commits/droidbox-virtual-phone.git
cd droidbox-virtual-phone

# Build the Android APK using Gradle Wrapper
./gradlew assembleRelease
# The APK will be in app/build/outputs/apk/release/app-release.apk
```

## 🔗 NIOOON Automation
Integrated with `@niooon/github` and `niooon-manage-token`. See [NIOOON_INTEGRATION_REPORT.md](NIOOON_INTEGRATION_REPORT.md) for complete details.
