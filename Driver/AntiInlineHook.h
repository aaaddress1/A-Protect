#ifndef _ANTI_INLINE_HOOK_H_
#define _ANTI_INLINE_HOOK_H_

#include "ntifs.h"
//#include "InlineHook.h"
#include "ntos.h"
#include "ldasm.h"
#include "libdasm.h"

DWORD JmpFunctionAddress;

ULONG ulRunAddress;  //һ��ִ���˶��ٴ���

int HookFunctionProcessPatchCodeLen;
PVOID HookFunctionProcessRet;

ULONG HookFunctionAddress;

extern BYTE *ImageModuleBase;
extern ULONG SystemKernelModuleBase;
extern ULONG SystemKernelModuleSize;

extern ULONG ulMyDriverBase;
extern ULONG ulMyDriverSize;

ULONG GetSystemRoutineAddress(
	int IntType,
	PVOID lpwzFunction
	);

unsigned long __fastcall GetFunctionCodeSize(void *Proc);

#endif