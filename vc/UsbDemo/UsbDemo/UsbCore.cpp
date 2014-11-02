
#include "stdafx.h"
#include "UsbCore.h"



UsbCore::UsbCore()
{
	//init
	setHandle(INVALID_HANDLE_VALUE);
}

static LPTSTR logFile = _T("d:\\usbLog.txt");
void UsbCore::setError(char * err)
{
	char msg[4096] = {0};
	
	SYSTEMTIME mtime;
	GetSystemTime(&mtime);

	sprintf_s(msg, "%04d%02d%02d-%02d.%02d.%02d:  %s\n", mtime.wYear, mtime.wMonth, mtime.wDay, mtime.wHour,mtime.wMinute, mtime.wSecond, err);

	CFile cf;
	cf.Open(logFile, CFile::modeCreate|CFile::modeWrite|CFile::modeNoTruncate);
	cf.SeekToEnd();

	cf.Write(msg, strlen(msg));
	cf.Close();
}
void UsbCore::setHandle(HANDLE mhandle)
{
	gHandle = mhandle;
}
HANDLE UsbCore::getHandle()
{
	return gHandle;
}

BOOL UsbCore::uOpen(char * path)
{
	int m_ErrorCode = 0;

	//open device ****************************
	HANDLE mHandle = CreateFileA(path,
		GENERIC_READ|GENERIC_WRITE, 
		FILE_SHARE_WRITE | FILE_SHARE_READ, 
		NULL, //must be NULL
		OPEN_EXISTING, //must be OPEN_EXISTING
		FILE_FLAG_OVERLAPPED, 
		NULL);

	setHandle(mHandle);

	if (mHandle == INVALID_HANDLE_VALUE)
	{
		char err[512] = {0};
		sprintf_s(err, "<Open> Create file error (%d).", GetLastError());
		setError(err);
		return FALSE;
	}
	return TRUE;
}
void UsbCore::uClose()
{
	HANDLE mhandle = getHandle();

	if (mhandle != INVALID_HANDLE_VALUE)
		CloseHandle(mhandle);
}

////////////////////////////////////////////////////////
int UsbCore::uRead(char * buf, DWORD bufLen)
{
	DWORD m_ErrorCode = 0;
	DWORD dwSize = 0; //saved recv data length

	OVERLAPPED mr_EvtOvlp;
	memset(&mr_EvtOvlp, 0, sizeof(OVERLAPPED));	

	//check options **************************
	if (bufLen == 0)
	{
		setError("<Read> bufLen is zero.");
		return m_ErrorCode;
	}

	//get usb handle **************************
	HANDLE mHandle = getHandle();
	if (mHandle == INVALID_HANDLE_VALUE)
	{
		setError("<Read> Invalid handle.");
		return m_ErrorCode;
	}

	//read fp **************************
	mr_EvtOvlp.hEvent = CreateEvent(NULL, TRUE, FALSE, _T("ReadEvent"));
	DWORD rtn = ReadFile(mHandle,buf,bufLen,&dwSize,&mr_EvtOvlp);
	if (!rtn)
	{
		rtn = GetLastError();
		if (rtn != ERROR_IO_PENDING)
		{
			char err[512] = {0};
			sprintf_s(err, "<Read> Create read event error (%d).", rtn);
			setError(err);

			//CloseHandle(mHandle);
			CloseHandle(mr_EvtOvlp.hEvent);
			return m_ErrorCode; //Error
		}
	}

	rtn = WaitForSingleObject(mr_EvtOvlp.hEvent,100);

	//get result **************************
	while (TRUE)
	{
		rtn = GetOverlappedResult(mHandle,&mr_EvtOvlp,&dwSize,FALSE);
		if (!rtn)
		{
			rtn = GetLastError();
			if (rtn != ERROR_IO_PENDING)
			{
				char err[512] = {0};
				sprintf_s(err, "<Read> GetOverlppedResult error (%d).", rtn);
				setError(err);

				//CloseHandle(mHandle);
				CloseHandle(mr_EvtOvlp.hEvent);
				return m_ErrorCode;
			}
			//IO pending, then wait 100ms 
			WaitForSingleObject(mr_EvtOvlp.hEvent,100);
		}
		else
		{
			break; //success
		}        
	}

	//close handles
	//CloseHandle(mHandle);
	CloseHandle(mr_EvtOvlp.hEvent);
	return dwSize;
}
int UsbCore::uWrite(char * buf, DWORD bufLen)
{
	DWORD m_ErrorCode = 0;
	DWORD dwSize = 0; //saved write data length

	OVERLAPPED mw_EvtOvlp;
	memset(&mw_EvtOvlp, 0, sizeof(OVERLAPPED));	

	//check params ********************************
	bufLen = bufLen ? bufLen : strlen(buf);
	if (buf == 0)
	{
		setError("<Write> buf is null.");
		return m_ErrorCode;
	}
	


	//get usb handle ********************************
	HANDLE mHandle = getHandle();
	if (mHandle == INVALID_HANDLE_VALUE)
	{
		setError("<Write> Invalid handle.");
		return m_ErrorCode;
	}

	//write fp
	mw_EvtOvlp.hEvent = CreateEvent(NULL, TRUE, FALSE, _T("WriteEvent"));
	DWORD rtn =WriteFile(mHandle,buf,bufLen,&dwSize,&mw_EvtOvlp);
	if (!rtn)
	{
		rtn = GetLastError();
		if (rtn != ERROR_IO_PENDING)
		{
			char err[512] = {0};
			sprintf_s(err, "<Write> Create event error (%d).", rtn);
			setError(err);

			//CloseHandle(mHandle);
			CloseHandle(mw_EvtOvlp.hEvent);
			return m_ErrorCode; //Error
		}
	}

	rtn = WaitForSingleObject(mw_EvtOvlp.hEvent,100);

	//get result
	while (TRUE)
	{
		rtn = GetOverlappedResult(mHandle,&mw_EvtOvlp,&dwSize,FALSE);
		if (!rtn)
		{
			rtn = GetLastError();
			if (rtn != ERROR_IO_PENDING)
			{
				char err[512] = {0};
				sprintf_s(err, "<Write> GetOverlappedResult error (%d).", rtn);
				setError(err);

				//CloseHandle(mHandle);
				CloseHandle(mw_EvtOvlp.hEvent);
				return m_ErrorCode;
			}
			//IO pending, then wait 100ms
			WaitForSingleObject(mw_EvtOvlp.hEvent,100);
		}
		else
		{
			break; //success
		}        
	}

	//close handles
	//CloseHandle(mHandle);
	CloseHandle(mw_EvtOvlp.hEvent);
	return dwSize;
}
int UsbCore::uCommandIo(DWORD dwIoControlCode, LPVOID inBuf, DWORD inLen, LPVOID outBuf, DWORD outLen)
{
	DWORD m_ErrorCode = 0;
	DWORD dwSize = 0; //saved recv data length

	OVERLAPPED m_EvtOvlp;
	memset(&m_EvtOvlp, 0, sizeof(OVERLAPPED));	


	//get usb handle
	HANDLE mHandle = getHandle();
	if (mHandle == INVALID_HANDLE_VALUE)
	{
		setError("<DeviceIo> Invalid handle.");
		return m_ErrorCode;
	}

	//device io control
	m_EvtOvlp.hEvent = CreateEvent(NULL, TRUE, FALSE, _T("WriteEvent"));
	DWORD rtn = DeviceIoControl(mHandle, dwIoControlCode, inBuf, inLen, outBuf, outLen, &dwSize, &m_EvtOvlp);	
	if (!rtn)
	{
		rtn = GetLastError();
		if (rtn != ERROR_IO_PENDING)
		{
			char err[512] = {0};
			sprintf_s(err, "<DeviceIo> Create event error (%d).", rtn);
			setError(err);

			//CloseHandle(mHandle);
			CloseHandle(m_EvtOvlp.hEvent);
			return m_ErrorCode; //Error
		}
	}

	rtn = WaitForSingleObject(m_EvtOvlp.hEvent,100);

	//get result
	while (TRUE)
	{
		rtn = GetOverlappedResult(mHandle,&m_EvtOvlp,&dwSize,FALSE);
		if (!rtn)
		{
			rtn = GetLastError();
			if (rtn != ERROR_IO_PENDING)
			{
				char err[512] = {0};
				sprintf_s(err, "<DeviceIo> GetOverlappedResult error (%d).", rtn);
				setError(err);

				//CloseHandle(mHandle);
				CloseHandle(m_EvtOvlp.hEvent);
				return m_ErrorCode;
			}
			//IO pending, then wait 100ms
			WaitForSingleObject(m_EvtOvlp.hEvent,100);
		}
		else
		{
			break; //success
		}        
	}

	//close handles
	//CloseHandle(mHandle);
	CloseHandle(m_EvtOvlp.hEvent);
	return dwSize;
}

