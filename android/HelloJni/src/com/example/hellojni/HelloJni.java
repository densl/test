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
import java.util.Set;
import java.util.logging.SimpleFormatter;

import javax.xml.transform.Templates;

import com.study.common.LocalUsb;




import android.R.integer;
import android.R.string;
import android.app.Activity;
import android.app.PendingIntent;
import android.bluetooth.BluetoothA2dp;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.res.Resources;
import android.graphics.BlurMaskFilter;
import android.hardware.usb.UsbConstants;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbInterface;
import android.hardware.usb.UsbManager;
import android.telephony.TelephonyManager;
import android.text.Layout;
import android.text.format.Time;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.os.Messenger;
import android.os.RemoteException;
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
	//
	private Resources grs;
	private TextView gtv;
	
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setLog("OnCreate.");
        
        setContentView(R.layout.main_layout);
        
        //grs.
        gtv = new TextView(this);

//        TextView  tv = new TextView(this);
//        tv.setText( "Test" );
//        setContentView(tv);    
    }
    
    
    @Override
    protected void onStart() {
    	super.onStart();
    	
    	setLog("It is in onStart.");
    	
    	
    	Button mTest = (Button)findViewById(R.id.test);
    	
    	mTest.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View v) {
				Message msg = Message.obtain(mHandler, 1);
				msg.obj = "onClick Message";
				mHandler.sendMessage(msg);
				setLog("onClick.");
			}
		});
    	
    	
//    	mHandler.postDelayed(new Runnable() {	
//			@Override
//			public void run() {	
//				
//			}
//		}, 5000);
    
    	
    	
    	OpenUsb();
    	//getBuildInfo();
    	//getPhoneInfo();
    	//bttest();
    	
    	
    	registerReceiver(mBroadcastReceiver, initIntentFilter());
    }
    
    
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
    	super.onActivityResult(requestCode, resultCode, data);
    	setLog("It is in onActivityResult");
    	
    	setLog("requestCode: "+requestCode+" resultCode: "+resultCode+" intent: "+data);
    	
    	switch (requestCode) {
		case 0x1:
			//bt part
			
			break;

		default:
			break;
		}
    }
   
    private IntentFilter initIntentFilter() {
    	IntentFilter mFilter = new IntentFilter();
    	mFilter.addAction(BluetoothAdapter.ACTION_STATE_CHANGED);
		return mFilter;
	}
    private final BroadcastReceiver mBroadcastReceiver = new BroadcastReceiver(){
    	public void onReceive(Context context, Intent intent) {
    		final String action = intent.getAction();
    		setLog("onReceive Action: "+action);
    		
    		if (BluetoothAdapter.ACTION_STATE_CHANGED.equals(action))
    		{
    			int state = intent.getIntExtra(BluetoothAdapter.EXTRA_STATE, BluetoothAdapter.ERROR);
    			setLog("extra state: "+Integer.valueOf(state).toString() );
    		}
    	}
    };
    
    private void bttest()
    {
    	setLog("It is in bluetooth test");
    	
    	BluetoothAdapter mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
    	if (mBluetoothAdapter == null)
    	{
    		setLog("BluetoothAdapter is null");
    		return;
    	}

    	//enable bluetooth
    	if ( !mBluetoothAdapter.isEnabled() )
    	{
    		Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
    		startActivityForResult(enableIntent, 0x1);
    	}
    	
    	//show bluetooth host name and address 
    	setLog("Bt host name: "+mBluetoothAdapter.getName()+" Host address: "+mBluetoothAdapter.getAddress());
    	
    	// bonded device
    	Set<BluetoothDevice> pairedDevices = mBluetoothAdapter.getBondedDevices();
    	Iterator<BluetoothDevice> mIterator = pairedDevices.iterator();
    	while( mIterator.hasNext() )
    	{
    		BluetoothDevice mBluetoothDevice = mIterator.next();
    		
    		setLog("Bonded BT: "+mBluetoothDevice.getName()+" addr: "+mBluetoothDevice.getAddress() );
    	}
    	
    	//
    	
    	
    	
    	
    	

    	

    	
    	
    	
    	
    	
    	
    	//debug log
    
    	
    	
    	
    }
    private void getPhoneInfo()
    {
    	setLog("It is in getPhoneInfo.");
    	
    	TelephonyManager phoneManager = (TelephonyManager)getSystemService(Context.TELEPHONY_SERVICE);
    	
    	//android.Manifest.permission#READ_PHONE_STATE
    	String softwareversion = phoneManager.getDeviceSoftwareVersion();
    	String devid = phoneManager.getDeviceId();
    	//String celllocation = phoneManager.getCellLocation().toString();
    	
    	
    	String phonetype = null;
    	switch ( phoneManager.getPhoneType() )
    	{
    	case TelephonyManager.PHONE_TYPE_CDMA:
    		phonetype = "CDMA";
    		break;
    	case TelephonyManager.PHONE_TYPE_GSM:
    		phonetype = "GSM";
    		break;
    	case TelephonyManager.PHONE_TYPE_SIP:
    		phonetype = "SIP";
    		break;
    	case TelephonyManager.PHONE_TYPE_NONE:
    		phonetype = "NONE";
    		break;
    	default:
    		phonetype = Integer.toString( phoneManager.getPhoneType() );
    		break;
    	}
    	String networkname = phoneManager.getNetworkOperatorName();
    	String networkcountryiso = phoneManager.getNetworkCountryIso();
    	String networktype = Integer.toString( phoneManager.getNetworkType() );
    	String subscripid = phoneManager.getSubscriberId();
    	String line1num = phoneManager.getLine1Number();
    	String datastate = Integer.toString( phoneManager.getDataState() );
    	   	
    	String str = "software version: "+softwareversion+"\ndevice id: "+devid+"\nphonetype: "+phonetype+"\nnetworkname: "+
    			networkname+"\nnetworkcountryiso: "+networkcountryiso+"\nnetworktype: "+networktype+"\nsubscripid: "+subscripid+
    			"\nline1 number: "+line1num+"\ndata state: "+datastate;
    	
    	//show on screen
    	Message msg = Message.obtain(mHandler, 1);
    	msg.obj = str; 	
    	try {
			mMessager.send(msg);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
    }
    
    private void getBuildInfo()
    {
    	setLog("It is in getBuildInfo");	
       	
    	String board = Build.BOARD;
    	String bootloader = Build.BOOTLOADER;
    	String brand = Build.BRAND;
    	String cpu_abi = Build.CPU_ABI;
    	String cpu_abi2 = Build.CPU_ABI2;
    	String device = Build.DEVICE;
    	String display = Build.DISPLAY;
    	String fingerprint = Build.FINGERPRINT;
    	String hardware = Build.HARDWARE;
    	String host = Build.HOST;
    	String id = Build.ID;
    	String manufacturer = Build.MANUFACTURER;
    	String model = Build.MODEL;
    	String product = Build.PRODUCT;
    	String radio = Build.getRadioVersion();
    	String serial = Build.SERIAL;
    	String tags = Build.TAGS;
    	String type = Build.TYPE;
    	String unknown = Build.UNKNOWN;
    	String user = Build.USER;
    	long time = Build.TIME;
    	
    	Time stime = new Time();
    	stime.set(time);
    	
    	//
    	String str = "board: "+board+"\nbootloader: "+bootloader+"\nbrand: "+brand+"\ncpu_abi: "+cpu_abi+"\ncpu_abi2: "+cpu_abi2+"\ndevice: "+device+
    			"\ndisplay: "+display+"\nfingerprint: "+fingerprint+"\nhardware: "+hardware+"\nhost: "+host+"\nid: "+id+"\nmanufacturer: "+manufacturer+
    			"\nmodel: "+model+"\nproduct: "+product+"\nradio: "+radio+"\nserial: "+serial+"\ntags: "+tags+"\ntype: "+type+"\nunknow: "+unknown+
    			"\nuser: "+user+"\ntime: "+stime;
    	
    	
    	//show on screen
    	Message msg = Message.obtain(mHandler, 1);
    	msg.obj = str; 	
    	try {
			mMessager.send(msg);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
    
    }

    @Override
    protected void onDestroy() {
    	mHandler.removeCallbacksAndMessages(null);
    	super.onDestroy();
    }
    
    
//    private final Handler mHandler 
//    private final Messenger mMesseger= new Messenger( new MHandler() ); 
//    private class MHandler extends Handler{
//    	
//    }
	private final Handler mHandler = new Handler(){
        @Override
        public void handleMessage(Message msg) {
        	
        	setLog("handleMessage");
        	setLog("msg.what: " + msg.what);
        	setLog("msg.obj: "+ msg.obj);
        	
            switch (msg.what) {
                case 1:
                    Log.i(TAG, "msg.what: 1");
                    TextView tv = new TextView(getApplicationContext());
                    tv.append( msg.obj.toString() );
                    setContentView(tv);
                    
                    break;
                case 2:
                    Log.i(TAG, "msg.what: 2");
                    break;
                default:
                	super.handleMessage(msg);                 
            }
        }
    };
    private Messenger mMessager = new Messenger(mHandler);
    
    private void OpenUsb() {
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
    	
//    	//write
//    	msg = "This is a test from java code.".getBytes();
//    	ret = gUsbDeviceConnection.bulkTransfer(gOutEp, msg, msg.length, 500);
    	
    	
    	// env part
    	//set jni env
    	gHandleNative = gUsbDeviceConnection.getFileDescriptor();
    	gInEpNative = gInEp.getAddress();
    	gOutEpNative = gOutEp.getAddress();   	
    	setLog("handle: "+gHandleNative+"  In ep: "+gInEpNative+"  Out ep: "+gOutEpNative);
    	
 	
    	// Test local usb
    	//#####################################################################
        ret = LocalUsb.nsetupEnv(gHandleNative, gInEpNative, gOutEpNative);
        setLog("nsetupEnv ret: "+ret);
        ret = LocalUsb.ngetStatus();
        setLog("ngetStatus ret: "+ret); 
        
        byte[] buf = "hello".getBytes();
        ret = LocalUsb.nwrite(buf, buf.length, 3000);
        setLog("nwrite ret: "+ret);
        LocalUsb.nclose();
        //##################################################################### 

//    	String retJni = dealwithUsb(gHandleNative, gInEpNative, gOutEpNative);
//    	setLog( retJni );
    	
    }
    
    
    
}
