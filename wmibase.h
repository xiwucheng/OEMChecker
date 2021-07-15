#pragma once
#ifndef __WMI_BASE__
#define __WMI_BASE__
#include <objbase.h>  
#include <windows.h>                                      
#include <stdio.h>  
#include <wbemidl.h>  
#include <comdef.h>  
#include <string> 
#include <list>

#pragma comment(lib, "wbemuuid.lib")  
#pragma comment(lib, "comsuppw.lib")  

using namespace std;

typedef struct
{
	wstring szA;
	wstring szB;
	wstring szC;
}WmiInfo;

class CWmiBase
{
public:
	list<WmiInfo> m_lInfo;
	CWmiBase();
	virtual ~CWmiBase();
	int Init(LPCTSTR lpcSQL, LPCTSTR lpcItemA, LPCTSTR lpcItemB=NULL, LPCTSTR lpcItemC=NULL);
	void Uninit();
private:

};
#endif//__WMI_BASE__