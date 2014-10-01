package com.example.appdemoone;

import android.content.BroadcastReceiver;
import android.content.ContentProvider;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class BroadcastTest {
	
	private static final String TAG = "BroadcastTest";
	
    public static BroadcastReceiver mUsbReceiver = new BroadcastReceiver() {
 
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();

            Log.i(TAG, "This is on Receive. And action is " + action);

        }
    };
    
}
