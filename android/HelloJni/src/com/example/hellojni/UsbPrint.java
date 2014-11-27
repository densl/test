package com.example.hellojni;

import java.util.HashMap;


import android.app.Activity;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbEndpoint;
import android.hardware.usb.UsbInterface;
import android.hardware.usb.UsbManager;
import android.util.Log;
import android.widget.Toast;


public class UsbPrint {
	private Activity activity;
	private PendingIntent pendingIntent;
	private static final String ACTION_USB_PERMISSION = "com.android.example.USB_PERMISSION";
	private UsbInterface usbInterface;
	private UsbDeviceConnection connection;
	private UsbEndpoint outEndpoint;
	private UsbEndpoint inEndpoint;
	private UsbManager usbManager;
	private UsbDevice usbDevice;
	private boolean usb_flag = false;
	private byte[] sta=new byte[1];
	private byte[] pName=new byte[128];
	
	
	
	public UsbPrint(){
	
		Log.i("zeng", "It is in UsbPrint construction.");
	}
	
	public void SetAct(Activity activity){
		Log.i("zeng", "It is in setAct.");
		this.activity=activity;
		
		this.usbManager = ((UsbManager)activity.getSystemService("usb"));

				
		Log.i("zeng", "End setAct.");
	}

	  public boolean OpenPrenission()
	  {
	    if (this.usbManager.hasPermission(this.usbDevice))
	    {
	      return true;
	    }

	    this.usbManager.requestPermission(this.usbDevice, this.pendingIntent);
	    return true;
	  }
	public void OpenUsb(){
		
		Log.i("zeng", "It is in OpenUsb");
		
		
		
		if(ScannUSB()==true && usb_flag==false){
			this.connection = this.usbManager.openDevice(this.usbDevice);
		    this.usbInterface = this.usbDevice.getInterface(0);
		    this.outEndpoint = this.usbInterface.getEndpoint(0);
		    this.inEndpoint = this.usbInterface.getEndpoint(1);
		    
		    OpenPrenission();
		    
		    
		    byte []buf = "hello, this is a test.".getBytes();
		    this.connection.claimInterface(this.usbInterface, true);
		    int ba=this.connection.bulkTransfer(this.outEndpoint, buf, buf.length, 3000);
		    
		    
		    byte[] aass = new byte[512];
	        this.connection.controlTransfer(161, 0, 0, 0, aass, 128, 1000);
	        String n=new String(aass);
	        Log.i("zeng", n);
	        
	        Toast.makeText(this.activity.getApplicationContext(), n, Toast.LENGTH_LONG).show();
	   
	        
	        if (n.contains("DASCOM")||n.contains("Aisino")) {
	        	this.usb_flag = true;
//	            return true;
	        }
	        this.usb_flag = true;
		}
//		    return false;
//		}else if(usb_flag==true){
////			return true;
//		}else{		
////			return false;
//		}		
	}
//是否连接usb	
	public boolean IsAvailable(){
		return ScannUSB();
	}
//是否打开usb设备端口	
	public boolean IsOpenUsb(){
		return usb_flag;
	}
	public void CloseUsb(){
		if(ScannUSB()==true){
			this.connection.releaseInterface(this.usbInterface);			
		}
		usb_flag=false;
	}
	public boolean ScannUSB()
	  {
	    HashMap<String, UsbDevice> map = this.usbManager.getDeviceList();

	    for (UsbDevice device : map.values())
	    {
	      if ((2237 == device.getVendorId()) || (8401 == device.getVendorId())) {
	        this.usbDevice = device;
	        return true;
	      }
	    }

	    return false;
	  }
	
	
	public boolean UsbRead(byte []buf, int length) {
		Log.i("zeng", "It is in UsbRead.");
	    if (this.usb_flag) {
		      if (length == 0) {		        
		        int b=this.connection.controlTransfer(161, 1, 0, 0, sta, 1, 1000);
		        if(b>=0)
		        	return true;
		        else
		        	return false;
		      }
		      if (length == 1) {		        
		        int b=this.connection.controlTransfer(161, 0, 0, 0, pName, 128, 1000);
		        if(b>=0)
		        	return true;
		        else
		        	return false;
		      }
	     return false;
	    }else{
	    	return false;
	    }	   
	 }
	
	public boolean UsbWrite(byte []buf, int length)
	  {
		Log.i("zeng", "It is in UsbWrite.");
		String temp;
		temp = String.format("%c%c", buf[0], buf[1]);
		Log.i("zeng", temp);
		if (this.usb_flag) {
//	    this.connection.claimInterface(this.usbInterface, true);
	    int ba=this.connection.bulkTransfer(this.outEndpoint, buf, length, 3000);
		    if(ba>=0)
	        	return true;
	        else
	        	return false;
		}else{
			return false;
		}
		//return true;
	  }
	
	public String SayHello()
	{
		String test  = "hello from UsbPrint.";
		return test;
	}
}
