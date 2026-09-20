/**
 * @file apk_manager.hpp
 * @brief Virtual APK Package Manager & App Isolation Engine.
 *
 * Manages APK installation, virtual UID allocation, AndroidManifest.xml parsing,
 * native library (.so) extraction, dex2oat compilation simulation, and launch intents.
 */

#ifndef APK_MANAGER_HPP
#define APK_MANAGER_HPP

#include <string>
#include <vector>
#include <map>
#include <cstdint>

namespace VPhone {

struct InstalledAppInfo {
    std::string package_name;
    std::string app_name;
    std::string version_name;
    int version_code;
    uid_t virtual_uid;
    std::string main_activity;
    std::string apk_path;
    std::string data_dir;
    std::string lib_dir;
    std::vector<std::string> permissions;
    bool is_system_app;
    bool is_running;
    pid_t guest_pid;
    uint64_t app_size_bytes;
    uint64_t data_size_bytes;
};

class ApkManager {
public:
    ApkManager();
    ~ApkManager();

    bool initialize(const std::string& virtual_data_path);

    // APK operations
    bool installApk(const std::string& host_apk_path, InstalledAppInfo& out_info);
    bool uninstallApp(const std::string& package_name);
    bool isAppInstalled(const std::string& package_name) const;

    // Launch & execution
    pid_t launchApp(const std::string& package_name, const std::string& intent_action = "android.intent.action.MAIN");
    bool stopApp(const std::string& package_name);

    // Queries
    std::vector<InstalledAppInfo> getInstalledApps() const;
    bool getAppInfo(const std::string& package_name, InstalledAppInfo& out_info) const;
    std::vector<std::string> getRunningPackages() const;

    // Default system apps provisioning
    void provisionPreinstalledApps();

private:
    std::string virtual_data_path_;
    std::map<std::string, InstalledAppInfo> installed_packages_;
    uid_t next_virtual_uid_ = 10001;

    bool parseManifest(const std::string& apk_path, InstalledAppInfo& info);
    bool extractNativeLibs(const std::string& apk_path, const std::string& target_lib_dir);
};

} // namespace VPhone

#endif // APK_MANAGER_HPP
