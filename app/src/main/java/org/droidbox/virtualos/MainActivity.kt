package org.droidbox.virtualos

import android.content.Intent
import android.net.Uri
import android.os.Build
import android.os.Bundle
import android.provider.Settings
import android.widget.Button
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity

class MainActivity : AppCompatActivity() {

    private lateinit var tvStatus: TextView
    private lateinit var tvTitle: TextView
    private lateinit var btnToggleOS: Button
    private lateinit var btnInstallApk: Button
    private lateinit var btnLaunchPlayStore: Button
    private lateinit var btnLaunchYouTube: Button
    private lateinit var btnFloatingMode: Button
    private var isRunning = false

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        tvTitle = findViewById(R.id.tvTitle)
        tvStatus = findViewById(R.id.tvStatus)
        btnToggleOS = findViewById(R.id.btnToggleOS)
        btnInstallApk = findViewById(R.id.btnInstallApk)
        btnLaunchPlayStore = findViewById(R.id.btnLaunchPlayStore)
        btnLaunchYouTube = findViewById(R.id.btnLaunchYouTube)
        btnFloatingMode = findViewById(R.id.btnFloatingMode)

        // Initialize VirtualOS Bridge
        VirtualOSBridge.init(this)
        updateStatus()

        btnToggleOS.setOnClickListener {
            if (!isRunning) {
                val serviceIntent = Intent(this, VirtualOSService::class.java).apply {
                    action = VirtualOSService.ACTION_START
                }
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                    startForegroundService(serviceIntent)
                } else {
                    startService(serviceIntent)
                }
                isRunning = true
                btnToggleOS.text = "Stop Virtual OS"
                Toast.makeText(this, "Virtual Android Container Started", Toast.LENGTH_SHORT).show()
            } else {
                val serviceIntent = Intent(this, VirtualOSService::class.java).apply {
                    action = VirtualOSService.ACTION_STOP
                }
                startService(serviceIntent)
                isRunning = false
                btnToggleOS.text = "Start Virtual OS"
                Toast.makeText(this, "Virtual Android Container Stopped", Toast.LENGTH_SHORT).show()
            }
            updateStatus()
        }

        btnInstallApk.setOnClickListener {
            val success = VirtualOSBridge.installApk("/data/local/tmp/app-test.apk")
            Toast.makeText(this, if (success) "Virtual APK Installation Simulated" else "Failed", Toast.LENGTH_SHORT).show()
            updateStatus()
        }

        btnLaunchPlayStore.setOnClickListener {
            VirtualOSBridge.launchApp("com.android.vending")
            Toast.makeText(this, "Opening Virtual Google Play Store...", Toast.LENGTH_SHORT).show()
        }

        btnLaunchYouTube.setOnClickListener {
            VirtualOSBridge.launchApp("com.google.android.youtube")
            Toast.makeText(this, "Opening Virtual YouTube...", Toast.LENGTH_SHORT).show()
        }

        btnFloatingMode.setOnClickListener {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M && !Settings.canDrawOverlays(this)) {
                val intent = Intent(
                    Settings.ACTION_MANAGE_OVERLAY_PERMISSION,
                    Uri.parse("package:$packageName")
                )
                startActivity(intent)
            } else {
                startService(Intent(this, FloatingPhoneService::class.java))
                finish()
            }
        }
    }

    private fun updateStatus() {
        val status = VirtualOSBridge.getStatus()
        tvStatus.text = status
    }
}
