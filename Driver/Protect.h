#ifndef _PROTECTCODE_H_
#define _PROTECTCODE_H_

#include "ntifs.h"
#include "InitWindowsVersion.h"
#include "ldasm.h"
#include "ntos.h"
#include "ObjectHookCheck.h"

#define SystemHandleInformation 0x10

UCHAR *PsGetProcessImageFileName(__in PEPROCESS eprocess);

typedef VOID (FASTCALL *KIINSERTQUEUEAPC)(
	IN PKAPC Apc,
	IN KPRIORITY Increment
	); 

typedef BOOLEAN (__stdcall *KEINSERTQUEUEAPC)(
	__inout PRKAPC Apc,
	__in_opt PVOID SystemArgument1,
	__in_opt PVOID SystemArgument2,
	__in KPRIORITY Increment
	);

typedef NTSTATUS (_stdcall*PARSEPROCEDURE) (
	__in PVOID RootDirectory,
	__in POBJECT_TYPE ObjectType,
	__in PACCESS_STATE AccessState,
	__in KPROCESSOR_MODE AccessCheckMode,
	__in ULONG Attributes,
	__in PUNICODE_STRING ObjectName,
	__in PUNICODE_STRING RemainingName,
	__in PVOID ParseContext ,
	__in PSECURITY_QUALITY_OF_SERVICE SecurityQos,
	__out PVOID *Object
	);

int KiInsertQueueApcPatchCodeLen;
PVOID KiInsertQueueApcRet;

int KeInsertQueueApcPatchCodeLen;
PVOID KeInsertQueueApcRet;

int ParseFilePatchCodeLen;
PVOID ParseFileRet;

BOOL KiInsertQueueApcHooked;
BOOL KeInsertQueueApcHooked;
BOOL ParseFileHooked;

WIN_VER_DETAIL WinVer;
ULONG ulThreadOffset;
ULONG ulApcStateOffset;
ULONG ulProcessOffset;
ULONG ulSystemArgument1Offset;

extern PEPROCESS CsrssEProcess;
extern PEPROCESS ProtectEProcess;  //�Լ����̵�ep
extern PEPROCESS SystemEProcess;

extern BOOL DebugOn;
extern BOOL bProtectDriverFile;

extern BYTE *ImageModuleBase;
extern ULONG SystemKernelModuleBase;

extern BOOL bProtectProcess;   //�Ƿ񱣻�����
extern BOOL bIsInitSuccess;

typedef struct _SYSTEM_HANDLE_INFORMATION{
	ULONG ProcessID;                //���̵ı�ʶID
	UCHAR ObjectTypeNumber;         //��������
	UCHAR Flags;                    //0x01 = PROTECT_FROM_CLOSE,0x02 = INHERIT
	USHORT Handle;                  //����������ֵ
	PVOID  Object;                  //��������ָ���ں˶����ַ
	ACCESS_MASK GrantedAccess;      //�������ʱ��׼��Ķ���ķ���Ȩ
}SYSTEM_HANDLE_INFORMATION, * PSYSTEM_HANDLE_INFORMATION;

typedef struct _SYSTEM_HANDLE_INFORMATION_EX
{
	ULONG NumberOfHandles;
	SYSTEM_HANDLE_INFORMATION Information[];
}SYSTEM_HANDLE_INFORMATION_EX, *PSYSTEM_HANDLE_INFORMATION_EX;

typedef enum _OBJECT_INFORMATION_CLASS{
	ObjectBasicInformation,
	ObjectNameInformation,
	ObjectTypeInformation,
	ObjectAllInformation,
	ObjectDataInformation
}OBJECT_INFORMATION_CLASS, *POBJECT_INFORMATION_CLASS;

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
ULONG ulLogCount;
//------------------------------------------------------------
NTSTATUS ZwQuerySystemInformation(
	__in       ULONG SystemInformationClass,
	__inout    PVOID SystemInformation,
	__in       ULONG SystemInformationLength,
	__out_opt  PULONG ReturnLength
	);

NTSTATUS ZwQueryObject(
	__in_opt   HANDLE Handle,
	__in       OBJECT_INFORMATION_CLASS ObjectInformationClass,
	__out_opt  PVOID ObjectInformation,
	__in       ULONG ObjectInformationLength,
	__out_opt  PULONG ReturnLength
	);

BOOL HookFunctionByHeaderAddress(
	DWORD NewFunctionAddress,
	DWORD oldFunctionAddress,
	PVOID HookZone,
	int *patchCodeLen,
	PVOID *lpRet
	);

NTSTATUS LookupProcessByPid(
	IN HANDLE hProcessPid,
	OUT PEPROCESS *pEprocess
	);

ULONG GetSystemRoutineAddress(
	int IntType,
	PVOID lpwzFunction
	);

BOOLEAN ValidateUnicodeString(PUNICODE_STRING usStr);

#endif