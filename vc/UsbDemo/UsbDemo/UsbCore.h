

class UsbCore
{
public:
	UsbCore();

public:
	BOOL uOpen(char * path);
	void uClose();

	int uRead(char * buf, DWORD bufLen);
	int uWrite(char * buf, DWORD bufLen);
	int uCommandIo(DWORD dwIoControlCode, LPVOID inBuf, DWORD inLen, LPVOID outBuf, DWORD outLen);

public:
	void setError(char * err);
	void setHandle(HANDLE mhandle);
	HANDLE getHandle();

private:
	HANDLE gHandle;
};