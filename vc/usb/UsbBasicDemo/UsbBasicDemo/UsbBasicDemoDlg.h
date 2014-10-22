
// UsbBasicDemoDlg.h : 头文件
//

#pragma once
#include "afxwin.h"


// CUsbBasicDemoDlg 对话框
class CUsbBasicDemoDlg : public CDialogEx
{
// 构造
public:
	CUsbBasicDemoDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_USBBASICDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
