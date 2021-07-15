
// OEMCheckerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "OEMChecker.h"
#include "OEMCheckerDlg.h"
#include "afxdialogex.h"
#include "wmibase.h"
#include "CoreAudio.h"
#include "dmi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// COEMCheckerDlg 对话框

COEMCheckerDlg::COEMCheckerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_OEMCHECKER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bmpPass.LoadBitmap(IDB_PASS);
	m_bmpFail.LoadBitmap(IDB_FAIL);
	GetModuleFileName(NULL, m_szFilePath, MAX_PATH);
	(wcsrchr(m_szFilePath, TEXT('\\')))[0] = 0;
	wcscat_s(m_szFilePath, TEXT("\\setting.ini"));

}

void COEMCheckerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_ProgCtrl);
}

BEGIN_MESSAGE_MAP(COEMCheckerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &COEMCheckerDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// COEMCheckerDlg 消息处理程序
BOOL COEMCheckerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	m_hListView = (CListCtrl*)GetDlgItem(IDC_LIST1);
	m_hListView->SetExtendedStyle(m_hListView->GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_SIMPLESELECT | LVS_EX_FULLROWSELECT);
	m_hListView->InsertColumn(0, TEXT("检查条目"), LVCFMT_RIGHT, 160);
	m_hListView->InsertColumn(1, TEXT("设备ID"), LVCFMT_LEFT, 160);
	m_hListView->InsertColumn(2, TEXT("参考值"), LVCFMT_RIGHT, 180);
	m_hListView->InsertColumn(3, TEXT("实际值"), LVCFMT_RIGHT, 180);
	m_hListView->InsertColumn(4, TEXT("对比结果"), LVCFMT_RIGHT, 60);
	// TODO: 在此添加额外的初始化代码
	m_ProgCtrl.SetRange(0, 100);
	CFileFind ffd;
	m_bExist = ffd.FindFile(m_szFilePath);
	if (m_bExist)
	{
		//SetWindowText(TEXT("OEMChecker"));
		SetDlgItemText(IDC_BUTTON1, TEXT("开始检查"));
		GetDlgItem(IDC_TIP1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TIP2)->ShowWindow(SW_SHOW);
		ReadSettings();
		ffd.Close();
	}
	else
	{
		//SetWindowText(TEXT("OEMChecker-Configuration"));
		SetDlgItemText(IDC_BUTTON1, TEXT("生成配置"));
		GetDlgItem(IDC_TIP1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_TIP2)->ShowWindow(SW_HIDE);
	}

	CString ver;
	CDmi dmi;
	//dmi.Init();
	dmi.GetVersion(0, ver);
	ver = TEXT("OEMChecker - ") + ver;
	SetWindowText(ver);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void COEMCheckerDlg::OnPaint()
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
HCURSOR COEMCheckerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL COEMCheckerDlg::GetMicLevel(int level)
{
	return TRUE;
}


void COEMCheckerDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	//GetMicLevel(0);
	CCoreAudio ca;
	ca.Init();
	float level = 0.;
	level=ca.GetMicLevel();
	level = ca.GetSpeakerLevel();
	GetDlgItem(IDC_BUTTON1)->EnableWindow(0);
	::EnableMenuItem(::GetSystemMenu(m_hWnd, FALSE), SC_CLOSE, MF_BYCOMMAND | MF_DISABLED);
	CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)CheckThread, this, 0, 0));
}

UINT COEMCheckerDlg::CheckThread(LPVOID lpv)
{
	COEMCheckerDlg* p = (COEMCheckerDlg*)lpv;
	if (p->m_bExist)
	{
		p->ReadSettings();
		p->CheckSettings();
	}
	else
	{
		p->RebuildSettings();
		p->WriteSettings();
	}
	p->GetDlgItem(IDC_BUTTON1)->EnableWindow();
	::EnableMenuItem(::GetSystemMenu(p->m_hWnd, FALSE), SC_CLOSE, MF_BYCOMMAND | MF_ENABLED);
	return 0;
}

void COEMCheckerDlg::RebuildSettings()
{
	DevList dl;
	ULONGLONG dwCap = 0;
	CString szTmp, szMonitor, szDisk(L"");
	wstring tmp;
	float level, levelscalar;
	CDmi dmi;
	CWmiBase wb;
	m_ProgCtrl.SetPos(10);
#if 0
	dmi.Init();
	m_devList.clear();
	dl.sItemName = TEXT("Processor");
	dl.sRefValue = dmi.get_processor_version();
	m_devList.push_back(dl);

	dl.sItemName = TEXT("Bios_version");
	dl.sRefValue = dmi.get_bios_version();
	m_devList.push_back(dl);

	dl.sItemName = TEXT("Bios_release_date");
	dl.sRefValue = dmi.get_bios_release_date();
	m_devList.push_back(dl);

	dl.sItemName = TEXT("EC_version");
	szTmp.Format(TEXT("%02d.%02d"), dmi.get_ec_major(), dmi.get_ec_minor());
	dl.sRefValue = szTmp;
	m_devList.push_back(dl);

	dl.sItemName = TEXT("System_uuid");
	dl.sRefValue = dmi.get_uuid();
	m_devList.push_back(dl);

	dl.sItemName = TEXT("Serial_number");
	dl.sRefValue = dmi.get_system_serial_number();
	m_devList.push_back(dl);
#else
	wb.Init(L"SELECT Name FROM Win32_Processor", L"Name");//CPU名字
	if (!wb.m_lInfo.empty())
	{
		dl.sItemName = TEXT("Processor");
		dl.sRefValue = wb.m_lInfo.front().szA;
		m_devList.push_back(dl);
	}

	wb.Init(L"SELECT * FROM Win32_BIOS", L"Name",L"ReleaseDate",L"SerialNumber");//BIOS信息
	if (!wb.m_lInfo.empty())
	{
		dl.sItemName = TEXT("Bios_version");
		dl.sRefValue = wb.m_lInfo.front().szA;
		m_devList.push_back(dl);

		dl.sItemName = TEXT("Bios_release_date");
		szTmp = wb.m_lInfo.front().szB.c_str();
		dl.sRefValue = szTmp.Left(4) + L"/" + szTmp.Mid(4,2) + L"/" + szTmp.Mid(6,2);
		m_devList.push_back(dl);

		dl.sItemName = TEXT("Serial_number");
		dl.sRefValue = wb.m_lInfo.front().szC;
		m_devList.push_back(dl);
	}

	wb.Init(L"SELECT * FROM Win32_BIOS", L"EmbeddedControllerMajorVersion",L"EmbeddedControllerMinorVersion");//EC信息
	if (!wb.m_lInfo.empty())
	{
		dl.sItemName = TEXT("EC_version");
		szTmp.Format(L"%s.%s", wb.m_lInfo.front().szA.c_str(), wb.m_lInfo.front().szB.c_str());
		dl.sRefValue = szTmp;
		m_devList.push_back(dl);
	}

	wb.Init(L"SELECT * FROM Win32_ComputerSystemProduct", L"UUID");//UUID信息
	if (!wb.m_lInfo.empty())
	{
		dl.sItemName = TEXT("System_uuid");
		dl.sRefValue = wb.m_lInfo.front().szA;
		m_devList.push_back(dl);
	}

#endif
	dl.sItemName = TEXT("Operating_system");
	dmi.GetOSVersion(tmp);
	dl.sRefValue = tmp + TEXT("[");
	dmi.GetOSBuildNumber(tmp);
	dl.sRefValue += tmp + TEXT("]");
	m_devList.push_back(dl);

	m_ProgCtrl.SetPos(20);

	dwCap = 0;
	wb.Init(L"SELECT * FROM Win32_DiskDrive", L"Size", L"MediaType");//硬盘容量计算
	while (!wb.m_lInfo.empty())
	{
		if (wcsstr(wb.m_lInfo.front().szB.c_str(), L"Fixed"))
		{
			dwCap = wcstoull(wb.m_lInfo.front().szA.c_str(), 0, 10);
			dwCap /= 1000000000;
			if (szDisk.GetLength())
			{
				szTmp.Format(TEXT(" + %dGB"), dwCap);
			}
			else
			{
				szTmp.Format(TEXT("%dGB"), dwCap);
			}
			szDisk += szTmp;
		}
		wb.m_lInfo.pop_front();
	}
	dl.sItemName = TEXT("Disk_capacity");
	dl.sRefValue = szDisk;
	m_devList.push_back(dl);
	

	wb.Init(L"SELECT Capacity FROM Win32_PhysicalMemory", L"Capacity");//内存容量计算
	dwCap = 0;
	while (!wb.m_lInfo.empty())
	{
		dwCap += wcstoull(wb.m_lInfo.front().szA.c_str(), 0, 10);
		wb.m_lInfo.pop_front();
	}
	dwCap >>= 30;
	szTmp.Format(TEXT("%dGB"), dwCap);
	dl.sItemName = TEXT("Ram_capacity");
	dl.sRefValue = szTmp;
	m_devList.push_back(dl);

	//Microphone level
	CCoreAudio ca;
	ca.Init();
	levelscalar = ca.GetMicLevelScalar() * 100;
	level = ca.GetMicLevel();
	szTmp.Format(TEXT("%d[%.1f dB]"), (int)levelscalar, level);
	dl.sItemName = TEXT("Mic_level");
	dl.sRefValue = szTmp;
	m_devList.push_back(dl);
	ca.Uninit();
	m_ProgCtrl.SetPos(30);

	wb.Init(L"SELECT DeviceID FROM Win32_PnPSignedDriver WHERE DeviceID like 'DISPLAY%'", L"DeviceID");
	while (!wb.m_lInfo.empty())
	{
		BYTE buff[128] = { 0 };
		BYTE* ptr = buff + 0x36;
		wchar_t name[14] = { 0 };
		DWORD len = 128;
		szTmp = wb.m_lInfo.front().szA.c_str();
		szTmp = TEXT("SYSTEM\\CurrentControlSet\\Enum\\") + szTmp + TEXT("\\Device Parameters\\");
		CRegKey rk;
		if (ERROR_SUCCESS == rk.Open(HKEY_LOCAL_MACHINE, szTmp, KEY_ALL_ACCESS))
		{
			rk.QueryBinaryValue(TEXT("EDID"), buff, &len);
			rk.Close();
			szTmp = TEXT("");
			for (int i = 0; i < 4; i++, ptr += 18)
			{
				if (ptr[0] == 0 && ptr[1] == 0 && ptr[2] == 0)
				{
					if (ptr[3] == 0xfe || ptr[3] == 0xfc)
					{
						for (int j = 5; j < 18; j++)
						{
							name[j - 5] = ptr[j];
							if (ptr[j] == 0x0a)
							{
								name[j - 5] = 0x00;
								break;
							}
						}
						szMonitor += name;
						szMonitor += TEXT(" ");
					}
				}
			}
		}
		wb.m_lInfo.pop_front();
	}
	dl.sItemName = TEXT("LCD_Type");
	dl.sRefValue = szMonitor;
	m_devList.push_back(dl);
	m_ProgCtrl.SetPos(50);
	//driver info.
	wb.Init(L"SELECT * FROM Win32_PnPSignedDriver WHERE DriverProviderName != 'Microsoft'", L"DeviceName", L"DriverVersion", L"DeviceID");
	while (!wb.m_lInfo.empty())
	{
		if (wcslen(wb.m_lInfo.front().szA.c_str()) && wcslen(wb.m_lInfo.front().szB.c_str()) && wcslen(wb.m_lInfo.front().szC.c_str()))
		{
			dl.sItemName = wb.m_lInfo.front().szA.c_str();
			dl.sRefValue = wb.m_lInfo.front().szB.c_str();
			dl.sDidValue = wb.m_lInfo.front().szC.c_str();
			m_devList.push_back(dl);
		}
		wb.m_lInfo.pop_front();
	}
	m_ProgCtrl.SetPos(80);
}

void COEMCheckerDlg::ReadSettings()
{
	CStdioFile fp;
	CString strBuff;
	int nPos1,nPos2,idx=0;
	m_hListView->DeleteAllItems();
	if (fp.Open(m_szFilePath, CFile::modeRead | CFile::typeUnicode))
	{
		m_ProgCtrl.SetPos(0);
		fp.Seek(2, SEEK_SET);
		while (fp.ReadString(strBuff))
		{
			nPos1 = strBuff.Find('|');
			nPos2 = strBuff.Find('=');
			m_hListView->InsertItem(idx, strBuff.Left(nPos1));
			m_hListView->SetItemText(idx, 1, strBuff.Mid(nPos1 + 1,nPos2 - nPos1-1));
			m_hListView->SetItemText(idx, 2, strBuff.Mid(nPos2 + 1));
			idx++;
		}
		fp.Close();
	}
}

void COEMCheckerDlg::WriteSettings()
{
	CStdioFile fp;
	CString strBuff;
	if (fp.Open(m_szFilePath, CFile::modeCreate | CFile::modeReadWrite | CFile::typeUnicode))
	{
		BYTE UH[] = { 0xff, 0xfe };
		fp.Write(UH, 2);
		for (list <DevList>::iterator it = m_devList.begin(); it != m_devList.end(); ++it)
		{
			DevList dl = *it;
			strBuff.Format(TEXT("%s|%s=%s\r\n"), dl.sItemName.c_str(), dl.sDidValue.c_str(), dl.sRefValue.c_str());
			fp.WriteString(strBuff);
		}
		fp.Close();
		m_bExist = TRUE;
		MessageBox(TEXT("程序所在目录的配置文件已生成，可以进行检查了！"), TEXT("OEMChecker"), MB_ICONINFORMATION);
		SetDlgItemText(IDC_BUTTON1, TEXT("开始检查"));
		GetDlgItem(IDC_TIP1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TIP2)->ShowWindow(SW_SHOW);
		ReadSettings();
	}
	m_ProgCtrl.SetPos(100);
}

void COEMCheckerDlg::CheckSettings()
{
	if (m_hListView->GetItemCount() == 0)
	{
		m_ProgCtrl.SetPos(100);
		MessageBox(TEXT("未找到参考配置或配置文件错误，请先删除配置文件生成配置！"), TEXT("错误"), MB_ICONERROR);
	}
	DWORD dwResult=0;
	RebuildSettings();
	int nCount=m_hListView->GetItemCount();
	CString szItem0, szItem1, szItem2, szItem3,szItem4;
	DevList dl;
	for (int i = 0; i < nCount; i++)
	{
		szItem0 = m_hListView->GetItemText(i, 0);//Name
		szItem1 = m_hListView->GetItemText(i, 1);//DeviceID
		szItem2 = m_hListView->GetItemText(i, 2);//RefValue
		szItem3 = m_hListView->GetItemText(i, 3);//ActValue
		dl.sItemName = szItem0;
		dl.sDidValue = szItem1;
		if (szItem1.GetLength())//先比较DeviceID
		{
			for (list<DevList>::iterator it = m_devList.begin(); it != m_devList.end(); it++)
			{
				if (wcscmp(dl.sDidValue.c_str(), it->sDidValue.c_str()) == 0)
				{
					szItem3 = it->sRefValue.c_str();
					break;
				}
			}
		}
		else if (szItem0.GetLength())//再比较DeviceName
		{
			for (list<DevList>::iterator it = m_devList.begin(); it != m_devList.end(); it++)
			{
				if (wcscmp(dl.sItemName.c_str(), it->sItemName.c_str()) == 0)
				{
					szItem3 = it->sRefValue.c_str();
					break;
				}
			}
		}
		m_hListView->SetItemText(i, 3, szItem3);
		if (szItem0.Compare(TEXT("System_uuid")) == 0)
		{
			if (szItem3.Find(TEXT("000")) == -1)
			{
				m_hListView->SetItemText(i, 4, TEXT("✔"));
			}
			else
			{
				m_hListView->SetItemText(i, 4, TEXT("✘"));
				dwResult++;
			}
		}
		else if (szItem0.Compare(TEXT("Serial_number")) == 0)
		{
			if (szItem3.Find(TEXT("filled")) == -1 && szItem3.Find(TEXT("string")) == -1)
			{
				m_hListView->SetItemText(i, 4, TEXT("✔"));
			}
			else
			{
				m_hListView->SetItemText(i, 4, TEXT("✘"));
				dwResult++;
			}
		}
		else if (szItem2.Compare(szItem3) == 0)
		{
			m_hListView->SetItemText(i, 4, TEXT("✔"));
		}
		else
		{
			m_hListView->SetItemText(i, 4, TEXT("✘"));
			dwResult++;
		}
	}
	m_ProgCtrl.SetPos(100);
	if (dwResult)
	{
		((CStatic*)GetDlgItem(IDC_STATUS))->SetBitmap((HBITMAP)m_bmpFail.m_hObject);
	}
	else
	{
		((CStatic*)GetDlgItem(IDC_STATUS))->SetBitmap((HBITMAP)m_bmpPass.m_hObject);
	}
}
