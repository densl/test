

class UsbCore
{
public:
	UsbCore();

public:
	BOOL uSetUsbPort(WCHAR * usbport);
	BOOL IsPrinterAvailable(void);
	BSTR getPortDescription();

	BOOL uOpen();
	void uClose();

	int uRead(char * buf, DWORD bufLen);
	int uWrite(char * buf, DWORD bufLen);
	int uCommandIo(DWORD dwIoControlCode, LPVOID inBuf, DWORD inLen, LPVOID outBuf, DWORD outLen);

	HANDLE getHandle();
	BSTR getDevicePath();
	DWORD getPortNumber();
	DWORD getDevMaxCount();
	BSTR getDevInstance();

protected:
	void setError(char * err);
	void setHandle(HANDLE mhandle);
	
private:
	

	HANDLE gHandle;


private:
	WORD m_devIndex;

	WORD getDevIndex();
	void setDevIndex(WORD devIndex);
	
	
	
	BOOL getPortParametersKey(HKEY &PortParaKey);
	
	
	
	HANDLE OpenUsbDevice();
};