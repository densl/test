
#pragma once

#include "stdafx.h"

#define ERROR_IP_ADDRESS 300    //ip address error
#define ERROR_WSASTARTUP 400    //WSAStartup  error
#define ERROR_SOCKET_CREATE 401 //create socket error
#define ERROR_SOCKET_CONNECT 402 //connect socket error
#define ERROR_SOCKET_CLOSE 403  //close socket error
#define ERROR_SOCKET_GET 404 //get socket error
#define ERROR_SOCKET_SEND 405 //send socket data error
#define ERROR_SOCKET_RECV 406 //recv socket data error
#define ERROR_SOCKET_IOCTL 407 //ioctl socket

class SockCore{
public:
	SockCore();

private:
	SOCKET g_sock;
	DWORD g_errorCode;

	SOCKET getSocket();
	

	DWORD strToHexArray(CHAR * inBuf, CHAR * outBuf);
	CHAR AscToHex(CHAR asc);	

public:
	//SOCKET connectSockBase(DWORD ipAddr, WORD port);	
	//BOOL connectSock(WCHAR * inIpAddr, WCHAR * inPort);
	

	//DWORD sendBase(CHAR * sbuf, DWORD sLen);
	//DWORD recvBase(CHAR * rbuf, DWORD rLen);

	DWORD sendSocket(char * sBuf, DWORD sLen);
	DWORD recvSocket(char * rBuf, DWORD rLen);

	DWORD sendDataW(WCHAR *inData, BOOL dataflag);
	BSTR getRecvStrW();

	BOOL isAvailable();	
	BOOL connectSocket(char * ipAddr, WORD port);

	void setErrorCode(DWORD errorCode);
	void closeSocket();
	void setSocket(SOCKET m_sock);
	BOOL isWritable(SOCKET m_sock);
};