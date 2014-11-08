
#include "stdafx.h"
#include "UsbCore.h"

#include "SetupAPI.h"
#include <initguid.h>

#pragma comment(lib, "SetupApi.lib")

//usb printer define
DEFINE_GUID(USB_PRINTER,0x28d78fad, 0x5a12, 0x11D1, 0xae, 0x5b, 0x00, 0x00, 0xf8, 0x03, 0xa8, 0xc2);
static TCHAR * usb_dev_enum = _T("SYSTEM\\CurrentControlSet\\Services\\usbprint\\Enum");
static TCHAR * usb_guid_str = _T("28d78fad-5a12-11d1-ae5b-0000f803a8c2");

//global control
GUID DEV_GUID = USB_PRINTER;
static TCHAR * DEV_ENUM = usb_dev_enum;
static TCHAR * GUID_STR = usb_guid_str;


UsbCore::UsbCore()
{
	//init
	setHandle(INVALID_HANDLE_VALUE);
	m_devIndex = 1; //first printer device
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

BOOL UsbCore::uOpen()
{
	int m_ErrorCode = 0;

	////open device ****************************
	//HANDLE mHandle = CreateFileA(path,
	//	GENERIC_READ|GENERIC_WRITE, 
	//	FILE_SHARE_WRITE | FILE_SHARE_READ, 
	//	NULL, //must be NULL
	//	OPEN_EXISTING, //must be OPEN_EXISTING
	//	FILE_FLAG_OVERLAPPED, 
	//	NULL);

	HANDLE mHandle = OpenUsbDevice(); //get first usb device
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


// ***************************************************************
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
BOOL UsbCore::uSetUsbPort(WCHAR * usbport)
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