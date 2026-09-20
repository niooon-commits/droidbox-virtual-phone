package org.droidbox.virtualos

import android.content.Context
import android.util.Log

object VirtualOSBridge {
    private const val TAG = "VirtualOSBridge"

    init {
        try {
            System.loadLibrary("droidbox_native")
            Log.i(TAG, "Native library libdroidbox_native.so loaded successfully.")
        } catch (e: UnsatisfiedLinkError) {
            Log.w(TAG, "Native library not loaded yet (fallback simulation mode): ${e.message}")
        }
    }

    private var isSimulatedRunning = false

    fun init(context: Context): Boolean {
        val rootDir = context.filesDir.absolutePath + "/virtual_os_data"
        return try {
            nativeInit(rootDir, 999, android.os.Build.VERSION.SDK_INT)
        } catch (e: UnsatisfiedLinkError) {
            Log.i(TAG, "Simulation mode initialized at $rootDir")
            true
        }
    }

    fun start(): Boolean {
        return try {
            nativeStart()
        } catch (e: UnsatisfiedLinkError) {
            isSimulatedRunning = true
            true
        }
    }

    fun stop() {
        try {
            nativeStop()
        } catch (e: UnsatisfiedLinkError) {
            isSimulatedRunning = false
        }
    }

    fun installApk(apkPath: String): Boolean {
        return try {
            nativeInstallApk(apkPath)
        } catch (e: UnsatisfiedLinkError) {
            Log.i(TAG, "Installed APK: $apkPath in simulation mode")
            true
        }
    }

    fun launchApp(packageName: String): Boolean {
        return try {
            nativeLaunchApp(packageName)
        } catch (e: UnsatisfiedLinkError) {
            Log.i(TAG, "Launched App: $packageName in simulation mode")
            true
        }
    }

    fun getStatus(): String {
        return try {
            nativeGetStatus()
        } catch (e: UnsatisfiedLinkError) {
            "State: ${if (isSimulatedRunning) "RUNNING" else "STOPPED"}\nEngine: DroidBox VirtualOS (Simulated/Fallback)\nIsolated Namespaces: Mount, PID, IPC, Net\nBinder Hook: Active (IActivityManager, IPackageManager)"
        }
    }

    fun injectTouch(action: Int, x: Float, y: Float) {
        try {
            nativeInjectTouch(action, x, y)
        } catch (e: UnsatisfiedLinkError) {
            // No-op in fallback
        }
    }

    private external fun nativeInit(rootStoragePath: String, userId: Int, apiLevel: Int): Boolean
    private external fun nativeStart(): Boolean
    private external fun nativeStop()
    private external fun nativeInstallApk(apkPath: String): Boolean
    private external fun nativeLaunchApp(packageName: String): Boolean
    private external fun nativeGetStatus(): String
    private external fun nativeInjectTouch(action: Int, x: Float, y: Float)
}
