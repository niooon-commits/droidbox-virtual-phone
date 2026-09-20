#include <jni.h>
#include <string>
#include <android/log.h>
#include "virtual_os.hpp"

#define LOG_TAG "DroidBoxNative"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

using namespace DroidBox;

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

    VirtualOSConfig config;
    config.root_storage_path = path;
    config.virtual_user_id = userId;
    config.android_api_level = apiLevel;
    config.enable_gms = true;
    config.enable_binder_hook = true;
    config.isolated_network = false;

    bool success = VirtualOS::getInstance().initialize(config);
    env->ReleaseStringUTFChars(rootStoragePath, path);
    return success ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_org_droidbox_virtualos_VirtualOSBridge_nativeStart(
        JNIEnv* /* env */,
        jobject /* this */) {
    LOGI("Starting DroidBox Virtual OS runtime...");
    return VirtualOS::getInstance().start() ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_org_droidbox_virtualos_VirtualOSBridge_nativeStop(
        JNIEnv* /* env */,
        jobject /* this */) {
    LOGI("Stopping DroidBox Virtual OS runtime...");
    VirtualOS::getInstance().stop();
}

JNIEXPORT jboolean JNICALL
Java_org_droidbox_virtualos_VirtualOSBridge_nativeInstallApk(
        JNIEnv* env,
        jobject /* this */,
        jstring apkPath) {
    const char* path = env->GetStringUTFChars(apkPath, nullptr);
    LOGI("Installing APK inside sandboxed environment: %s", path);
    bool result = VirtualOS::getInstance().installApk(path);
    env->ReleaseStringUTFChars(apkPath, path);
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_org_droidbox_virtualos_VirtualOSBridge_nativeLaunchApp(
        JNIEnv* env,
        jobject /* this */,
        jstring packageName) {
    const char* pkg = env->GetStringUTFChars(packageName, nullptr);
    LOGI("Launching virtual app: %s", pkg);
    bool result = VirtualOS::getInstance().launchApp(pkg);
    env->ReleaseStringUTFChars(packageName, pkg);
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jstring JNICALL
Java_org_droidbox_virtualos_VirtualOSBridge_nativeGetStatus(
        JNIEnv* env,
        jobject /* this */) {
    std::string status = VirtualOS::getInstance().getFormattedStatus();
    return env->NewStringUTF(status.c_str());
}

JNIEXPORT void JNICALL
Java_org_droidbox_virtualos_VirtualOSBridge_nativeInjectTouch(
        JNIEnv* /* env */,
        jobject /* this */,
        jint action,
        jfloat x,
        jfloat y) {
    VirtualOS::getInstance().getInputSystem().injectTouchEvent(action, x, y);
}

} // extern "C"
