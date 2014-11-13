
#include "stdafx.h"
#include "UtilCore.h"


UtilCore::UtilCore()
{

}

UtilCore::~UtilCore()
{

}


// 1,2,3,4  ->  0x01020304
// 0x12, 0xab, 0xcd, 0xef  ->  0x12abcdef
DWORD UtilCore::getDword(byte hh, byte hl, byte lh, byte ll)
{
	return ll + (lh<<8) + (hl<<16) + (hh<<24);
}

// 31  ->  1
// 'f' ->  0x0f
byte UtilCore::ascToHex(byte inChar)
{
	if (inChar >= 'a' && inChar <= 'f')
		return inChar - 'a' + 10;
	if (inChar >= 'A' && inChar <= 'F')
		return inChar - 'A' + 10;
	if (inChar >= '0' && inChar <= '9')
		return inChar - '0';

	return 0xFF; // indicated error
}

// 313233  ->  "123"
DWORD UtilCore::strToHexArray(byte *inBuf, byte *outBuf)
{
	DWORD len = strlen( (char*)inBuf );

	// shield
	if ( ((len&0x01) == 1) || inBuf == NULL || outBuf == NULL)
		return 0;

	CHAR temp = 0;
	DWORD index = 0;
	for (WORD i=0; i<len; i++)
	{
		if ( 0 == (i&0x1) )//even
		{
			temp = 0;
			temp = ascToHex(inBuf[i]) << 4;
		}
		else
		{//odd
			temp += ascToHex(inBuf[i]);
			outBuf[index++] = temp;
		}
	}
	return index;
}

// 0x31 -> 0x33 0x31 ; 0x3f -> '3' 'F'
void UtilCore::hexToAsc(byte inByte, byte *outh, byte *outl)
{
	byte temp;
	temp = (inByte & 0xF0) >> 4;
	*outh = (temp>9) ? (temp -10 + 'A') : (temp + '0');

	temp = (inByte & 0x0F);
	*outl = (temp>9) ? (temp -10 + 'A') : (temp + '0');
}

// 0x31 0x2f  ->  '3' '1' '2' 'F'
BOOL UtilCore::hexStrToAscStr(byte *inBuf, byte *outBuf)
{
	int len = strlen( (char*)inBuf );
	if (len == 0 || inBuf == NULL || outBuf == NULL)
		return FALSE;

	for (int i=0,j=0; i<len; i++)
	{
		hexToAsc(inBuf[i], &outBuf[j], &outBuf[j+1]);
		j += 2;
	}
	return TRUE;
}

// L"1234"  ->  "1234"
DWORD UtilCore::unicodeToUTF8(LPCWSTR lpWideCharStr, int ccWideChar, LPSTR lpMuttiByteStr, int cbMultiByte)
{
	if (lpWideCharStr == NULL || ccWideChar < 1)
		return 0;

	return WideCharToMultiByte(CP_UTF8, 0, lpWideCharStr, ccWideChar, lpMuttiByteStr, cbMultiByte, NULL, 0);
}

// 1234  ->  "1234"
void UtilCore::dexnumToString(DWORD num, char *outStr)
{
	if (outStr == NULL)
		return;

	char str[50] = {0};

	int i = 0;
	char temp;
	do 
	{
		temp = num % 10;
		str[i++] = temp + '0';
		num = num/10;
	} while ( num != 0);

	int k = 0;
	while(i--)
	{
		outStr[k++] = str[i];
	}
	//outStr[k] = str[0];

	return;
}

//static LPTSTR logFile = _T("d:\\usbLog.txt");
//void UtilCore::setError(char * err)
//{
//	char msg[4096] = {0};
//
//	SYSTEMTIME mtime;
//	GetSystemTime(&mtime);
//
//	sprintf_s(msg, "%04d%02d%02d-%02d.%02d.%02d:  %s\n", mtime.wYear, mtime.wMonth, mtime.wDay, mtime.wHour,mtime.wMinute, mtime.wSecond, err);
//
//	CFile cf;
//	BOOL ret = cf.Open(logFile, CFile::modeCreate|CFile::modeWrite|CFile::modeNoTruncate);
//	if (ret == FALSE)
//		return;
//
//	ULONGLONG fLen = cf.SeekToEnd();
//
//	cf.Write(msg, strlen(msg));
//	cf.Close();
//}


