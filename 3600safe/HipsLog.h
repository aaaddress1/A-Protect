#include "stdafx.h"
#include "Md5.h"
//------------------------------------------------------------
//������־
//------------------------------------------------------------
typedef struct _LOGDEFENSE_INFORMATION {          //LOGDEFENSE_INFORMATION
	int  Type;     //���� 1:�Լ��Ƿ񱻽��� 2:�����޸ķ���
	ULONG ulPID;
	ULONG ulInheritedFromProcessId;      //������
	CHAR lpszProName[100];            //������
	ULONG EProcess;
	WCHAR lpwzMoreEvents[1024];
	WCHAR lpwzCreateProcess[260];

} LOGDEFENSE_INFORMATION, *PLOGDEFENSE_INFORMATION;
typedef struct _LOGDEFENSE {          //������־
	ULONG ulCount;
	LOGDEFENSE_INFORMATION LogDefense[1];
} LOGDEFENSE, *PLOGDEFENSE;

PLOGDEFENSE LogDefenseInfo;

BOOL InstallDepthServicesScan(CHAR * serviceName);
void ShutdownWindows( DWORD dwReason );

extern unsigned int conv(unsigned int a);

extern WCHAR WindowsXPSP3_CN[1900][50];
extern WCHAR WindowsXPSP2_CN[1753][50];

extern WCHAR Windows2003SP1_CN[1776][50];
extern WCHAR Windows2003SP2_CN[1856][50];

extern WCHAR Windows7Home_CN[2409][50];
extern WCHAR Windows7SP1_CN[2526][50];

BOOL FileVerify(char *lpszFullPath,WCHAR *lpwzFileMd5,WCHAR *lpwzTrue);
void SaveToFile(CHAR *lpszBuffer,WCHAR *lpwzFilePath);