/**
 * @file virtual_os.hpp
 * @brief Main Virtual OS Core Controller for Android-in-Android Sandbox.
 *
 * Coordinates Linux container namespaces, Binder IPC hooking, virtual filesystem,
 * surface composition, and isolated APK runtime execution.
 */

#ifndef VIRTUAL_OS_HPP
#define VIRTUAL_OS_HPP

#include <string>
#include <vector>
#include <memory>
#include <atomic>
#include <mutex>
#include <functional>

#include "namespace_isolator.hpp"
#include "binder_hook.hpp"
#include "virtual_fs.hpp"
#include "display_compositor.hpp"
#include "apk_manager.hpp"
#include "gms_core_bridge.hpp"
#include "input_system.hpp"

namespace VPhone {

enum class EngineState {
    STOPPED,
    INITIALIZING,
    BOOTING,
    RUNNING,
    PAUSED,
    ERROR
};

struct VirtualDeviceConfig {
    std::string device_name = "Pixel 8 Pro (Private Space)";
    std::string manufacturer = "Google";
    std::string brand = "google";
    std::string model = "Pixel 8 Pro";
    std::string fingerprint = "google/husky/husky:14/UD1A.230803.041/10777894:user/release-keys";
    int android_api_level = 34; // Android 14
    int screen_width = 1080;
    int screen_height = 2400;
    int screen_density_dpi = 420;
    int refresh_rate = 60;
    bool enable_gms = true;
    bool root_access = false;
    std::string root_storage_path = "/data/data/com.vphone.engine/virtual_root";
};

class VirtualOS {
public:
    static VirtualOS& getInstance();

    // Prevent copying
    VirtualOS(const VirtualOS&) = delete;
    VirtualOS& operator=(const VirtualOS&) = delete;

    // Lifecycle
    bool initialize(const VirtualDeviceConfig& config);
    bool boot();
    bool pause();
    bool resume();
    bool shutdown();
    bool reboot();

    // Subsystem accessors
    NamespaceIsolator& getNamespaceIsolator();
    BinderHook& getBinderHook();
    VirtualFileSystem& getFileSystem();
    DisplayCompositor& getDisplayCompositor();
    ApkManager& getApkManager();
    GmsCoreBridge& getGmsBridge();
    InputSystem& getInputSystem();

    // Status and state
    EngineState getState() const;
    const VirtualDeviceConfig& getConfig() const;
    std::string getStatusString() const;

    // Logging callback
    void setLogListener(std::function<void(const std::string& level, const std::string& msg)> listener);
    void log(const std::string& level, const std::string& message);

private:
    VirtualOS();
    ~VirtualOS();

    VirtualDeviceConfig config_;
    std::atomic<EngineState> current_state_{EngineState::STOPPED};
    mutable std::mutex lifecycle_mutex_;

    std::unique_ptr<NamespaceIsolator> namespace_isolator_;
    std::unique_ptr<BinderHook> binder_hook_;
    std::unique_ptr<VirtualFileSystem> virtual_fs_;
    std::unique_ptr<DisplayCompositor> display_compositor_;
    std::unique_ptr<ApkManager> apk_manager_;
    std::unique_ptr<GmsCoreBridge> gms_bridge_;
    std::unique_ptr<InputSystem> input_system_;

    std::function<void(const std::string&, const std::string&)> log_listener_;
};

} // namespace VPhone

#endif // VIRTUAL_OS_HPP
