

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

public:
	void setError(char * err);
	void setHandle(HANDLE mhandle);
	
private:
	HANDLE getHandle();

	HANDLE gHandle;


private:
	WORD m_devIndex;

	WORD getDevIndex();
	void setDevIndex(WORD devIndex);
	DWORD getDevMaxCount();
	
	BSTR getDevInstance();
	BSTR getDevicePath();
	BOOL getPortParametersKey(HKEY &PortParaKey);
	
	DWORD getPortNumber();
	
	HANDLE OpenUsbDevice();
};