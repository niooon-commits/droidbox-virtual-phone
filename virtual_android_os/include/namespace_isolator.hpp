/**
 * @file namespace_isolator.hpp
 * @brief Linux Kernel Namespaces and Process Isolation for Guest Android.
 *
 * Implements PID namespace, Mount namespace, Network namespace, UTS namespace,
 * IPC namespace, and User namespace sandboxing using unshare(2), clone(2),
 * and chroot/pivot_root mechanics.
 */

#ifndef NAMESPACE_ISOLATOR_HPP
#define NAMESPACE_ISOLATOR_HPP

#include <string>
#include <vector>
#include <map>
#include <sys/types.h>

namespace VPhone {

struct ProcessContext {
    pid_t host_pid = -1;
    pid_t guest_pid = -1;
    uid_t virtual_uid = 10000; // Standard Android unprivileged app UID range
    gid_t virtual_gid = 10000;
    std::string package_name;
    bool is_isolated = true;
};

class NamespaceIsolator {
public:
    NamespaceIsolator();
    ~NamespaceIsolator();

    bool setupNamespaces(const std::string& guest_root_path);
    pid_t spawnIsolatedProcess(const std::string& binary_path, 
                               const std::vector<std::string>& args,
                               const std::string& package_name,
                               uid_t target_uid);

    bool terminateGuestProcess(pid_t guest_pid);
    bool freezeProcessTree(pid_t root_pid);
    bool resumeProcessTree(pid_t root_pid);

    ProcessContext getProcessContext(pid_t guest_pid) const;
    std::vector<ProcessContext> listActiveProcesses() const;

    // Kernel security controls
    bool applySeccompFilter();
    bool setProcessCapabilities();

private:
    bool configureMountNamespace(const std::string& guest_root);
    bool configurePidNamespace();
    bool configureUtsNamespace(const std::string& hostname);
    bool configureNetworkNamespace();

    std::string root_path_;
    std::map<pid_t, ProcessContext> active_processes_;
    pid_t guest_init_pid_ = -1;
};

} // namespace VPhone

#endif // NAMESPACE_ISOLATOR_HPP
