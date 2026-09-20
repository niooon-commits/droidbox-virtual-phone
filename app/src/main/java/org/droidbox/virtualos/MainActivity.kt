package org.droidbox.virtualos

import android.content.Intent
import android.net.Uri
import android.os.Build
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.provider.Settings
import android.view.LayoutInflater
import android.view.View
import android.webkit.WebChromeClient
import android.webkit.WebView
import android.webkit.WebViewClient
import android.widget.*
import androidx.activity.result.contract.ActivityResultContracts
import androidx.appcompat.app.AppCompatActivity
import java.io.File
import java.io.FileOutputStream
import java.text.SimpleDateFormat
import java.util.*

class MainActivity : AppCompatActivity() {

    // Status bar & headers
    private lateinit var tvStatusTime: TextView
    private lateinit var tvEngineBadge: TextView
    private lateinit var btnFloatingMode: Button
    private lateinit var btnReboot: Button

    // Launcher views
    private lateinit var homeScreenContainer: RelativeLayout
    private lateinit var tvGlanceDate: TextView
    private lateinit var appsGrid: GridLayout

    // Nav Bar
    private lateinit var btnNavBack: Button
    private lateinit var btnNavHome: Button
    private lateinit var btnNavRecents: Button

    // App Window Container
    private lateinit var appWindowContainer: FrameLayout
    private lateinit var tvAppTitle: TextView
    private lateinit var btnAppBack: Button
    private lateinit var btnAppClose: Button
    private lateinit var appContentFrame: FrameLayout

    // Active state
    private var currentActiveApp: String? = null
    private var activeWebView: WebView? = null
    private val handler = Handler(Looper.getMainLooper())

    // App definition model
    data class StoreApp(
        val id: String,
        val name: String,
        val packageName: String,
        val iconEmoji: String,
        val iconColor: Int,
        val category: String,
        val size: String,
        val rating: String,
        val description: String,
        var isInstalled: Boolean = false
    )

    private val availableStoreApps = mutableListOf(
        StoreApp("whatsapp", "WhatsApp Messenger", "com.whatsapp", "💬", 0xFF16A34A.toInt(), "Communication", "48 MB", "★ 4.6", "Sandboxed messaging with virtual media sandbox", true),
        StoreApp("telegram", "Telegram", "org.telegram.messenger", "✈️", 0xFF0284C7.toInt(), "Communication", "62 MB", "★ 4.7", "Fast, secure messaging with private virtual space"),
        StoreApp("spotify", "Spotify Music", "com.spotify.music", "🎵", 0xFF10B981.toInt(), "Music & Audio", "35 MB", "★ 4.8", "Stream music in background with virtual audio routing"),
        StoreApp("instagram", "Instagram", "com.instagram.android", "📷", 0xFFE11D48.toInt(), "Social", "54 MB", "★ 4.5", "Photo and video sharing with separate private gallery"),
        StoreApp("retroarch", "RetroArch Emulator", "com.retroarch", "🕹️", 0xFF8B5CF6.toInt(), "Games & Emulation", "75 MB", "★ 4.9", "Multi-system game engine running on virtual GPU"),
        StoreApp("vlc", "VLC for Android", "org.videolan.vlc", "🎬", 0xFFF59E0B.toInt(), "Video Players", "38 MB", "★ 4.7", "Sandboxed media player for all video formats")
    )

    // APK File Picker contract
    private val apkPickerLauncher = registerForActivityResult(ActivityResultContracts.GetContent()) { uri: Uri? ->
        if (uri != null) {
            installApkFromUri(uri)
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        // Initialize VirtualOS C++ Engine
        VirtualOSBridge.init(this)
        VirtualOSBridge.start()

        bindViews()
        setupClock()
        setupLauncherClickListeners()
        setupNavBarListeners()
    }

    private fun bindViews() {
        tvStatusTime = findViewById(R.id.tvStatusTime)
        tvEngineBadge = findViewById(R.id.tvEngineBadge)
        btnFloatingMode = findViewById(R.id.btnFloatingMode)
        btnReboot = findViewById(R.id.btnReboot)

        homeScreenContainer = findViewById(R.id.homeScreenContainer)
        tvGlanceDate = findViewById(R.id.tvGlanceDate)
        appsGrid = findViewById(R.id.appsGrid)

        btnNavBack = findViewById(R.id.btnNavBack)
        btnNavHome = findViewById(R.id.btnNavHome)
        btnNavRecents = findViewById(R.id.btnNavRecents)

        appWindowContainer = findViewById(R.id.appWindowContainer)
        tvAppTitle = findViewById(R.id.tvAppTitle)
        btnAppBack = findViewById(R.id.btnAppBack)
        btnAppClose = findViewById(R.id.btnAppClose)
        appContentFrame = findViewById(R.id.appContentFrame)

        btnAppBack.setOnClickListener { closeCurrentApp() }
        btnAppClose.setOnClickListener { closeCurrentApp() }

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

        btnReboot.setOnClickListener {
            tvEngineBadge.text = "● Rebooting Virtual OS..."
            VirtualOSBridge.stop()
            handler.postDelayed({
                VirtualOSBridge.start()
                tvEngineBadge.text = "● C++ Native Kernel Active"
                Toast.makeText(this, "DroidBox Virtual OS Rebooted", Toast.LENGTH_SHORT).show()
            }, 600)
        }
    }

    private fun setupClock() {
        val timeFormat = SimpleDateFormat("hh:mm", Locale.getDefault())
        val dateFormat = SimpleDateFormat("EEEE, MMM d", Locale.getDefault())
        val now = Date()
        tvStatusTime.text = timeFormat.format(now)
        tvGlanceDate.text = dateFormat.format(now)
    }

    private fun setupLauncherClickListeners() {
        findViewById<View>(R.id.appStore).setOnClickListener { openAppStore() }
        findViewById<View>(R.id.dockStore).setOnClickListener { openAppStore() }

        findViewById<View>(R.id.appBrowser).setOnClickListener { openBrowser("https://www.google.com") }
        findViewById<View>(R.id.dockBrowser).setOnClickListener { openBrowser("https://www.google.com") }
        findViewById<View>(R.id.btnSearchLauncher).setOnClickListener { openBrowser("https://www.google.com") }

        findViewById<View>(R.id.appSettings).setOnClickListener { openSettings() }
        findViewById<View>(R.id.appTerminal).setOnClickListener { openTerminal() }

        findViewById<View>(R.id.appFiles).setOnClickListener { openFiles() }
        findViewById<View>(R.id.appCamera).setOnClickListener { openCamera() }
        findViewById<View>(R.id.dockCamera).setOnClickListener { openCamera() }

        findViewById<View>(R.id.dockPhone).setOnClickListener { openDialer() }
        findViewById<View>(R.id.dockMessages).setOnClickListener { openMessages() }

        findViewById<View>(R.id.appYouTube).setOnClickListener {
            VirtualOSBridge.launchApp("com.google.android.youtube")
            openBrowser("https://m.youtube.com")
            tvAppTitle.text = "YouTube (Virtual)"
        }

        findViewById<View>(R.id.appWhatsApp).setOnClickListener {
            VirtualOSBridge.launchApp("com.whatsapp")
            openGenericAppView("WhatsApp", "💬", 0xFF16A34A.toInt(), "Virtual Sandbox Chat Session\nStatus: Connected via Virtual Network\nVirtual UID: 10005")
        }
    }

    private fun setupNavBarListeners() {
        btnNavBack.setOnClickListener {
            if (activeWebView != null && activeWebView!!.canGoBack()) {
                activeWebView!!.goBack()
            } else if (appWindowContainer.visibility == View.VISIBLE) {
                closeCurrentApp()
            }
        }

        btnNavHome.setOnClickListener {
            closeCurrentApp()
        }

        btnNavRecents.setOnClickListener {
            val status = VirtualOSBridge.getStatus()
            Toast.makeText(this, "DroidBox Virtual OS: Active\n$status", Toast.LENGTH_LONG).show()
        }
    }

    // ==========================================
    // App Windows Launchers
    // ==========================================

    private fun openAppWindow(title: String, view: View) {
        currentActiveApp = title
        tvAppTitle.text = title
        appContentFrame.removeAllViews()
        appContentFrame.addView(view)
        appWindowContainer.visibility = View.VISIBLE
        homeScreenContainer.visibility = View.GONE
    }

    private fun closeCurrentApp() {
        currentActiveApp = null
        activeWebView?.destroy()
        activeWebView = null
        appContentFrame.removeAllViews()
        appWindowContainer.visibility = View.GONE
        homeScreenContainer.visibility = View.VISIBLE
    }

    // 1. DroidStore / Virtual App Downloader
    private fun openAppStore() {
        VirtualOSBridge.launchApp("com.android.vending")
        val view = LayoutInflater.from(this).inflate(R.layout.view_droid_store, appContentFrame, false)

        val llStoreAppsList = view.findViewById<LinearLayout>(R.id.llStoreAppsList)
        val etStoreSearch = view.findViewById<EditText>(R.id.etStoreSearch)
        val btnDirectApk = view.findViewById<Button>(R.id.btnDirectApkInstall)

        fun renderApps(filter: String = "") {
            llStoreAppsList.removeAllViews()
            val filtered = availableStoreApps.filter {
                it.name.contains(filter, ignoreCase = true) || it.category.contains(filter, ignoreCase = true)
            }

            for (app in filtered) {
                val itemView = LayoutInflater.from(this).inflate(R.layout.item_store_app, llStoreAppsList, false)
                val tvItemIcon = itemView.findViewById<TextView>(R.id.tvItemIcon)
                val tvItemName = itemView.findViewById<TextView>(R.id.tvItemName)
                val tvItemDetails = itemView.findViewById<TextView>(R.id.tvItemDetails)
                val tvItemDesc = itemView.findViewById<TextView>(R.id.tvItemDesc)
                val btnItemInstall = itemView.findViewById<Button>(R.id.btnItemInstall)
                val pbItemProgress = itemView.findViewById<ProgressBar>(R.id.pbItemProgress)
                val tvItemStatus = itemView.findViewById<TextView>(R.id.tvItemStatus)

                tvItemIcon.text = app.iconEmoji
                tvItemName.text = app.name
                tvItemDetails.text = "${app.category} • ${app.size} • ${app.rating}"
                tvItemDesc.text = app.description

                if (app.isInstalled) {
                    btnItemInstall.text = "Open"
                    btnItemInstall.setBackgroundColor(0xFF059669.toInt())
                } else {
                    btnItemInstall.text = "Install"
                    btnItemInstall.setBackgroundColor(0xFF2563EB.toInt())
                }

                btnItemInstall.setOnClickListener {
                    if (app.isInstalled) {
                        VirtualOSBridge.launchApp(app.packageName)
                        openGenericAppView(app.name, app.iconEmoji, app.iconColor, "Virtual Application Session: ${app.packageName}\nSandboxed Process UID: 100${app.id.hashCode() % 899 + 100}")
                    } else {
                        // Simulate real download and C++ installation
                        btnItemInstall.isEnabled = false
                        pbItemProgress.visibility = View.VISIBLE
                        tvItemStatus.visibility = View.VISIBLE
                        tvItemStatus.text = "Downloading virtual APK (${app.size})..."

                        var progress = 0
                        val timer = Timer()
                        timer.scheduleAtFixedRate(object : TimerTask() {
                            override fun run() {
                                progress += 20
                                handler.post {
                                    pbItemProgress.progress = progress
                                    if (progress >= 100) {
                                        timer.cancel()
                                        tvItemStatus.text = "Installing into C++ Sandbox (/data/user/0)..."
                                        handler.postDelayed({
                                            app.isInstalled = true
                                            VirtualOSBridge.installApk("/data/local/tmp/${app.id}.apk")
                                            pbItemProgress.visibility = View.GONE
                                            tvItemStatus.visibility = View.GONE
                                            btnItemInstall.isEnabled = true
                                            btnItemInstall.text = "Open"
                                            btnItemInstall.setBackgroundColor(0xFF059669.toInt())
                                            Toast.makeText(this@MainActivity, "${app.name} installed in Sandbox!", Toast.LENGTH_SHORT).show()
                                        }, 700)
                                    }
                                }
                            }
                        }, 150, 200)
                    }
                }

                llStoreAppsList.addView(itemView)
            }
        }

        renderApps()

        etStoreSearch.addTextChangedListener(object : android.text.TextWatcher {
            override fun afterTextChanged(s: android.text.Editable?) {
                renderApps(s?.toString() ?: "")
            }
            override fun beforeTextChanged(s: CharSequence?, start: Int, count: Int, after: Int) {}
            override fun onTextChanged(s: CharSequence?, start: Int, before: Int, count: Int) {}
        })

        btnDirectApk.setOnClickListener {
            try {
                apkPickerLauncher.launch("application/vnd.android.package-archive")
            } catch (e: Exception) {
                Toast.makeText(this, "Selecting APK...", Toast.LENGTH_SHORT).show()
            }
        }

        openAppWindow("DroidStore", view)
    }

    private fun installApkFromUri(uri: Uri) {
        try {
            val contentResolver = applicationContext.contentResolver
            val inputStream = contentResolver.openInputStream(uri)
            val tempApk = File(cacheDir, "picked_virtual_app.apk")
            val outputStream = FileOutputStream(tempApk)
            inputStream?.copyTo(outputStream)
            inputStream?.close()
            outputStream.close()

            val success = VirtualOSBridge.installApk(tempApk.absolutePath)
            Toast.makeText(
                this,
                if (success) "Custom APK installed in C++ Virtual Sandbox!" else "Installation simulated.",
                Toast.LENGTH_LONG
            ).show()
        } catch (e: Exception) {
            Toast.makeText(this, "Error installing APK: ${e.message}", Toast.LENGTH_SHORT).show()
        }
    }

    // 2. Chrome / Browser
    private fun openBrowser(url: String) {
        VirtualOSBridge.launchApp("com.android.chrome")
        val view = LayoutInflater.from(this).inflate(R.layout.view_browser, appContentFrame, false)
        val webView = view.findViewById<WebView>(R.id.webView)
        val etUrl = view.findViewById<EditText>(R.id.etBrowserUrl)
        val btnGo = view.findViewById<Button>(R.id.btnBrowserGo)
        val btnBack = view.findViewById<Button>(R.id.btnBrowserBack)
        val pbBrowser = view.findViewById<ProgressBar>(R.id.pbBrowser)

        activeWebView = webView
        webView.settings.javaScriptEnabled = true
        webView.settings.domStorageEnabled = true

        webView.webChromeClient = object : WebChromeClient() {
            override fun onProgressChanged(view: WebView?, newProgress: Int) {
                pbBrowser.progress = newProgress
                pbBrowser.visibility = if (newProgress in 1..99) View.VISIBLE else View.GONE
            }
        }

        webView.webViewClient = object : WebViewClient() {
            override fun shouldOverrideUrlLoading(view: WebView?, url: String?): Boolean {
                if (url != null) {
                    etUrl.setText(url)
                    view?.loadUrl(url)
                }
                return true
            }
        }

        btnGo.setOnClickListener {
            var target = etUrl.text.toString().trim()
            if (!target.startsWith("http://") && !target.startsWith("https://")) {
                target = "https://www.google.com/search?q=$target"
            }
            webView.loadUrl(target)
        }

        btnBack.setOnClickListener {
            if (webView.canGoBack()) webView.goBack()
        }

        etUrl.setText(url)
        webView.loadUrl(url)
        openAppWindow("Chrome", view)
    }

    // 3. System Settings
    private fun openSettings() {
        VirtualOSBridge.launchApp("com.android.settings")
        val view = LayoutInflater.from(this).inflate(R.layout.view_settings, appContentFrame, false)
        val tvDiagnostic = view.findViewById<TextView>(R.id.tvSettingsDiagnostic)
        val status = VirtualOSBridge.getStatus()
        tvDiagnostic.text = status.ifEmpty { "Virtual OS State: RUNNING (PID Isolation Active)" }
        openAppWindow("Settings", view)
    }

    // 4. Sandboxed Terminal (ADB Shell)
    private fun openTerminal() {
        VirtualOSBridge.launchApp("com.android.terminal")
        val view = LayoutInflater.from(this).inflate(R.layout.view_terminal, appContentFrame, false)
        val tvOutput = view.findViewById<TextView>(R.id.tvTerminalOutput)
        val etInput = view.findViewById<EditText>(R.id.etTerminalInput)
        val btnSend = view.findViewById<Button>(R.id.btnTerminalSend)

        fun executeCommand(cmd: String) {
            val trimmed = cmd.trim()
            var response = ""
            when {
                trimmed == "help" -> response = "Available commands: uname, getprop, ps, pm, df, clear, status"
                trimmed == "uname -a" || trimmed == "uname" -> response = "Linux droidbox-vphone 6.1.25 #1 SMP PREEMPT aarch64 Android"
                trimmed.startsWith("getprop") -> response = "[ro.product.model]: [Pixel 8 Pro]\n[ro.build.version.release]: [14]\n[ro.build.version.sdk]: [34]\n[ro.vphone.engine]: [C++20 Namespace Sandbox]"
                trimmed == "ps" -> response = "USER     PID   PPID  VSIZE  RSS   NAME\nroot     1     0     8400   1200  init\nsystem   104   1     45200  8400  vphone_surfaceflinger\nsystem   108   1     38900  7200  vphone_servicemanager\nu0_a105  201   1     94500  18400 com.android.vending\nu0_a106  214   1     120400 24100 com.android.chrome"
                trimmed.startsWith("pm") -> response = "package:com.android.vending\npackage:com.android.chrome\npackage:com.android.settings\npackage:com.google.android.youtube\npackage:com.whatsapp"
                trimmed == "df -h" -> response = "Filesystem      Size  Used Avail Use% Mounted on\n/dev/root        64G  2.4G   61G   4% /\ntmpfs           3.8G  4.0K  3.8G   1% /dev\n/data/user/0     64G  2.4G   61G   4% /data/user/0"
                trimmed == "clear" -> {
                    tvOutput.text = "$ "
                    return
                }
                trimmed == "status" -> response = VirtualOSBridge.getStatus()
                else -> response = "sh: command not found: $trimmed (type 'help' for built-in tools)"
            }
            tvOutput.append("$trimmed\n$response\n\n$ ")
            etInput.setText("")
        }

        btnSend.setOnClickListener {
            executeCommand(etInput.text.toString())
        }

        openAppWindow("Terminal (ADB)", view)
    }

    // 5. Files Sandbox
    private fun openFiles() {
        VirtualOSBridge.launchApp("com.android.documentsui")
        val linearLayout = LinearLayout(this).apply {
            orientation = LinearLayout.VERTICAL
            setPadding(32, 32, 32, 32)
            setBackgroundColor(0xFF0F172A.toInt())
        }

        val title = TextView(this).apply {
            text = "Virtual Filesystem Explorer"
            setTextColor(0xFF38BDF8.toInt())
            textSize = 16f
            setPadding(0, 0, 0, 16)
        }
        linearLayout.addView(title)

        val folders = listOf(
            "📁 /data/user/0/ (Sandbox Root UID 10001)",
            "📁 /sdcard/Download/ (APK Downloads & Cache)",
            "📁 /sdcard/DCIM/ (Virtual Camera Photos)",
            "📁 /system/etc/build.prop (Spoofed Device Props)",
            "📄 vphone_engine.log (Active C++ Telemetry)"
        )

        for (folder in folders) {
            val tv = TextView(this).apply {
                text = folder
                setTextColor(0xFFE2E8F0.toInt())
                textSize = 14f
                setPadding(16, 20, 16, 20)
                setBackgroundResource(R.drawable.bg_card_rounded)
                val params = LinearLayout.LayoutParams(
                    LinearLayout.LayoutParams.MATCH_PARENT,
                    LinearLayout.LayoutParams.WRAP_CONTENT
                )
                params.setMargins(0, 0, 0, 16)
                layoutParams = params
            }
            linearLayout.addView(tv)
        }

        val scroll = ScrollView(this).apply { addView(linearLayout) }
        openAppWindow("Files", scroll)
    }

    // 6. Camera Viewfinder
    private fun openCamera() {
        VirtualOSBridge.launchApp("com.android.camera2")
        val root = RelativeLayout(this).apply {
            setBackgroundColor(0xFF000000.toInt())
        }

        val viewfinder = TextView(this).apply {
            text = "📷 Virtual Camera Viewfinder\n[1080p 60FPS Virtual DirectStream]"
            setTextColor(0xFF94A3B8.toInt())
            textSize = 14f
            textAlignment = View.TEXT_ALIGNMENT_CENTER
            val params = RelativeLayout.LayoutParams(
                RelativeLayout.LayoutParams.WRAP_CONTENT,
                RelativeLayout.LayoutParams.WRAP_CONTENT
            ).apply {
                addRule(RelativeLayout.CENTER_IN_PARENT)
            }
            layoutParams = params
        }
        root.addView(viewfinder)

        val btnShutter = Button(this).apply {
            text = "⚪"
            textSize = 28f
            setBackgroundResource(R.drawable.bg_app_icon_circle)
            val params = RelativeLayout.LayoutParams(160, 160).apply {
                addRule(RelativeLayout.ALIGN_PARENT_BOTTOM)
                addRule(RelativeLayout.CENTER_HORIZONTAL)
                setMargins(0, 0, 0, 60)
            }
            layoutParams = params
            setOnClickListener {
                Toast.makeText(this@MainActivity, "Captured to /sdcard/DCIM/IMG_${System.currentTimeMillis()}.jpg", Toast.LENGTH_SHORT).show()
            }
        }
        root.addView(btnShutter)

        openAppWindow("Camera", root)
    }

    // 7. Phone Dialer
    private fun openDialer() {
        VirtualOSBridge.launchApp("com.android.dialer")
        val view = LinearLayout(this).apply {
            orientation = LinearLayout.VERTICAL
            gravity = android.view.Gravity.CENTER
            setPadding(40, 40, 40, 40)
            setBackgroundColor(0xFF0B1120.toInt())
        }

        val tvNumber = TextView(this).apply {
            text = ""
            textSize = 28f
            setTextColor(0xFFFFFFFF.toInt())
            textAlignment = View.TEXT_ALIGNMENT_CENTER
            setPadding(0, 20, 0, 30)
        }
        view.addView(tvNumber)

        val dialpad = GridLayout(this).apply {
            columnCount = 3
            alignmentMode = GridLayout.ALIGN_BOUNDS
        }

        val digits = listOf("1", "2", "3", "4", "5", "6", "7", "8", "9", "*", "0", "#")
        for (digit in digits) {
            val btn = Button(this).apply {
                text = digit
                textSize = 20f
                setTextColor(0xFFFFFFFF.toInt())
                setBackgroundResource(R.drawable.bg_card_rounded)
                val params = GridLayout.LayoutParams().apply {
                    width = 160
                    height = 140
                    setMargins(12, 12, 12, 12)
                }
                layoutParams = params
                setOnClickListener { tvNumber.append(digit) }
            }
            dialpad.addView(btn)
        }
        view.addView(dialpad)

        val btnCall = Button(this).apply {
            text = "📞 Call via Virtual Cellular Bridge"
            setBackgroundColor(0xFF10B981.toInt())
            setTextColor(0xFFFFFFFF.toInt())
            val params = LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.MATCH_PARENT,
                LinearLayout.LayoutParams.WRAP_CONTENT
            ).apply {
                setMargins(0, 40, 0, 0)
            }
            layoutParams = params
            setOnClickListener {
                if (tvNumber.text.isNotEmpty()) {
                    Toast.makeText(this@MainActivity, "Calling ${tvNumber.text} on Virtual Network...", Toast.LENGTH_SHORT).show()
                } else {
                    Toast.makeText(this@MainActivity, "Enter phone number", Toast.LENGTH_SHORT).show()
                }
            }
        }
        view.addView(btnCall)

        openAppWindow("Phone", view)
    }

    // 8. Messages
    private fun openMessages() {
        VirtualOSBridge.launchApp("com.android.mms")
        openGenericAppView(
            "Messages",
            "✉️",
            0xFF3B82F6.toInt(),
            "Virtual SMS Sandboxed Hub\n\n[Google Play]: Security update verified.\n[DroidBox]: Welcome to Virtual Android OS 14.\n\nAll messages isolated from host SIM."
        )
    }

    private fun openGenericAppView(title: String, icon: String, color: Int, content: String) {
        val view = LinearLayout(this).apply {
            orientation = LinearLayout.VERTICAL
            gravity = android.view.Gravity.CENTER
            setPadding(40, 40, 40, 40)
            setBackgroundColor(0xFF0F172A.toInt())
        }

        val tvIcon = TextView(this).apply {
            text = icon
            textSize = 54f
            gravity = android.view.Gravity.CENTER
        }
        view.addView(tvIcon)

        val tvTitle = TextView(this).apply {
            text = title
            textSize = 22f
            setTextColor(color)
            setTypeface(null, android.graphics.Typeface.BOLD)
            setPadding(0, 16, 0, 16)
        }
        view.addView(tvTitle)

        val tvBody = TextView(this).apply {
            text = content
            textSize = 14f
            setTextColor(0xFFE2E8F0.toInt())
            textAlignment = View.TEXT_ALIGNMENT_CENTER
            setLineSpacing(8f, 1.2f)
        }
        view.addView(tvBody)

        openAppWindow(title, view)
    }
}
