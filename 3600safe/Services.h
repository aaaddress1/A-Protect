#include "stdafx.h"
#include "Md5.h"

typedef struct _SERVICESREGISTRY_INFORMATION {          //SERVICESREGISTRY_INFORMATION
	WCHAR lpwzSrvName[256];
	WCHAR lpwzImageName[256];
	WCHAR lpwzDLLPath[256];
	//WCHAR lpwzDescription[256];
	//ULONG BootType;
	//ULONG RunStatus;     //����״̬

} SERVICESREGISTRY_INFORMATION, *PSERVICESREGISTRY_INFORMATION;

typedef struct _SERVICESREGISTRY {          //SERVICESREGISTRY
	ULONG ulCount;
	SERVICESREGISTRY_INFORMATION SrvReg[1];

} SERVICESREGISTRY, *PSERVICESREGISTRY;

PSERVICESREGISTRY ServicesRegistry;

BOOL bIsHideServices = FALSE;

extern ULONG ulKernelSize;

extern unsigned int conv(unsigned int a);

extern WCHAR WindowsXPSP3_CN[1900][50];
extern WCHAR WindowsXPSP2_CN[1753][50];

extern WCHAR Windows2003SP1_CN[1776][50];
extern WCHAR Windows2003SP2_CN[1856][50];

extern WCHAR Windows7Home_CN[2409][50];
extern WCHAR Windows7SP1_CN[2526][50];

BOOL FileVerify(char *lpszFullPath,WCHAR *lpwzFileMd5,WCHAR *lpwzTrue);
BOOL InstallDepthServicesScan(CHAR * serviceName);
BOOL WINAPI EnableDebugPriv(LPCTSTR name);  //����Ȩ����
void ShutdownWindows( DWORD dwReason );
