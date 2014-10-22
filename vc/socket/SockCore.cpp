
#pragma once
#include "stdafx.h"
#include "SockCore.h"


SockCore::SockCore()
{
	g_errorCode = 0;
	setSocket(INVALID_SOCKET);
}



//connect socket
BOOL SockCore::connectSocket(char * ipAddr, WORD port)
{
	BOOL m_ErrorCode = FALSE;

	int rtn = 0;
	SOCKET sock = getSocket();
	WSADATA wsaData;
	memset(&wsaData, 0, sizeof(wsaData));

	//make sure close before create
	if (sock != INVALID_SOCKET)
	{
		closeSocket();
		sock = INVALID_SOCKET;
	}

	//initial
	rtn = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (rtn != 0)
	{
		setErrorCode(ERROR_WSASTARTUP);
		return m_ErrorCode;
	}

	//create socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		setErrorCode(ERROR_SOCKET_CREATE);
		return m_ErrorCode;
	}

	//set unblocking
	u_long iMode = 1; //unblock
	rtn = ioctlsocket(sock, FIONBIO, &iMode);
	if (rtn != 0)
	{
		setErrorCode(ERROR_SOCKET_IOCTL);
		return m_ErrorCode;
	}

	//set ip address and port
	SOCKADDR_IN inAddr;
	inAddr.sin_family = AF_INET;
	inAddr.sin_addr.s_addr = inet_addr(ipAddr);
	inAddr.sin_port = htons(port);
	if (inAddr.sin_addr.s_addr == INADDR_NONE) //invalid ipv4 address
	{
		setErrorCode(ERROR_IP_ADDRESS);
		return m_ErrorCode;
	}

	//connect
	rtn = connect(sock, (SOCKADDR *)&inAddr, sizeof(inAddr));

	if ( isWritable(sock) )
	{
		setSocket(sock);
		return TRUE;
	}
	else
	{
		setSocket(INVALID_SOCKET);
		return m_ErrorCode;
	}
	////select part
	//fd_set rFds, wFds, eFds;
	//FD_ZERO(&rFds);
	//FD_ZERO(&wFds);
	//FD_ZERO(&eFds);

	//FD_SET(sock, &wFds);
	//timeval mtime = {0, 0};

	//rtn = select(NULL, &rFds, &wFds, &eFds, &mtime);


	//if (FD_ISSET(sock, &wFds))
	//{
	//	setSocket(sock);
	//}
	//else
	//{
	//	setSocket(INVALID_SOCKET);
	//}

	//FD_CLR(sock, &wFds);
	return TRUE;
}

void SockCore::closeSocket()
{
	int rtn = 0;
	SOCKET sock = getSocket();
	if (sock != INVALID_SOCKET)
		rtn= closesocket(sock);

	rtn = (WSACleanup() == SOCKET_ERROR) ? SOCKET_ERROR : rtn;

	setSocket(INVALID_SOCKET);

	if (rtn == SOCKET_ERROR)
	{
		setErrorCode(ERROR_SOCKET_CLOSE);
		return;
	}
}

SOCKET SockCore::getSocket()
{
	return g_sock;
}

void SockCore::setSocket(SOCKET m_sock)
{
	g_sock = m_sock;
}

DWORD SockCore::sendSocket(char * sBuf, DWORD sLen)
{
	DWORD m_ErrorCode = 0;
	int rtn = 0;

	SOCKET sock = getSocket();
	if (sock == INVALID_SOCKET)
	{
		setErrorCode(ERROR_SOCKET_GET);
		return m_ErrorCode;
	}

	rtn = send(sock, sBuf, sLen, 0);
	if (rtn == SOCKET_ERROR)
	{
		setErrorCode(ERROR_SOCKET_SEND);
		return m_ErrorCode;
	}

	return rtn;
}

DWORD SockCore::recvSocket(char * rBuf, DWORD rLen)
{
	DWORD m_ErrorCode = 0;
	int rtn = 0;

	SOCKET sock = getSocket();
	if (sock == INVALID_SOCKET)
	{
		setErrorCode(ERROR_SOCKET_GET);
		return m_ErrorCode;
	}

	rtn = recv(sock, rBuf, rLen, 0);
	if (rtn == SOCKET_ERROR)
	{
		setErrorCode(ERROR_SOCKET_RECV);
		return m_ErrorCode;
	}

	return rtn;
}

DWORD SockCore::sendDataW(WCHAR *inData, BOOL dataflag)
{
	DWORD m_ErrorCode = 0;
	WCHAR * data = inData;

	if (!isAvailable())
		return m_ErrorCode; //must connect first

	//check over flow
	DWORD len = _wcstombsz(0, data, 0);
	if (len == 0)
		return m_ErrorCode;

	DWORD arrayLen = len + 2;

	CHAR * tmpBuf = new CHAR[arrayLen];
	memset(tmpBuf, 0, sizeof(CHAR) * arrayLen);
	_wcstombsz(tmpBuf, data, arrayLen);

	//data
	if (dataflag)
	{
		return sendSocket(tmpBuf, len-1);
	}

	//commands
	CString test = data;
	CString tpl = _T("0123456789abcdefABCDEF");
	if ( !test.Trim(tpl).IsEmpty() )
		return m_ErrorCode;

	CHAR * cmd_send = new CHAR[arrayLen];
	memset(cmd_send, 0, sizeof(BYTE) * arrayLen);

	len = strToHexArray(tmpBuf, cmd_send);
	return sendSocket(cmd_send, len);
}

BSTR SockCore::getRecvStrW()
{
	BSTR m_ErrorCode = NULL;

	if (!isAvailable())
		return m_ErrorCode; //must connect first

	CHAR *strChar = new CHAR[4096];
	memset(strChar, 0, sizeof(CHAR) * 4096);

	TCHAR *strTchar = new TCHAR[4096];
	memset(strTchar, 0, sizeof(CHAR) * 4096);

	DWORD retLen = recvSocket(strChar, 4096);
	if (retLen == 0)
		return m_ErrorCode;

	//to void mistakes
	for (DWORD i=0; i<retLen; i++)
	{
		if ( strChar[i] == 0)
			strChar[i] = 0x20;
	}

	DWORD len = _mbstowcsz(0, strChar, 0);
	_mbstowcsz(strTchar, strChar, len);

	CString strResult = strTchar;
	return strResult.AllocSysString();
}

//12345  -->  0x12 0x34
DWORD SockCore::strToHexArray(char * inBuf, char * outBuf)
{
	DWORD len = strlen(inBuf);
	CHAR temp = 0;
	DWORD index = 0;
	for (WORD i=0; i<len; i++)
	{
		if ( 0 == (i&0x1) )//even
		{
			temp = 0;
			temp = AscToHex(inBuf[i]) << 4;
		}
		else
		{//odd
			temp += AscToHex(inBuf[i]);
			outBuf[index++] = temp;
		}
	}//ignore tail
	return index;
}

char SockCore::AscToHex(char asc)
{
	if (asc >= '0' && asc <= '9')
		return asc - '0';
	if (asc >= 'a' && asc <= 'f')
		return asc - 'a' + 10;
	if (asc >= 'A' && asc <= 'F')
		return asc - 'A' + 10;

	return 0;
}

BOOL SockCore::isAvailable()
{
	if (getSocket() == INVALID_SOCKET)
		return FALSE;
	else
		return TRUE;
}

void SockCore::setErrorCode(DWORD errorCode)
{
	g_errorCode = errorCode;
}

BOOL SockCore::isWritable(SOCKET m_sock)
{
	BOOL m_ErrorCode = FALSE;

	int rtn = 0;
	SOCKET sock = m_sock;
	//select part
	fd_set rFds, wFds, eFds;
	FD_ZERO(&rFds);
	FD_ZERO(&wFds);
	FD_ZERO(&eFds);

	FD_SET(sock, &wFds);
	timeval mtime = {0, 0};

	rtn = select(NULL, &rFds, &wFds, &eFds, &mtime);
	if (rtn == SOCKET_ERROR)
		return m_ErrorCode;

	rtn = FD_ISSET(sock, &wFds);
	FD_CLR(sock, &wFds);
	if (rtn)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
//
//BOOL SockCore::connectSock(WCHAR * inIpAddr, WCHAR * inPort)
//{
//	BOOL m_ErrorCode = FALSE;
//
//	CString ipAddr = inIpAddr;
//	CString port = inPort;
//	CString ipTpl = _T("0123456789");
//
//	//if ( (ipAddr.Trim(ipTpl).GetLength() != 3) || (!port.Trim(ipTpl).IsEmpty()) )
//	//	return m_ErrorCode;
//
//	ipAddr = inIpAddr;
//	port = inPort;
//	CHAR ipBuf[50], portBuf[50];
//	memset(ipBuf, 0, 50);
//	memset(portBuf, 0, 50);
//
//	DWORD len = _wcstombsz(0, inIpAddr, 0);
//	if (len < 7)
//		return m_ErrorCode;
//	_wcstombsz(ipBuf, inIpAddr, len);
//
//	len = _wcstombsz(0, port, 0);
//	_wcstombsz(portBuf, port, len);
//
//	m_sock = connectSockBase(inet_addr(ipBuf), htons(atoi(portBuf)));
//	if (m_sock == INVALID_SOCKET)
//		return m_ErrorCode;
//	return TRUE;
//}
//
//SOCKET SockCore::connectSockBase(DWORD ipAddr, WORD port)
//{
//	SOCKET m_ErrorCode = INVALID_SOCKET;
//
//	int rtn = 0;
//	SOCKET sock = INVALID_SOCKET;
//	WSADATA wsaData;
//	memset(&wsaData, 0, sizeof(wsaData));
//
//	//initial
//	rtn = WSAStartup(MAKEWORD(2,2), &wsaData);
//	if (rtn != 0)
//		return m_ErrorCode;
//
//	//create socket
//	sock = socket(AF_INET, SOCK_STREAM, 0);
//	if (sock == INVALID_SOCKET)
//		return m_ErrorCode;
//
//	SOCKADDR_IN inAddr;
//	inAddr.sin_family = AF_INET;
//	inAddr.sin_addr.s_addr = ipAddr;//inet_addr("172.20.138.10");
//	inAddr.sin_port = port;//htons(9100);
//
//	rtn = connect(sock, (SOCKADDR *)&inAddr, sizeof(inAddr));
//	if (rtn == SOCKET_ERROR)
//		return m_ErrorCode;
//
//	return sock;
//}
//DWORD wstr2cstr(WCHAR * inBuf, CHAR * outBuf)
//{
//	DWORD len = wcstombs(NULL, inBuf, 0);
//	wcstombs(outBuf, inBuf, len);
//	return len;
//}
//
//DWORD SockCore::sendBase(CHAR * sBuf, DWORD sLen)
//{
//	DWORD m_ErrorCode = 0;
//	int rtn = 0;
//
//	SOCKET sock = getSocket();
//	if (sock == INVALID_SOCKET)
//		return m_ErrorCode;
//
//	rtn = send(sock, sBuf, sLen, 0);
//	if (rtn == SOCKET_ERROR)
//		return m_ErrorCode;
//
//	return rtn;
//}
//
//DWORD SockCore::recvBase(CHAR * rBuf, DWORD rLen)
//{
//	DWORD m_ErrorCode = 0;
//	int rtn = 0;
//
//	SOCKET sock = getSocket();
//	if (sock == INVALID_SOCKET)
//		return m_ErrorCode;
//
//	rtn = recv(sock, rBuf, rLen, 0);
//	if (rtn == SOCKET_ERROR)
//		return m_ErrorCode;
//
//	return rtn;
//}
//BSTR SockCore::getRecvStr()
//{
//	BSTR m_ErrorCode = NULL;
//
//	if (!isAvailable())
//		return m_ErrorCode; //must connect first
//
//	CHAR *strChar = new CHAR[4096];
//	memset(strChar, 0, sizeof(CHAR) * 4096);
//
//	TCHAR *strTchar = new TCHAR[4096];
//	memset(strTchar, 0, sizeof(CHAR) * 4096);
//
//	DWORD retLen = recvBase(strChar, 4096);
//	if (retLen == 0)
//		return m_ErrorCode;
//
//	//to void mistakes
//	for (DWORD i=0; i<retLen; i++)
//	{
//		if ( strChar[i] == 0)
//			strChar[i] = 0x20;
//	}
//
//	DWORD len = _mbstowcsz(0, strChar, 0);
//	_mbstowcsz(strTchar, strChar, len);
//
//	CString strResult = strTchar;
//	return strResult.AllocSysString();
//}