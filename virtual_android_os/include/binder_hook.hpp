/**
 * @file binder_hook.hpp
 * @brief Android Binder IPC Interception and Virtual System Services Proxy.
 *
 * Hooks Android's /dev/binder and /dev/hwbinder ioctl transactions (BINDER_WRITE_READ)
 * to rewrite parcel data and redirect calls to virtualized ActivityManagerService,
 * PackageManagerService, WindowManagerService, and AccountManagerService.
 */

#ifndef BINDER_HOOK_HPP
#define BINDER_HOOK_HPP

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <cstdint>

namespace VPhone {

enum class ServiceType {
    ACTIVITY_MANAGER,   // "activity"
    PACKAGE_MANAGER,    // "package"
    WINDOW_MANAGER,     // "window"
    AUDIO_SERVICE,      // "audio"
    INPUT_METHOD,       // "input_method"
    NOTIFICATION_MGR,   // "notification"
    ACCOUNT_MANAGER,    // "account" (vital for Google Login)
    CONNECTIVITY_MGR,   // "connectivity"
    DISPLAY_MANAGER     // "display"
};

struct BinderTransaction {
    uint32_t target_handle;
    uint32_t code;
    uint32_t flags;
    std::string interface_token;
    std::vector<uint8_t> data;
    std::vector<uint8_t> reply;
    bool intercepted = false;
};

using BinderInterceptorCallback = std::function<bool(BinderTransaction& tx)>;

class BinderHook {
public:
    BinderHook();
    ~BinderHook();

    bool initialize();
    bool installIoctlHook();
    bool uninstallIoctlHook();

    // Register handlers for specific Android system services
    void registerServiceProxy(ServiceType type, BinderInterceptorCallback callback);

    // Parcel rewriting functions
    bool rewriteGetInstalledPackages(BinderTransaction& tx);
    bool rewriteStartActivity(BinderTransaction& tx);
    bool rewriteGetAccountTypes(BinderTransaction& tx);
    bool rewriteRequestPermissions(BinderTransaction& tx);

    // Simulated Binder ioctl router
    bool processIoctl(int fd, unsigned long request, void* arg);

    uint64_t getTotalInterceptedTransactions() const;
    uint64_t getRedirectedServiceCalls() const;

private:
    bool is_hook_installed_ = false;
    uint64_t total_transactions_ = 0;
    uint64_t redirected_calls_ = 0;
    std::map<ServiceType, BinderInterceptorCallback> service_proxies_;
};

} // namespace VPhone

#endif // BINDER_HOOK_HPP
