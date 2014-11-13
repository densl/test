

class UtilCore
{
public:
	UtilCore();
	~UtilCore();
public:
	DWORD getDword(byte hh, byte hl, byte lh, byte ll);
	byte ascToHex(byte inChar);
	DWORD strToHexArray(byte *inBuf, byte *outBuf);
	DWORD unicodeToUTF8(LPCWSTR lpWideCharStr, int ccWideChar, LPSTR lpMuttiByteStr, int cbMultiByte);
	void hexToAsc(byte inByte, byte *outh, byte *outl);
	BOOL hexStrToAscStr(byte *inBuf, byte *outBuf);
	void dexnumToString(DWORD num, char *outStr);
};
