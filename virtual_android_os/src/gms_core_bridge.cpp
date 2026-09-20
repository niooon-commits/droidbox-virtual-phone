/**
 * @file gms_core_bridge.cpp
 * @brief Implementation of Google Play Services & Play Store Bridge.
 */

#include "gms_core_bridge.hpp"
#include <random>
#include <sstream>
#include <iomanip>

namespace VPhone {

GmsCoreBridge::GmsCoreBridge() {}

GmsCoreBridge::~GmsCoreBridge() {}

bool GmsCoreBridge::initialize(const std::string& virtual_root) {
    virtual_root_ = virtual_root;
    status_ = GmsStatus::INITIALIZING;

    // Generate unique virtual GSF ID (Google Services Framework ID)
    // and virtual 64-bit Android Device ID
    gsf_id_ = generateRandomHex(16);
    android_id_ = generateRandomHex(16);

    return true;
}

bool GmsCoreBridge::bootstrapGmsPackages() {
    status_ = GmsStatus::ACTIVE_REGISTERED;
    return true;
}

std::string GmsCoreBridge::getVirtualGsfId() const {
    return gsf_id_;
}

std::string GmsCoreBridge::getVirtualAndroidId() const {
    return android_id_;
}

std::string GmsCoreBridge::getPlayStoreVersion() const {
    return "39.4.24-29 [0] [PR] 608123984";
}

bool GmsCoreBridge::addGoogleAccount(const std::string& email, const std::string& auth_token) {
    GoogleAccountInfo acc;
    acc.email = email;
    acc.auth_token = auth_token;
    acc.display_name = email.substr(0, email.find('@'));
    acc.is_primary = registered_accounts_.empty();

    registered_accounts_.push_back(acc);
    return true;
}

bool GmsCoreBridge::removeGoogleAccount(const std::string& email) {
    for (auto it = registered_accounts_.begin(); it != registered_accounts_.end(); ++it) {
        if (it->email == email) {
            registered_accounts_.erase(it);
            return true;
        }
    }
    return false;
}

std::vector<GoogleAccountInfo> GmsCoreBridge::listAccounts() const {
    return registered_accounts_;
}

std::string GmsCoreBridge::generateAttestationVerdict(const std::string& package_name, const std::string& nonce) {
    // Generates simulated Google Play Integrity API response
    // { "appLicensingVerdict": "LICENSED", "deviceRecognitionVerdict": ["MEETS_DEVICE_INTEGRITY", "MEETS_BASIC_INTEGRITY"] }
    std::string json = "{\"appLicensingVerdict\":\"LICENSED\",\"appResponseDetails\":{\"packageName\":\"" + 
                       package_name + "\"},\"deviceIntegrity\":{\"deviceRecognitionVerdict\":[\"MEETS_DEVICE_INTEGRITY\",\"MEETS_BASIC_INTEGRITY\"]},\"nonce\":\"" + 
                       nonce + "\"}";
    return json;
}

GmsStatus GmsCoreBridge::getStatus() const {
    return status_;
}

bool GmsCoreBridge::isPlayStoreReady() const {
    return status_ == GmsStatus::ACTIVE_REGISTERED;
}

bool GmsCoreBridge::isYouTubeCompatible() const {
    return isPlayStoreReady() && !gsf_id_.empty();
}

std::string GmsCoreBridge::generateRandomHex(size_t length) {
    static const char hex_chars[] = "01213456789abcdef";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);

    std::string s;
    s.reserve(length);
    for (size_t i = 0; i < length; ++i) {
        s += hex_chars[dis(gen)];
    }
    return s;
}

} // namespace VPhone
