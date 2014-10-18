
// UsbBasicDemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "UsbBasicDemo.h"
#include "UsbBasicDemoDlg.h"
#include "afxdialogex.h"

#include "UsbCore.h"
#include "SetupAPI.h"
#include <initguid.h>


#include "devioctl.h"
#include "usbprint.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CUsbBasicDemoDlg 对话框


UsbCore usbcore;

CUsbBasicDemoDlg::CUsbBasicDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CUsbBasicDemoDlg::IDD, pParent)
	, m_output(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUsbBasicDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_input);
	DDX_Text(pDX, IDC_EDIT2, m_output);
	DDX_Control(pDX, IDC_CHECK1, m_cmdflag);
	DDX_Control(pDX, IDC_BUTTON3, mm_output);
}

BEGIN_MESSAGE_MAP(CUsbBasicDemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CUsbBasicDemoDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CUsbBasicDemoDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CUsbBasicDemoDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CUsbBasicDemoDlg::OnBnClickedButton4)
END_MESSAGE_MAP()


// CUsbBasicDemoDlg 消息处理程序

BOOL CUsbBasicDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CUsbBasicDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CUsbBasicDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CUsbBasicDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




BYTE CUsbBasicDemoDlg::AscToHexNum(CHAR cchar)
{
	BYTE num = 0;

	if (cchar >= '0' && cchar <= '9')
		return cchar - '0';

	if (cchar >= 'a' && cchar <= 'f')
		return cchar -'a'+10;

	if (cchar >='A' && cchar <= 'F')
		return cchar - 'A'+10;

	return 0;
}

void CUsbBasicDemoDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CString test;
	DWORD count;

	usbcore.setUsbPort(_T("usb001"));

	count = usbcore.getDevMaxCount();
	count = usbcore.getPortNumber();
	count = usbcore.IsPrinterAvailable();

	test = usbcore.getPortDescription();
	if (!test.IsEmpty())
		MessageBox(test);

	test = usbcore.getDevicePath();
	if (!test.IsEmpty())
		MessageBox(test);

	
}

//send
void CUsbBasicDemoDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码

	CString input ;
	m_input.GetWindowTextW(input);

	DWORD rtn = 0;
	if (m_cmdflag.GetCheck())
	{
		rtn = usbcore.sendData(input.GetBuffer(), FALSE);
	}
	else
	{
		rtn = usbcore.sendData(input.GetBuffer(), TRUE);
	}


	CString str;
	str.Format(_T("send data: %d bytes\r\n"), rtn);

	UpdateData(TRUE);
	m_output += str;
	UpdateData(FALSE);
	GetDlgItem(IDC_EDIT2)->SendMessage(EM_SCROLL, SB_BOTTOM, 0);//滚动到顶部

}


//recv
void CUsbBasicDemoDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码

	//FOR TEST
	//CHAR cmd_send[50] = {0x1d, 0x53, 0x4c, 0x44, 0};
	//usbcore.send(cmd_send, 4);
	//END TEST

	CString rString = usbcore.getRecvStr();


	UpdateData(TRUE);
	m_output += rString;
	m_output += _T("#@recv done.\r\n");
	UpdateData(FALSE);
	GetDlgItem(IDC_EDIT2)->SendMessage(EM_SCROLL, SB_BOTTOM, 0);//滚动到顶部
}


//ioctrl
void CUsbBasicDemoDlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	CString input ;
	m_input.GetWindowTextW(input);
	
	DWORD IoCtlCode;

	if (0 == input.Find(_T("status")))
	{
		IoCtlCode = IOCTL_USBPRINT_GET_LPT_STATUS;
	}
	else if (0 == input.Find(_T("id")))
	{
		IoCtlCode = IOCTL_USBPRINT_GET_1284_ID;
	}
	else if (0 ==input.Find(_T("reset")))
	{
		IoCtlCode = IOCTL_USBPRINT_SOFT_RESET;
	}
	else
	{
		IoCtlCode = IOCTL_USBPRINT_GET_1284_ID;
	}

	CString tempStr = usbcore.getReqStr(IoCtlCode);
	CString str;
	str.Format(_T("#@req done.\r\n"));
	str = tempStr + str;

	UpdateData(TRUE);
	m_output += str;
	UpdateData(FALSE);
	GetDlgItem(IDC_EDIT2)->SendMessage(EM_SCROLL, SB_BOTTOM, 0);//滚动到顶部
}
