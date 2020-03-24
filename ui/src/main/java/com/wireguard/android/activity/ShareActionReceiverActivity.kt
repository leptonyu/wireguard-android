/*
 * Copyright © 2020 WireGuard LLC. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

package com.wireguard.android.activity

import android.content.Intent
import android.os.Bundle
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import com.wireguard.android.R
import com.wireguard.android.util.DownloadsFileSaver
import java.io.PrintWriter

class ShareActionReceiverActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        val receievedIntent = requireNotNull(intent) { "This activity must be launched with a valid intent" }
        if (receievedIntent.action == Intent.ACTION_SEND) {
            (intent.extras?.get(Intent.EXTRA_TEXT) as String?)?.let { text ->
                val outputFile = DownloadsFileSaver.save(this, "wireguard-log.txt", "text/plain", true)
                PrintWriter(outputFile.outputStream).use {
                    it.println(text)
                }
                outputFile.outputStream.close()
                Toast.makeText(this, getString(R.string.log_export_success, outputFile.fileName), Toast.LENGTH_LONG).show()
            }
        }
        finish()
    }

    companion object {
        const val CATEGORY_LOG_SAVE = "com.wireguard.android.category.LOG_SAVE_TARGET"
    }
}
