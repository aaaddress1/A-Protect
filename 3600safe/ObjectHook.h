#include "stdafx.h"


typedef struct _OBJECTHOOKINFO_INFORMATION {          //INLINEHOOKINFO_INFORMATION
	ULONG ulMemoryFunctionBase;    //ԭʼ��ַ
	ULONG ulMemoryHookBase;        //HOOK ��ַ
	CHAR lpszFunction[256];
	CHAR lpszHookModuleImage[256];
	ULONG ulObjectTypeBase;
	ULONG ulHookType;
	CHAR lpszObjectTypeName[256];

} OBJECTHOOKINFO_INFORMATION, *POBJECTHOOKINFO_INFORMATION;

typedef struct _OBJECTHOOKINFO {          //InlineHook
	ULONG ulCount;
	OBJECTHOOKINFO_INFORMATION ObjectHook[1];
} OBJECTHOOKINFO, *POBJECTHOOKINFO;

POBJECTHOOKINFO ObjectHookInfo;
