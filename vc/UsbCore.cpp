#pragma once

#include "stdafx.h"

#include "UsbCore.h"


#include "SetupAPI.h"
#include <initguid.h>


DEFINE_GUID(USB_PRINTER,0x28d78fad, 0x5a12, 0x11D1, 0xae, 0x5b, 0x00, 0x00, 0xf8, 0x03, 0xa8, 0xc2);

//
UsbCore::UsbCore()
{
	m_timeout = 5000; //5s
	m_devIndex = 1; //first printer device
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

//get device index
WORD UsbCore::getDevIndex()
{
	return m_devIndex;
}
//device index < max usb count
void UsbCore::setDevIndex(WORD devIndex)
{
	m_devIndex = devIndex;
}

//get max usb printer device count
DWORD UsbCore::getDevMaxCount()
{
	DWORD m_ErrorCode = 0;
	DWORD ret = 0;

	//open enum key
	HKEY hKey;
	TCHAR *subkey_usbprint = _T("SYSTEM\\CurrentControlSet\\Services\\usbprint\\Enum");
	ret = RegOpenKeyExW(HKEY_LOCAL_MACHINE,subkey_usbprint,0,KEY_READ,&hKey);
	if( ERROR_SUCCESS !=  ret)
	{
		return m_ErrorCode;
	}

	//get usb printer count
	DWORD retLen = sizeof(DWORD);
	DWORD usbCount = 0;
	ret = RegQueryValueExW(hKey,_T("Count"),NULL,NULL,(BYTE*)&usbCount,&retLen);
	if( ERROR_SUCCESS != ret)
	{
		RegCloseKey(hKey);
		return m_ErrorCode;
	}
	return usbCount;
}
//check if printer is available
BOOL UsbCore::IsPrinterAvailable(void)
{
	WORD index = getDevIndex();
	DWORD maxcount = getDevMaxCount();

	if (maxcount == 0 || index > maxcount)
		return FALSE;

	return TRUE;
}

//get device string
BSTR UsbCore::getDevString()
{
	DWORD ret;
	BSTR m_ErrorCode = NULL;

	//check printer states
	if ( !IsPrinterAvailable() )
		return m_ErrorCode;

	//open enum key
	HKEY hKey;
	TCHAR *subkey_usbprint = _T("SYSTEM\\CurrentControlSet\\Services\\usbprint\\Enum");
	ret = RegOpenKeyExW(HKEY_LOCAL_MACHINE,subkey_usbprint,0,KEY_READ,&hKey);
	if( ERROR_SUCCESS !=  ret)
	{
		return m_ErrorCode;
	}

	//init variables
	CString strIndex; //saved device index
	strIndex.Format(_T("%d"), getDevIndex()-1);

	TCHAR strPathTmp[MAX_PATH];
	DWORD retLen = MAX_PATH * sizeof(TCHAR);
	memset(strPathTmp, 0, retLen);

	//query device string
	ret = RegQueryValueExW(hKey,strIndex,NULL,NULL,(BYTE*)strPathTmp,&retLen);
	RegCloseKey(hKey);
	if( ERROR_SUCCESS != ret)
	{
		return m_ErrorCode;
	}

	CString strPath = strPathTmp;
	return strPath.AllocSysString();
}

//get first printer device full path
BSTR UsbCore::getDevicePath()
{
	CString strTpl;
	BSTR m_ErrorCode = NULL;
	strTpl = _T("\\\\?\\replacement#{28d78fad-5a12-11d1-ae5b-0000f803a8c2}");

	//get device string
	CString strPath = getDevString();
	if (strPath.IsEmpty())
		return m_ErrorCode;

	//fullfil path
	DWORD ret = strPath.Replace(_T('\\'), _T('#'));
	strTpl.Replace(_T("replacement"), strPath); 

	return strTpl.AllocSysString();
}


//get port parameters key
BOOL UsbCore::getPortParametersKey(HKEY &PortParaKey)
{
	BOOL m_ErrorCode = FALSE;
	TCHAR subKeyStr[] = _T("#");
	TCHAR subDevKeyStr[] = _T("Device Parameters");
	TCHAR subPortDscStr[] = _T("Port Description");

	//open class reg
	DWORD ret;
	HKEY hKey;
	hKey = SetupDiOpenClassRegKeyEx((LPGUID)&(USB_PRINTER),KEY_READ,DIOCR_INTERFACE,NULL,NULL);
	if(hKey == INVALID_HANDLE_VALUE )
	{
		return m_ErrorCode;
	}

	//get device string
	CString tempPath = getDevString();
	if (tempPath.IsEmpty())
	{
		return m_ErrorCode;
	}
	tempPath.Replace(_T('\\'), _T('#'));
	CString printerPath = _T("##?#replacement#{28d78fad-5a12-11d1-ae5b-0000f803a8c2}");
	printerPath.Replace(_T("replacement"), tempPath);

	//open printer class path
	HKEY	hSubKey;
	ret = RegOpenKeyEx(hKey, printerPath, 0, KEY_READ, &hSubKey);
	RegCloseKey(hKey);
	if (ret)
	{	
		return m_ErrorCode;
	}

	//open #
	HKEY	hSub1Key;
	ret = RegOpenKeyEx(hSubKey, subKeyStr, 0, KEY_READ, &hSub1Key);
	RegCloseKey(hSubKey);
	if (ret)
	{		
		return m_ErrorCode;
	}

	//open device parameter
	ret = RegOpenKeyEx(hSub1Key, subDevKeyStr, 0, KEY_READ, &PortParaKey);
	RegCloseKey(hSub1Key);
	if (ret)
	{		
		return m_ErrorCode;
	}

	// "SYSTEM\\CurrentControlSet\\DeviceClasses\\##?#USB****\\#\\Device Parameters"
	return TRUE;
}

//get port description
BSTR UsbCore::getPortDescription()
{
	BSTR m_ErrorCode = NULL;
	TCHAR subPortDscStr[] = _T("Port Description");

	//open port parameters key reg
	DWORD ret;
	HKEY hKey;
	ret = getPortParametersKey(hKey);
	if (!ret) //False then return
	{	
		return m_ErrorCode;
	}

	//get port description
	TCHAR	datBuf[512];
	DWORD retLen = sizeof(TCHAR) * 512;
	memset(datBuf, 0, retLen);
	
	ret = RegQueryValueEx(hKey, subPortDscStr, NULL, NULL, (BYTE*)datBuf, &retLen);
	RegCloseKey(hKey);
	if (ret)
	{	
		return m_ErrorCode;
	}

	CString strRet = datBuf;
	return strRet.AllocSysString();
}
//get the port number of usb printer
DWORD UsbCore::getPortNumber()
{
	DWORD m_ErrorCode = 0;
	TCHAR subPortDscStr[] = _T("Port Number");
	DWORD ret;

	//open class reg	
	HKEY hKey;
	ret = getPortParametersKey(hKey);
	if (!ret) //False then return
	{	
		return m_ErrorCode;
	}

	//get port number
	DWORD portNum = 0;
	DWORD retLen = 8;
	ret = RegQueryValueEx(hKey, subPortDscStr, NULL, NULL, (LPBYTE)&portNum, &retLen);
	RegCloseKey(hKey);
	if (ret)
	{	
		return m_ErrorCode;
	}

	return portNum;
}


//open usb device
HANDLE UsbCore::OpenUsbDevice()
{
	//get device path
	CString strPath = getDevicePath();
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

// return usb handle
HANDLE UsbCore::getUsbHandle()
{
	HANDLE m_handle = OpenUsbDevice(); //get first usb device
	return m_handle;
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
