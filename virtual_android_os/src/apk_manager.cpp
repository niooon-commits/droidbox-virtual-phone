/**
 * @file apk_manager.cpp
 * @brief Implementation of Virtual APK Package Manager.
 */

#include "apk_manager.hpp"
#include <iostream>

namespace VPhone {

ApkManager::ApkManager() {}

ApkManager::~ApkManager() {}

bool ApkManager::initialize(const std::string& virtual_data_path) {
    virtual_data_path_ = virtual_data_path;
    return true;
}

void ApkManager::provisionPreinstalledApps() {
    // 1. Google Play Store (Vending)
    InstalledAppInfo play_store;
    play_store.package_name = "com.android.vending";
    play_store.app_name = "Google Play Store";
    play_store.version_name = "39.4.24-29";
    play_store.version_code = 83942400;
    play_store.virtual_uid = next_virtual_uid_++;
    play_store.main_activity = "com.google.android.finsky.activities.MainActivity";
    play_store.apk_path = "/system/priv-app/Phonesky/Phonesky.apk";
    play_store.data_dir = virtual_data_path_ + "/com.android.vending";
    play_store.is_system_app = true;
    play_store.is_running = false;
    play_store.app_size_bytes = 54200000;
    play_store.data_size_bytes = 12500000;
    play_store.permissions = {
        "android.permission.INTERNET",
        "android.permission.INSTALL_PACKAGES",
        "android.permission.GET_ACCOUNTS",
        "android.permission.MANAGE_ACCOUNTS"
    };
    installed_packages_[play_store.package_name] = play_store;

    // 2. YouTube
    InstalledAppInfo youtube;
    youtube.package_name = "com.google.android.youtube";
    youtube.app_name = "YouTube";
    youtube.version_name = "19.08.36";
    youtube.version_code = 15432000;
    youtube.virtual_uid = next_virtual_uid_++;
    youtube.main_activity = "com.google.android.apps.youtube.app.WatchWhileActivity";
    youtube.apk_path = "/data/app/com.google.android.youtube/base.apk";
    youtube.data_dir = virtual_data_path_ + "/com.google.android.youtube";
    youtube.is_system_app = false;
    youtube.is_running = false;
    youtube.app_size_bytes = 112000000;
    youtube.data_size_bytes = 48000000;
    youtube.permissions = {
        "android.permission.INTERNET",
        "android.permission.ACCESS_NETWORK_STATE",
        "android.permission.WAKE_LOCK",
        "android.permission.RECORD_AUDIO"
    };
    installed_packages_[youtube.package_name] = youtube;

    // 3. Android System Settings (Virtual Phone Preferences)
    InstalledAppInfo settings;
    settings.package_name = "com.android.settings";
    settings.app_name = "Settings";
    settings.version_name = "14.0.0";
    settings.version_code = 34;
    settings.virtual_uid = 1000;
    settings.main_activity = "com.android.settings.Settings";
    settings.apk_path = "/system/priv-app/Settings/Settings.apk";
    settings.data_dir = virtual_data_path_ + "/com.android.settings";
    settings.is_system_app = true;
    settings.is_running = false;
    settings.app_size_bytes = 22000000;
    settings.data_size_bytes = 3500000;
    installed_packages_[settings.package_name] = settings;

    // 4. Private Space File Manager
    InstalledAppInfo files;
    files.package_name = "com.android.documentsui";
    files.app_name = "Files (Private Space)";
    files.version_name = "14.0.0";
    files.version_code = 34;
    files.virtual_uid = 1000;
    files.main_activity = "com.android.documentsui.files.FilesActivity";
    files.apk_path = "/system/priv-app/DocumentsUI/DocumentsUI.apk";
    files.data_dir = virtual_data_path_ + "/com.android.documentsui";
    files.is_system_app = true;
    files.is_running = false;
    files.app_size_bytes = 14000000;
    files.data_size_bytes = 1200000;
    installed_packages_[files.package_name] = files;
}

bool ApkManager::installApk(const std::string& host_apk_path, InstalledAppInfo& out_info) {
    std::string fake_pkg = "com.example.app_" + std::to_string(next_virtual_uid_);
    out_info.package_name = fake_pkg;
    out_info.app_name = "Sideloaded App";
    out_info.version_name = "1.0.0";
    out_info.version_code = 1;
    out_info.virtual_uid = next_virtual_uid_++;
    out_info.apk_path = host_apk_path;
    out_info.data_dir = virtual_data_path_ + "/" + fake_pkg;
    out_info.is_system_app = false;
    out_info.is_running = false;
    out_info.app_size_bytes = 35000000;
    out_info.data_size_bytes = 4096;

    installed_packages_[fake_pkg] = out_info;
    return true;
}

bool ApkManager::uninstallApp(const std::string& package_name) {
    auto it = installed_packages_.find(package_name);
    if (it != installed_packages_.end()) {
        if (it->second.is_system_app) {
            return false; // System apps cannot be uninstalled
        }
        installed_packages_.erase(it);
        return true;
    }
    return false;
}

bool ApkManager::isAppInstalled(const std::string& package_name) const {
    return installed_packages_.find(package_name) != installed_packages_.end();
}

pid_t ApkManager::launchApp(const std::string& package_name, const std::string& /* intent_action */) {
    auto it = installed_packages_.find(package_name);
    if (it != installed_packages_.end()) {
        it->second.is_running = true;
        it->second.guest_pid = 200 + (it->second.virtual_uid % 100);
        return it->second.guest_pid;
    }
    return -1;
}

bool ApkManager::stopApp(const std::string& package_name) {
    auto it = installed_packages_.find(package_name);
    if (it != installed_packages_.end()) {
        it->second.is_running = false;
        it->second.guest_pid = 0;
        return true;
    }
    return false;
}

std::vector<InstalledAppInfo> ApkManager::getInstalledApps() const {
    std::vector<InstalledAppInfo> apps;
    apps.reserve(installed_packages_.size());
    for (const auto& pair : installed_packages_) {
        apps.push_back(pair.second);
    }
    return apps;
}

bool ApkManager::getAppInfo(const std::string& package_name, InstalledAppInfo& out_info) const {
    auto it = installed_packages_.find(package_name);
    if (it != installed_packages_.end()) {
        out_info = it->second;
        return true;
    }
    return false;
}

std::vector<std::string> ApkManager::getRunningPackages() const {
    std::vector<std::string> running;
    for (const auto& pair : installed_packages_) {
        if (pair.second.is_running) {
            running.push_back(pair.first);
        }
    }
    return running;
}

} // namespace VPhone
