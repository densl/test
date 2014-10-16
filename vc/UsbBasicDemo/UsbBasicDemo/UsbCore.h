#pragma once


class UsbCore
{
public:
	UsbCore();
public:
	DWORD getDevMaxCount();

	BOOL IsPrinterAvailable(void);
	SHORT getWaitTime(); //default 5000(5s)
	void setWaitTime(SHORT timeout);
public:
	BSTR getDevicePath();
	BSTR getPortDescription();	
	DWORD getPortNumber();
public:
	DWORD recv(CHAR *rBuf, DWORD rLen);
	DWORD send(CHAR *sBuf,DWORD MsgLen);
	

	BSTR getRecvStr();
	DWORD sendData(WCHAR *data, BOOL dataflag);

	DWORD getRequestFull(DWORD IoCtlCode, CHAR *inBuf, DWORD inbufLen, LPVOID lpBuffer,DWORD dwBufSize);

	BSTR getRequestStr(DWORD IoCtlCode, CHAR *inBuf, DWORD inbufLen);
	BSTR getReqStr(DWORD IoCtlCode);
private:
	SHORT m_timeout;
	WORD m_devIndex;

	WORD getDevIndex();
	void setDevIndex(WORD devIndex);

	HANDLE getUsbHandle();
	HANDLE OpenUsbDevice();

	BSTR getDevInstance();
	BOOL getPortParametersKey(HKEY &PortParaKey);
	
	DWORD strToHexArray(CHAR * inBuf, CHAR * outBuf);
	CHAR AscToHex(CHAR asc);
	
public:

};
