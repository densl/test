
#pragma once
#include "stdafx.h"

class RegCore{
public:
	RegCore();
	~RegCore();
public:
	BOOL uOpenKey(HKEY hKey, LPCTSTR subKey, PHKEY retKey);
	BOOL uGetValue(HKEY hKey, LPCTSTR valueName, LPBYTE outBuf, LPDWORD cbLen);
	BOOL uGetClassKey(LPGUID devGuid, PHKEY phKey);
};