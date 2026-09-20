/**
 * @file virtual_fs.hpp
 * @brief Sandboxed Virtual Filesystem & System Properties Spoofing.
 *
 * Provides completely isolated directories for Guest Android apps,
 * private /data/data/ hierarchies, redirected /sdcard/ storage, and
 * spoofed /system/build.prop for Google Play Protect certification.
 */

#ifndef VIRTUAL_FS_HPP
#define VIRTUAL_FS_HPP

#include <string>
#include <vector>
#include <map>
#include <cstdint>

namespace VPhone {

struct FileNode {
    std::string path;
    std::string virtual_path;
    bool is_directory;
    uint64_t size_bytes;
    uint32_t permissions;
    uid_t owner_uid;
};

class VirtualFileSystem {
public:
    VirtualFileSystem();
    ~VirtualFileSystem();

    bool initialize(const std::string& base_dir);
    bool mountVirtualHierarchies();
    bool unmountAll();

    // Directory creation and isolation
    std::string getGuestDataDir(const std::string& package_name) const;
    std::string getGuestSdcardDir() const;
    std::string getGuestSystemDir() const;

    // Path translation between Host and Virtual Guest
    std::string translateToVirtualPath(const std::string& host_path) const;
    std::string translateToHostPath(const std::string& virtual_path) const;

    // Device fingerprint and build.prop spoofing
    bool generateBuildProperties(const std::map<std::string, std::string>& props);
    std::string getProperty(const std::string& key) const;
    void setProperty(const std::string& key, const std::string& value);

    // Sandbox storage statistics
    uint64_t getUsedStorageBytes() const;
    uint64_t getAvailableStorageBytes() const;
    std::vector<FileNode> listVirtualDirectory(const std::string& virtual_path) const;

private:
    std::string base_dir_;
    std::string data_dir_;
    std::string sdcard_dir_;
    std::string system_dir_;
    std::string cache_dir_;
    std::map<std::string, std::string> build_props_;
};

} // namespace VPhone

#endif // VIRTUAL_FS_HPP
