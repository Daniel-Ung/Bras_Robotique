package com.example.brasrobotiseapp

import android.app.AlertDialog
import android.os.Bundle
import android.text.InputType
import android.util.Log
import android.webkit.*
import android.widget.*
import androidx.appcompat.app.AppCompatActivity
import java.io.PrintWriter
import java.net.Socket
import java.nio.ByteBuffer
import java.nio.ByteOrder
import javax.crypto.Mac
import javax.crypto.spec.SecretKeySpec

class MainActivity : AppCompatActivity() {

    private lateinit var statusTextView: TextView
    private lateinit var customCommandInput: EditText
    private lateinit var videoWebView: WebView

    private val pcIp = "192.168.5.2"
    private val pcPort = 12345
    private val base32Secret = "JBSWY3DPEHPK3PXP" // Clé à scanner dans Google Authenticator

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        showOtpPrompt()
        setContentView(R.layout.activity_main)

        // Initialisation UI
        statusTextView = findViewById(R.id.status)
        customCommandInput = findViewById(R.id.commandInput)
        videoWebView = findViewById(R.id.videoWebView)
        statusTextView.text = getString(R.string.txt_status_default)

        setupVideoStream()
        setupControls()
    }

    private fun setupVideoStream() {
        videoWebView.settings.loadWithOverviewMode = true
        videoWebView.settings.useWideViewPort = true
        videoWebView.webViewClient = object : WebViewClient() {
            override fun onReceivedError(view: WebView?, request: WebResourceRequest?, error: WebResourceError?) {
                Toast.makeText(this@MainActivity, getString(R.string.video_error), Toast.LENGTH_SHORT).show()
            }
        }

        val html = """
            <html><body style="margin:0;padding:0;">
            <img src='http://192.168.5.2:8080' style='width:100%;height:auto;' />
            </body></html>
        """.trimIndent()
        videoWebView.loadDataWithBaseURL(null, html, "text/html", "UTF-8", null)
    }

    private fun setupControls() {
        // Direction
        findViewById<Button>(R.id.up).setOnClickListener { sendCommand("E") }
        findViewById<Button>(R.id.down).setOnClickListener { sendCommand("e") }
        findViewById<Button>(R.id.left).setOnClickListener { sendCommand("b") }
        findViewById<Button>(R.id.right).setOnClickListener { sendCommand("B") }
        findViewById<Button>(R.id.stop).setOnClickListener { sendCommand("S") }

        // Avant-bras / Poignet
        findViewById<Button>(R.id.avbUp).setOnClickListener { sendCommand("c") }
        findViewById<Button>(R.id.avbDown).setOnClickListener { sendCommand("C") }
        findViewById<Button>(R.id.wristUp).setOnClickListener { sendCommand("m") }
        findViewById<Button>(R.id.wristDown).setOnClickListener { sendCommand("M") }

        // Pince
        findViewById<Button>(R.id.pinLeft).setOnClickListener { sendCommand("r") }
        findViewById<Button>(R.id.pinRight).setOnClickListener { sendCommand("R") }
        findViewById<Button>(R.id.pinOpen).setOnClickListener { sendCommand("O") }
        findViewById<Button>(R.id.pinClose).setOnClickListener { sendCommand("o") }

        // Reset
        findViewById<Button>(R.id.reset).setOnClickListener {
            sendCommand("I")
            statusTextView.text = getString(R.string.txt_status_default)
            Toast.makeText(this, getString(R.string.reset_message), Toast.LENGTH_SHORT).show()
        }

        // Commande personnalisée
        findViewById<Button>(R.id.sendTextCmd).setOnClickListener {
            val inputText = customCommandInput.text.toString().trim()
            if (inputText.isNotEmpty()) {
                sendCommand(inputText)
                customCommandInput.text.clear()
            } else {
                Toast.makeText(this, getString(R.string.empty_input_warning), Toast.LENGTH_SHORT).show()
            }
        }

        // Séquence automatique
        findViewById<Button>(R.id.autoSequence).setOnClickListener {
            val sequence = listOf("E", "e", "b", "S", "c", "C", "r", "R", "o", "O", "m", "M", "B", "I")
            Thread {
                runOnUiThread { statusTextView.text = getString(R.string.seq_running) }
                for (cmd in sequence) {
                    runOnUiThread { sendCommand(cmd) }
                    Thread.sleep(700)
                }
                runOnUiThread { statusTextView.text = getString(R.string.seq_done) }
            }.start()
        }
    }

    private fun sendCommand(command: String) {
        val label = getCommandLabel(command)
        val message = getString(R.string.commande_envoyee, label)
        Log.d("COMMAND", message)
        statusTextView.text = message
        sendToPC(command)
    }

    private fun sendToPC(command: String) {
        Thread {
            try {
                val socket = Socket(pcIp, pcPort)
                PrintWriter(socket.getOutputStream(), true).use { writer ->
                    writer.println(command)
                    writer.flush()
                    Thread.sleep(100)
                }
            } catch (e: Exception) {
                e.printStackTrace()
                runOnUiThread {
                    Toast.makeText(this, "Connection error: ${e.message}", Toast.LENGTH_SHORT).show()
                }
            }
        }.start()
    }

    private fun getCommandLabel(cmd: String): String {
        return when (cmd) {
            "E" -> "HAUT"
            "e" -> "BAS"
            "b" -> "GAUCHE"
            "B" -> "DROITE"
            "c" -> "AVANT BRAS HAUT"
            "C" -> "AVANT BRAS BAS"
            "r" -> "PINCE GAUCHE"
            "R" -> "PINCE DROITE"
            "O" -> "PINCE OUVERTE"
            "o" -> "PINCE FERMER"
            "m" -> "POIGNET HAUT"
            "M" -> "POIGNET BAS"
            "S" -> "STOP"
            "I" -> "RESET"
            else -> "Commande : $cmd"
        }
    }

    // ---------- Authentification TOTP ----------
    private fun showOtpPrompt() {
        val input = EditText(this)
        input.inputType = InputType.TYPE_CLASS_NUMBER
        input.hint = "Code Google Authenticator"

        AlertDialog.Builder(this)
            .setTitle("Authentification requise")
            .setMessage("Veuillez entrer le code à 6 chiffres depuis Google Authenticator")
            .setCancelable(false)
            .setView(input)
            .setPositiveButton("Valider") { _, _ ->
                val userCode = input.text.toString().trim()
                val expected = generateTotpCode(base32Secret)
                Log.d("OTP", "Code attendu : $expected")

                if (userCode == expected) {
                    Toast.makeText(this, "✅ Accès autorisé", Toast.LENGTH_SHORT).show()
                } else {
                    Toast.makeText(this, "❌ Code incorrect. Fermeture...", Toast.LENGTH_SHORT).show()
                    finish()
                }
            }
            .show()
    }

    private fun generateTotpCode(secret: String): String {
        val keyBytes = decodeBase32(secret)
        val key = SecretKeySpec(keyBytes, "HmacSHA1")

        val timeStep = (System.currentTimeMillis() / 1000) / 30
        val buffer = ByteBuffer.allocate(8).order(ByteOrder.BIG_ENDIAN).putLong(timeStep)
        val hmac = Mac.getInstance("HmacSHA1").apply { init(key) }
        val hash = hmac.doFinal(buffer.array())

        val offset = hash.last().toInt() and 0x0F
        val binary = ((hash[offset].toInt() and 0x7f) shl 24) or
                ((hash[offset + 1].toInt() and 0xff) shl 16) or
                ((hash[offset + 2].toInt() and 0xff) shl 8) or
                (hash[offset + 3].toInt() and 0xff)

        return String.format("%06d", binary % 1_000_000)
    }

    private fun decodeBase32(base32: String): ByteArray {
        val alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567"
        val cleaned = base32.replace("=", "").uppercase()
        val output = ByteArray(cleaned.length * 5 / 8)
        var buffer = 0
        var bitsLeft = 0
        var count = 0

        for (char in cleaned) {
            val value = alphabet.indexOf(char)
            if (value == -1) continue
            buffer = (buffer shl 5) or value
            bitsLeft += 5
            if (bitsLeft >= 8) {
                output[count++] = (buffer shr (bitsLeft - 8)).toByte()
                bitsLeft -= 8
            }
        }

        return output.copyOf(count)
    }
}
