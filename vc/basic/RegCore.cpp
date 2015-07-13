
#pragma once
#include "stdafx.h"
#include "RegCore.h"

#include "SetupAPI.h"
#include <initguid.h>

#pragma comment(lib, "SetupApi")

//HKEY_CLASSES_ROOT
//HKEY_CURRENT_CONFIG
//HKEY_CURRENT_USER
//HKEY_LOCAL_MACHINE
//HKEY_USERS

RegCore::RegCore()
{

}

RegCore::~RegCore()
{

}



BOOL RegCore::uOpenKey(HKEY hKey, LPCTSTR subKey, PHKEY retKey)
{
	BOOL m_ErrorCode = FALSE;
	long ret = RegOpenKeyEx(hKey, subKey, 0, KEY_READ, retKey);
	if( ERROR_SUCCESS !=  ret)
	{
		return m_ErrorCode;
	}

	return TRUE;
}



BOOL RegCore::uGetValue(HKEY hKey, LPCTSTR valueName, LPBYTE outBuf, LPDWORD cbLen)
{
	BOOL m_ErrorCode = FALSE;
	long ret = RegQueryValueEx(hKey, valueName, NULL, NULL, outBuf, cbLen);
	if (ret != ERROR_SUCCESS)
	{
		return m_ErrorCode;
	}
	return TRUE;
}

// ****** SYSTEM\CurrentControlSet\Control\DeviceClasses ******
BOOL RegCore::uGetClassKey(LPGUID devGuid, PHKEY phKey)
{
	BOOL m_ErrorCode = FALSE;

	HKEY hKey;
	hKey = SetupDiOpenClassRegKeyEx(devGuid, KEY_READ, DIOCR_INTERFACE, NULL, NULL);
	if(hKey == INVALID_HANDLE_VALUE )
	{
		DWORD ret = GetLastError();
		return m_ErrorCode;
	}
	*phKey = hKey;
	return TRUE;
}



//usb printer define
//DEFINE_GUID(USB_PRINTER,0x28d78fad, 0x5a12, 0x11D1, 0xae, 0x5b, 0x00, 0x00, 0xf8, 0x03, 0xa8, 0xc2);
//static TCHAR * usb_dev_enum = _T("SYSTEM\\CurrentControlSet\\Services\\usbprint\\Enum");
//static TCHAR * usb_guid_str = _T("28d78fad-5a12-11d1-ae5b-0000f803a8c2");
//
////global control
//static GUID DEV_GUID = USB_PRINTER;
//static TCHAR * DEV_ENUM = usb_dev_enum;
//static TCHAR * GUID_STR = usb_guid_str;
//void RegCore::test()
//{
//	HKEY hKey;
//	BOOL ret = uOpenKey(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\services\\360WifiNAT"), &hKey);
//
//	TCHAR buf[1024] = {0};
//	DWORD retLen = 2;
//
//	ret = RegQueryValueEx(hKey,_T("Start"),NULL,NULL,(BYTE*)buf,&retLen);
//	CString k = buf;
//	DWORD kk  = k.GetLength();
//
//	RegCloseKey(hKey);
//}

//TCHAR strPathTmp[MAX_PATH];
//DWORD retLen = MAX_PATH * sizeof(TCHAR);
//memset(strPathTmp, 0, retLen);
//
////query device string
//ret = RegQueryValueExW(hKey,strIndex,NULL,NULL,(BYTE*)strPathTmp,&retLen);
//RegCloseKey(hKey);
//if( ERROR_SUCCESS != ret)
//{
//	return m_ErrorCode;
//}



static DWORD getRegValue(LPCTSTR keyName, LPCTSTR keyValue, LPVOID regValue, LPDWORD len) 
{
	DWORD ret = 0;

	//open key
	HKEY hkey;
	ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, keyName, 0, KEY_READ, &hkey);
	if ( ERROR_SUCCESS != ret )
	{
		return ret;
	}

	ret = RegQueryValueEx(heky, keyValue, NULL, NULL, (BYTE*)regValue, len);	RegCloseKey(hkey);
	return ret;
}

static DWORD getRegList(LPCTSTR keyName, CString& keyList, LPDWORD count)
{
	DWORD ret = 0;
	HKEY hkey;
	ret = RegOpenKeyEx( HKEY_LOCAL_MACHINE, keyName, 0, KEY_READ, &hkey);
	if ( ERROR_SUCCESS != ret )
	{
		return ret;
	}

	keyList.Empty();
	DWORD keyCount = 0;
	char temp[512] = {0};
	DWORD retLen = 512;
	for (int i=0;;i++)
	{
		retLen = 512;
		memset(temp, 0, 512);
		ret = RegEnumKeyEx(hkey, i, temp, &retLen, NULL, NULL, NULL, NULL);
		if ( ERROR_SUCCESS != ret ) break;

		keyCount++;
		keyList.Append(temp);
		keyList.Append(";");

	}//end for

	*count = keyCount;
	return ( keyCount > 0) ? ERROR_SUCCESS : ret;
}

