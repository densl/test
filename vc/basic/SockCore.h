
#pragma once

#include "stdafx.h"

class SockCore{
public:
	SockCore();

private:
	SOCKET g_sock;
	DWORD g_errorCode;

	SOCKET getSocket();
	
// utils *******************************************
private:
	DWORD strToHexArray(CHAR * inBuf, CHAR * outBuf);
	CHAR AscToHex(CHAR asc);
	BOOL isWritable(SOCKET m_sock);
// for test ****************************************
private:
	void setSocket(SOCKET m_sock);
	void setError(char * err);

public:
	DWORD sendSocket(char * sBuf, DWORD sLen);
	DWORD recvSocket(char * rBuf, DWORD rLen);
	DWORD sendData(char *inData, BOOL dataflag);

	BOOL isAvailable();	
	BOOL connectSocket(char * ipAddr, WORD port);
	void closeSocket();
	
	
};