
## 

Step one, import files:
        -- android.content.Context;
	android.hardware.usb.UsbManager;
	android.hardware.usb.UsbDevice;
	android.hardware.usb.Interface;
	android.hardware.usb.UsbDeviceConnection;
	android.hardware.usb.UsbEndPoint;

Step two, get usb manager:
	UsbManager mManager = (UsbManager)getSystemService(Context.USB_SERVICE);

Step three, get usb devices:
	<String, UsbDevice> = mManager.getDeviceList();

Step four, get usb interface:
	UsbInterface mUsbInterface = mUsbDevice.getInterface(index);

Step five, create usb connection:
	UsbDeviceConnection conn = UsbManager.openDevice(mUsbDevice);

Step six, claim exlusive usb interface:
	conn.claimInterface(mUsbInterface, false);

Step seven, transfer control message or bulk data:
	conn.controlTransfer(int, int, int, int, byte[], int, int, int);
	  requestType, request, value, index, buffer, offset, length, timeout.
      ## or ##
        conn.bulkTransfer(UsbEndpoit, byte[], int, int, int);
	  endpoint, buffer, offset, length, timeout.
   
Step eight, releases exclusive access to a UsbInterface:
	conn.releaseInterface(mUsbInterface);

End.
