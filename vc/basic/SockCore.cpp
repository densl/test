
#pragma once
#include "stdafx.h"
#include "SockCore.h"


SockCore::SockCore()
{
	g_errorCode = 0;
	setSocket(INVALID_SOCKET);
}



// connect socket =============================================================
BOOL SockCore::connectSocket(char * ipAddr, WORD port)
{
	BOOL m_ErrorCode = FALSE;

	int rtn = 0;
	SOCKET sock = getSocket();
	WSADATA wsaData;
	memset(&wsaData, 0, sizeof(wsaData));

	// make sure close before create *****
	closeSocket();

	// initial *****
	rtn = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (rtn != 0)
	{
		char err[512] = {0};
		sprintf_s(err, "<connectSocket> WSAStartup error (%d).", rtn);
		setError(err);
		return m_ErrorCode;
	}

	// create socket **********************************************
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		char err[512] = {0};
		sprintf_s(err, "<connectSocket> Create socket error (%d).", WSAGetLastError());
		setError(err);

		return m_ErrorCode;
	}

	// set socket nonblocking **************************************
	u_long iMode = 1;
	rtn = ioctlsocket(sock, FIONBIO, &iMode);
	if (rtn != 0)
	{
		char err[512] = {0};
		sprintf_s(err, "<connectSocket> Set socket nonblocking error (%d).", WSAGetLastError());
		setError(err);

		return m_ErrorCode;
	}

	// set ip address and port **************************************
	SOCKADDR_IN inAddr;
	inAddr.sin_family = AF_INET;
	inAddr.sin_addr.s_addr = inet_addr(ipAddr);
	inAddr.sin_port = htons(port);
	if (inAddr.sin_addr.s_addr == INADDR_NONE || inAddr.sin_addr.s_addr == INADDR_ANY || inAddr.sin_addr.s_addr == 0) //invalid ipv4 address
	{
		setError("<connectSocket> Invalid Ip address.");
		return m_ErrorCode;
	}

	// nonblocking connect **************************************
	rtn = connect(sock, (SOCKADDR *)&inAddr, sizeof(inAddr));
	
	// to valid writable **************************************
	if ( isWritable(sock) )
	{	
		setSocket(sock);
		return TRUE;
	}
	else
	{
		setSocket(INVALID_SOCKET);
		setError("<connectSocket> connect failed.");
		return m_ErrorCode;
	}

	return TRUE;
}

// close socket and cleanup env =============================================================
void SockCore::closeSocket()
{
	int rtn = 0;
	SOCKET sock = getSocket();
	if (sock != INVALID_SOCKET)
	{
		rtn= closesocket(sock);
		setSocket(INVALID_SOCKET);
	}

	rtn = WSACleanup();
	if (rtn == SOCKET_ERROR)
	{
		char err[512] = {0};
		sprintf_s(err, "<closeSocket> WSACleanup error (%d).", WSAGetLastError());
		setError(err);

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

	// protected programming
	if (sLen == 0)
	{
		sLen = strlen(sBuf);
	}

	if (sBuf == NULL)
		return m_ErrorCode;
	


	// check socket **************
	SOCKET sock = getSocket();
	if (sock == INVALID_SOCKET)
	{
		setError("<sendSocket> INVALID socket.");
		return m_ErrorCode;
	}

	// send data to socket *********************
	rtn = send(sock, sBuf, sLen, 0);
	if (rtn == SOCKET_ERROR)
	{
		char err[512] = {0};
		sprintf_s(err, "<sendSocket> Socket send error (%d).", WSAGetLastError());
		setError(err);

		return m_ErrorCode;
	}

	return rtn;
}

DWORD SockCore::recvSocket(char * rBuf, DWORD rLen)
{
	DWORD m_ErrorCode = 0;
	int rtn = 0;

	// protect ********************
	if (rLen == 0)
	{
		rLen = strlen(rBuf);
	}
	if (rBuf == NULL)
		return m_ErrorCode;

	// get socket instance **********************
	SOCKET sock = getSocket();
	if (sock == INVALID_SOCKET)
	{
		setError("<recvSocket> INVALID socket.");
		return m_ErrorCode;
	}

	// revc data from socket *********************
	rtn = recv(sock, rBuf, rLen, 0);
	if (rtn == SOCKET_ERROR)
	{
		char err[512] = {0};
		sprintf_s(err, "<recvSocket> Socket recv error (%d).", WSAGetLastError());
		setError(err);

		return m_ErrorCode;
	}

	return rtn;
}

DWORD SockCore::sendData(char *inData, BOOL dataflag)
{
	DWORD m_ErrorCode = 0;

	// protected *************************
	if (inData == NULL)
		return m_ErrorCode;

	if (!isAvailable())
		return m_ErrorCode; //must connect first
	

	// data part *******************************
	if (dataflag)
	{
		return sendSocket(inData, 0);
	}

	// cmd part *********************************
	DWORD len = strlen(inData);
	CStringA test = inData;
	CStringA tpl = "0123456789abcdefABCDEF";
	if ( !test.Trim(tpl).IsEmpty() )
		return m_ErrorCode;

	CHAR * cmd_send = new CHAR[len];
	memset(cmd_send, 0, len);

	len = strToHexArray(inData, cmd_send);
	return sendSocket(cmd_send, len);
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

static LPTSTR logFile = _T("d:\\log2014-11-3235923dsfsdkfhs.txt");
void SockCore::setError(char * err)
{
	return;
#ifdef VERSION_RELEASE
	return;
#endif

	char msg[4096] = {0};

	SYSTEMTIME mtime;
	GetSystemTime(&mtime);

	sprintf_s(msg, "%04d%02d%02d-%02d.%02d.%02d:  %s\n", mtime.wYear, mtime.wMonth, mtime.wDay, mtime.wHour,mtime.wMinute, mtime.wSecond, err);

	CFile cf;
	if( !cf.Open(logFile, CFile::modeWrite) )
		return;

	//cf.Open(logFile, CFile::modeCreate|CFile::modeWrite|CFile::modeNoTruncate);
	cf.SeekToEnd();

	cf.Write(msg, strlen(msg));
	cf.Close();
}


// check if socket is writable ***********************************************
BOOL SockCore::isWritable(SOCKET m_sock)
{
	BOOL m_ErrorCode = FALSE;
	int rtn = 0;

	SOCKET sock = m_sock;
	fd_set rFds, wFds, eFds;
	timeval mtime;

	int count = 0;
	while(++count)
	{
		timerclear(&mtime);
		FD_ZERO(&rFds);
		FD_ZERO(&wFds);
		FD_ZERO(&eFds);

		FD_SET(sock, &wFds);
		FD_SET(sock, &eFds);
		FD_SET(sock, &rFds);

		// select **********************************************************
		rtn = select(NULL, &rFds, &wFds, &eFds, &mtime);
		if (rtn == SOCKET_ERROR)
		{
			char err[512] = {0};
			sprintf_s(err, "Select function error (%d).", WSAGetLastError());
			setError(err);

			return m_ErrorCode;
		}

		// check error **********************************************************
		rtn = FD_ISSET(sock, &eFds);
		if (rtn)
		{
			char err[512] = {0};
			sprintf_s(err, "Select fds error (%d).", WSAGetLastError());
			setError(err);

			return m_ErrorCode;
		}
		// check writable **********************************************************
		rtn = FD_ISSET(sock, &wFds);
		if (rtn)
		{
			break;
		}

		// time out **********************************************************
		Sleep(100);
		if (count > 22)//time out
		{
			setError("select time out 23 * 100ms");
			return m_ErrorCode;
		}
	}
	return TRUE;
}