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
    static {
        System.loadLibrary("hello-jni");
        setLog("This is static load library.");
    }
		
    //usb jni test
	private native String dealwithUsb(int handle, int inep, int outep); 
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
    
   	
    private static String TAG = "zeng";
	private UsbManager gUsbManager;
	private UsbDevice gUsbDevice;
	private UsbInterface gUsbInterface;
	private UsbDeviceConnection gUsbDeviceConnection;
	private UsbEndpoint gInEp;
	private UsbEndpoint gOutEp;
	//native use
	private int gHandleNative;
	private int gInEpNative;
	private int gOutEpNative;
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setLog("OnCreate.");
        
        TextView  tv = new TextView(this);
        tv.setText( "Test" );
        setContentView(tv);    
    }
    
    
    @Override
    protected void onStart() {
    	super.onStart();
    	
    	setLog("It is in onStart.");
    	OpenUsb();
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
    	
    	//get endpoint
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
    	
    	//get connection
    	gUsbDeviceConnection = gUsbManager.openDevice(gUsbDevice);
    	setLog("UsbDeviceConnection: "+gUsbDeviceConnection);

    	if ( !gUsbDeviceConnection.claimInterface(gUsbInterface, true) )
    	{
    		setLog("claimInterface failed.");
    		return;
    	}
    	
    	
    	//---------------------------------------------------------------------------------
    	// data/command transfer part
    	//control transfer
    	int ret = 0;
    	byte []msg = new byte[1024];
    	ret = gUsbDeviceConnection.controlTransfer(0xa1, 00, 00, 00, msg,  msg.length, 500);
    	setLog("ctrl ret: "+ret+"  msg: "+new String(msg) );
    	
    	//write
    	msg = "This is a test from java code.".getBytes();
    	ret = gUsbDeviceConnection.bulkTransfer(gOutEp, msg, msg.length, 500);
    	
    	
    	// env part
    	//set jni env
    	gHandleNative = gUsbDeviceConnection.getFileDescriptor();
    	gInEpNative = gInEp.getAddress();
    	gOutEpNative = gOutEp.getAddress();   	
    	setLog("handle: "+gHandleNative+"  In ep: "+gInEpNative+"  Out ep: "+gOutEpNative);
    	
 	
    	String retJni = dealwithUsb(gHandleNative, gInEpNative, gOutEpNative);
    	setLog( retJni );
    	
    }
}
