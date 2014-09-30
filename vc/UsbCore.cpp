#pragma once

#include "stdafx.h"

#include "UsbCore.h"


#include "SetupAPI.h"
#include <initguid.h>


DEFINE_GUID(USB_PRINTER,0x28d78fad, 0x5a12, 0x11D1, 0xae, 0x5b, 0x00, 0x00, 0xf8, 0x03, 0xa8, 0xc2);

//
UsbCore::UsbCore()
{
	m_timeout = 10000; //10s
}


//mlocation={1 ~ max_word},return mlocation's path
BSTR UsbCore::findPrinterPath(WORD &mlocation, DWORD &maxCount)
{
	DWORD ret;

	//open enum key
	HKEY hKey;
	TCHAR *subkey_usbprint = _T("SYSTEM\\CurrentControlSet\\Services\\usbprint\\Enum");
	ret = RegOpenKeyExW(HKEY_LOCAL_MACHINE,subkey_usbprint,0,KEY_READ,&hKey);
	if( ERROR_SUCCESS !=  ret)
	{
		return NULL;
	}

	//get usb printer count
	DWORD retLen = sizeof(DWORD);
	DWORD usbCount = 0;
	ret = RegQueryValueExW(hKey,_T("Count"),NULL,NULL,(BYTE*)&usbCount,&retLen);
	if( ERROR_SUCCESS != ret || usbCount == 0)
	{
		RegCloseKey(hKey);
		return NULL;
	}

	//check max limited
	maxCount = usbCount;
	if (mlocation > usbCount || mlocation == 0){
		mlocation = 1;
	}

	//Enum
	CString strIndex; //saved index
	TCHAR strPath[MAX_PATH];

	for (WORD index = 0; index<usbCount; ++index)
	{
		strIndex.Format(_T("%d"), index);
		retLen = MAX_PATH*sizeof(TCHAR);
		memset(strPath, 0, retLen);


		ret = RegQueryValueExW(hKey,strIndex,NULL,NULL,(BYTE*)strPath,&retLen);	
		if( ERROR_SUCCESS != ret)
		{
			RegCloseKey(hKey);
			return NULL;
		}
		if (mlocation == (index+1) )
			break;
	}
	//found certain path
	RegCloseKey(hKey);
	CString str = strPath;

	return str.AllocSysString();
}

//get first printer device full path
BSTR UsbCore::getPrinterDevPath(WORD index)
{
	CString strTpl;
	strTpl = _T("\\\\?\\replacement#{28d78fad-5a12-11d1-ae5b-0000f803a8c2}");

	//get device path
	//WORD index = 0;
	DWORD maxcount = 0;
	CString strPath = findPrinterPath(index, maxcount);
	if (strPath.IsEmpty())
		return NULL;

	//fullfil path
	DWORD ret = strPath.Replace(_T('\\'), _T('#'));
	strTpl.Replace(_T("replacement"), strPath); 

	return strTpl.AllocSysString();
}

// return usb handle
HANDLE UsbCore::getUsbHandle()
{
	HANDLE m_handle = OpenUsbDevice(1);
	return m_handle;
}
//get wait time
SHORT UsbCore::getWaitTime()
{
	return m_timeout;
}
void UsbCore::setWaitTime(SHORT timeout)
{
	m_timeout = timeout;
}
BOOL UsbCore::IsPrinterAvailable(void)
{
	WORD index = 0;
	DWORD maxcount = 0;

	CString strPath = findPrinterPath(index, maxcount);
	if (strPath.IsEmpty())
		return FALSE;

	return TRUE;
}
//open usb device
HANDLE UsbCore::OpenUsbDevice(WORD index)
{
	//get device path
	CString strPath = getPrinterDevPath(index);
	if (strPath.IsEmpty())
		return INVALID_HANDLE_VALUE;

	//open device
	HANDLE usbHandle = CreateFileW(strPath,
		GENERIC_READ|GENERIC_WRITE, 
		FILE_SHARE_WRITE | FILE_SHARE_READ, 
		NULL, //must be NULL
		OPEN_EXISTING, //must be OPEN_EXISTING
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED , 
		NULL);

	return usbHandle;
}



//recv data
BOOL UsbCore::RecvData(BYTE *rBuf)
{
	DWORD dwSize, revlen, rtn;
	OVERLAPPED mr_EvtOvlp;
	memset(&mr_EvtOvlp, 0, sizeof(OVERLAPPED));	
	revlen=1024;//259;

	//get usb handle
	SHORT waitTime = getWaitTime();
	HANDLE m_device = getUsbHandle();
	if (m_device == INVALID_HANDLE_VALUE)
		return FALSE;

	mr_EvtOvlp.hEvent = CreateEvent(NULL, TRUE, FALSE, _T("ReadEvent"));

	rtn = ReadFile(m_device,&rBuf[0],revlen,&dwSize,&mr_EvtOvlp);
	if (!rtn)
	{
		rtn = GetLastError();
		if (rtn != ERROR_IO_PENDING)
		{
			CloseHandle(m_device);
			CloseHandle(mr_EvtOvlp.hEvent);
			return FALSE; //Error
		}
	}
	rtn = WaitForSingleObject(mr_EvtOvlp.hEvent,waitTime);

	while (TRUE)
	{
		rtn = GetOverlappedResult(m_device,&mr_EvtOvlp,&dwSize,FALSE);
		if (!rtn)
		{
			rtn = GetLastError();
			if (rtn != ERROR_IO_PENDING)
			{
				CloseHandle(m_device);
				CloseHandle(mr_EvtOvlp.hEvent);
				return FALSE; //Error
			}
		}
		else
		{
			break; //success
		}        
	}

	//close handles
	CloseHandle(m_device);
	CloseHandle(mr_EvtOvlp.hEvent);
	return TRUE;
}

//send data
BOOL  UsbCore::SendTask(BYTE *sBuf,DWORD MsgLen)
{
	DWORD dwSize, rtn;
	OVERLAPPED mw_EvtOvlp;
	memset(&mw_EvtOvlp, 0, sizeof(OVERLAPPED));	
	rtn = 0;
	dwSize = 0;

	//get usb handle
	HANDLE m_device = getUsbHandle();
	if (m_device == INVALID_HANDLE_VALUE)
		return FALSE;


	mw_EvtOvlp.hEvent = CreateEvent(NULL, TRUE, FALSE, _T("WriteEvent"));

	//write file	
	rtn = WriteFile(m_device,&sBuf[0],MsgLen,&dwSize,&mw_EvtOvlp);
	if (!rtn)
	{
		rtn = GetLastError();
		if (rtn != ERROR_IO_PENDING)
		{
			CloseHandle(m_device);
			CloseHandle(mw_EvtOvlp.hEvent);
			return FALSE; //Error
		}
	}
	rtn = WaitForSingleObject(mw_EvtOvlp.hEvent,100);

	//get result
	while (TRUE)
	{
		rtn = GetOverlappedResult(m_device,&mw_EvtOvlp,&dwSize,FALSE);
		if (!rtn)
		{
			rtn = GetLastError();
			if (rtn != ERROR_IO_PENDING)
			{
				CloseHandle(m_device);
				CloseHandle(mw_EvtOvlp.hEvent);
				return FALSE; //Error
			}
		}
		else
		{
			break; //success
		}        
	}

	//close handles
	CloseHandle(m_device);
	CloseHandle(mw_EvtOvlp.hEvent);
	return TRUE;
}

//device io control
BOOL UsbCore::getRequest(DWORD IoCtlCode, BYTE *inBuf, DWORD inbufLen, LPVOID lpBuffer,DWORD dwBufSize,LPDWORD pdwNumberOfByteReturned)
{
	HANDLE m_device = getUsbHandle();
	if(m_device == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	OVERLAPPED m_EvtOvlp;
	memset(&m_EvtOvlp, 0, sizeof(OVERLAPPED));
	m_EvtOvlp.hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);

	DWORD ret = FALSE;//00220034  //a1000000 0000f103
	ret = DeviceIoControl(
		m_device, 
		IoCtlCode,
		inBuf,
		inbufLen,
		lpBuffer,
		dwBufSize, 
		pdwNumberOfByteReturned,
		NULL
		);
	ret = WaitForSingleObject(m_EvtOvlp.hEvent,100);
	if(!ret)
	{
		if (ERROR_IO_PENDING != GetLastError())
		{
			CloseHandle(m_EvtOvlp.hEvent);
			CloseHandle(m_device);
			return FALSE;
		}
	}

	CloseHandle(m_EvtOvlp.hEvent);
	CloseHandle(m_device);
	return TRUE;
}



