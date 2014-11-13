
#include "stdafx.h"

#ifdef _DEBUG
#include <assert.h>
#include "TestHeader.h"

#include "UtilCore.h"

static UtilCore utilcore;


//DWORD getDword(byte hh, byte hl, byte lh, byte ll);
//byte ascToHex(byte inChar);
//DWORD strToHexArray(byte *inBuf, byte *outBuf);
//DWORD unicodeToUTF8(LPCWSTR lpWideCharStr, int ccWideChar, LPSTR lpMuttiByteStr, int cbMultiByte);
//void hexToAsc(byte inByte, byte *outh, byte *outl);
//BOOL hexStrToAscStr(byte *inBuf, byte *outBuf);
//void dexnumToString(DWORD num, char *outStr);

static void testgetDword()
{
	testLogError("testgetDword ......");

	assert( 0x01020304 == utilcore.getDword(1,2,3,4) );

	assert( 0x12abcdef == utilcore.getDword(0x12, 0xab, 0xcd, 0xef) );

	assert( 0x0 == utilcore.getDword(0, 0, 0, 0) );

}
static void testascToHex()
{
	testLogError("testascToHex ......");

	assert( 0xf == utilcore.ascToHex('f') );
	assert( 0xe == utilcore.ascToHex('E') );
	assert( 0x3 == utilcore.ascToHex('3') );
	assert( 0xff == utilcore.ascToHex('z') );

}
//DWORD strToHexArray(byte *inBuf, byte *outBuf)
static void teststrToHexArray()
{
	testLogError("teststrToHexArray ......");

	byte *inBuf = (byte *)"3132333435";
	byte *outBuf = new byte[1024];
	memset(outBuf, 0, 1024);

	assert( 5 == utilcore.strToHexArray(inBuf, outBuf) );
	assert( '2' == outBuf[1] );

}
//unicodeToUTF8(LPCWSTR lpWideCharStr, int ccWideChar, LPSTR lpMuttiByteStr, int cbMultiByte)
static void testunicodeToUTF8()
{
	testLogError("testunicodeToUTF8 ......");

	CStringW strws = L"12345";
	CStringA strcs = "12345";

	char *buf = new char[1024];
	memset(buf, 0, 1024);

	DWORD count = utilcore.unicodeToUTF8(strws, strws.GetLength(), NULL, 0);
	assert( 0< utilcore.unicodeToUTF8(strws, strws.GetLength(), buf, 1024) );

	strcs = buf;
	assert( 0 == strcs.Compare("12345") );

}
//void hexToAsc(byte inByte, byte *outh, byte *outl);
static void testhexToAsc()
{
	testLogError("testhexToAsc ......");

	byte a, b;

	utilcore.hexToAsc(0x31, &a, &b);
	assert( a == '3' );
	assert( b == '1' );

	utilcore.hexToAsc(0xef, &a, &b);
	assert( a == 'E' );
	assert( b == 'F' );
	
}
//BOOL hexStrToAscStr(byte *inBuf, byte *outBuf);
static void testhexStrToAscStr()
{
	testLogError("testhexStrToAscStr ......");

	byte inbuf[512] = {0x12, 0x34, 0xef};
	byte outbuf[512] = {0};

	utilcore.hexStrToAscStr(inbuf, outbuf);

	assert( '1' == outbuf[0] );

	CStringA str = (char*)outbuf;
	assert( 0 == str.Compare("1234EF") );

}
//void dexnumToString(DWORD num, char *outStr);
static void testdexnumToString()
{
	testLogError("testdexnumToString ......");

	char buf[512] = {0};
	CStringA str;
	utilcore.dexnumToString(1234, buf);

	str = buf;
	assert( 0 == str.Compare("1234") );

	memset(buf, 0, 512);
	utilcore.dexnumToString(78901234, buf);
	str = buf;
	assert( 0 == str.Compare("78901234"));
}



//typedef void(*TestElement)();
static TestElement TestArray[] = {
	testgetDword,
	testascToHex,
	teststrToHexArray,
	testunicodeToUTF8,
	testhexToAsc,
	testhexStrToAscStr,
	testdexnumToString,
	NULL
};


// ***************** main test *****************************
void TestUtilCore()
{
	testLogError("Start TestUtilCore @@@@@@@@");

	TestElement k;
	for (int i=0; ;i++)
	{
		k = TestArray[i];
		// end loop
		if (k == NULL)
			break;

		k();
	}

	testLogError("End TestUtilCore   ********");
}

//end if define _DEBUG
#endif