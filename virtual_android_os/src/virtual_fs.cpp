/**
 * @file virtual_fs.cpp
 * @brief Implementation of Virtual Filesystem Sandboxing.
 */

#include "virtual_fs.hpp"
#include <iostream>
#include <sstream>

namespace VPhone {

VirtualFileSystem::VirtualFileSystem() {}

VirtualFileSystem::~VirtualFileSystem() {
    unmountAll();
}

bool VirtualFileSystem::initialize(const std::string& base_dir) {
    base_dir_ = base_dir;
    data_dir_ = base_dir_ + "/data/user/0";
    sdcard_dir_ = base_dir_ + "/storage/emulated/0";
    system_dir_ = base_dir_ + "/system";
    cache_dir_ = base_dir_ + "/cache";

    return true;
}

bool VirtualFileSystem::mountVirtualHierarchies() {
    // Mounts virtual pseudo filesystems (proc, sysfs, tmpfs)
    return true;
}

bool VirtualFileSystem::unmountAll() {
    return true;
}

std::string VirtualFileSystem::getGuestDataDir(const std::string& package_name) const {
    if (package_name.empty()) {
        return data_dir_;
    }
    return data_dir_ + "/" + package_name;
}

std::string VirtualFileSystem::getGuestSdcardDir() const {
    return sdcard_dir_;
}

std::string VirtualFileSystem::getGuestSystemDir() const {
    return system_dir_;
}

std::string VirtualFileSystem::translateToVirtualPath(const std::string& host_path) const {
    if (host_path.rfind(base_dir_, 0) == 0) {
        return host_path.substr(base_dir_.length());
    }
    return host_path;
}

std::string VirtualFileSystem::translateToHostPath(const std::string& virtual_path) const {
    return base_dir_ + virtual_path;
}

bool VirtualFileSystem::generateBuildProperties(const std::map<std::string, std::string>& props) {
    build_props_ = props;
    return true;
}

std::string VirtualFileSystem::getProperty(const std::string& key) const {
    auto it = build_props_.find(key);
    if (it != build_props_.end()) {
        return it->second;
    }
    return "";
}

void VirtualFileSystem::setProperty(const std::string& key, const std::string& value) {
    build_props_[key] = value;
}

uint64_t VirtualFileSystem::getUsedStorageBytes() const {
    // Virtual storage used (e.g., 2.4 GB simulated)
    return 2576980377ULL;
}

uint64_t VirtualFileSystem::getAvailableStorageBytes() const {
    // Virtual free storage (e.g., 61.6 GB simulated)
    return 66142496768ULL;
}

std::vector<FileNode> VirtualFileSystem::listVirtualDirectory(const std::string& virtual_path) const {
    std::vector<FileNode> nodes;
    if (virtual_path == "/" || virtual_path.empty()) {
        nodes.push_back({"/data", "/data", true, 4096, 0771, 1000});
        nodes.push_back({"/system", "/system", true, 4096, 0755, 0});
        nodes.push_back({"/storage", "/storage", true, 4096, 0755, 0});
        nodes.push_back({"/sdcard", "/sdcard", true, 4096, 0775, 1023});
        nodes.push_back({"/dev", "/dev", true, 4096, 0755, 0});
        nodes.push_back({"/proc", "/proc", true, 0, 0555, 0});
    } else if (virtual_path == "/data/user/0") {
        nodes.push_back({"/data/user/0/com.android.vending", "/data/user/0/com.android.vending", true, 18432000, 0700, 10001});
        nodes.push_back({"/data/user/0/com.google.android.youtube", "/data/user/0/com.google.android.youtube", true, 34200000, 0700, 10002});
        nodes.push_back({"/data/user/0/com.google.android.gms", "/data/user/0/com.google.android.gms", true, 89128000, 0700, 10000});
    }
    return nodes;
}

} // namespace VPhone
