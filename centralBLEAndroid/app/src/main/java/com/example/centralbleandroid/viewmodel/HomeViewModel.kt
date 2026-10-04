package com.example.centralbleandroid.viewmodel

import android.bluetooth.BluetoothManager
import androidx.lifecycle.ViewModel
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow


@HiltViewModel
class HomeViewModel @Inject constructor(btManager: BluetoothManager): ViewModel() {
    private val _bluetoothOn = MutableStateFlow(btManager.adapter?.isEnabled == true)
    val bluetoothOn: StateFlow<Boolean> = _bluetoothOn.asStateFlow()

}