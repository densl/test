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
	BOOL RecvData(BYTE *rBuf);
	BOOL SendTask(BYTE *sBuf,DWORD MsgLen);
	BOOL getRequest(DWORD IoCtlCode, BYTE *inBuf, DWORD inbufLen, LPVOID lpBuffer,DWORD dwBufSize,LPDWORD pdwNumberOfByteReturned);

private:
	SHORT m_timeout;
	WORD m_devIndex;

	WORD getDevIndex();
	void setDevIndex(WORD devIndex);

	HANDLE getUsbHandle();
	HANDLE OpenUsbDevice();

	BSTR getDevString();
	BOOL getPortParametersKey(HKEY &PortParaKey);

public:

};
