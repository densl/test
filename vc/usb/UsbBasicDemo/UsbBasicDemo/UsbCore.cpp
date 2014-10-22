#pragma once

#include "stdafx.h"

#include "UsbCore.h"


#include "SetupAPI.h"
#include <initguid.h>

//usb printer define
DEFINE_GUID(USB_PRINTER,0x28d78fad, 0x5a12, 0x11D1, 0xae, 0x5b, 0x00, 0x00, 0xf8, 0x03, 0xa8, 0xc2);
static TCHAR * usb_dev_enum = _T("SYSTEM\\CurrentControlSet\\Services\\usbprint\\Enum");
static TCHAR * usb_guid_str = _T("28d78fad-5a12-11d1-ae5b-0000f803a8c2");

//for test {53f56307-b6bf-11d0-94f2-00a0c91efb8b}\##?#USBSTOR#Disk&Ven_TOSHIBA&Prod_TransMemory&Rev_PMAP#001CC0C6117CED110319005C&0#{53f56307-b6bf-11d0-94f2-00a0c91efb8b}
//{4d36e967-e325-11ce-bfc1-08002be10318}
DEFINE_GUID(TEST_MCLASS,0x53f56307, 0xb6bf, 0x11d0, 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b);
static TCHAR * test_dev_enum = _T("SYSTEM\\CurrentControlSet\\Services\\USBSTOR\\Enum");
static TCHAR * test_guid_str = _T("53f56307-b6bf-11d0-94f2-00a0c91efb8b");

//global control
GUID DEV_GUID = USB_PRINTER;
static TCHAR * DEV_ENUM = usb_dev_enum;
static TCHAR * GUID_STR = usb_guid_str;


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
	TCHAR *subkey_usbprint = DEV_ENUM;//_T("SYSTEM\\CurrentControlSet\\Services\\usbprint\\Enum");
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
BSTR UsbCore::getDevInstance()
{
	DWORD ret;
	BSTR m_ErrorCode = NULL;

	//check printer states
	if ( !IsPrinterAvailable() )
		return m_ErrorCode;

	//open enum key
	HKEY hKey;
	TCHAR *subkey_usbprint = DEV_ENUM;//_T("SYSTEM\\CurrentControlSet\\Services\\usbprint\\Enum");
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
	strTpl = _T("\\\\?\\replacement#{guid}");
	strTpl.Replace(_T("guid"), GUID_STR);

	//get device string
	CString strPath = getDevInstance();
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
	hKey = SetupDiOpenClassRegKeyEx((LPGUID)&(DEV_GUID),KEY_READ,DIOCR_INTERFACE,NULL,NULL);
	if(hKey == INVALID_HANDLE_VALUE )
	{
		return m_ErrorCode;
	}

	//get device string
	CString tempPath = getDevInstance();
	if (tempPath.IsEmpty())
	{
		return m_ErrorCode;
	}
	tempPath.Replace(_T('\\'), _T('#'));
	CString printerPath = _T("##?#replacement#{guid");
	printerPath.Replace(_T("guid"), GUID_STR);
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


BOOL UsbCore::setUsbPort(WCHAR * usbport)
{
	BOOL m_ErrorCode = FALSE;

	CString temp = usbport;
	temp = temp.MakeUpper().TrimLeft(_T("USB0"));

	CString bktemp = temp;
	if (!temp.Trim(_T("0123456789")).IsEmpty())
	{
		return m_ErrorCode;
	}

	DWORD portnum = _ttoi(bktemp);
	DWORD devMax = getDevMaxCount();
	if (devMax == 0)
		return m_ErrorCode;

	WORD cur = getDevIndex();
	for(WORD i=0; i< devMax; i++)
	{
		setDevIndex(i);
		if (portnum == getPortNumber() )
			return TRUE;
		else
			continue;
	}

	setDevIndex(cur);
	return m_ErrorCode;
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

void UsbCore::closeUsbHandle(HANDLE m_handle)
{
	if (m_handle != INVALID_HANDLE_VALUE)
		CloseHandle(m_handle);
}






//recv data
DWORD UsbCore::recv(CHAR *rBuf, DWORD rLen)
{
	DWORD m_ErrorCode = 0;
	DWORD dwSize = 0; //saved recv data length

	OVERLAPPED mr_EvtOvlp;
	memset(&mr_EvtOvlp, 0, sizeof(OVERLAPPED));	


	//get usb handle
	SHORT waitTime = getWaitTime();
	HANDLE m_device = getUsbHandle();
	if (m_device == INVALID_HANDLE_VALUE)
		return m_ErrorCode;

	//read fp
	mr_EvtOvlp.hEvent = CreateEvent(NULL, TRUE, FALSE, _T("ReadEvent"));
	DWORD rtn = ReadFile(m_device,rBuf,rLen,&dwSize,&mr_EvtOvlp);
	if (!rtn)
	{
		rtn = GetLastError();
		if (rtn != ERROR_IO_PENDING)
		{
			closeUsbHandle(m_device);
			CloseHandle(mr_EvtOvlp.hEvent);
			return m_ErrorCode; //Error
		}
	}

	rtn = WaitForSingleObject(mr_EvtOvlp.hEvent,waitTime);

	//get result
	while (TRUE)
	{
		rtn = GetOverlappedResult(m_device,&mr_EvtOvlp,&dwSize,FALSE);
		if (!rtn)
		{
			rtn = GetLastError();
			if (rtn != ERROR_IO_PENDING)
			{
				closeUsbHandle(m_device);
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
	closeUsbHandle(m_device);
	CloseHandle(mr_EvtOvlp.hEvent);
	return dwSize;
}

//send data
DWORD UsbCore::send(CHAR *sBuf,DWORD sLen)
{
	DWORD m_ErrorCode = 0;
	DWORD dwSize = 0; //saved data length had been send

	OVERLAPPED mw_EvtOvlp;
	memset(&mw_EvtOvlp, 0, sizeof(OVERLAPPED));	


	//get usb handle
	HANDLE m_device = getUsbHandle();
	if (m_device == INVALID_HANDLE_VALUE)
		return m_ErrorCode;

	//write file	
	mw_EvtOvlp.hEvent = CreateEvent(NULL, TRUE, FALSE, _T("WriteEvent"));
	DWORD rtn = WriteFile(m_device,sBuf,sLen,&dwSize,&mw_EvtOvlp);
	if (!rtn)
	{
		rtn = GetLastError();
		if (rtn != ERROR_IO_PENDING)
		{
			closeUsbHandle(m_device);
			CloseHandle(mw_EvtOvlp.hEvent);
			return m_ErrorCode;
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
				closeUsbHandle(m_device);
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
	closeUsbHandle(m_device);
	CloseHandle(mw_EvtOvlp.hEvent);
	return dwSize;
}

BSTR UsbCore::getRecvStr()
{
	BSTR m_ErrorCode = NULL;

	CHAR *strChar = new CHAR[4096];
	memset(strChar, 0, sizeof(CHAR) * 4096);

	TCHAR *strTchar = new TCHAR[4096];
	memset(strTchar, 0, sizeof(CHAR) * 4096);

	DWORD retLen = recv(strChar, 4096);
	if (retLen == 0)
		return m_ErrorCode;

	//to void mistakes
	for (DWORD i=0; i<retLen; i++)
	{
		if ( strChar[i] == 0)
			strChar[i] = 0x20;
	}

	DWORD len = _mbstowcsz(0, strChar, 0);
	_mbstowcsz(strTchar, strChar, len);

	CString strResult = strTchar;
	return strResult.AllocSysString();
}

DWORD UsbCore::sendData(WCHAR *inData, BOOL dataflag)
{
	DWORD m_ErrorCode = 0;
	WCHAR * data = inData;

	//check over flow
	DWORD len = _wcstombsz(0, data, 0);
	if (len == 0)
		return m_ErrorCode;

	DWORD arrayLen = len + 2;

	CHAR * tmpBuf = new CHAR[arrayLen];
	memset(tmpBuf, 0, sizeof(CHAR) * arrayLen);
	_wcstombsz(tmpBuf, data, arrayLen);

	//data
	if (dataflag)
	{
		return send(tmpBuf, len);
	}

	//commands
	CString test = data;
	CString tpl = _T("0123456789abcdefABCDEF");
	if ( !test.Trim(tpl).IsEmpty() )
		return m_ErrorCode;

	CHAR * cmd_send = new CHAR[arrayLen];
	memset(cmd_send, 0, sizeof(BYTE) * arrayLen);

	len = strToHexArray(tmpBuf, cmd_send);
	return send(cmd_send, len);
}


//device io control
DWORD UsbCore::getRequestFull(DWORD IoCtlCode, CHAR *inBuf, DWORD inbufLen, LPVOID lpBuffer,DWORD dwBufSize)
{
	DWORD m_ErrorCode = 0;
	HANDLE m_device = getUsbHandle();
	if(m_device == INVALID_HANDLE_VALUE)
	{
		return m_ErrorCode;
	}

	OVERLAPPED m_EvtOvlp;
	memset(&m_EvtOvlp, 0, sizeof(OVERLAPPED));
	m_EvtOvlp.hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);

	DWORD retLen = 0;
	DWORD ret = DeviceIoControl(
		m_device, 
		IoCtlCode,
		inBuf,
		inbufLen,
		lpBuffer,
		dwBufSize, 
		&retLen,
		&m_EvtOvlp
		);
	ret = WaitForSingleObject(m_EvtOvlp.hEvent,100);
	ret = GetOverlappedResult(m_device,&m_EvtOvlp,&retLen,FALSE);
	if(!ret)
	{
		ret = GetLastError();
		if (ERROR_IO_PENDING != ret)
		{
			closeUsbHandle(m_device);
			CloseHandle(m_EvtOvlp.hEvent);
			return m_ErrorCode;
		}
	}

	closeUsbHandle(m_device);
	CloseHandle(m_EvtOvlp.hEvent);
	return retLen;
}

BSTR UsbCore::getRequestStr(DWORD IoCtlCode, CHAR *inBuf, DWORD inbufLen)
{
	static BYTE * rBuf = new BYTE[4096];
	memset(rBuf, 0, sizeof(BYTE) * 4096);
	DWORD rLen = 4096;

	static TCHAR *tempStr = new TCHAR[4096];
	memset(tempStr, 0, sizeof(TCHAR) * 4096);	

	DWORD  retLen = getRequestFull(IoCtlCode, inBuf, inbufLen, rBuf, rLen);
	if ( retLen > 0)
	{
		DWORD len = _mbstowcsz(0, (char*)rBuf, 0);
		if (len > 0)
		{
			_mbstowcsz(tempStr, (char*)rBuf, len);
		}
	}

	CString strResult = tempStr;
	return strResult.AllocSysString();
}

BSTR UsbCore::getReqStr(DWORD IoCtlCode)
{
	return getRequestStr(IoCtlCode, NULL, 0);
}

DWORD UsbCore::strToHexArray(CHAR * inBuf, CHAR * outBuf)
{
	DWORD len = strlen(inBuf);
	CHAR temp = 0;
	DWORD index = 0;
	for (WORD i=0; i<len; i++)
	{
		if ( 0 == (i&0x1) )//even
		{
			temp = 0;
			temp = AscToHex(inBuf[i]) << 8;
		}
		else
		{//odd
			temp += AscToHex(inBuf[i]);
			outBuf[index++] = temp;
		}
	}//ignore tail
	return index;
}

CHAR UsbCore::AscToHex(CHAR asc)
{
	if (asc >= '0' && asc <= '9')
		return asc - '0';
	if (asc >= 'a' && asc <= 'f')
		return asc - 'a';
	if (asc >= 'A' && asc <= 'F')
		return asc - 'A';
	
	return 0;
}


void UsbCore::logUsb(WCHAR * str)
{
		return;
}