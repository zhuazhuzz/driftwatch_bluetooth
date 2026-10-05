package com.example.centralbleandroid

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.statusBarsPadding
import androidx.compose.foundation.layout.systemBarsPadding
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.tooling.preview.Preview
import androidx.hilt.lifecycle.viewmodel.compose.hiltViewModel
import androidx.lifecycle.compose.collectAsStateWithLifecycle
import com.example.centralbleandroid.components.permissions.BlePermissionGate
import com.example.centralbleandroid.ui.theme.CentralBLEAndroidTheme
import com.example.centralbleandroid.viewmodel.HomeViewModel
import dagger.hilt.android.AndroidEntryPoint


@AndroidEntryPoint
class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContent {
            CentralBLEAndroidTheme {
                Scaffold(
                    modifier = Modifier.fillMaxSize()
                ) { innerPadding ->
                    Box(modifier = Modifier.padding(innerPadding)) {
                        BlePermissionGate {
                            HomeRoute(
                                modifier = Modifier.padding(innerPadding)
                            )
                        }

                    }
                }
            }
        }
    }





//    override fun onResume() {
//        super.onResume()
//    }
//    override fun onPause() {
//        super.onPause()
//    }
//    override fun onDestroy() {
//        super.onDestroy()
//    }
}

@Composable
fun HomeRoute(modifier: Modifier = Modifier) {
    val vm: HomeViewModel = hiltViewModel()
    val on by vm.bluetoothOn.collectAsStateWithLifecycle()
    HomeScreen(bluetoothOn = on, modifier = modifier)
}

@Composable
fun HomeScreen(bluetoothOn: Boolean, modifier: Modifier = Modifier) {
    Text(
        text = "Bluetooth: ${if (bluetoothOn) "ON" else "OFF"}",
        modifier = modifier
    )
}

@Preview(showBackground = true)
@Composable
fun HomeScreenPreview() {
    CentralBLEAndroidTheme {
        HomeScreen(bluetoothOn = true)
    }
}