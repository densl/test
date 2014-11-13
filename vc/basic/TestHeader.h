
#include "stdafx.h"

#ifdef _DEBUG

typedef void(*TestElement)();

void testLogError(char *err);


#endif

static LPTSTR logFile = _T("d:\\testLogError.txt");
static void testLogError(char * err)
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