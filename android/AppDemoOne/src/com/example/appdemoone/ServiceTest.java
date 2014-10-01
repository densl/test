package com.example.appdemoone;

import android.app.Service;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.IBinder;
import android.util.Log;

public class ServiceTest extends Service{
	private static final String TAG = "ServiceTest";
	
	@Override
	public void onCreate() {
		// TODO Auto-generated method stub
		super.onCreate();
		Log.i(TAG, "This is onCreate in Service Test");
		
	}
	
	@Override
	public IBinder onBind(Intent intent) {
		// TODO Auto-generated method stub
		
		Log.i(TAG, "This is onBind in Service Test");
		
		return null;
	}
	
	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		// TODO Auto-generated method stub
		
		Log.i(TAG, "This is onStartCommand in Service Test");
		
		return super.onStartCommand(intent, flags, startId);
	}


	@Override
	public void onDestroy() {
		// TODO Auto-generated method stub
		super.onDestroy();
		
		Log.i(TAG, "This is onDestroy in Service Test");
	}


}
