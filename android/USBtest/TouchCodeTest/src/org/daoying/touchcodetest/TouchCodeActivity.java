package org.daoying.touchcodetest;

import java.util.HashMap;
import java.util.Iterator;

import android.app.Activity;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.InputDevice;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class TouchCodeActivity extends Activity {
	protected static final String TAG = TouchCodeActivity.class.getSimpleName();
	private TextView tv;
	private Button btnPrint, btnCheckFileExist, btnReadFile;
	private String[] joystickInfo = { "", "" };
	private boolean bUsbDeviceDetached;
	private boolean stopCheckJoystickThread;

	private static final String ACTION_USB_PERMISSION = "com.android.example.USB_PERMISSION";

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
		tv = (TextView) findViewById(R.id.tv);
		tv.setText("");
		tv.setScrollContainer(true);
		btnPrint = (Button) findViewById(R.id.btnPrint);
		btnCheckFileExist = (Button) findViewById(R.id.btnCheckFileExist);
		btnReadFile = (Button) findViewById(R.id.btnReadFile);

		btnPrint.setOnClickListener(listener);
		btnCheckFileExist.setOnClickListener(listener);
		btnReadFile.setOnClickListener(listener);
		filter.addAction(Intent.ACTION_UMS_CONNECTED);
		filter.addAction(Intent.ACTION_UMS_DISCONNECTED);
		filter.addAction(Intent.ACTION_POWER_CONNECTED);

		filter.addAction("android.intent.action.MEDIA_MOUNTED");
		filter.addAction("android.intent.action.MEDIA_EJECT");

		filter.addAction(UsbManager.ACTION_USB_DEVICE_ATTACHED);
		filter.addAction(UsbManager.ACTION_USB_DEVICE_DETACHED);
		filter.addAction(UsbManager.ACTION_USB_ACCESSORY_ATTACHED);
		filter.addAction(UsbManager.ACTION_USB_ACCESSORY_DETACHED);
		filter.addAction(UsbManager.EXTRA_PERMISSION_GRANTED);
		filter.addAction(UsbManager.EXTRA_DEVICE);
		filter.addAction(UsbManager.EXTRA_ACCESSORY);

		filter.addAction("android.hardware.usb.action.USB_DEVICE_ATTACHED");
		filter.addAction("android.hardware.usb.action.USB_ACCESSORY_ATTACHED");

		UsbManager mUsbManager = (UsbManager) getSystemService(Context.USB_SERVICE);

		PendingIntent mPermissionIntent = PendingIntent.getBroadcast(this, 0,
				new Intent(ACTION_USB_PERMISSION), 0);
		IntentFilter filter = new IntentFilter(ACTION_USB_PERMISSION);

		new Thread(ListenerDeviceRunnable).start();
	}

	private final BroadcastReceiver mUsbReceiver = new BroadcastReceiver() {

		public void onReceive(Context context, Intent intent) {
			String action = intent.getAction();

			tv.setText(tv.getText() + "  onReceive = " + action + "\n");

			if (ACTION_USB_PERMISSION.equals(action)) {
				synchronized (this) {
					UsbDevice device = (UsbDevice) intent
							.getParcelableExtra(UsbManager.EXTRA_DEVICE);

					if (intent.getBooleanExtra(
							UsbManager.EXTRA_PERMISSION_GRANTED, false)) {
						if (device != null) {
							Log.d(TAG,
									"permission for device-------------------------- ");
						}
					} else {
						Log.d(TAG, "permission denied for device " + device);
					}
				}
			}
		}
	};

	@Override
	public boolean onKeyUp(int keyCode, KeyEvent event) {
		tv.setText(tv.getText() + "keyUp " + keyCode + "\n");
		InputDevice device = event.getDevice();
		if (device != null && device.getSources() == 0x1000511 && keyCode == 4) {
			return true;
		}
		return super.onKeyUp(keyCode, event);
	}

	@Override
	public boolean onKeyLongPress(int keyCode, KeyEvent event) {
		tv.setText(tv.getText() + "keyLongPress " + keyCode + "\n");
		return super.onKeyLongPress(keyCode, event);
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		// System.out.println(" keyCode = " + keyCode);
		// int action = event.getAction();
		// tv.setText(tv.getText() + "    keyDown keyCode:" + keyCode +
		// " Number:"+event.getNumber() + " ScanCode:" + event.getScanCode()
		// + " Source:" + event.getSource() + " UnicodeChar:" +
		// event.getUnicodeChar() + " DeviceId:" + event.getDeviceId()
		// + " action:" + action + "      \n");
		// if(event.getDevice() != null){
		// tv.setText(tv.getText() + " event.getDevice().getSources:" +
		// event.getDevice().getSources()
		// + ", name:" + event.getDevice().getName()
		// + ", id:" + event.getDevice().getId()
		// + ", string:" + event.getDevice().toString()
		// + ", describeContents:" + event.describeContents());
		// if(event.getDevice().getSources() == 0x1000511){
		// tv.setText(tv.getText() + "   joystick event");
		// }
		// }

		// int code = Integer.parseInt("8" + Integer.toHexString(288) + "0000",
		// 16);
		// int code = 0x80000000 | (288 << 16);
		// long code = Long.parseLong("8" + Integer.toHexString(288) + "0000",
		// 16);
		// System.out.println(" code = " + code);
		// System.out.printf("code = 0x%x", code);
		// InputDevice device = event.getDevice();
		// if(device != null){
		// String deviceIdentify = Integer.toHexString(device.get)
		// }

		tv.setText(tv.getText() + "keyDown keyCode = " + keyCode
				+ ", scanCode = " + event.getScanCode() + "\n");

		InputDevice device = event.getDevice();
		if (device != null && device.getSources() == 0x1000511 && keyCode == 4) {
			return true;
		}

		return super.onKeyDown(keyCode, event);
	}

	@Override
	public boolean onGenericMotionEvent(MotionEvent event) {
		float x = event.getX();
		float y = event.getY();

		tv.setText(tv.getText() + " onGenericMotionEvent x = " + event.getX()
				+ " y = " + event.getY() + "\n");

		if ((x == -1 && y == -1) || (x == 1 && y == 1) || (x == 1 && y == -1)
				|| (x == -1 && y == 1)) {
			return true;
		} else {
			return super.onGenericMotionEvent(event);
		}
	}

	@Override
	public boolean onTouchEvent(MotionEvent event) {
		System.out.println("onTouchEvent");
		System.out.println("x = " + event.getX());
		System.out.println("y = " + event.getY());
		// tv.setText(tv.getText() + " onTouchEvent x = " + event.getX());
		return super.onTouchEvent(event);
	}

	View.OnClickListener listener = new View.OnClickListener() {

		@Override
		public void onClick(View v) {
			if (v == btnPrint) {
				// printText();
			} else if (v == btnCheckFileExist) {
				// checkFile();
			} else if (v == btnReadFile) {
				tv.setText("");
				// readFile();
			}
		}
	};

	@Override
	protected void onResume() {
		this.registerReceiver(receiver, filter);
		registerReceiver(mUsbReceiver, filter);
		super.onResume();
	}

	@Override
	protected void onStop() {
		this.unregisterReceiver(receiver);
		this.unregisterReceiver(mUsbReceiver);
		super.onStop();
	}

	private IntentFilter filter = new IntentFilter();

	private BroadcastReceiver receiver = new BroadcastReceiver() {

		@Override
		public void onReceive(Context arg0, Intent arg1) {
			String action = arg1.getAction();
			Log.d(TAG, "onReceive()   action = " + action);
			tv.setText(tv.getText() + "  onReceive = " + action + "\n");

			if (action.equals(Intent.ACTION_UMS_CONNECTED)) {
				tv.setText(tv.getText() + "  usb connected");
			} else if (action.equals(Intent.ACTION_UMS_DISCONNECTED)) {
				tv.setText(tv.getText() + "  usb disconnected");
				bUsbDeviceDetached = true;
			}

			if (action.equalsIgnoreCase("android.hardware.usb.action.USB_DEVICE_DETACHED")) {
				//usb 设备拔出事件
				bUsbDeviceDetached = true;
			}

		}
	};

	
	/**
	 * 这个线程在启动程序的时候启动，每隔3秒检测一次usb设备，实现热插拔。
	 * 由于是测试程序， 所以打印了很多东西出来。
	 */
	Runnable ListenerDeviceRunnable = new Runnable() {
		@Override
		public void run() {
			UsbManager manager = (UsbManager) getSystemService(Context.USB_SERVICE);
			String joystickIdentify = null;
			while (!stopCheckJoystickThread) {
				HashMap deviceList = manager.getDeviceList();

				Iterator deviceIterator = deviceList.values().iterator();

				if (bUsbDeviceDetached) {
					//  有设备拔出， 检测是哪个设备拔出；
					String strDevices = "";
					UsbDevice device_1;
					
					//迭代出所有设备，拼成字符串
					Iterator it = deviceList.values().iterator();
					while (it.hasNext()) {
						device_1 = (UsbDevice) it.next();
						strDevices = device_1.toString();
						strDevices = strDevices
								+ makeJoystickIdentify(device_1.getVendorId(), device_1.getProductId())
								+ ",getDeviceId:" + device_1.getDeviceId()
								+ ",DeviceName:" + device_1.getDeviceName()
								+ ",DeviceProtocol:"
								+ device_1.getDeviceProtocol()
								+ ",DeviceSubclass:"
								+ device_1.getDeviceSubclass()
								+ ",describeContents:"
								+ device_1.describeContents() + ",DeviceClass:"
								+ device_1.getDeviceClass() + "\n";
					}
					Log.d(TAG, "strDevices = " + strDevices);
					Message msg = new Message();
					msg.what = 0;
					msg.obj = strDevices;
					_handler.sendMessage(msg);

					//如果设备没有在字符串内， 说明被拔出了
					if (strDevices.indexOf(joystickInfo[0]) == -1) {
						joystickInfo[0] = "";
					}
					if (strDevices.indexOf(joystickInfo[1]) == -1) {
						joystickInfo[1] = "";
					}

					bUsbDeviceDetached = false;

				}

				//目前只支持两个usb设备，设备信息储存在joystickInfo 数组里面
				if (joystickInfo[0].equals("") || joystickInfo[1].equals("")) {
					//joystickInfo数组还有空位
					while (deviceIterator.hasNext()) {
						String strDevicesInfo = "";
						UsbDevice device = (UsbDevice) deviceIterator.next();

						// 这里就可以取得vendorId 和 productId
						int vendorId = device.getVendorId();
						int productId = device.getProductId();
						joystickIdentify = makeJoystickIdentify(vendorId, productId);

						if (!joystickInfo[0].equals(joystickIdentify)
								&& !joystickInfo[1].equals(joystickIdentify)) {
							if (joystickInfo[0].equals("")) {
								joystickInfo[0] = joystickIdentify;
							} else {
								joystickInfo[1] = joystickIdentify;
							}

							strDevicesInfo = strDevicesInfo
									+ "\n" + "joystick[0] = " + joystickInfo[0]
									+ " joystick[1] = " + joystickInfo[1]
									+ "\n";

							//打印信息在屏幕上
							Message msg1 = new Message();
							msg1.what = 0;
							msg1.obj = strDevicesInfo;
							_handler.sendMessage(msg1);
						}
					}

					// Message msg1 = new Message();
					// msg1.what = 0;
					// msg1.obj = "joystick[0] = " + joystickInfo[0] +
					// " joystick[1] = " + joystickInfo[1] + "\n";
					// _handler.sendMessage(msg1);
				}
				try {
					Thread.sleep(3000);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}

			}
		}

	};

	
	/**
	 * 根据vendorId 和 productId 生成一个 字符串，转换规则是，先转成16进制，不够4位的前面补0，用冒号连接起来。
	 * @param vendorId
	 * @param productId
	 * @return
	 */
	private String makeJoystickIdentify(int vendorId, int productId) {
		String strVendorId = Integer.toHexString(vendorId);
		String strProductId = Integer.toHexString(productId);
		int vendorIdLength = strVendorId.length();
		int productIdLength = strProductId.length();
		for (int i = 4 - vendorIdLength; i > 0; i--) {
			strVendorId = "0" + strVendorId;
		}
		for (int i = 4 - productIdLength; i > 0; i--) {
			strProductId = "0" + strProductId;
		}

		return strVendorId + ":" + strProductId;
	}

	private Handler _handler = new Handler() {
		@Override
		public void handleMessage(Message msg) {
			int what = msg.what;
			switch (what) {
			case 0:
				tv.setText(tv.getText() + msg.obj.toString());
				break;

			}
			super.handleMessage(msg);
		}
	};

	@Override
	protected void onDestroy() {
		stopCheckJoystickThread = true;
		super.onDestroy();
	}
}