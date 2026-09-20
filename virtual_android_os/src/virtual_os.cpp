/**
 * @file virtual_os.cpp
 * @brief Implementation of the Virtual OS Master Controller.
 */

#include "virtual_os.hpp"
#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>

namespace VPhone {

VirtualOS::VirtualOS()
    : namespace_isolator_(std::make_unique<NamespaceIsolator>()),
      binder_hook_(std::make_unique<BinderHook>()),
      virtual_fs_(std::make_unique<VirtualFileSystem>()),
      display_compositor_(std::make_unique<DisplayCompositor>()),
      apk_manager_(std::make_unique<ApkManager>()),
      gms_bridge_(std::make_unique<GmsCoreBridge>()),
      input_system_(std::make_unique<InputSystem>()) {
}

VirtualOS::~VirtualOS() {
    shutdown();
}

VirtualOS& VirtualOS::getInstance() {
    static VirtualOS instance;
    return instance;
}

bool VirtualOS::initialize(const VirtualDeviceConfig& config) {
    std::lock_guard<std::mutex> lock(lifecycle_mutex_);
    config_ = config;
    current_state_ = EngineState::INITIALIZING;

    log("INFO", "Initializing Virtual Phone Engine for [" + config_.device_name + "]");
    log("INFO", "Android Target API: " + std::to_string(config_.android_api_level) + ", Model: " + config_.model);

    // 1. Initialize Virtual Filesystem Sandbox
    if (!virtual_fs_->initialize(config_.root_storage_path)) {
        log("ERROR", "Failed to initialize virtual filesystem sandbox.");
        current_state_ = EngineState::ERROR;
        return false;
    }

    // Configure build properties (Samsung/Pixel spoofing)
    std::map<std::string, std::string> props = {
        {"ro.build.version.release", "14"},
        {"ro.build.version.sdk", std::to_string(config_.android_api_level)},
        {"ro.product.model", config_.model},
        {"ro.product.brand", config_.brand},
        {"ro.product.manufacturer", config_.manufacturer},
        {"ro.build.fingerprint", config_.fingerprint},
        {"ro.vphone.version", "2.4.0-cpp-release"}
    };
    virtual_fs_->generateBuildProperties(props);

    // 2. Setup Linux Kernel Namespaces
    if (!namespace_isolator_->setupNamespaces(config_.root_storage_path)) {
        log("WARN", "Running in rootless container mode (PRoot/fake-chroot fallback enabled).");
    }

    // 3. Install Binder IPC Hook
    if (!binder_hook_->initialize() || !binder_hook_->installIoctlHook()) {
        log("ERROR", "Failed to bind interceptor to /dev/binder.");
        current_state_ = EngineState::ERROR;
        return false;
    }

    // 4. Initialize Virtual Display SurfaceFlinger
    DisplayMetrics metrics;
    metrics.width = config_.screen_width;
    metrics.height = config_.screen_height;
    metrics.dpi = config_.screen_density_dpi;
    metrics.refresh_rate = config_.refresh_rate;
    if (!display_compositor_->initialize(metrics)) {
        log("ERROR", "Failed to initialize virtual display compositor.");
        return false;
    }

    // 5. Initialize Input System
    input_system_->initialize(config_.screen_width, config_.screen_height);

    // 6. Initialize Virtual APK Manager
    apk_manager_->initialize(virtual_fs_->getGuestDataDir(""));
    apk_manager_->provisionPreinstalledApps();

    // 7. Initialize Google Play Services & Play Store Bridge
    if (config_.enable_gms) {
        log("INFO", "Bootstrapping Google Play Services (GMS) & Play Store bridge...");
        gms_bridge_->initialize(config_.root_storage_path);
        gms_bridge_->bootstrapGmsPackages();
    }

    current_state_ = EngineState::STOPPED;
    log("INFO", "Virtual Phone Engine initialized successfully.");
    return true;
}

bool VirtualOS::boot() {
    std::lock_guard<std::mutex> lock(lifecycle_mutex_);
    if (current_state_ == EngineState::RUNNING) {
        log("WARN", "Virtual Phone is already running.");
        return true;
    }

    current_state_ = EngineState::BOOTING;
    log("INFO", "Booting Android Kernel in isolated sandbox...");

    // Simulated boot stages
    log("INFO", "[Stage 1/4] Mounting /system, /vendor, /data/user/0, /apex in mount namespace...");
    virtual_fs_->mountVirtualHierarchies();

    log("INFO", "[Stage 2/4] Launching virtual init daemon (PID 1 in guest namespace)...");
    namespace_isolator_->spawnIsolatedProcess("/system/bin/init", {"init", "second_stage"}, "system.init", 0);

    log("INFO", "[Stage 3/4] Starting virtual SurfaceFlinger & display compositor at 60 Hz...");
    display_compositor_->startRenderLoop();

    log("INFO", "[Stage 4/4] Starting Zygote, SystemServer, and Launcher3 desktop...");
    apk_manager_->launchApp("com.android.launcher3");

    // If GMS enabled, start Play Store background sync
    if (config_.enable_gms) {
        log("INFO", "Google Services Framework (GSF ID: " + gms_bridge_->getVirtualGsfId() + ") registered.");
        log("INFO", "Google Play Store service ready.");
    }

    current_state_ = EngineState::RUNNING;
    log("INFO", ">>> VIRTUAL ANDROID PHONE BOOT COMPLETE - READY FOR USER INTERACTION <<<");
    return true;
}

bool VirtualOS::pause() {
    std::lock_guard<std::mutex> lock(lifecycle_mutex_);
    if (current_state_ != EngineState::RUNNING) return false;

    current_state_ = EngineState::PAUSED;
    log("INFO", "Freezing guest Android process trees (cgroups freezer)...");
    namespace_isolator_->freezeProcessTree(1);
    display_compositor_->stopRenderLoop();
    return true;
}

bool VirtualOS::resume() {
    std::lock_guard<std::mutex> lock(lifecycle_mutex_);
    if (current_state_ != EngineState::PAUSED) return false;

    log("INFO", "Thawing guest Android processes...");
    namespace_isolator_->resumeProcessTree(1);
    display_compositor_->startRenderLoop();
    current_state_ = EngineState::RUNNING;
    return true;
}

bool VirtualOS::shutdown() {
    std::lock_guard<std::mutex> lock(lifecycle_mutex_);
    if (current_state_ == EngineState::STOPPED) return true;

    log("INFO", "Shutting down Virtual Android Phone...");
    display_compositor_->stopRenderLoop();
    binder_hook_->uninstallIoctlHook();
    virtual_fs_->unmountAll();
    current_state_ = EngineState::STOPPED;
    log("INFO", "Virtual Phone is powered off.");
    return true;
}

bool VirtualOS::reboot() {
    shutdown();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    return boot();
}

NamespaceIsolator& VirtualOS::getNamespaceIsolator() { return *namespace_isolator_; }
BinderHook& VirtualOS::getBinderHook() { return *binder_hook_; }
VirtualFileSystem& VirtualOS::getFileSystem() { return *virtual_fs_; }
DisplayCompositor& VirtualOS::getDisplayCompositor() { return *display_compositor_; }
ApkManager& VirtualOS::getApkManager() { return *apk_manager_; }
GmsCoreBridge& VirtualOS::getGmsBridge() { return *gms_bridge_; }
InputSystem& VirtualOS::getInputSystem() { return *input_system_; }

EngineState VirtualOS::getState() const { return current_state_; }
const VirtualDeviceConfig& VirtualOS::getConfig() const { return config_; }

std::string VirtualOS::getStatusString() const {
    switch (current_state_.load()) {
        case EngineState::STOPPED: return "STOPPED (POWERED OFF)";
        case EngineState::INITIALIZING: return "INITIALIZING";
        case EngineState::BOOTING: return "BOOTING KERNEL";
        case EngineState::RUNNING: return "ACTIVE & RUNNING";
        case EngineState::PAUSED: return "SUSPENDED (FREEZER ACTIVE)";
        case EngineState::ERROR: return "KERNEL ERROR";
        default: return "UNKNOWN";
    }
}

void VirtualOS::setLogListener(std::function<void(const std::string&, const std::string&)> listener) {
    log_listener_ = listener;
}

void VirtualOS::log(const std::string& level, const std::string& message) {
    if (log_listener_) {
        log_listener_(level, message);
    } else {
        std::cout << "[VPhone][" << level << "] " << message << std::endl;
    }
}

} // namespace VPhone
