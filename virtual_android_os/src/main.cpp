/**
 * @file main.cpp
 * @brief Standalone Daemon & Verification Runner for DroidBox Virtual Android OS Engine.
 */

#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include "virtual_os.hpp"

using namespace VPhone;

void printBanner() {
    std::cout << "===============================================================\n";
    std::cout << "       DROIDBOX - VIRTUAL ANDROID PHONE ENGINE (C++ NATIVE)    \n";
    std::cout << "       Isolated Second Space, GMS Bridge & Sandbox Runtime     \n";
    std::cout << "===============================================================\n\n";
}

int main(int argc, char* argv[]) {
    printBanner();

    VirtualOS& vphone = VirtualOS::getInstance();

    // 1. Configure virtual device
    VirtualDeviceConfig config;
    config.device_name = "Virtual Android 14 (Private Space)";
    config.model = "Pixel 8 Pro";
    config.brand = "google";
    config.android_api_level = 34;
    config.enable_gms = true;
    config.root_storage_path = "/data/local/tmp/vphone_sandbox";

    std::cout << "[1/6] Initializing Virtual Phone Engine...\n";
    if (!vphone.initialize(config)) {
        std::cerr << "Failed to initialize engine.\n";
        return 1;
    }
    std::cout << "      Status: " << vphone.getStatusString() << "\n\n";

    // 2. Booting Virtual Phone
    std::cout << "[2/6] Booting Virtual OS Subsystem...\n";
    if (!vphone.boot()) {
        std::cerr << "Failed to boot virtual OS.\n";
        return 1;
    }
    std::cout << "      Status: " << vphone.getStatusString() << "\n\n";

    // 3. Inspecting Google Play Services Bridge
    std::cout << "[3/6] Verifying Google Play Services & Play Store Bridge...\n";
    GmsCoreBridge& gms = vphone.getGmsBridge();
    std::cout << "      Virtual GSF ID:        " << gms.getVirtualGsfId() << "\n";
    std::cout << "      Virtual Android ID:    " << gms.getVirtualAndroidId() << "\n";
    std::cout << "      Play Store Version:    " << gms.getPlayStoreVersion() << "\n";
    std::cout << "      Play Store Ready:      " << (gms.isPlayStoreReady() ? "YES [OK]" : "NO") << "\n";
    std::cout << "      YouTube Compatibility: " << (gms.isYouTubeCompatible() ? "CERTIFIED [OK]" : "NO") << "\n\n";

    // 4. Inspecting Installed Applications
    std::cout << "[4/6] Listing Sandboxed Applications:\n";
    ApkManager& apk_mgr = vphone.getApkManager();
    auto apps = apk_mgr.getInstalledApps();
    for (const auto& app : apps) {
        std::cout << "      - " << std::left << std::setw(25) << app.app_name 
                  << " (" << app.package_name << ") [UID: " << app.virtual_uid << "]\n";
    }
    std::cout << "\n";

    // 5. Testing Application Launch (Google Play Store & YouTube)
    std::cout << "[5/6] Simulating App Launch inside Virtual Sandbox:\n";
    pid_t play_pid = apk_mgr.launchApp("com.android.vending");
    std::cout << "      Launched Google Play Store -> Guest PID: " << play_pid << "\n";

    pid_t yt_pid = apk_mgr.launchApp("com.google.android.youtube");
    std::cout << "      Launched YouTube           -> Guest PID: " << yt_pid << "\n\n";

    // 6. Running Display Compositor & Touch Emulation
    std::cout << "[6/6] Testing Display Compositor & Input Dispatcher:\n";
    DisplayCompositor& compositor = vphone.getDisplayCompositor();
    InputSystem& input = vphone.getInputSystem();

    input.sendSingleTouch(TouchAction::DOWN, 0.5f, 0.5f);
    input.sendSingleTouch(TouchAction::UP, 0.5f, 0.5f);
    input.triggerBackButton();

    std::cout << "      Display Resolution:    " << compositor.getMetrics().width << "x" << compositor.getMetrics().height << " @ " << compositor.getCurrentFps() << " FPS\n";
    std::cout << "      Total Input Events:    " << input.getTotalInputEvents() << "\n";
    std::cout << "      Binder IPC Proxy Calls: " << vphone.getBinderHook().getRedirectedServiceCalls() << "\n\n";

    std::cout << ">>> Virtual Android System is operating normally in sandboxed space. <<<\n";
    std::cout << "Engine ready for JNI binding or container execution.\n";

    return 0;
}
