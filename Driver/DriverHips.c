//**************************************************************************
//*
//*          A�ܵ��Է��� website:http://www.3600safe.com/
//*        
//*�ļ�˵����
//*         ��ֹ��������
//**************************************************************************
#include "DriverHips.h"

__declspec(naked) BOOLEAN SeSinglePrivilegeCheckHookZone(,...)
{
	_asm
	{
		_emit 0x90;
		_emit 0x90;
		_emit 0x90;
		_emit 0x90;
		_emit 0x90;
		_emit 0x90;
		_emit 0x90;
		_emit 0x90;
		_emit 0x90;
		_emit 0x90;
		_emit 0x90;
		_emit 0x90;
		_emit 0x90;
		_emit 0x90;
		_emit 0x90;
		_emit 0x90;
		jmp [SeSinglePrivilegeCheckRet];
	}
}
//Ȩ�޼���ʱ�򷵻�ʧ�����ﵽ��ֹ��������
BOOLEAN __stdcall NewSeSinglePrivilegeCheck(
	__in  LUID PrivilegeValue,
	__in  KPROCESSOR_MODE PreviousMode
	)
{
	ULONG ulPage;

	if (!bIsInitSuccess)
		goto _FunctionRet;

	//ȡ���ص�ַ
	_asm
	{
		mov eax,dword ptr[ebp+4]
		mov ulPage,eax
	}
	//KdPrint(("ulPage:%08x\r\n",ulPage));
	//RPsGetCurrentProcess = ReLoadNtosCALL(L"PsGetCurrentProcess",SystemKernelModuleBase,ImageModuleBase);
	if (!RPsGetCurrentProcess)
	{
		goto _FunctionRet;
	}
	if (RPsGetCurrentProcess() == ProtectEProcess)
	{
		goto _FunctionRet;
	}

	if (ulPage >= ulNtLoadDriverBase && ulPage <= ulNtLoadDriverBase+ulNtLoadDriverSize)
		return FALSE;
	
	if (ulPage >= ulZwSetSystemInformationBase && ulPage <= ulZwSetSystemInformationBase+ulZwSetSystemInformationSize)
		return FALSE;

_FunctionRet:
	OldSeSinglePrivilegeCheck = (SeSinglePrivilegeCheck_1)SeSinglePrivilegeCheckHookZone;
	return OldSeSinglePrivilegeCheck(
		PrivilegeValue,
		PreviousMode
		);
}
//��ֹ��������
NTSTATUS DisEnableDriverLoading()
{
	int bRet;

	ulZwSetSystemInformationBase = GetSystemRoutineAddress(1,L"ZwSetSystemInformation");
	ulNtLoadDriverBase = GetSystemRoutineAddress(1,L"ZwLoadDriver");
	if (ulNtLoadDriverBase &&
		ulZwSetSystemInformationBase)
	{
		ulNtLoadDriverSize = SizeOfProc(ulNtLoadDriverBase);
		ulZwSetSystemInformationSize = SizeOfProc(ulZwSetSystemInformationBase);
	}

	ulSeSinglePrivilegeCheck = GetSystemRoutineAddress(1,L"SeSinglePrivilegeCheck");
	if (!ulSeSinglePrivilegeCheck ||
		!ulNtLoadDriverBase ||
		!ulZwSetSystemInformationBase)
	{
		return STATUS_UNSUCCESSFUL;
	}
	//����reload��ĵ�ַ����Ȼ�жϲ���
	ulNtLoadDriverBase = ulNtLoadDriverBase - SystemKernelModuleBase+ImageModuleBase;
	ulZwSetSystemInformationBase = ulZwSetSystemInformationBase - SystemKernelModuleBase+ImageModuleBase;
	ulReloadSeSinglePrivilegeCheck = ulSeSinglePrivilegeCheck - SystemKernelModuleBase+ImageModuleBase;

	//hook reload SeSinglePrivilegeCheck

	bRet = HookFunctionByHeaderAddress(ulReloadSeSinglePrivilegeCheck,ulSeSinglePrivilegeCheck,SeSinglePrivilegeCheckHookZone,&SeSinglePrivilegeCheckPatchCodeLen,&SeSinglePrivilegeCheckRet);
	if(bRet)
	{
		bRet = FALSE;
		bRet = HookFunctionByHeaderAddress(
			NewSeSinglePrivilegeCheck,
			ulReloadSeSinglePrivilegeCheck,
			SeSinglePrivilegeCheckHookZone,
			&SeSinglePrivilegeCheckPatchCodeLen,
			&SeSinglePrivilegeCheckRet
			);
		if (bRet)
		{
			SeSinglePrivilegeCheckHooked = TRUE;
			//DbgPrint("hook SeSinglePrivilegeCheck success\n");
		}
	}
	return STATUS_SUCCESS;
}
//������������
NTSTATUS EnableDriverLoading()
{
	if (SeSinglePrivilegeCheckHooked == TRUE)
	{
		SeSinglePrivilegeCheckHooked = FALSE;
		UnHookFunctionByHeaderAddress((DWORD)ulReloadSeSinglePrivilegeCheck,SeSinglePrivilegeCheckHookZone,SeSinglePrivilegeCheckPatchCodeLen);
		UnHookFunctionByHeaderAddress((DWORD)ulSeSinglePrivilegeCheck,SeSinglePrivilegeCheckHookZone,SeSinglePrivilegeCheckPatchCodeLen);
	}
}