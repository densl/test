
#include "stdafx.h"
#include "TestHeader.h"
#include "RegCore.h"

#ifdef _DEBUG
#include <assert.h>


static RegCore regcore;
static void testuOpenKey()
{
	testLogError("testuOpenKey ......");

	LPCTSTR subKey = _T("SYSTEM\\CurrentControlSet\\services\\Disk\\Enum");
	HKEY hKey = HKEY_LOCAL_MACHINE;
	HKEY retKey;

	assert( regcore.uOpenKey(hKey, subKey, &retKey) == TRUE );
}

static void testuGetValue()
{
	testLogError("testuGetValue ......");

	LPCTSTR subKey = _T("SYSTEM\\CurrentControlSet\\services\\Disk\\Enum");
	HKEY hKey = HKEY_LOCAL_MACHINE;
	HKEY retKey;
	
	assert( regcore.uOpenKey(hKey, subKey, &retKey) == TRUE );

	LPCTSTR valueName = _T("Count");
	DWORD count = 0;
	DWORD len = 4;
	
	assert( regcore.uGetValue(retKey, valueName, (byte*)&count, &len) == TRUE );
}


//HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\DeviceClasses
DEFINE_GUID(USB_PRINTER,0x28d78fad, 0x5a12, 0x11D1, 0xae, 0x5b, 0x00, 0x00, 0xf8, 0x03, 0xa8, 0xc2);
static void testuGetClassKey()
{
	testLogError("testuGetClassKey ......");

	GUID usbGuid = USB_PRINTER;
	HKEY hKey;
	
	assert(TRUE == regcore.uGetClassKey(&usbGuid, &hKey) );
}

//typedef void(*TestElement)();
static TestElement TestArray[] = {
	testuOpenKey,
	testuGetValue,
	testuGetClassKey,
	NULL
};

// ***************** main test *****************************
void TestRegCore()
{
	testLogError("Start TestRegCore @@@@@@@@");

	TestElement k;
	for (int i=0; ;i++)
	{
		k = TestArray[i];
		// end loop
		if (k == NULL)
			break;

		k();
	}

	testLogError("End TestRegCore   ********");
}

//end if define _DEBUG
#endif