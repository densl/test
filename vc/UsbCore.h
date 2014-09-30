#pragma once


class UsbCore
{
public:
	UsbCore();

public:
	BOOL IsPrinterAvailable(void);
	SHORT getWaitTime(); //default 5000(5s)
	void setWaitTime(SHORT timeout);
public:

	BOOL RecvData(BYTE *rBuf);
	BOOL SendTask(BYTE *sBuf,DWORD MsgLen);
	BOOL getRequest(DWORD IoCtlCode, BYTE *inBuf, DWORD inbufLen, LPVOID lpBuffer,DWORD dwBufSize,LPDWORD pdwNumberOfByteReturned);
private:
	SHORT m_timeout;

	HANDLE getUsbHandle();
	HANDLE OpenUsbDevice(WORD index);
	
	BSTR findPrinterPath(WORD &mlocation, DWORD &maxCount);
	BSTR getPrinterDevPath(WORD index);
};
