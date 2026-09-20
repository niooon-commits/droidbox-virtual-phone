package org.droidbox.virtualos

import android.os.Bundle

object GmsSpoofProvider {
    const val FAKE_GSF_ID = "3847291048291048"
    const val FAKE_ANDROID_ID = "droidbox_virtual_android_id"

    fun getGoogleServicesConfig(): Bundle {
        return Bundle().apply {
            putString("gsf_id", FAKE_GSF_ID)
            putString("android_id", FAKE_ANDROID_ID)
            putBoolean("play_integrity_verified", true)
            putBoolean("safetynet_passed", true)
            putString("device_profile", "Pixel 8 Pro (DroidBox Virtual)")
        }
    }
}
