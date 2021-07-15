
// OEMCheckerDlg.h : ͷ�ļ�
//
#pragma once
#include <list>
#include <string> 
#include <algorithm>

#include "afxcmn.h"
using namespace std;
typedef struct
{
	wstring sItemName;
	wstring sRefValue;
	wstring sActValue;
	wstring sDidValue;
}DevList;

// COEMCheckerDlg �Ի���
class COEMCheckerDlg : public CDialogEx
{
// ����
public:
	COEMCheckerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OEMCHECKER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
	CBitmap m_bmpPass, m_bmpFail;
	BOOL m_bExist;
	CListCtrl* m_hListView;
	list<DevList> m_devList;
	wchar_t m_szFilePath[MAX_PATH];
	BOOL GetMicLevel(int level);
	void RebuildSettings();
	void ReadSettings();
	void WriteSettings();
	void CheckSettings();
	static UINT CheckThread(LPVOID lpv);
	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	CProgressCtrl m_ProgCtrl;
};
