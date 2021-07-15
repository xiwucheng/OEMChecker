#include "stdafx.h"
#include "wmibase.h"

CWmiBase::CWmiBase()
{

}

CWmiBase::~CWmiBase()
{
	Uninit();
}

int CWmiBase::Init(LPCTSTR lpcSQL, LPCTSTR lpcItemA, LPCTSTR lpcItemB, LPCTSTR lpcItemC)
{
	HRESULT hr = S_OK;
	IWbemLocator    * pLocator = NULL;
	IWbemServices   * pNamespace = NULL;
	IEnumWbemClassObject *pEnum = NULL;
	IWbemClassObject* pclsObj = NULL;
	ULONG uReturn = 0;
	int retValue = 0;
	m_lInfo.clear();
	hr = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		goto cleanup;
	}
	//  NOTE:  
	//  When using asynchronous WMI API's remotely in an environment where the "Local System" account  
	//  has no network identity (such as non-Kerberos domains), the authentication level of  
	//  RPC_C_AUTHN_LEVEL_NONE is needed. However, lowering the authentication level to  
	//  RPC_C_AUTHN_LEVEL_NONE makes your application less secure. It is wise to  
	// use semi-synchronous API's for accessing WMI data and events instead of the asynchronous ones.  

	hr = CoInitializeSecurity(NULL,
		-1,
		NULL,
		NULL,
		RPC_C_AUTHN_LEVEL_DEFAULT,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		NULL,
		EOAC_NONE, //change to EOAC_NONE if you change dwAuthnLevel to RPC_C_AUTHN_LEVEL_NONE  
		NULL);
	if (FAILED(hr) && RPC_E_TOO_LATE != hr)
	{
		goto cleanup;
	}

	hr = CoCreateInstance(CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator,
		(LPVOID *)&pLocator);
	if (FAILED(hr))
	{
		goto cleanup;
	}
	hr = pLocator->ConnectServer(bstr_t(L"ROOT\\CIMV2"),
		NULL,
		NULL,
		NULL,
		0,
		NULL,
		NULL,
		&pNamespace);
	if (hr != WBEM_S_NO_ERROR)
	{
		goto cleanup;
	}

	hr = CoSetProxyBlanket(pNamespace,
		RPC_C_AUTHN_WINNT,
		RPC_C_AUTHZ_NONE,
		NULL,
		RPC_C_AUTHN_LEVEL_CALL,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		NULL,
		EOAC_NONE);

	if (hr != WBEM_S_NO_ERROR)
	{
		goto cleanup;
	}

	//-------------------------------------------------------------------------------
	hr = pNamespace->ExecQuery(
		bstr_t(L"WQL"),
		//          bstr_t("SELECT * FROM Win32_SoundDevice"),                          // 音频设备  
		//          bstr_t("SELECT * FROM Win32_Product"),    
		//          bstr_t("SELECT * FROM Win32_Registry"),                             // 注册表  
		//          bstr_t("SELECT * FROM Win32_OnBoardDevice"),                        // 主板上的嵌入设备  
		//          bstr_t("SELECT * FROM Win32_MotherboardDevice"),                    // 母板  
		//          bstr_t("SELECT * FROM Win32_BaseBoard"),                            // 主板  
		//          bstr_t("SELECT * FROM Win32_DMAChannel"),                           // DMA通道  
		//          bstr_t("SELECT * FROM Win32_Bus"),                                  // 总线  
		//          bstr_t("SELECT * FROM Win32_BIOS"),                                 // BIOS系统  
		//          bstr_t("SELECT * FROM Win32_SystemBIOS"),  
		//          bstr_t("SELECT * FROM Win32_Processor"),                            // 处理器  
		//          bstr_t("SELECT * FROM Win32_SystemProcesses"),                      //   
		//          bstr_t("SELECT * FROM Win32_Process"),                              // 系统进程  
		//          bstr_t("SELECT * FROM Win32_Thread"),                               // 系统线程  
		//          bstr_t("SELECT * FROM Win32_AssociatedProcessorMemory"),            // CACHE  
		//          bstr_t("SELECT * FROM Win32_CacheMemory"),                          // 二级缓存内存  
		//          bstr_t("SELECT * FROM Win32_PhysicalMedia"),                        // 物理媒体信息 (硬盘)  
		//          bstr_t("SELECT * FROM Win32_LogicalDisk "),                         // 逻辑驱动器  
		//          bstr_t("SELECT * FROM Win32_DiskDrive"),                            // 磁盘驱动器  
		//          bstr_t("SELECT * FROM Win32_MemoryDevice"),                         // 内存设备  
		//          bstr_t("SELECT * FROM Win32_PhysicalMemoryArray"),                  // 物理内存数组  
		//          bstr_t("SELECT * FROM Win32_PhysicalMemoryLocation"),               // 物理内存位置  
		//          bstr_t("SELECT * FROM CIM_NumericSensor"),                          // 数字传感器  
		//          bstr_t("SELECT * FROM Win32_VoltageProbe"),                         // 数字传感器   
		//          bstr_t("SELECT * FROM Win32_TemperatureProbe"),                     // 温度传感器  
		//          bstr_t("SELECT * FROM Win32_CurrentProbe"),       
		//          bstr_t("SELECT * FROM Win32_OperatingSystem"),                      // 操作系统      
		//          bstr_t("SELECT * FROM Win32_UserAccount"),                          // 用户账号  
		//          bstr_t("SELECT * FROM Win32_SerialPort"),                           // 串行接口  
		//          bstr_t("SELECT * FROM Win32_ParallelPort"),                         // 并行接口  
		//          bstr_t("SELECT * FROM Win32_SCSIController"),                       // 小型计算机系统接口  
		//          bstr_t("SELECT * FROM Win32_PortResource"),                         // I/O 端口  
		//          bstr_t("SELECT * FROM Win32_PNPDevice"),                            // 即插即用设备  
		//          bstr_t("SELECT * FROM Win32_NetworkAdapter"),                       // 网络适配器  
		//          bstr_t("SELECT * FROM Win32_NetworkAdapterConfiguration"),  
		//          bstr_t("SELECT * FROM Win32_NetworkAdapterSetting"),  
		//          bstr_t("SELECT * FROM Win32_AssociatedBattery"),  
		//          bstr_t("SELECT * FROM Win32_Battery"),                              // 内部电池  
		//          bstr_t("SELECT * FROM Win32_PortableBattery"),  
		//          bstr_t("SELECT * FROM Win32_PowerManagementEvent"),  
		//          bstr_t("SELECT * FROM Win32_UninterruptiblePowerSupply"),  
		//          bstr_t("SELECT * FROM Win32_DriverForDevice"),  
		//          bstr_t("SELECT * FROM Win32_Printer"),                              // 打印机  
		//          bstr_t("SELECT * FROM Win32_TCPIPPrinterPort"),  
		//          bstr_t("SELECT * FROM Win32_POTSModem"),  
		//          bstr_t("SELECT * FROM Win32_DesktopMonitor"),                       // 显示器  
		//			bstr_t("SELECT * FROM Win32_VideoController"),                      // 显卡  
		//          bstr_t("SELECT * FROM Win32_CDROMDrive"),  
		//          bstr_t("SELECT * FROM Win32_Keyboard"),                             // 键盘  
		//          bstr_t("SELECT * FROM Win32_AutochkSetting"),  
		//          bstr_t("SELECT * FROM Win32_PointingDevice"),                       // 点击设备：鼠标、触摸板  
		//          bstr_t("SELECT * FROM Win32_Fan"),                                  // 风扇  
		//          bstr_t("SELECT * FROM Win32_WMISetting"),  
		//          bstr_t("SELECT * FROM Win32_TimeZone"),       
		//          bstr_t("SELECT * FROM Win32_Environment"),                          // 环境路径  
		//          bstr_t("SELECT * FROM Win32_QuotaSetting"),  
		//          bstr_t("SELECT * FROM Win32_NetworkProtocol"),                      // 己安装的网络协议  
		//bstr_t(L"SELECT * FROM Win32_PnPSignedDriver WHERE DeviceID like '%DEV_AE34%' or DeviceID like '%ESSX8336%'"),
		bstr_t(lpcSQL),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnum);

	if (hr != WBEM_S_NO_ERROR)
	{
		goto cleanup;
	}

	//bstr_t(L"SELECT DeviceName, ,HardWareID, DeviceID, Manufacturer, DriverDate, DriverVersion FROM Win32_PnPSignedDriver"),
	while (pEnum)
	{
		HRESULT hr = pEnum->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
		if ((0 == uReturn) || (0 == pclsObj))
		{
			break;
		}

		VARIANT vtProp;
		// Get the value of the Name property   
		WmiInfo wi;
		VariantInit(&vtProp);
		hr = pclsObj->Get(lpcItemA, 0, &vtProp, 0, 0);     
		if (vtProp.bstrVal != 0)
		{
			if (vtProp.vt != VT_BSTR)
			{
				CString sTmp;
				sTmp.Format(L"%d", vtProp.uintVal);
				wi.szA = sTmp;
			}
			else
			{
				wi.szA = vtProp.bstrVal;
			}
		}

		if (lpcItemB)
		{
			VariantInit(&vtProp);
			hr = pclsObj->Get(lpcItemB, 0, &vtProp, 0, 0);
			if (vtProp.bstrVal != 0)
			{
				if (vtProp.vt != VT_BSTR)
				{
					CString sTmp;
					sTmp.Format(L"%d", vtProp.uintVal);
					wi.szB = sTmp;
				}
				else
				{
					wi.szB = vtProp.bstrVal;
				}
			}
		}

		if (lpcItemC)
		{
			VariantInit(&vtProp);
			hr = pclsObj->Get(lpcItemC, 0, &vtProp, 0, 0);
			if (vtProp.bstrVal != 0)
			{
				if (vtProp.vt != VT_BSTR)
				{
					CString sTmp;
					sTmp.Format(L"%d", vtProp.uintVal);
					wi.szC = sTmp;
				}
				else
				{
					wi.szC = vtProp.bstrVal;
				}
			}
		}
		m_lInfo.push_back(wi);
		retValue++;
	}
	//-------------------------------------------------------------------------------
	// Free up resources  
cleanup:
	if (pLocator)
	{
		pLocator->Release();
	}
	if (pNamespace)
	{
		pNamespace->Release();
	}
	if (pEnum)
	{
		pEnum->Release();
	}
	if (pclsObj)
	{
		pclsObj->Release();
	}
	CoUninitialize();

	return retValue;
}

void CWmiBase::Uninit()
{
	m_lInfo.clear();
}