// win socket ************************************************
void OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	char * strIp = "127.0.0.1";
	int intPort = 1234;
	int mHandle = 0;

	int ret = 0;

	//setup env**********************
	WSADATA wsaData;
	ret = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (ret != NO_ERROR)
	{
		MessageBox(_T("WSAStartup fail."));
		return;
	}

	//create socket**********************
	SOCKET ConnectSocket = INVALID_SOCKET;
	ConnectSocket = socket(AF_INET, SOCK_STREAM, NULL);
	if (ConnectSocket == INVALID_SOCKET)
	{
		WSACleanup();
		MessageBox(_T("Create socket fail."));
		return;
	}

	//set socket address and family**********************
	SOCKADDR_IN inAddr;
	inAddr.sin_family = AF_INET;
	inAddr.sin_addr.s_addr = inet_addr(strIp);
	inAddr.sin_port = htons(intPort);

	//set nonblocking socket**********************
	u_long iMode = 1;
	ret = ioctlsocket(ConnectSocket, FIONBIO, &iMode);
	if (ret != NO_ERROR)
	{
		ret = WSAGetLastError();
		MessageBox(_T("ioctlsocket socket nonblocking fail."));
		return;
	}

	//connect socket **********************
	ret = connect(ConnectSocket, (SOCKADDR*)&inAddr, sizeof(inAddr));
	if (ret == SOCKET_ERROR)
	{
		ret = WSAGetLastError();
	}


	//select part**********************
	mHandle = ConnectSocket;
	struct timeval mtime;
	timerclear(&mtime);

	int count = 0;
	fd_set rFds, wFds, eFds;
	while(TRUE){
		FD_ZERO(&rFds);
		FD_ZERO(&wFds);
		FD_ZERO(&eFds);

		FD_SET(mHandle, &rFds);
		FD_SET(mHandle, &wFds);
		FD_SET(mHandle, &eFds);

		ret = select(0, &rFds, &wFds, &eFds, &mtime);
		if (ret > 0)
			break;
		count++;
	}
	ret = FD_ISSET(mHandle, &rFds);
	ret = FD_ISSET(mHandle, &wFds);
	ret = FD_ISSET(mHandle, &eFds);

	//send data**********************
	char *buf = "Hello from clien.";
	ret = send(ConnectSocket, buf, strlen(buf), 0);
	if (ret == SOCKET_ERROR)
	{
		ret = WSAGetLastError();
		return;
	}

	//recv data**********************
	char *rBuf = new CHAR[1024];
	memset(rBuf, 0, 1024);
	ret = recv(ConnectSocket, rBuf, 1024, 0);
	if (ret == SOCKET_ERROR)
	{
		ret = WSAGetLastError();
		return;
	}

	//close socket**********************
	closesocket(ConnectSocket);
	WSACleanup();
}

//UDP socket*******************************
void OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	char * strIp = "127.0.0.1";
	int intPort = 1234;
	int mHandle = 0;

	int ret = 0;

	//setup env**********************
	WSADATA wsaData;
	ret = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (ret != NO_ERROR)
	{
		MessageBox(_T("WSAStartup fail."));
		return;
	}

	//create socket**********************
	SOCKET SendSocket = INVALID_SOCKET;
	SendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (SendSocket == INVALID_SOCKET)
	{
		WSACleanup();
		MessageBox(_T("Create socket fail."));
		return;
	}

	//set socket address and family**********************
	SOCKADDR_IN inAddr;
	inAddr.sin_family = AF_INET;
	inAddr.sin_addr.s_addr = inet_addr(strIp);
	inAddr.sin_port = htons(intPort);

	//sendto socket**********************
	char * buf = "Sending a datagram to the receiver.";
	ret = sendto(SendSocket, buf, strlen(buf), 0, (SOCKADDR*)&inAddr, sizeof(inAddr));
	if (ret == SOCKET_ERROR)
	{
		return;
	}

	// recv from socket**********************
	char * rBuf = new CHAR[1024];
	memset(rBuf, 0, 1024);
	ret = recvfrom(SendSocket, rBuf, 1024, 0, (SOCKADDR*)&inAddr, 1024);
	if (ret == SOCKET_ERROR)
	{
		return;
	}


	//close socket**********************
	closesocket(SendSocket);
	WSACleanup();
}