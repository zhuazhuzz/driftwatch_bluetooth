package com.example.centralbleandroid.model

data class ScannedDevice(
    val address: String,
    val name: String?,
    val rssi: Int
)