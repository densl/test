
// UsbDemoDlg.h : ͷ�ļ�
//

#pragma once


// CUsbDemoDlg �Ի���
class CUsbDemoDlg : public CDialogEx
{
// ����
public:
	CUsbDemoDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_USBDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString m_output;
	afx_msg void OnBnClickedButton1();
	//void WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv);
	afx_msg void OnBnClickedButton2();
};
