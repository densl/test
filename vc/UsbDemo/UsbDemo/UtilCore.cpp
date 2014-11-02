
#include "stdafx.h"
#include "UtilCore.h"

UtilCore::UtilCore()
{

}


DWORD UtilCore::getDword(byte hh, byte hl, byte lh, byte ll)
{
	return ll + (lh<<8) + (hl<<16) + (hh<<24);
}

//1b53

byte asc2Hex(char inChar)
{
	if (inChar >= 'a' && inChar <= 'z')
		return inChar - 'a';

}