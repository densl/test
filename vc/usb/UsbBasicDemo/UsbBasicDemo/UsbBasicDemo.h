
// UsbBasicDemo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CUsbBasicDemoApp:
// �йش����ʵ�֣������ UsbBasicDemo.cpp
//

class CUsbBasicDemoApp : public CWinApp
{
public:
	CUsbBasicDemoApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CUsbBasicDemoApp theApp;