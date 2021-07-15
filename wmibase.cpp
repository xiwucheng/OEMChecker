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
		//          bstr_t("SELECT * FROM Win32_SoundDevice"),                          // ��Ƶ�豸  
		//          bstr_t("SELECT * FROM Win32_Product"),    
		//          bstr_t("SELECT * FROM Win32_Registry"),                             // ע���  
		//          bstr_t("SELECT * FROM Win32_OnBoardDevice"),                        // �����ϵ�Ƕ���豸  
		//          bstr_t("SELECT * FROM Win32_MotherboardDevice"),                    // ĸ��  
		//          bstr_t("SELECT * FROM Win32_BaseBoard"),                            // ����  
		//          bstr_t("SELECT * FROM Win32_DMAChannel"),                           // DMAͨ��  
		//          bstr_t("SELECT * FROM Win32_Bus"),                                  // ����  
		//          bstr_t("SELECT * FROM Win32_BIOS"),                                 // BIOSϵͳ  
		//          bstr_t("SELECT * FROM Win32_SystemBIOS"),  
		//          bstr_t("SELECT * FROM Win32_Processor"),                            // ������  
		//          bstr_t("SELECT * FROM Win32_SystemProcesses"),                      //   
		//          bstr_t("SELECT * FROM Win32_Process"),                              // ϵͳ����  
		//          bstr_t("SELECT * FROM Win32_Thread"),                               // ϵͳ�߳�  
		//          bstr_t("SELECT * FROM Win32_AssociatedProcessorMemory"),            // CACHE  
		//          bstr_t("SELECT * FROM Win32_CacheMemory"),                          // ���������ڴ�  
		//          bstr_t("SELECT * FROM Win32_PhysicalMedia"),                        // ����ý����Ϣ (Ӳ��)  
		//          bstr_t("SELECT * FROM Win32_LogicalDisk "),                         // �߼�������  
		//          bstr_t("SELECT * FROM Win32_DiskDrive"),                            // ����������  
		//          bstr_t("SELECT * FROM Win32_MemoryDevice"),                         // �ڴ��豸  
		//          bstr_t("SELECT * FROM Win32_PhysicalMemoryArray"),                  // �����ڴ�����  
		//          bstr_t("SELECT * FROM Win32_PhysicalMemoryLocation"),               // �����ڴ�λ��  
		//          bstr_t("SELECT * FROM CIM_NumericSensor"),                          // ���ִ�����  
		//          bstr_t("SELECT * FROM Win32_VoltageProbe"),                         // ���ִ�����   
		//          bstr_t("SELECT * FROM Win32_TemperatureProbe"),                     // �¶ȴ�����  
		//          bstr_t("SELECT * FROM Win32_CurrentProbe"),       
		//          bstr_t("SELECT * FROM Win32_OperatingSystem"),                      // ����ϵͳ      
		//          bstr_t("SELECT * FROM Win32_UserAccount"),                          // �û��˺�  
		//          bstr_t("SELECT * FROM Win32_SerialPort"),                           // ���нӿ�  
		//          bstr_t("SELECT * FROM Win32_ParallelPort"),                         // ���нӿ�  
		//          bstr_t("SELECT * FROM Win32_SCSIController"),                       // С�ͼ����ϵͳ�ӿ�  
		//          bstr_t("SELECT * FROM Win32_PortResource"),                         // I/O �˿�  
		//          bstr_t("SELECT * FROM Win32_PNPDevice"),                            // ���弴���豸  
		//          bstr_t("SELECT * FROM Win32_NetworkAdapter"),                       // ����������  
		//          bstr_t("SELECT * FROM Win32_NetworkAdapterConfiguration"),  
		//          bstr_t("SELECT * FROM Win32_NetworkAdapterSetting"),  
		//          bstr_t("SELECT * FROM Win32_AssociatedBattery"),  
		//          bstr_t("SELECT * FROM Win32_Battery"),                              // �ڲ����  
		//          bstr_t("SELECT * FROM Win32_PortableBattery"),  
		//          bstr_t("SELECT * FROM Win32_PowerManagementEvent"),  
		//          bstr_t("SELECT * FROM Win32_UninterruptiblePowerSupply"),  
		//          bstr_t("SELECT * FROM Win32_DriverForDevice"),  
		//          bstr_t("SELECT * FROM Win32_Printer"),                              // ��ӡ��  
		//          bstr_t("SELECT * FROM Win32_TCPIPPrinterPort"),  
		//          bstr_t("SELECT * FROM Win32_POTSModem"),  
		//          bstr_t("SELECT * FROM Win32_DesktopMonitor"),                       // ��ʾ��  
		//			bstr_t("SELECT * FROM Win32_VideoController"),                      // �Կ�  
		//          bstr_t("SELECT * FROM Win32_CDROMDrive"),  
		//          bstr_t("SELECT * FROM Win32_Keyboard"),                             // ����  
		//          bstr_t("SELECT * FROM Win32_AutochkSetting"),  
		//          bstr_t("SELECT * FROM Win32_PointingDevice"),                       // ����豸����ꡢ������  
		//          bstr_t("SELECT * FROM Win32_Fan"),                                  // ����  
		//          bstr_t("SELECT * FROM Win32_WMISetting"),  
		//          bstr_t("SELECT * FROM Win32_TimeZone"),       
		//          bstr_t("SELECT * FROM Win32_Environment"),                          // ����·��  
		//          bstr_t("SELECT * FROM Win32_QuotaSetting"),  
		//          bstr_t("SELECT * FROM Win32_NetworkProtocol"),                      // ����װ������Э��  
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