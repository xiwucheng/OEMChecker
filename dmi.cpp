#include "stdafx.h"
#include "dmi.h"

const char *dmi_chassis_type(BYTE code)
{
	/* 7.4.1 */
	static const char *type[] = {
		"Other", /* 0x01 */
		"Unknown",
		"Desktop",
		"Low Profile Desktop",
		"Pizza Box",
		"Mini Tower",
		"Tower",
		"Portable",
		"Laptop",
		"Notebook",
		"Hand Held",
		"Docking Station",
		"All In One",
		"Sub Notebook",
		"Space-saving",
		"Lunch Box",
		"Main Server Chassis", /* CIM_Chassis.ChassisPackageType says "Main System Chassis" */
		"Expansion Chassis",
		"Sub Chassis",
		"Bus Expansion Chassis",
		"Peripheral Chassis",
		"RAID Chassis",
		"Rack Mount Chassis",
		"Sealed-case PC",
		"Multi-system",
		"CompactPCI",
		"AdvancedTCA",
		"Blade",
		"Blade Enclosing", /* 0x1D */
		"Tablet",
		"Convertible",
		"Detachable",
		"IoT Gateway",
		"Embedded PC",
		"Mini PC",
		"Stick PC"/* 0x24 */
	};

	code &= 0x7F; /* bits 6:0 are chassis type, 7th bit is the lock bit */

	if (code >= 0x01 && code <= 0x24)
		return type[code - 0x01];
	return "unknown";
}

CDmi::CDmi()
{
}

CDmi::~CDmi()
{

}

wstring CDmi::string2wstring(string str)
{
	wstring result;
	//获取缓冲区大小，并申请空间，缓冲区大小按字符计算  
	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
	TCHAR* buffer = new TCHAR[len + 1];
	//多字节编码转换成宽字节编码  
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
	buffer[len] = '\0';             //添加字符串结尾  
									//删除缓冲区并返回值  
	result.append(buffer);
	delete[] buffer;
	return result;
}

//将wstring转换成string  
string CDmi::wstring2string(wstring wstr)
{
	string result;
	//获取缓冲区大小，并申请空间，缓冲区大小事按字节计算的  
	int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
	char* buffer = new char[len + 1];
	//宽字节编码转换成多字节编码  
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);
	buffer[len] = '\0';
	//删除缓冲区并返回值  
	result.append(buffer);
	delete[] buffer;
	return result;
}

void CDmi::Init()
{
	DWORD iSignature = 'ACPI';
	DWORD iSigId = 'TDSX';//PCAF,TDSR,TDSX,PDSR,TDSD
	RawSMBIOSData *pData;
	BYTE buff[4096] = { 0 };
	int ret, iBuffersize;
	ret = GetSystemFirmwareTable(iSignature, iSigId, 0, 0);
	if (!ret)
	{
		return;
	}

	iBuffersize = ret;
	ret = GetSystemFirmwareTable(iSignature, iSigId, buff, iBuffersize);
	if (!ret)
	{
		return;
	}

	dmioemtag.oem_id.clear();
	dmioemtag.oem_table_id.clear();
	dmioemtag.oem_id.append((char*)buff + 10, 6);
	dmioemtag.oem_table_id.append((char*)buff + 16, 8);
	//------------------------------------------------------
	iSignature = 'RSMB';
	iSigId = 0;//'TDSX';//PCAF,TDSR,TDSX,PDSR,TDSD
	memset(buff, 0, 4096);
	ret = GetSystemFirmwareTable(iSignature, iSigId, 0, 0);
	if (!ret)
	{
		return;
	}
	iBuffersize = ret;
	ret = GetSystemFirmwareTable(iSignature, iSigId, buff, iBuffersize);
	if (!ret)
	{
		return;
	}
	pData = (RawSMBIOSData*)buff;
	DWORD dwLen = pData->Length;

	if (iBuffersize != dwLen + 8)
	{
		return;
	}

	dmi_header *dh = 0;
	BYTE* pRaw = buff + 8;
	while (pRaw < buff + dwLen)
	{
		dh = (dmi_header*)pRaw;
		if (dh->type == 0)
		{
			dmitype0.vendor = dmi_string(dh, pRaw[4]);
			dmitype0.bios_version = dmi_string(dh, pRaw[5]);
			dmitype0.bios_release_date = dmi_string(dh, pRaw[8]);
			dmitype0.bios_major = pRaw[0x14];
			dmitype0.bios_minor = pRaw[0x15];
			dmitype0.ec_major = pRaw[0x16];
			dmitype0.ec_minor = pRaw[0x17];
		}
		else if (dh->type == 1)
		{
			dmitype1.manufacturer = dmi_string(dh, pRaw[4]);
			dmitype1.product_name = dmi_string(dh, pRaw[5]);
			dmitype1.version = dmi_string(dh, pRaw[6]);
			dmitype1.serial_number = dmi_string(dh, pRaw[7]);
			dmi_system_uuid(pRaw+8, dmitype1.uuid);
			dmitype1.sku_number = dmi_string(dh, pRaw[0x19]);
			dmitype1.family = dmi_string(dh, pRaw[0x1a]);
		}
		else if (dh->type == 2)
		{
			dmitype2.manufacturer = dmi_string(dh, pRaw[4]);
			dmitype2.product_name = dmi_string(dh, pRaw[5]);
			dmitype2.version = dmi_string(dh, pRaw[6]);
			dmitype2.serial_number = dmi_string(dh, pRaw[7]);
			dmitype2.asset_tag = dmi_string(dh, pRaw[8]);
			dmitype2.location = dmi_string(dh, pRaw[0xa]);
		}
		else if (dh->type == 3)
		{
			dmitype3.manufacturer = dmi_string(dh, pRaw[4]);
			dmitype3.type = pRaw[5];
			dmitype3.form_factor = dmi_chassis_type(pRaw[5]);
			dmitype3.version = dmi_string(dh, pRaw[6]);
			dmitype3.serial_number = dmi_string(dh, pRaw[7]);
			dmitype3.asset_tag_number = dmi_string(dh, pRaw[8]);
			dmitype3.sku_number = dmi_string(dh, pRaw[0x15]);
		}
		else if (dh->type == 4)
		{
			dmitype4.socket_designation = dmi_string(dh, pRaw[4]);
			dmitype4.processor_manufacturer = dmi_string(dh, pRaw[7]);
			dmitype4.processor_version = dmi_string(dh, pRaw[0x10]);
			dmitype4.serial_number = dmi_string(dh, pRaw[0x20]);
			dmitype4.asset_tag = dmi_string(dh, pRaw[0x21]);
			dmitype4.part_number = dmi_string(dh, pRaw[0x22]);
		}
		else if (dh->type == 6)
		{
			dmitype6.socket_designation = dmi_string(dh, pRaw[4]);
			char ws[16] = { 0 };
			ULONGLONG msize = 2 << pRaw[9];
			msize >>= 20;
			sprintf_s(ws, "%lldMB", msize);
			dmitype6.installed_memory = ws;
		}
		else if (dh->type == 0x7f)
		{
			break;//End of Table
		}
		pRaw += dh->length;
		while (*(WORD*)pRaw != 0) pRaw++;
		pRaw += 2;
	}
}

string CDmi::dmi_string(dmi_header* dm, BYTE index)
{
	char* bp = (char*)dm;
	size_t i, len;
	if (index == 0)
	{
		return "Not Specified";
	}

	bp += dm->length;
	while (*bp && index > 1)
	{
		bp += strlen(bp);
		bp++;
		index--;
	}

	if (!*bp)
	{
		return "BAD_INDEX";
	}

	/* ASCII filtering */
	len = strlen(bp);
	for (i = 0; i < len; i++)
	{
		if (bp[i] < 32 || bp[i] == 127)
		{
			bp[i] = '.';
		}
	}
	return bp;
}

void CDmi::dmi_system_uuid(const BYTE *p, std::string &uuid)
{
	int only0xFF = 1, only0x00 = 1;
	int i;

	for (i = 0; i < 16 && (only0x00 || only0xFF); i++)
	{
		if (p[i] != 0x00) only0x00 = 0;
		if (p[i] != 0xFF) only0xFF = 0;
	}

	if (only0xFF)
	{
		//printf("Not Present");
		return;
	}
	if (only0x00)
	{
		//printf("Not Settable");
		return;
	}

	char ch[256]; memset(ch, 0, sizeof(ch));
		sprintf_s(ch, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
			p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7],
			p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]);
		//sprintf_s(ch, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
		//	p[3], p[2], p[1], p[0], p[5], p[4], p[7], p[6],
		//	p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]);
	uuid = string(ch);
}

wstring CDmi::get_oem_table_id()
{
	return string2wstring(dmioemtag.oem_table_id);
}

wstring CDmi::get_oem_id()
{
	return string2wstring(dmioemtag.oem_id);
}

wstring CDmi::get_bios_version()
{
	return string2wstring(dmitype0.bios_version);
}

wstring CDmi::get_bios_release_date()
{
	return string2wstring(dmitype0.bios_release_date);
}

BYTE CDmi::get_bios_major()
{
	return dmitype0.bios_major;
}

BYTE CDmi::get_bios_minor()
{
	return dmitype0.bios_minor;
}

BYTE CDmi::get_ec_major()
{
	return dmitype0.ec_major;
}

BYTE CDmi::get_ec_minor()
{
	return dmitype0.ec_minor;
}

wstring CDmi::get_vendor()
{
	return string2wstring(dmitype0.vendor);
}

wstring CDmi::get_system_manufacturer()
{
	return string2wstring(dmitype1.manufacturer);
}

wstring CDmi::get_system_product_name()
{
	return string2wstring(dmitype1.product_name);
}

wstring CDmi::get_system_version()
{
	return string2wstring(dmitype1.version);
}

wstring CDmi::get_system_serial_number()
{
	return string2wstring(dmitype1.serial_number);
}

wstring CDmi::get_uuid()
{
	return string2wstring(dmitype1.uuid);
}

wstring CDmi::get_sku_number()
{
	return string2wstring(dmitype1.sku_number);
}

wstring CDmi::get_family()
{
	return string2wstring(dmitype1.family);
}

wstring CDmi::get_baseboard_manufacturer()
{
	return string2wstring(dmitype2.manufacturer);
}

wstring CDmi::get_baseboard_product_name()
{
	return string2wstring(dmitype2.product_name);
}

wstring CDmi::get_baseboard_serial_number()
{
	return string2wstring(dmitype2.serial_number);
}

wstring CDmi::get_baseboard_version()
{
	return string2wstring(dmitype2.version);
}
wstring CDmi::get_processor_version()
{
	return string2wstring(dmitype4.processor_version);
}

wstring CDmi::get_form_factor()
{
	return string2wstring(dmitype3.form_factor);
}

BYTE CDmi::get_form_factor_type()
{
	return dmitype3.type;
}
wstring CDmi::get_installed_memory()
{
	return string2wstring(dmitype6.installed_memory);
}

BOOL CDmi::GetVersion(LPCTSTR szPEFile, CString &ver)
{
	DWORD dwInfoSize = 0;
	TCHAR exePath[MAX_PATH];
	memset(exePath, 0, sizeof(exePath));

	ver.Format(_T("V1.00"));
	if (szPEFile == NULL)
	{
		// 得到程序的自身路径  
		GetModuleFileName(NULL, exePath, sizeof(exePath) / sizeof(TCHAR));
	}
	else
	{
		wcscpy_s(exePath, szPEFile);
	}

	if (_waccess(exePath, 0) == -1)
	{
		return FALSE;
	}
	// 判断是否能获取版本号  
	dwInfoSize = GetFileVersionInfoSize(exePath, NULL);

	if (dwInfoSize == 0)
	{
		::OutputDebugString(L"GetFileVersionInfoSize fail\r\n");
		return FALSE;
	}
	else
	{
		BYTE* pData = new BYTE[dwInfoSize];

		// 获取版本信息  
		if (!GetFileVersionInfo(exePath, NULL, dwInfoSize, pData))
		{
			::OutputDebugString(L"GetFileVersionInfo fail\r\n");
		}
		else
		{
			// 查询版本信息中的具体键值  
			LPVOID lpBuffer;
			UINT uLength;
			if (!::VerQueryValue((LPCVOID)pData, _T("\\"), &lpBuffer, &uLength))
			{
				::OutputDebugString(L"GetFileVersionInfo fail\r\n");
			}
			else
			{
				DWORD dwVerMS;
				DWORD dwVerLS;
				dwVerMS = ((VS_FIXEDFILEINFO*)lpBuffer)->dwProductVersionMS;
				dwVerLS = ((VS_FIXEDFILEINFO*)lpBuffer)->dwProductVersionLS;
				ver.Format(_T("V%d.%d.%d.%d"), (dwVerMS >> 16), (dwVerMS & 0xFFFF), (dwVerLS >> 16), (dwVerLS & 0xFFFF));
			}
		}

		delete pData;
	}
	return TRUE;
}

BOOL CDmi::GetFileExist(LPCTSTR lpFile)
{
	if (_waccess(lpFile, 0) == -1)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CDmi::GetVersion(LPCTSTR szPEFile, LPDWORD pVer)
{
	DWORD dwInfoSize = 0;
	TCHAR exePath[MAX_PATH];
	memset(exePath, 0, sizeof(exePath));

	if (szPEFile == NULL)
	{
		// 得到程序的自身路径  
		GetModuleFileName(NULL, exePath, sizeof(exePath) / sizeof(TCHAR));
	}
	else
	{
		wcscpy_s(exePath, szPEFile);
	}

	if (_waccess(exePath, 0) == -1)
	{
		return FALSE;
	}
	if (pVer == NULL)
	{
		return FALSE;
	}
	// 判断是否能获取版本号  
	dwInfoSize = GetFileVersionInfoSize(exePath, NULL);

	if (dwInfoSize == 0)
	{
		::OutputDebugString(L"GetFileVersionInfoSize fail\r\n");
		return FALSE;
	}
	else
	{
		BYTE* pData = new BYTE[dwInfoSize];

		// 获取版本信息  
		if (!GetFileVersionInfo(exePath, NULL, dwInfoSize, pData))
		{
			::OutputDebugString(L"GetFileVersionInfo fail\r\n");
		}
		else
		{
			// 查询版本信息中的具体键值  
			LPVOID lpBuffer;
			UINT uLength;
			if (!::VerQueryValue((LPCVOID)pData, _T("\\"), &lpBuffer, &uLength))
			{
				::OutputDebugString(L"GetFileVersionInfo fail\r\n");
			}
			else
			{
				DWORD dwVerMS;
				DWORD dwVerLS;
				dwVerMS = ((VS_FIXEDFILEINFO*)lpBuffer)->dwProductVersionMS;
				dwVerLS = ((VS_FIXEDFILEINFO*)lpBuffer)->dwProductVersionLS;
				*pVer = (dwVerLS >> 16);
			}
		}

		delete pData;
	}
	return TRUE;
}

BOOL CDmi::GetOSVersion(wstring &wsVer)
{
	BOOL retval, result = FALSE;
	PROCESS_INFORMATION pi = { 0 };
	STARTUPINFOA si = { 0 };
	SECURITY_ATTRIBUTES sa = { 0 };
	HANDLE hReadPipe, hWritePipe;
	DWORD retcode = -1;
	string ver;

	sa.bInheritHandle = TRUE;
	sa.nLength = sizeof SECURITY_ATTRIBUTES;
	sa.lpSecurityDescriptor = NULL;
	retval = CreatePipe(&hReadPipe, &hWritePipe, &sa, 0);

	si.cb = sizeof STARTUPINFO;
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.hStdOutput = si.hStdError = hWritePipe;

	retval = CreateProcessA(NULL, "powershell.exe (Get-WmiObject Win32_OperatingSystem).Caption", &sa, &sa, TRUE, 0, NULL, 0, &si, &pi);
	if (retval)
	{
		DWORD dwLen, dwRead;
		WaitForSingleObject(pi.hThread, INFINITE);//等待命令行执行完毕
		GetExitCodeProcess(pi.hProcess, &retcode);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		dwLen = GetFileSize(hReadPipe, NULL);
		char *buff = new char[dwLen + 1];
		memset(buff, 0, dwLen + 1);
		retval = ReadFile(hReadPipe, buff, dwLen, &dwRead, NULL);
		if (dwLen)
		{
			ver.clear();
			ver.append(buff, dwLen - 2);
			wsVer = string2wstring(ver);
		}
		delete buff;
	}
	/*
	retval = CreateProcessA(NULL, "powershell.exe (Get-WmiObject Win32_OperatingSystem).BuildNumber", &sa, &sa, TRUE, 0, NULL, 0, &si, &pi);
	if (retval)
	{
		DWORD dwLen, dwRead;
		WaitForSingleObject(pi.hThread, INFINITE);//等待命令行执行完毕
		GetExitCodeProcess(pi.hProcess, &retcode);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		dwLen = GetFileSize(hReadPipe, NULL);
		char *buff = new char[dwLen + 1];
		memset(buff, 0, dwLen + 1);
		retval = ReadFile(hReadPipe, buff, dwLen, &dwRead, NULL);
		if (dwLen)
		{
			ver.clear();
			ver.append(buff, dwLen - 2);
			wsVer += TEXT("（BuildNumber:") + string2wstring(ver) + TEXT("）");
		}
		delete buff;
	}
	*/
	return TRUE;
}

BOOL CDmi::GetOSBuildNumber(wstring &wsVer)
{
	BOOL retval, result = FALSE;
	PROCESS_INFORMATION pi = { 0 };
	STARTUPINFOA si = { 0 };
	SECURITY_ATTRIBUTES sa = { 0 };
	HANDLE hReadPipe, hWritePipe;
	DWORD retcode = -1;
	string ver;

	sa.bInheritHandle = TRUE;
	sa.nLength = sizeof SECURITY_ATTRIBUTES;
	sa.lpSecurityDescriptor = NULL;
	retval = CreatePipe(&hReadPipe, &hWritePipe, &sa, 0);

	si.cb = sizeof STARTUPINFO;
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.hStdOutput = si.hStdError = hWritePipe;

	retval = CreateProcessA(NULL, "powershell.exe (Get-WmiObject Win32_OperatingSystem).BuildNumber", &sa, &sa, TRUE, 0, NULL, 0, &si, &pi);
	if (retval)
	{
		DWORD dwLen, dwRead;
		WaitForSingleObject(pi.hThread, INFINITE);//等待命令行执行完毕
		GetExitCodeProcess(pi.hProcess, &retcode);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		dwLen = GetFileSize(hReadPipe, NULL);
		char *buff = new char[dwLen + 1];
		memset(buff, 0, dwLen + 1);
		retval = ReadFile(hReadPipe, buff, dwLen, &dwRead, NULL);
		if (dwLen)
		{
			ver.clear();
			ver.append(buff, dwLen - 2);
			wsVer = string2wstring(ver);
		}
		delete buff;
	}
	return TRUE;
}

BOOL CDmi::GetMemoryInfo(wstring & wsInfo)
{
	BOOL retval, result = FALSE;
	PROCESS_INFORMATION pi = { 0 };
	STARTUPINFOA si = { 0 };
	SECURITY_ATTRIBUTES sa = { 0 };
	HANDLE hReadPipe, hWritePipe;
	DWORD retcode = -1;

	sa.bInheritHandle = TRUE;
	sa.nLength = sizeof SECURITY_ATTRIBUTES;
	sa.lpSecurityDescriptor = NULL;
	retval = CreatePipe(&hReadPipe, &hWritePipe, &sa, 0);

	si.cb = sizeof STARTUPINFO;
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.hStdOutput = si.hStdError = hWritePipe;

	retval = CreateProcessA(NULL, "powershell.exe (Get-WmiObject Win32_PhysicalMemory).Capacity", &sa, &sa, TRUE, 0, NULL, 0, &si, &pi);
	if (retval)
	{
		DWORD dwLen, dwRead;
		WaitForSingleObject(pi.hThread, INFINITE);//等待命令行执行完毕
		GetExitCodeProcess(pi.hProcess, &retcode);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		dwLen = GetFileSize(hReadPipe, NULL);
		char *buff = new char[dwLen + 1];
		memset(buff, 0, dwLen + 1);
		retval = ReadFile(hReadPipe, buff, dwLen, &dwRead, NULL);
		if (dwLen)
		{
			ULONGLONG nTotalPhys = 0, nPhys = 0;
			char* szContext, *szToken = strtok_s(buff, "\r\n",&szContext);
			while (szToken)
			{
				nPhys = strtoull(szToken, 0, 10);
				nPhys >>= 30;//to GB
				nTotalPhys += nPhys;
				szToken = strtok_s(NULL, "\r\n",&szContext);
			}
			CString ver;
			ver.Format(TEXT("%1.2lfGB"), (double)nTotalPhys);
			wsInfo = ver;
		}
		delete buff;
	}
	return TRUE;
}

wstring CDmi::get_manufacture_date()
{
	CFile fp;
	BOOL bX64;
	IsWow64Process(GetCurrentProcess(), &bX64);
	CString szPath;
	string info;
	if (bX64)
	{
		szPath = L"c:\\windows\\sysnative\\oem\\version.txt";
	}
	else
	{
		szPath = L"c:\\windows\\system32\\oem\\version.txt";
	}
	BOOL ret = fp.Open(szPath, CFile::modeRead | CFile::typeBinary);
	if (ret)
	{
		DWORD len = (UINT)fp.GetLength();
		char* buf = new char[len + 1];
		fp.Read(buf, len);
		fp.Close();
		char* p = strchr(buf, ':');
		*strstr(buf, "\r\n") = 0;
		info.append(p + 1);
		delete buf;
	}
	else
	{
		return wstring(L"");
	}
	return string2wstring(info);
}