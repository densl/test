
// UsbBasicDemoDlg.cpp : ʵ���ļ�
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


// CUsbBasicDemoDlg �Ի���


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


// CUsbBasicDemoDlg ��Ϣ�������

BOOL CUsbBasicDemoDlg::OnInitDialog()
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CUsbBasicDemoDlg::OnPaint()
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������

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
	GetDlgItem(IDC_EDIT2)->SendMessage(EM_SCROLL, SB_BOTTOM, 0);//����������

}


//recv
void CUsbBasicDemoDlg::OnBnClickedButton3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	//FOR TEST
	//CHAR cmd_send[50] = {0x1d, 0x53, 0x4c, 0x44, 0};
	//usbcore.send(cmd_send, 4);
	//END TEST

	CString rString = usbcore.getRecvStr();


	UpdateData(TRUE);
	m_output += rString;
	m_output += _T("#@recv done.\r\n");
	UpdateData(FALSE);
	GetDlgItem(IDC_EDIT2)->SendMessage(EM_SCROLL, SB_BOTTOM, 0);//����������
}


//ioctrl
void CUsbBasicDemoDlg::OnBnClickedButton4()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	GetDlgItem(IDC_EDIT2)->SendMessage(EM_SCROLL, SB_BOTTOM, 0);//����������
}
