package com.example.hellojni;

import java.util.HashMap;


import android.R.integer;
import android.app.Activity;
import android.content.Context;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbInterface;
import android.hardware.usb.UsbManager;
import android.util.Log;
import android.widget.Toast;

public class UsbCore extends Activity{


	public void OpenUsb()
	{
		Log.i("zeng", "open usb in usbcore.java");
		
		super.onCreate(null);
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
			
			for (int index=0; index < count; index++)
			{
				UsbInterface mInterface = mDevice.getInterface(index);
				
				//mInterface.describeContents
				
			}
			
			Log.i("zeng", mDevice.toString());
			Toast.makeText(getApplicationContext(), "/nUsbDevice:: "+mDevice.toString(), Toast.LENGTH_LONG).show();
			
			UsbDeviceConnection mConnection =  mManager.openDevice(mDevice);
			
		}
		
		
		
	}
	
}
