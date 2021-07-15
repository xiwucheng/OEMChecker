#pragma once
#ifndef __DMI_H__
#define __DMI_H__
#include <Windows.h>
#include <string>
#pragma comment(lib, "version.lib")

using namespace std;

typedef struct _dmi_header
{
	BYTE type;
	BYTE length;
	WORD handle;
}dmi_header;

typedef struct
{
	BYTE Used20CallingMethod;
	BYTE SMBIOSMajorVersion;
	BYTE SMBIOSMinorVersion;
	BYTE DmiRevision;
	DWORD Length;
	BYTE SMBIOSTableData[1];
}RawSMBIOSData;

typedef struct _dmi_type_desc
{
	DWORD Length;
	BYTE table[1];
}dmi_type_desc;

typedef struct _dmi_type_0//BIOS Information structure
{
	string vendor;
	string bios_version;
	string bios_release_date;
	BYTE   bios_major;
	BYTE   bios_minor;
	BYTE   ec_major;
	BYTE   ec_minor;
}dmi_type_0;

typedef struct _dmi_type_1//System Information structure
{
	string manufacturer;
	string product_name;
	string version;
	string serial_number;
	string uuid;
	string sku_number;
	string family;

}dmi_type_1;

typedef struct _dmi_type_2//Baseboard Information structure
{
	string manufacturer;
	string product_name;
	string version;
	string serial_number;
	string asset_tag;
	string location;

}dmi_type_2;

typedef struct _dmi_type_3// System Enclosure or Chassis
{
	string manufacturer;
	string form_factor;
	BYTE   type;
	string version;
	string serial_number;
	string asset_tag_number;
	string sku_number;
}dmi_type_3;

typedef struct _dmi_type_4//Processor Information
{
	string socket_designation;
	string processor_manufacturer;
	string processor_version;
	string serial_number;
	string asset_tag;
	string part_number;
}dmi_type_4;

typedef struct _dmi_type_6//Memory Module Information
{
	string socket_designation;
	string installed_memory;
}dmi_type_6;

typedef struct _oem_tag_
{
	string oem_table_id;
	string oem_id;
}oem_tag;

 class CDmi
{
public:
	CDmi();
	virtual ~CDmi();
public:
	void Init();
	string wstring2string(wstring wstr);
	wstring string2wstring(string str);
	wstring get_oem_table_id();
	wstring get_oem_id();
	wstring get_bios_version();
	wstring get_bios_release_date();
	BYTE get_bios_major();
	BYTE get_bios_minor();
	BYTE get_ec_major();
	BYTE get_ec_minor();
	wstring get_vendor();
	wstring get_system_manufacturer();
	wstring get_system_product_name();
	wstring get_system_version();
	wstring get_system_serial_number();
	wstring get_uuid();
	wstring get_sku_number();
	wstring get_family();
	wstring get_baseboard_manufacturer();
	wstring get_baseboard_product_name();
	wstring get_baseboard_serial_number();
	wstring get_baseboard_version();
	wstring get_processor_version();
	wstring get_form_factor();
	BYTE get_form_factor_type();
	wstring get_installed_memory();
	wstring get_manufacture_date();
	BOOL GetVersion(LPCTSTR szPEFile, LPDWORD pVer);
	BOOL GetVersion(LPCTSTR szPEFile, CString& Ver);
	BOOL GetFileExist(LPCTSTR lpFile);
	BOOL GetOSVersion(wstring& wsVer);
	BOOL GetOSBuildNumber(wstring& wsVer);
	BOOL GetMemoryInfo(wstring & wsInfo);
protected:
	dmi_type_0 dmitype0;
	dmi_type_1 dmitype1;
	dmi_type_2 dmitype2;
	dmi_type_3 dmitype3;
	dmi_type_4 dmitype4;
	dmi_type_6 dmitype6;
	oem_tag dmioemtag;
private:
	string dmi_string(dmi_header* dm, BYTE index);
	void dmi_system_uuid(const BYTE *p, std::string &uuid);
};
#endif//__DMI_H__