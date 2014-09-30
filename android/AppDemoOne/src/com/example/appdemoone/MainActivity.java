package com.example.appdemoone;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.Toast;

import android.hardware.usb.*;

public class MainActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
        Toast.makeText(getApplicationContext(), "This is a test onCreate", Toast.LENGTH_SHORT).show();
    }
    
    @Override
    protected void onStart()
    {
    	super.onStart();
    	
    	Button button1 = (Button)findViewById(R.id.func1);
    	Button button2 = (Button)findViewById(R.id.func2);
    	
    	
        // listen for new devices
        IntentFilter filter = new IntentFilter();
        filter.addAction("helloaction");
        registerReceiver(mUsbReceiver, filter);
        
        
    	button1.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub		        
				Toast.makeText(getApplicationContext(), "click button 1", Toast.LENGTH_SHORT).show();
				
				Intent bintent = new Intent("helloaction");
				sendBroadcast(bintent);
			}
		});
    	
    	button2.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				Toast.makeText(getApplicationContext(), "click button 2 ", Toast.LENGTH_SHORT).show();
				
				Intent intent = new Intent("myandroid.login"); 
				intent.putExtra("mainname", "main-value");
				startActivity(intent);
			}
		});
    	Toast.makeText(getApplicationContext(), "It is onStart", Toast.LENGTH_SHORT).show();
    }
    
    public BroadcastReceiver mUsbReceiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();

            Toast.makeText(getApplicationContext(), "onReceive  action is "+action, Toast.LENGTH_SHORT).show();
            		
            if (UsbManager.ACTION_USB_DEVICE_ATTACHED.equals(action)) {
                UsbDevice device = (UsbDevice)intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);
                String deviceName = device.getDeviceName();
                
                Toast.makeText(getApplicationContext(), "onReceive  action is "+action+" .device name is "+deviceName, Toast.LENGTH_SHORT).show();
                
            } else if (UsbManager.ACTION_USB_DEVICE_DETACHED.equals(action)) {
                UsbDevice device = intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);
                String deviceName = device.getDeviceName();
                Toast.makeText(getApplicationContext(), "onReceive  action is "+action+" .device name is "+deviceName, Toast.LENGTH_SHORT).show();
            }
            
        }
    };
    
}
