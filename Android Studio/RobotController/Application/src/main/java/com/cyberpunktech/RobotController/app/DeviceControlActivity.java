/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.cyberpunktech.RobotController.app;

import android.app.Activity;
import android.bluetooth.BluetoothGattCharacteristic;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Button;
import android.widget.TextView;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.widget.Toast;


import java.util.ArrayList;

/**
 * For a given BLE device, this Activity provides the user interface to connect, display data,
 * and display GATT services and characteristics supported by the device.  The Activity
 * communicates with {@code BluetoothLeService}, which in turn interacts with the
 * Bluetooth LE API.
 */
public class DeviceControlActivity extends Activity  implements SensorEventListener {
    private final static String TAG = DeviceControlActivity.class.getSimpleName();

    public static final String EXTRAS_DEVICE_NAME = "DEVICE_NAME";
    public static final String EXTRAS_DEVICE_ADDRESS = "DEVICE_ADDRESS";

    private String mDeviceName;
    private String mDeviceAddress;
    private BluetoothLeService mBluetoothLeService;
    private ArrayList<ArrayList<BluetoothGattCharacteristic>> mGattCharacteristics =
            new ArrayList<ArrayList<BluetoothGattCharacteristic>>();
    private boolean mConnected = false;
    private BluetoothGattCharacteristic mNotifyCharacteristic;

    private final String LIST_NAME = "NAME";
    private final String LIST_UUID = "UUID";

    private Button StatusButton;
    private SensorManager sensorManager;
    private Sensor accelerometer;
    private long lastUpdate;
    private static final int PERIOD = 200;
    private TextView textView;
    private float pitch, roll;
    private static final float limit = 20.0f;



    // Code to manage Service lifecycle.
    private final ServiceConnection mServiceConnection = new ServiceConnection() {


        @Override
        public void onServiceConnected(ComponentName componentName, IBinder service) {
            mBluetoothLeService = ((BluetoothLeService.LocalBinder) service).getService();
            if (!mBluetoothLeService.initialize()) {
                Log.e(TAG, "Unable to initialize Bluetooth");
                finish();
            }
            // Automatically connects to the device upon successful start-up initialization.
            mBluetoothLeService.connect(mDeviceAddress);
        }

        @Override
        public void onServiceDisconnected(ComponentName componentName) {
            mBluetoothLeService = null;
        }
    };

    // Handles various events fired by the Service.
    // ACTION_GATT_CONNECTED: connected to a GATT server.
    // ACTION_GATT_DISCONNECTED: disconnected from a GATT server.
    // ACTION_GATT_SERVICES_DISCOVERED: discovered GATT services.
    // ACTION_DATA_AVAILABLE: received data from the device.  This can be a result of read
    //                        or notification operations.
    private final BroadcastReceiver mGattUpdateReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            if (BluetoothLeService.ACTION_GATT_CONNECTED.equals(action)) {
                mConnected = true;
                //updateConnectionState(R.string.connected);
                invalidateOptionsMenu();
            } else if (BluetoothLeService.ACTION_GATT_DISCONNECTED.equals(action)) {
                mConnected = false;
                //updateConnectionState(R.string.disconnected);
                invalidateOptionsMenu();
                //clearUI();
            } else if (BluetoothLeService.ACTION_GATT_SERVICES_DISCOVERED.equals(action)) {
                // Show all the supported services and characteristics on the user interface.
                //displayGattServices(mBluetoothLeService.getSupportedGattServices());
            } else if (BluetoothLeService.ACTION_DATA_AVAILABLE.equals(action)) {
                //displayData(intent.getStringExtra(BluetoothLeService.EXTRA_DATA));
            }
        }
    };

    public void sendMessage(String message)
 {
     mBluetoothLeService.writeCharacteristic(message);
 }


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.gatt_services_characteristics);

        final Intent intent = getIntent();
        mDeviceName = intent.getStringExtra(EXTRAS_DEVICE_NAME);
        mDeviceAddress = intent.getStringExtra(EXTRAS_DEVICE_ADDRESS);

        getActionBar().setTitle(mDeviceName);
        getActionBar().setDisplayHomeAsUpEnabled(true);
        Intent gattServiceIntent = new Intent(this, BluetoothLeService.class);
        bindService(gattServiceIntent, mServiceConnection, BIND_AUTO_CREATE);

        Activity activityX = this;
        sensorManager = (SensorManager) activityX.getSystemService(Context.SENSOR_SERVICE);
        if((accelerometer = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER)) == null)
        {
            DeviceControlActivity activity = this;
            Toast.makeText(activity, "Accelerometer is not available", Toast.LENGTH_LONG).show();
            activity.finish();
        }

        StatusButton = (Button) findViewById(R.id.button_status);
        textView = (TextView) findViewById(R.id.textView);
    }

    @Override
    public void onSensorChanged(SensorEvent event)
    {
        if(event.sensor.getType() == Sensor.TYPE_ACCELEROMETER)
        {
            long actualTime = System.currentTimeMillis();
            if(actualTime - lastUpdate > PERIOD)
            {
                lastUpdate = actualTime;
                float x = event.values[0], y = event.values[1], z = event.values[2];
                pitch = (float)Math.atan2((double)y, Math.sqrt((double)(x*x + z*z))) * 57.29577951f;
                roll = (float)Math.atan2((double)x, Math.sqrt((double)(y*y + z*z))) * 57.29577951f;
                textView.setText("x: " + String.valueOf(x) + "\ny: " + String.valueOf(y) + "\nz: "
                        + String.valueOf(z) + "\nroll: " + String.valueOf(roll) + "\npitch: " + String.valueOf(pitch));
                String message = "";

                if(pitch<25.0f && pitch>-25.0f && roll<25.0f && roll>-25.0f)
                {
                    message="stop\n";
                    StatusButton.setText("STOP");
                }
                if(pitch>limit)
                {
                    message="backward\n";
                    StatusButton.setText("BACKWARD");
                }else if(pitch<-limit)
                {
                    message="forward\n";
                    StatusButton.setText("FORWARD");
                }else if(roll<-limit)
                {
                    message="right\n";
                    StatusButton.setText("RIGHT");
                }else if(roll>limit)
                {
                    message="left\n";
                    StatusButton.setText("LEFT");
                }

                if (mConnected==true)
                {
                    sendMessage(message);
                }
            }

        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        sensorManager.registerListener((SensorEventListener) this, accelerometer, SensorManager.SENSOR_DELAY_UI);
        lastUpdate = System.currentTimeMillis();
        registerReceiver(mGattUpdateReceiver, makeGattUpdateIntentFilter());
        if (mBluetoothLeService != null) {
            final boolean result = mBluetoothLeService.connect(mDeviceAddress);
            Log.d(TAG, "Connect request result=" + result);
        }
    }

    @Override
    protected void onPause() {
        sensorManager.unregisterListener((SensorEventListener) this);
        super.onPause();
        unregisterReceiver(mGattUpdateReceiver);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        unbindService(mServiceConnection);
        mBluetoothLeService = null;
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.gatt_services, menu);
        if (mConnected) {
            menu.findItem(R.id.menu_connect).setVisible(false);
            menu.findItem(R.id.menu_disconnect).setVisible(true);
        } else {
            menu.findItem(R.id.menu_connect).setVisible(true);
            menu.findItem(R.id.menu_disconnect).setVisible(false);
        }

        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch(item.getItemId()) {
            case R.id.menu_connect:
                mBluetoothLeService.connect(mDeviceAddress);
                return true;
            case R.id.menu_disconnect:
                mBluetoothLeService.disconnect();
                return true;
            case android.R.id.home:
                onBackPressed();
                return true;
        }
        return super.onOptionsItemSelected(item);
    }




    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy)
    {

    }


    private static IntentFilter makeGattUpdateIntentFilter() {
        final IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_CONNECTED);
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_DISCONNECTED);
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_SERVICES_DISCOVERED);
        intentFilter.addAction(BluetoothLeService.ACTION_DATA_AVAILABLE);
        return intentFilter;
    }

}
