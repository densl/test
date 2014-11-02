
// UsbDemoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "UsbDemo.h"
#include "UsbDemoDlg.h"
#include "afxdialogex.h"

#include "Winsvc.h"
#include "UsbCore.h"
#include "UtilCore.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


static UsbCore usbcore;
static UtilCore utilcore;
static HWND mhwnd;

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CUsbDemoDlg �Ի���




CUsbDemoDlg::CUsbDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CUsbDemoDlg::IDD, pParent)
	, m_output(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	//mhwnd = this->m_hWnd;
}

void CUsbDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_OUTPUT, m_output);
}

BEGIN_MESSAGE_MAP(CUsbDemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CUsbDemoDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CUsbDemoDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CUsbDemoDlg ��Ϣ�������

BOOL CUsbDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CUsbDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CUsbDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CUsbDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CUsbDemoDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	DWORD ret = utilcore.getDword(1,2,3,4);
	return;

	usbcore.uOpen("d:\\fackusb.txt");
	TCHAR * testFile = _T("d:\\fakeusb.txt");
	CFile cf;
	cf.Open(testFile, CFile::modeCreate | CFile::modeReadWrite);
	
	usbcore.setHandle( cf.m_hFile);

	usbcore.uWrite("hello", 0);

	cf.Close();
}


void CUsbDemoDlg::OnBnClickedButton2()
{

}



// service manager
SERVICE_STATUS_HANDLE gSvcStatusHandle;
SERVICE_STATUS gSvcStatus;
TCHAR szServiceName[] = _T("ServiceDemo6");

void WINAPI ServiceCtrl(DWORD dwCtrl)
{

	switch(dwCtrl)
	{
	case SERVICE_CONTROL_STOP:
		//ReportSvcStatus( SERVICE_STOP_PENDING, NO_ERROR, 0);

		gSvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		gSvcStatus.dwServiceSpecificExitCode = 0;
		gSvcStatus.dwCurrentState = SERVICE_STOP_PENDING;
		gSvcStatus.dwWin32ExitCode = 0;

		SetServiceStatus(gSvcStatusHandle, &gSvcStatus);
		return;
	default:
		break;
	}
}

void WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv)
{ 

	gSvcStatusHandle = RegisterServiceCtrlHandler(
		szServiceName,
		ServiceCtrl);

	memset(&gSvcStatus, 0, sizeof(gSvcStatus));

	gSvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	gSvcStatus.dwServiceSpecificExitCode = 0;
	gSvcStatus.dwCurrentState = SERVICE_START_PENDING;
	gSvcStatus.dwWin32ExitCode = 0;

	SetServiceStatus(gSvcStatusHandle, &gSvcStatus);

	//SERVICE_STATUS_HANDLE hServiceStatus = RegisterServiceCtrlHandler(szServiceName, ServiceCtrl);

	Sleep(100);
	gSvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	gSvcStatus.dwServiceSpecificExitCode = 0;
	gSvcStatus.dwCurrentState = SERVICE_RUNNING;
	gSvcStatus.dwWin32ExitCode = 0;


	SetServiceStatus(gSvcStatusHandle, &gSvcStatus);

	CFile fp;
	fp.Open(_T("d:\\service.txt"), CFile::modeCreate|CFile::modeReadWrite);
	char * buf = "hello 5";
	fp.Write(buf, strlen(buf));
	fp.Close();
	//while(TRUE){};


	while(TRUE){};
}


void mysocket()
{
	char * strIp = "127.0.0.1";
	int intPort = 1234;
	int mHandle = 0;

	int ret = 0;

	//setup env**********************
	WSADATA wsaData;
	ret = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (ret != NO_ERROR)
	{
//		MessageBox(_T("WSAStartup fail."));
		return;
	}


	//create socket**********************
	SOCKET ConnectSocket = INVALID_SOCKET;
	ConnectSocket = socket(AF_INET, SOCK_STREAM, NULL);
	if (ConnectSocket == INVALID_SOCKET)
	{
		WSACleanup();
	//	MessageBox(_T("Create socket fail."));
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
	//	MessageBox(_T("ioctlsocket socket nonblocking fail."));
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


void uninstallService()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString testname;
//	UpdateData(TRUE);
//	testname = m_output;

	//szServiceName = test.GetBuffer();

	int ret = 0;

	//open service manager*********************************
	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCM == NULL)
	{
		return;
	}

	// open service*********************************
	SC_HANDLE hService = OpenService(hSCM, testname, SERVICE_STOP | DELETE);
	if (hService == NULL)
	{
		return;
	}

	// stop service*********************************
	SERVICE_STATUS Statuss;
	ret = ControlService( hService, SERVICE_CONTROL_STOP, &Statuss);

	// delete service*********************************
	ret = DeleteService(hService);
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCM);
	return;
	// get the exicutable file path*********************************
	TCHAR szFilePath[MAX_PATH];
	memset(szFilePath, 0, sizeof(TCHAR)*MAX_PATH);
	GetModuleFileName(NULL, szFilePath, MAX_PATH);

	// get current thread id*********************************
	DWORD dwThreadID = GetCurrentThreadId();

	// create service*********************************

	hService = CreateService(hSCM,//SC_HANDLE
		szServiceName,		//lp service name
		szServiceName,		//lp display name
		SERVICE_ALL_ACCESS,	//dw desire access
		SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS,//dw service type
		SERVICE_DEMAND_START,	//dw start type
		SERVICE_ERROR_NORMAL, //dw error control
		szFilePath,			//lp binary path name
		NULL,				//lp load order group
		NULL,				//lp dw tag id
		_T(""),				//lp dependencies
		NULL,				//lp service start name
		NULL				//lp password
		);
	if (hService == NULL)
	{
		int err = GetLastError();
		if (err = ERROR_SERVICE_EXISTS)
		{
			hService = OpenService(hSCM, szServiceName, SERVICE_ALL_ACCESS);
		}
	}

	// start service ctrl dispatcher*********************************
	SERVICE_TABLE_ENTRY st[] = 
	{
		{ szServiceName, (LPSERVICE_MAIN_FUNCTION)ServiceMain},
		{NULL, NULL}
	};

	ret = StartServiceCtrlDispatcher(st);

	//ret = StartService(hService, 1, NULL);

	CString test;
	test.Format(_T("%d"), dwThreadID);
//	MessageBox(test);
}

void installService()
{
	int ret = 0;

	//open service manager*********************************
	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCM == NULL)
	{
		return;
	}

	// get the exicutable file path*********************************
	TCHAR szFilePath[MAX_PATH];
	memset(szFilePath, 0, sizeof(TCHAR)*MAX_PATH);
	GetModuleFileName(NULL, szFilePath, MAX_PATH);

	// get current thread id*********************************
	DWORD dwThreadID = GetCurrentThreadId();

	// create service*********************************

	SC_HANDLE hService = CreateService(hSCM,//SC_HANDLE
		szServiceName,		//lp service name
		szServiceName,		//lp display name
		SERVICE_ALL_ACCESS,	//dw desire access
		SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS,//dw service type
		SERVICE_DEMAND_START,	//dw start type
		SERVICE_ERROR_NORMAL, //dw error control
		szFilePath,			//lp binary path name
		NULL,				//lp load order group
		NULL,				//lp dw tag id
		_T(""),				//lp dependencies
		NULL,				//lp service start name
		NULL				//lp password
		);
	if (hService == NULL)
	{
		int err = GetLastError();
		if (err = ERROR_SERVICE_EXISTS)
		{
			hService = OpenService(hSCM, szServiceName, SERVICE_ALL_ACCESS);
		}
	}

	// start service ctrl dispatcher*********************************
	SERVICE_TABLE_ENTRY st[] = 
	{
		{ szServiceName, (LPSERVICE_MAIN_FUNCTION)ServiceMain},
		{NULL, NULL}
	};

	ret = StartServiceCtrlDispatcher(st);

	//ret = StartService(hService, 1, NULL);

	CString test;
	test.Format(_T("%d"), dwThreadID);
	//MessageBox(test);


}
