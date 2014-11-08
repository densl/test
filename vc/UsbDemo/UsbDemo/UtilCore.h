

class UtilCore
{
public:
	UtilCore();

public:
	DWORD getDword(byte hh, byte hl, byte lh, byte ll);
	byte ascToHex(byte inChar);
	DWORD strToHexArray(byte *inBuf, byte *outBuf);
	DWORD unicodeToUTF8(LPCWSTR lpWideCharStr, int ccWideChar, LPSTR lpMuttiByteStr, int cbMultiByte);
	
public:

private:
	void setError(char * err);
	void hexToAsc(byte inByte, byte *outh, byte *outl);
	BOOL hexStrToAscStr(byte *inBuf, byte *outBuf);
};
