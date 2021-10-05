package com.example.backend

import android.app.*
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.TextView
import com.example.backend.databinding.ActivityMainBinding

//import android.R
import android.content.Context

import android.content.Intent
import android.graphics.Color
import android.graphics.drawable.Icon
import android.os.Build
import android.os.IBinder
import android.os.PowerManager
import android.speech.tts.TextToSpeech.STOPPED
import android.telephony.ServiceState
import android.util.Log
import android.view.View
import android.widget.Toast
import androidx.annotation.RequiresApi
import androidx.appcompat.content.res.AppCompatResources.getDrawable

import androidx.core.app.NotificationCompat
import androidx.core.content.ContextCompat.getDrawable
import androidx.work.Worker
import androidx.work.WorkerParameters
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.launch
import java.net.HttpURLConnection
import java.net.Socket
import java.net.URL

class MainActivity : AppCompatActivity() {
    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        startBackgroundService()
    }

    fun clicking(view: View) {
        Thread {
            val client = Socket("127.0.0.1", 9001)
            client.soTimeout = 1
            client.outputStream.write("Hello from the client!".toByteArray(Charsets.UTF_8))

            var response: ByteArray = ByteArray(200)
            var responseSize = client.inputStream.read(response)
            Log.d("Client", responseSize.toString())
            Log.d("Client", response.decodeToString(0, responseSize, false))

            runOnUiThread {
                binding.sampleText.text = response.decodeToString(0, responseSize, false)
            }
        }.start()
    }

    fun stopBackground(view: View) {
        Intent(this, EndlessService::class.java).also {
            it.action = "Stop"
            startForegroundService(it)
            return
        }
    }

    private fun startBackgroundService() {
        Intent(this, EndlessService::class.java).also {
            it.action = "Start"
            startForegroundService(it)
            return
        }
    }

    external fun pingServer(): String

    companion object {
        init {
            System.loadLibrary("backend")
        }
    }
}

class EndlessService : Service() {

    private var wakeLock: PowerManager.WakeLock? = null
    private var isServiceStarted = false
    private var shutdownAddr:Long = 0

    override fun onBind(intent: Intent): IBinder? {
        return null
    }

    override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {
        if (intent != null) {
            when (intent.action) {
                "Start" -> startService()
                "Stop" -> stopService()
            }
        } else {
            Log.d("Service","with a null intent. It has been probably restarted by the system.")
        }
        return START_STICKY
    }

    override fun onCreate() {
        super.onCreate()
        var notification = createNotification()
        startForeground(1, notification)
    }

    override fun onDestroy() {
        super.onDestroy()
    }

    private fun startService() {
        if (isServiceStarted) return
        isServiceStarted = true

        // Avoid Doze Mode
        wakeLock = (getSystemService(Context.POWER_SERVICE) as PowerManager).run {
            newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, "EndlessService::lock").apply {
                acquire()
            }
        }

        shutdownAddr = initBackend()
        Thread {
            mainCpp(shutdownAddr)
        }
    }

    private fun stopService() {
        Toast.makeText(this, "Service stopping", Toast.LENGTH_SHORT).show()
        try {
            wakeLock?.let {
                if (it.isHeld) {
                    it.release()
                }
            }
            stopForeground(true)
            stopSelf()
        } catch (e: Exception) {
            Log.d("Service","Service stopped without being started: ${e.message}")
        }
        isServiceStarted = false
        signalShutdownBackend(shutdownAddr) // Send stop signal to the server
        stopForeground(true) // Terminate server
        stopSelf()
    }

    private fun createNotification(): Notification {
        val notificationChannelId = "ENDLESS SERVICE CHANNEL"
        val channelName = "Endless Service notifications channel"
        val importance = NotificationManager.IMPORTANCE_HIGH

        val channel = NotificationChannel(
            notificationChannelId,
            channelName,
            importance
        ).let {
            it.description = "Endless Service channel"
            it.enableLights(true)
            it.lightColor = Color.RED
            it.enableVibration(true)
            it.vibrationPattern = longArrayOf(100, 200, 300, 400, 500, 400, 300, 200, 400)
            it
        }

        val notificationManager = getSystemService(Context.NOTIFICATION_SERVICE) as NotificationManager;
        notificationManager.createNotificationChannel(channel)

        val pendingIntent: PendingIntent = Intent(this, MainActivity::class.java).let { notificationIntent ->
            PendingIntent.getActivity(this, 0, notificationIntent, 0)
        }

        val shutdownIntent: Intent = Intent(this, EndlessService::class.java).apply {
            action = "Stop"
        }

        val shutdownPendingIntent: PendingIntent =
            PendingIntent.getForegroundService(application, 0, shutdownIntent, 0)

        val builder: Notification.Builder = Notification.Builder(this, notificationChannelId)

        val action: android.app.Notification.Action =
            Notification.Action.Builder( Icon.createWithResource(this, R.mipmap.ic_launcher), "Stop", shutdownPendingIntent)
                .build()

        return builder
            .setContentTitle("Endless Service")
            .setContentText("This is your favorite endless service working")
            .setContentIntent(pendingIntent)
            .setSmallIcon(R.mipmap.ic_launcher)
            .setTicker("Ticker text")
            .addAction(action)
            .build()
    }

    private external fun initBackend(): Long
    private external fun mainCpp(pointer:Long)
    private external fun signalShutdownBackend(pointer:Long)

    companion object {
        init {
            System.loadLibrary("backend")
        }
    }
}