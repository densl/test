
// UsbBasicDemoDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// CUsbBasicDemoDlg �Ի���
class CUsbBasicDemoDlg : public CDialogEx
{
// ����
public:
	CUsbBasicDemoDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_USBBASICDEMO_DIALOG };

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
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	CEdit m_input;
	CString m_output;
	afx_msg void OnBnClickedButton3();
	BYTE AscToHexNum(CHAR cchar);
	CButton m_cmdflag;
	CButton mm_output;
	afx_msg void OnBnClickedButton4();
};
