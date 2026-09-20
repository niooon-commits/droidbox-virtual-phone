#include <jni.h>
#include <string>
#include <android/log.h>
#include "virtual_os.hpp"

#define LOG_TAG "DroidBoxNative"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

using namespace VPhone;

extern "C" {

JNIEXPORT jboolean JNICALL
Java_org_droidbox_virtualos_VirtualOSBridge_nativeInit(
        JNIEnv* env,
        jobject /* this */,
        jstring rootStoragePath,
        jint userId,
        jint apiLevel) {
    const char* path = env->GetStringUTFChars(rootStoragePath, nullptr);
    LOGI("Initializing DroidBox Virtual OS at path: %s (User %d, API %d)", path, userId, apiLevel);

    VirtualDeviceConfig config;
    config.root_storage_path = path ? std::string(path) : "/data/local/tmp/droidbox_root";
    config.android_api_level = apiLevel > 0 ? apiLevel : 34;
    config.enable_gms = true;

    bool success = VirtualOS::getInstance().initialize(config);

    if (path) {
        env->ReleaseStringUTFChars(rootStoragePath, path);
    }
    return success ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_org_droidbox_virtualos_VirtualOSBridge_nativeStart(
        JNIEnv* /* env */,
        jobject /* this */) {
    LOGI("Starting DroidBox Virtual OS runtime...");
    return VirtualOS::getInstance().boot() ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_org_droidbox_virtualos_VirtualOSBridge_nativeStop(
        JNIEnv* /* env */,
        jobject /* this */) {
    LOGI("Stopping DroidBox Virtual OS runtime...");
    VirtualOS::getInstance().shutdown();
}

JNIEXPORT jboolean JNICALL
Java_org_droidbox_virtualos_VirtualOSBridge_nativeInstallApk(
        JNIEnv* env,
        jobject /* this */,
        jstring apkPath) {
    const char* path = env->GetStringUTFChars(apkPath, nullptr);
    LOGI("Installing APK inside sandboxed environment: %s", path);

    InstalledAppInfo appInfo;
    bool result = VirtualOS::getInstance().getApkManager().installApk(path ? std::string(path) : "", appInfo);

    if (path) {
        env->ReleaseStringUTFChars(apkPath, path);
    }
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_org_droidbox_virtualos_VirtualOSBridge_nativeLaunchApp(
        JNIEnv* env,
        jobject /* this */,
        jstring packageName) {
    const char* pkg = env->GetStringUTFChars(packageName, nullptr);
    LOGI("Launching virtual app: %s", pkg);

    pid_t pid = VirtualOS::getInstance().getApkManager().launchApp(pkg ? std::string(pkg) : "");

    if (pkg) {
        env->ReleaseStringUTFChars(packageName, pkg);
    }
    return (pid > 0) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jstring JNICALL
Java_org_droidbox_virtualos_VirtualOSBridge_nativeGetStatus(
        JNIEnv* env,
        jobject /* this */) {
    std::string status = VirtualOS::getInstance().getStatusString();
    return env->NewStringUTF(status.c_str());
}

JNIEXPORT void JNICALL
Java_org_droidbox_virtualos_VirtualOSBridge_nativeInjectTouch(
        JNIEnv* /* env */,
        jobject /* this */,
        jint action,
        jfloat x,
        jfloat y) {
    TouchAction touchAct = TouchAction::MOVE;
    if (action == 0) touchAct = TouchAction::DOWN;
    else if (action == 1) touchAct = TouchAction::UP;
    else if (action == 2) touchAct = TouchAction::MOVE;
    else if (action == 3) touchAct = TouchAction::CANCEL;

    VirtualOS::getInstance().getInputSystem().sendSingleTouch(touchAct, x, y);
}

} // extern "C"
