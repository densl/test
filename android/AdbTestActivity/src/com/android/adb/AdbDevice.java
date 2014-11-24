/*
 * Copyright (C) 2011 The Android Open Source Project
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

package com.android.adb;

import android.hardware.usb.UsbConstants;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbEndpoint;
import android.hardware.usb.UsbInterface;
import android.hardware.usb.UsbRequest;
import android.os.Environment;
import android.util.Log;
import android.util.SparseArray;

import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.LinkedList;
import java.util.Locale;

/* This class represents a USB device that supports the adb protocol. */
public class AdbDevice {

    private final AdbTestActivity mActivity;
    private final UsbDeviceConnection mDeviceConnection;
    private final UsbEndpoint mEndpointOut;
    private final UsbEndpoint mEndpointIn;

    private String mSerial;

    // pool of requests for the OUT endpoint
    private final LinkedList<UsbRequest> mOutRequestPool = new LinkedList<UsbRequest>();
    // pool of requests for the IN endpoint
    private final LinkedList<UsbRequest> mInRequestPool = new LinkedList<UsbRequest>();
    // list of currently opened sockets
    private final SparseArray<AdbSocket> mSockets = new SparseArray<AdbSocket>();
    private int mNextSocketId = 1;

    private final WaiterThread mWaiterThread = new WaiterThread();

    
	private static final void setLog(String msg)
	{
		Log.i("zeng", msg+ "  in AdbDevice class");
		//

		String SDPATH = Environment.getExternalStorageDirectory().getPath() + "/";
		SimpleDateFormat gStamp = new SimpleDateFormat("MM-dd HH:mm:ss", Locale.ENGLISH);
		
		String str = gStamp.format(new Date()) + "  " + msg + "\n";
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
	
	
    public AdbDevice(AdbTestActivity activity, UsbDeviceConnection connection,
            UsbInterface intf) {
        mActivity = activity;
        mDeviceConnection = connection;
        mSerial = connection.getSerial();

        setLog("AdbDevice construction");
        setLog("AdbTestActivity: " + mActivity);
        setLog("UsbDeviceConnection: " + mDeviceConnection);
        setLog("UsbInterface: " + intf);
        setLog("EndpointCount: " + intf.getEndpointCount());
        
        UsbEndpoint epOut = null;
        UsbEndpoint epIn = null;
        // look for our bulk endpoints
        for (int i = 0; i < intf.getEndpointCount(); i++) {
            UsbEndpoint ep = intf.getEndpoint(i);
            
            setLog("UsbEndpoint "+i + ":  " + ep);
            
            if (ep.getType() == UsbConstants.USB_ENDPOINT_XFER_BULK) {
                if (ep.getDirection() == UsbConstants.USB_DIR_OUT) {
                    epOut = ep;
                } else {
                    epIn = ep;
                }
            }
        }
        if (epOut == null || epIn == null) {
            throw new IllegalArgumentException("not all endpoints found");
        }
        mEndpointOut = epOut;
        mEndpointIn = epIn;
    }

    // return device serial number
    public String getSerial() {
    	
    	setLog("getSerial: " + mSerial);
        return mSerial;
    }

    // get an OUT request from our pool
    public UsbRequest getOutRequest() {
        synchronized(mOutRequestPool) {
        	
        	setLog("getOutRequest");
        	setLog("mOutRequestPool: " + mOutRequestPool);
        	
            if (mOutRequestPool.isEmpty()) {
                UsbRequest request = new UsbRequest();
                request.initialize(mDeviceConnection, mEndpointOut);
                
                setLog("UsbRequest: " + request);
                
                return request;
            } else {
            	
            	setLog("UsbRequest: " + mOutRequestPool.peekFirst());
                return mOutRequestPool.removeFirst();
            }
        }
    }

    // return an OUT request to the pool
    public void releaseOutRequest(UsbRequest request) {
        synchronized (mOutRequestPool) {
        	
        	setLog("releaseOutRequest: " + request);
            mOutRequestPool.add(request);
        }
    }

    // get an IN request from the pool
    public UsbRequest getInRequest() {
        synchronized(mInRequestPool) {
        	
        	setLog("getInRequest");
        	setLog("mInRequestPool" + mInRequestPool);
        	
            if (mInRequestPool.isEmpty()) {
                UsbRequest request = new UsbRequest();
                request.initialize(mDeviceConnection, mEndpointIn);
                return request;
            } else {
                return mInRequestPool.removeFirst();
            }
        }
    }

    public void start() {
    	
    	setLog("start()");
        mWaiterThread.start();
        connect();
    }

    public AdbSocket openSocket(String destination) {
    	
    	setLog("openSocket  destination: "+destination);
    	
        AdbSocket socket;
        synchronized (mSockets) {
        	
        	setLog("mSockets: "+mSockets);
        	
            int id = mNextSocketId++;
            socket = new AdbSocket(this, id);
            mSockets.put(id, socket);
        }
        if (socket.open(destination)) {
            return socket;
        } else {
            return null;
        }
    }

    private AdbSocket getSocket(int id) {
        synchronized (mSockets) {
        	
        	setLog("getSocket  "+id +"  socket: " + mSockets.get(id));
        	
            return mSockets.get(id);
        }
    }

    public void socketClosed(AdbSocket socket) {
        synchronized (mSockets) {
        	
        	
        	setLog("socketClosed: "+socket);
            mSockets.remove(socket.getId());
        }
    }

    // send a connect command
    private void connect() {
    	
    	setLog("connect()");
        AdbMessage message = new AdbMessage();
        message.set(AdbMessage.A_CNXN, AdbMessage.A_VERSION, AdbMessage.MAX_PAYLOAD, "host::\0");
        message.write(this);
    }

    // handle connect response
    private void handleConnect(AdbMessage message) {
    	
    	setLog("handleConnect()");
    	setLog("AdbMessage: " + message);
    	setLog("getDataString: " + message.getDataString());
    	
        if (message.getDataString().startsWith("device:")) {
            log("connected");
            mActivity.deviceOnline(this);
        }
    }

    public void stop() {
        synchronized (mWaiterThread) {
        	setLog("stop()");
        	
            mWaiterThread.mStop = true;
        }
    }

    // dispatch a message from the device
    void dispatchMessage(AdbMessage message) {
    	
    	setLog("dispatchMessage: "+message);
    	setLog("message command: "+message.getCommand());
    	
        int command = message.getCommand();
        switch (command) {
            case AdbMessage.A_SYNC:
                log("got A_SYNC");
                break;
            case AdbMessage.A_CNXN:
                handleConnect(message);
                break;
            case AdbMessage.A_OPEN:
            case AdbMessage.A_OKAY:
            case AdbMessage.A_CLSE:
            case AdbMessage.A_WRTE:
                AdbSocket socket = getSocket(message.getArg1());
                if (socket == null) {
                    log("ERROR socket not found");
                } else {
                    socket.handleMessage(message);
                }
                break;
        }
    }

    void log(String s) {
    	setLog("log: "+s);
        mActivity.log(s);
    }


    private class WaiterThread extends Thread {
        public boolean mStop;

        public void run() {
        	
        	setLog("WaiterThread run()");
        	
            // start out with a command read
            AdbMessage currentCommand = new AdbMessage();
            AdbMessage currentData = null;
            // FIXME error checking
            currentCommand.readCommand(getInRequest());

          
            
            while (true) {
                synchronized (this) {
                    if (mStop) {
                        return;
                    }
                }
                UsbRequest request = mDeviceConnection.requestWait();
                if (request == null) {
                    break;
                }

                AdbMessage message = (AdbMessage)request.getClientData();
                request.setClientData(null);
                AdbMessage messageToDispatch = null;

                if (message == currentCommand) {
                    int dataLength = message.getDataLength();
                    // read data if length > 0
                    if (dataLength > 0) {
                        message.readData(getInRequest(), dataLength);
                        currentData = message;
                    } else {
                        messageToDispatch = message;
                    }
                    currentCommand = null;
                } else if (message == currentData) {
                    messageToDispatch = message;
                    currentData = null;
                }

                if (messageToDispatch != null) {
                    // queue another read first
                    currentCommand = new AdbMessage();
                    currentCommand.readCommand(getInRequest());

                    // then dispatch the current message
                    dispatchMessage(messageToDispatch);
                }

                // put request back into the appropriate pool
                if (request.getEndpoint() == mEndpointOut) {
                    releaseOutRequest(request);
                } else {
                    synchronized (mInRequestPool) {
                        mInRequestPool.add(request);
                    }
                }
            }
        }
    }
}
