# DroidBox Virtual Android OS - System Architecture

DroidBox is a high-performance C++20 native engine that creates a fully sandboxed "Phone inside a Phone" environment on Android. It allows running an isolated second OS instance with separate apps, independent Google Play Services, YouTube, private storage, and isolated hardware device IDs.

---

## High-Level Architecture Diagram

```
+-------------------------------------------------------------------------+
|                           HOST ANDROID SYSTEM                           |
|  (User Apps, Host Android OS Kernel, System Services, /dev/binder)     |
+-------------------------------------------------------------------------+
                                   ▲
                                   │ Native JNI & Sandboxed Syscall Interception
                                   ▼
+-------------------------------------------------------------------------+
|                  DROIDBOX VIRTUAL OS ENGINE (C++20)                     |
|                                                                         |
|  +---------------------------+       +-------------------------------+  |
|  |    Namespace Isolator     |       |       Binder IPC Hook         |  |
|  |  (PID, Mount, Net, UTS)   |       | (AMS, PMS, WMS, AccountMgr)   |  |
|  +---------------------------+       +-------------------------------+  |
|                                                                         |
|  +---------------------------+       +-------------------------------+  |
|  |  Virtual Display Engine   |       |     Virtual Filesystem        |  |
|  | (SurfaceFlinger, Ashmem)  |       | (Sandbox /data, fake build.prop) |
|  +---------------------------+       +-------------------------------+  |
|                                                                         |
|  +---------------------------+       +-------------------------------+  |
|  |     APK Package Mgr       |       |       GMS Core Bridge         |  |
|  | (Isolated UID, Dex2Oat)   |       |  (GSF ID, Play Store, SafetyNet) |
|  +---------------------------+       +-------------------------------+  |
+-------------------------------------------------------------------------+
                                   ▲
                                   │ Guest Execution
                                   ▼
+-------------------------------------------------------------------------+
|                     GUEST VIRTUAL PHONE SPACE                           |
|                                                                         |
|   [ Google Play Store ]      [ YouTube App ]      [ Private Settings ]  |
|   [ Isolated WhatsApp ]      [ Sandboxed Files ]  [ Fake Device ID ]    |
+-------------------------------------------------------------------------+
```

---

## Core Engine Modules (C++)

### 1. Linux Namespace Sandbox (`namespace_isolator.cpp`)
- Creates isolated PID spaces where guest processes see themselves starting from PID 1 (`init`).
- Drops dangerous capabilities (`CAP_SYS_ADMIN`, `CAP_NET_RAW`) to ensure the virtual guest cannot tamper with the host phone.
- Supports both container mode (`unshare`/`clone`) and rootless PRoot/ptrace redirection mode.

### 2. Binder IPC Hooking (`binder_hook.cpp`)
- Android uses `/dev/binder` ioctl commands (`BINDER_WRITE_READ`) for all inter-process communication.
- The engine hooks `ioctl` via PLT/inline hooking.
- Intercepts `IPackageManager` to hide host applications and present only sandboxed virtual apps.
- Intercepts `IActivityManager` to encapsulate guest activities inside container window stubs.
- Intercepts `IAccountManager` so guest Google accounts remain completely isolated from host accounts.

### 3. Virtual Filesystem & Fingerprint Spoofing (`virtual_fs.cpp`)
- Implements isolated `/data/user/0/<package>` directories.
- Redirects external storage (`/sdcard/`) to a private encrypted container.
- Spoofs system properties (`ro.build.fingerprint`, `ro.product.model`, `ro.serialno`) so apps like Google Play Store and YouTube believe they are running on a certified Google Pixel or Samsung Galaxy device.

### 4. Display Compositor & SurfaceFlinger (`display_compositor.cpp`)
- Allocates shared memory buffers using Android `ashmem` or `AHardwareBuffer`.
- Runs a 60 FPS rendering pipeline that can output to a full-screen window or a draggable Picture-in-Picture floating window.

### 5. Google Play Services & Play Store Bridge (`gms_core_bridge.cpp`)
- Generates a unique virtual 64-bit Google Services Framework (GSF) ID.
- Provides SafetyNet & Play Integrity attestation responses (`MEETS_DEVICE_INTEGRITY`).
- Enables signing in with an independent Google Account without interfering with the host phone's Google accounts.
