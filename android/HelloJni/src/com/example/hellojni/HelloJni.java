/*
 * Copyright (C) 2009 The Android Open Source Project
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
package com.example.hellojni;

import java.nio.CharBuffer;
import java.util.HashMap;

import android.app.Activity;
import android.content.Context;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbInterface;
import android.hardware.usb.UsbManager;
import android.util.Log;
import android.widget.TextView;
import android.widget.Toast;
import android.os.Bundle;
import android.hardware.usb.UsbEndpoint;

public class HelloJni extends Activity
{
	public static String TAG = "zeng";
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        /* Create a TextView and set its content.
         * the text is retrieved by calling a native
         * function.
         */
        
        Toast.makeText(getApplicationContext(), "This is onCreate", Toast.LENGTH_LONG).show();
        Log.i(TAG, "This is onCreate.");
        
        
        TextView  tv = new TextView(this);
        tv.setText( stringFromJNIandInt(9) );
        setContentView(tv);
        
        
        Log.i(TAG, stringFromJNIandInt(9));
        
        
        //OpenUsb();
        
        
        
    }

    /* A native method that is implemented by the
     * 'hello-jni' native library, which is packaged
     * with this application.
     */
    public native String  stringFromJNI();

    //Java_com_example_hellojni_HelloJni_stringFromJNIandInt
    public native String stringFromJNIandInt(int x);
    /* This is another native method declaration that is *not*
     * implemented by 'hello-jni'. This is simply to show that
     * you can declare as many native methods in your Java code
     * as you want, their implementation is searched in the
     * currently loaded native libraries only the first time
     * you call them.
     *
     * Trying to call this function will result in a
     * java.lang.UnsatisfiedLinkError exception !
     */
    public native String  unimplementedStringFromJNI();

    /* this is used to load the 'hello-jni' library on application
     * startup. The library has already been unpacked into
     * /data/data/com.example.hellojni/lib/libhello-jni.so at
     * installation time by the package manager.
     */
    static {
        System.loadLibrary("hello-jni");
        Log.i(TAG, "This is static load library.");
    }
    
	public void OpenUsb()
	{
		// get usb manager ********************************
		UsbManager mManager = (UsbManager) getSystemService(Context.USB_SERVICE);
		
		// get device list ********************************
		HashMap<String, UsbDevice> mDeviceList = mManager.getDeviceList(); 
	
		int maxCount;
		maxCount = mDeviceList.size();
		
		Toast.makeText(getApplicationContext(), "max count of usb device is "+maxCount, Toast.LENGTH_SHORT).show();
		
		//int index;
		//UsbInterface mInterface = (UsbInterface)mDeviceList.get(0);
		for (UsbDevice mDevice : mDeviceList.values()) {	
			int count = mDevice.getInterfaceCount();
			Toast.makeText(getApplicationContext(), "max usb interface is "+count, Toast.LENGTH_SHORT).show();
			
			if (count == 8)
				continue;
			
			for (int index=0; index < count; index++)
			{
				UsbInterface mInterface = mDevice.getInterface(index);
				
				Log.i("zeng", "interface class is "+mInterface.getInterfaceClass() +
						"#interface sub class#" + mInterface.getInterfaceSubclass() + 
						"#interface protocol#" + mInterface.getInterfaceProtocol() + 
						"#endpoint count" + mInterface.getEndpointCount() +" string is " +
						mInterface.toString());
				
				//Toast.makeText(getApplicationContext(), "interface class is "+mInterface.getInterfaceClass() +
				//		"#interface sub class#" + mInterface.getInterfaceSubclass() + 
				//		"#interface protocol#" + mInterface.getInterfaceProtocol() + 
				//		"#endpoint count" + mInterface.getEndpointCount() +" string is " +
				//		mInterface.toString(), Toast.LENGTH_LONG).show();
						
				
				//mInterface.describeContents
				
				UsbDeviceConnection mConnection =  mManager.openDevice(mDevice);
				
				mConnection.claimInterface(mInterface, false);
				
				byte []buffer = "hello".getBytes();
				
				mConnection.bulkTransfer(mInterface.getEndpoint(0), buffer, 5, 1000);
				
			}
			
			Log.i("zeng", mDevice.toString());
			Toast.makeText(getApplicationContext(), "UsbDevice:: "+mDevice.toString(), Toast.LENGTH_LONG).show();
			

			
			//UsbDeviceConnection mConnection =  mManager.openDevice(mDevice);
			
		}
		
		
		
	}
	
	

}
