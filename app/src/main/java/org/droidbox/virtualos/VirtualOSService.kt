package org.droidbox.virtualos

import android.app.Notification
import android.app.NotificationChannel
import android.app.NotificationManager
import android.app.Service
import android.content.Intent
import android.os.Build
import android.os.IBinder
import androidx.core.app.NotificationCompat

class VirtualOSService : Service() {

    companion object {
        const val CHANNEL_ID = "droidbox_virtual_os_channel"
        const val NOTIFICATION_ID = 1001
        const val ACTION_START = "org.droidbox.action.START"
        const val ACTION_STOP = "org.droidbox.action.STOP"
    }

    override fun onCreate() {
        super.onCreate()
        createNotificationChannel()
        VirtualOSBridge.init(this)
    }

    override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {
        when (intent?.action) {
            ACTION_START -> {
                val notification = buildNotification("Virtual Phone Runtime Running", "Isolated Linux Namespaces & Binder Hook active")
                startForeground(NOTIFICATION_ID, notification)
                VirtualOSBridge.start()
            }
            ACTION_STOP -> {
                VirtualOSBridge.stop()
                stopForeground(STOP_FOREGROUND_REMOVE)
                stopSelf()
            }
        }
        return START_STICKY
    }

    override fun onBind(intent: Intent?): IBinder? = null

    private fun createNotificationChannel() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            val channel = NotificationChannel(
                CHANNEL_ID,
                "DroidBox Virtual OS Daemon",
                NotificationManager.IMPORTANCE_LOW
            ).apply {
                description = "Keeps the virtual Android container running in background"
            }
            val manager = getSystemService(NotificationManager::class.java)
            manager?.createNotificationChannel(channel)
        }
    }

    private fun buildNotification(title: String, text: String): Notification {
        return NotificationCompat.Builder(this, CHANNEL_ID)
            .setContentTitle(title)
            .setContentText(text)
            .setSmallIcon(android.R.drawable.ic_dialog_info)
            .setOngoing(true)
            .build()
    }
}
