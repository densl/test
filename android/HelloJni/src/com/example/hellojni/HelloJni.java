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

import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.CharBuffer;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Locale;
import java.util.logging.SimpleFormatter;

import javax.xml.transform.Templates;


import android.R.integer;
import android.R.string;
import android.app.Activity;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.hardware.usb.UsbConstants;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbInterface;
import android.hardware.usb.UsbManager;
import android.util.Log;
import android.widget.TextView;
import android.widget.Toast;
import android.os.Bundle;
import android.os.Environment;
import android.hardware.usb.UsbEndpoint;

public class HelloJni extends Activity
{
	private static String TAG = "zeng";
	private static final void setLog(String msg)
	{
		Log.i("zeng", msg+ "  in HelloJni class");
		//

		String SDPATH = Environment.getExternalStorageDirectory().getPath() + "/";
		SimpleDateFormat gStamp = new SimpleDateFormat("MM-dd HH:mm:ss", Locale.ENGLISH);
		
		String str = gStamp.format(new Date()) + " HelloJni: " + msg + "\n";
		try {
			FileOutputStream fos =  new FileOutputStream(SDPATH + "test.txt", true);;
			fos.write( str.getBytes() );
			fos.close();
			
		} catch (FileNotFoundException e) {
			Log.i("zeng", "FileNotFoundException.");
			e.printStackTrace();			
		} catch (IOException e) {
			Log.i("zeng", "IOException.");
			e.printStackTrace();
		}
		
//		Toast.makeText(getApplicationContext(), msg, Toast.LENGTH_LONG).show();
	}
    /** Called when the activity is first created. */
	
	private UsbManager gUsbManager;
	private UsbDevice gUsbDevice;
	private UsbInterface gUsbInterface;
	private UsbDeviceConnection gUsbDeviceConnection;
	private UsbEndpoint gInEp;
	private UsbEndpoint gOutEp;
	private int gHandleToNative;
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setLog("OnCreate.");
        
        /* Create a TextView and set its content.
         * the text is retrieved by calling a native
         * function.
         */
        TextView  tv = new TextView(this);
        tv.setText( stringFromJNI() );
        setContentView(tv);
     
    }
    
    
    @Override
    protected void onStart() {
    	super.onStart();
    	
    	setLog("It is in onStart.");
    	
    	//dealwithUsb(3);
    	OpenUsb();
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

    //usb test
    public native String dealwithUsb(int handle);
    
    
    public native String  testo(android.app.Activity obj);
    /* this is used to load the 'hello-jni' library on application
     * startup. The library has already been unpacked into
     * /data/data/com.example.hellojni/lib/libhello-jni.so at
     * installation time by the package manager.
     */
    static {
        System.loadLibrary("hello-jni");
        setLog("This is static load library.");
    }
    
    
    public void OpenUsbxxx()
	{
		Log.i(TAG, "This is in OpenUsb().");
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
		
		Log.i(TAG, "End OpenUsb().");
		
	}
	
	

    public void OpenUsb() {
    	setLog("It is in OpenUsb function.");

    	UsbManager mUsbManager = (UsbManager) getSystemService(Context.USB_SERVICE);
    	gUsbManager = mUsbManager;
    	
    	//find usb device
    	HashMap<String, UsbDevice> mDeviceList = mUsbManager.getDeviceList();
    	setLog("UsbDevices: " + mDeviceList.size());
    	for (UsbDevice mUsbDevice : mDeviceList.values()) {
    		int maxUsbInterface = mUsbDevice.getInterfaceCount();

    		setLog("UsbDevice: " + mUsbDevice);
    		setLog("Max Device interface is " + maxUsbInterface);
    		for (int i = 0; i < maxUsbInterface; i++) {
    			UsbInterface mUsbInterface = mUsbDevice.getInterface(i);

    			setLog("UsbInterface " + i + ": " + mUsbInterface);
    			if (mUsbInterface.getInterfaceClass() == 7
    					&& mUsbInterface.getInterfaceSubclass() == 1
    					&& mUsbInterface.getInterfaceProtocol() == 2) {
    				gUsbInterface = mUsbInterface;
    				gUsbDevice = mUsbDevice;
    				
    				break;
    			}
    			if (mUsbInterface.getInterfaceClass() == 8
    					&& mUsbInterface.getInterfaceSubclass() == 6
    					&& mUsbInterface.getInterfaceProtocol() == 80) {
    				gUsbInterface = mUsbInterface;
    				gUsbDevice = mUsbDevice;
    				
    				break;
    			}
    		}
    	}//end for
    	
    	//gain permission
    	if (gUsbDevice == null || gUsbInterface == null)
    	{
    		setLog("Failed get devices.");
    		return;
    	}
    	
    	if ( gUsbManager.hasPermission(gUsbDevice) )
    	{
    		setLog("Current device hasn't permission.");
    		
    		PendingIntent mPendingIntentUsb;
    		mPendingIntentUsb = PendingIntent.getBroadcast(this, 0, new Intent(UsbManager.EXTRA_PERMISSION_GRANTED), 0);
    		gUsbManager.requestPermission(gUsbDevice, mPendingIntentUsb);
    		
    		setLog("PendingIntent: " + mPendingIntentUsb);

    		if (!gUsbManager.hasPermission(gUsbDevice) )
    		{
    			setLog("Gain usb permission failed.");
    			return;
    		}
    		setLog("Already gain usb permission.");	
    		
    	}
    	else {
    		setLog("Current device has permission.");
    	}
    	
    	for (int i=0; i<gUsbInterface.getEndpointCount(); i++)
    	{
    		UsbEndpoint ep = gUsbInterface.getEndpoint(i);
    		setLog("UsbEndpoint: "+ep+" type: "+ep.getType()+" number: "+ep.getEndpointNumber());
    		switch (ep.getDirection()) {
    		case UsbConstants.USB_DIR_IN:
    			gInEp = ep;
    			break;
    		case UsbConstants.USB_DIR_OUT:
    			gOutEp = ep;
    			break;
    		default:
    			break;
    		}
    		
    	}
    	
    	
    	//
    	gUsbDeviceConnection = gUsbManager.openDevice(gUsbDevice);
    	setLog("UsbDeviceConnection: "+gUsbDeviceConnection);
    	
    	if ( !gUsbDeviceConnection.claimInterface(gUsbInterface, true) )
    	{
    		setLog("claimInterface failed.");
    		return;
    	}
    	
    	//jni part
    	setLog( dealwithUsb(gHandleToNative) );
    	if (true )
    		return;
    	//
    	
    	//data transfer
    	byte []inBuf = new byte[1024];
    	inBuf[0] = 0x1d;
    	inBuf[1] = 0x53;
    	inBuf[2] = 0x4c;
    	inBuf[3] = 0x44;
    	
    	int ret = gUsbDeviceConnection.bulkTransfer(gOutEp, inBuf, 4, 0);
    	setLog("bulkTransfer ret: "+ret);
    	
    	
    	byte []outBuf = new byte[1024];
    	ret = gUsbDeviceConnection.bulkTransfer(gInEp, outBuf, outBuf.length, 3000);
    	setLog("outBuf: "+new String(outBuf));
    	
    	byte []rawDesc = new byte[1024];
    	rawDesc = gUsbDeviceConnection.getRawDescriptors();
    	String st = "";
    	for (byte i: rawDesc)
    	{
    		st = st + Integer.toHexString(i);
    	}		
    	setLog("rawDesc: "+st);
    	
    	byte []stdstatus = new byte[1024];
    	setLog("stdstatus length: "+stdstatus.length);
//        public int controlTransfer(int requestType, int request, int value,
//                int index, byte[] buffer, int length, int timeout)
    	ret = gUsbDeviceConnection.controlTransfer(0xa1, 00, 00, 00, stdstatus,  stdstatus.length, 500);
    	
    	setLog("ret: "+ret+"  std status: "+new String(stdstatus) );
    	setLog(stdstatus.toString());
    	
    	gHandleToNative = gUsbDeviceConnection.getFileDescriptor();
    	
    	
    }


    



}

//if ( mUsbDevice.getDeviceClass() == 7 && mUsbDevice.getDeviceSubclass() == 1 && mUsbDevice.getDeviceProtocol() == 2)
//{
//	//found device
//	gUsbDevice = mUsbDevice;
//}

//define in interface
//if ( mUsbDevice.getDeviceClass() == 0 && mUsbDevice.getDeviceSubclass() == 0 && mUsbDevice.getDeviceProtocol() == 0)
//{


//Activity topassActivity = this;
//tv.setText( testo(topassActivity) );
//setContentView(tv);


//Log.i(TAG, stringFromJNIandInt(9));
//
//
//
//OpenUsb();
//
//
//UsbCore usbCore = new UsbCore();
//usbCore.OpenUsb();

//Toast.makeText(getApplicationContext(), "interface class is "+mInterface.getInterfaceClass() +
//		"#interface sub class#" + mInterface.getInterfaceSubclass() + 
//		"#interface protocol#" + mInterface.getInterfaceProtocol() + 
//		"#endpoint count" + mInterface.getEndpointCount() +" string is " +
//		mInterface.toString(), Toast.LENGTH_LONG).show();
