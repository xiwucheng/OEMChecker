
// OEMChecker.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// COEMCheckerApp: 
// �йش����ʵ�֣������ OEMChecker.cpp
//

class COEMCheckerApp : public CWinApp
{
public:
	COEMCheckerApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern COEMCheckerApp theApp;