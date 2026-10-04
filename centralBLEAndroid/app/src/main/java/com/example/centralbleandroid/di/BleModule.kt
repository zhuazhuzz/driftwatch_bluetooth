package com.example.centralbleandroid.di

import android.bluetooth.BluetoothManager
import android.content.Context
import dagger.Module
import dagger.Provides
import dagger.hilt.InstallIn
import dagger.hilt.android.qualifiers.ApplicationContext
import dagger.hilt.components.SingletonComponent
import javax.inject.Singleton


@Module
@InstallIn(SingletonComponent::class)
object BleModule {

    @Provides
    @Singleton
    fun provideBluetoothManager(@ApplicationContext context: Context): BluetoothManager =
        context.getSystemService(BluetoothManager::class.java)
}