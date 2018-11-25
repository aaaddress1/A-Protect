//**************************************************************************
//*
//*          A�ܵ��Է��� website:http://www.3600safe.com/
//*        
//*�ļ�˵����
//*         �ƹ��ں�hook
//**************************************************************************
#include "AntiInlineHook.h"

/*
__declspec(naked) VOID __stdcall NewHookFunctionProcess()
{
	_asm
	{
		jmp [ulReloadAddress];   //ֱ��������reload�ĺ�����ַȥ~~������
	}
}
*/
//�������ǰ�����ѣ�
__declspec(naked) VOID HookFunctionProcessHookZone(,...)
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
		jmp [JmpFunctionAddress];
	}
}
VOID AntiInlineHook(WCHAR *FunctionName)
{
	ULONG ulTemp = NULL;
	PUCHAR p;
	BOOL bIsHooked = FALSE;
	INSTRUCTION	Inst;
	INSTRUCTION	Instb;
	BOOL bInit = FALSE;
	ULONG ulHookFunctionAddress;
	ULONG JmpReloadFunctionAddress;

	__try
	{
		//RMmIsAddressValid = ReLoadNtosCALL(L"MmIsAddressValid",SystemKernelModuleBase,ImageModuleBase);
		if (RMmIsAddressValid)
		{
			bInit = TRUE;
		}
		if (!bInit)
			return;

		JmpFunctionAddress = GetSystemRoutineAddress(1,FunctionName);  //�õ�������ַ

		if (DebugOn)
			KdPrint(("Get System Routine Address:%ws:%08x\r\n",FunctionName,JmpFunctionAddress));

		if (JmpFunctionAddress)
		{
			JmpReloadFunctionAddress = JmpFunctionAddress - SystemKernelModuleBase + (ULONG)ImageModuleBase;  //���ԭʼ��ַ

			if (!RMmIsAddressValid(JmpFunctionAddress) ||
				!RMmIsAddressValid(JmpReloadFunctionAddress))
			{
				if (DebugOn)
					KdPrint(("get function failed\r\n"));
				return;
			}
			if (GetFunctionCodeSize(JmpFunctionAddress) == GetFunctionCodeSize(JmpReloadFunctionAddress) &&
				memcmp(JmpFunctionAddress,JmpReloadFunctionAddress,GetFunctionCodeSize(JmpFunctionAddress)) != NULL)
			{
				if (DebugOn)
					KdPrint(("search---->%ws",FunctionName));
				//KdPrint(("---->%s:%08x",functionName,ulOldAddress));
				//��ʼɨ��hook
				for (p=JmpFunctionAddress ;p< JmpFunctionAddress+GetFunctionCodeSize(JmpFunctionAddress); p++)
				{
					//�Ƿ������
					if (*p == 0xcc ||
						*p == 0xc2)
					{
						break;
					}
					ulHookFunctionAddress = (*(PULONG)(p + 1) + (ULONG)p + 5);  //�õ�hook�ĵ�ַ
					if (!RMmIsAddressValid(ulHookFunctionAddress))
					{
						continue;
					}
					ulTemp = NULL;
					get_instruction(&Inst,p,MODE_32);
					switch (Inst.type)
					{
					case INSTRUCTION_TYPE_JMP:
						if(Inst.opcode==0xFF&&Inst.modrm==0x25)
						{
							//DIRECT_JMP
							ulTemp = Inst.op1.displacement;
						}
						else if (Inst.opcode==0xEB)
						{
							ulTemp = (ULONG)(p+Inst.op1.immediate);
						}
						else if(Inst.opcode==0xE9)
						{
							//RELATIVE_JMP;
							ulTemp = (ULONG)(p+Inst.op1.immediate);
						}
						break;
					case INSTRUCTION_TYPE_CALL:
						if(Inst.opcode==0xFF&&Inst.modrm==0x15)
						{
							//DIRECT_CALL
							ulTemp = Inst.op1.displacement;
						}
						else if (Inst.opcode==0x9A)
						{
							ulTemp = (ULONG)(p+Inst.op1.immediate);
						}
						else if(Inst.opcode==0xE8)
						{
							//RELATIVE_CALL;
							ulTemp = (ULONG)(p+Inst.op1.immediate);
						}
						break;
					case INSTRUCTION_TYPE_PUSH:
						if(!RMmIsAddressValid((PVOID)(p)))
						{
							break;
						}
						get_instruction(&Instb,(BYTE*)(p),MODE_32);
						if(Instb.type == INSTRUCTION_TYPE_RET)
						{
							//StartAddress+len-inst.length-instb.length;
							ulTemp = Instb.op1.displacement;
						}
						break;
					}
					if (ulTemp &&
						RMmIsAddressValid(ulTemp))
					{
						if (ulTemp > SystemKernelModuleBase &&
							ulTemp < SystemKernelModuleBase+SystemKernelModuleSize)   //̫������Ҳ����
						{
							continue;
						}
						//ulTempҲ����С�� SystemKernelModuleBase
						if (ulTemp < SystemKernelModuleBase)
						{
							continue;
						}
						if (*(ULONG *)ulTemp == 0x00000000 ||
							*(ULONG *)ulTemp == 0x00000005)
						{
							continue;
						}
						if (ulTemp > ulMyDriverBase &&
							ulTemp < ulMyDriverBase + ulMyDriverSize)
						{
							if (DebugOn)
								KdPrint(("safesystem hook, denied access��"));
							return;
						}
						ulRunAddress = (ULONG)p - (ULONG)JmpFunctionAddress;   //ִ�е���hook���ʱ��һ��ִ���˶��ٳ��ȵĴ���
						JmpReloadFunctionAddress = JmpReloadFunctionAddress + ulRunAddress;     //����ǰ��ִ�еĴ��룬��������ִ�� 

						if (DebugOn)
							KdPrint(("found hook---->%ws:%08x:%08x-%x-%08x",FunctionName,ulTemp + 0x5,p,ulRunAddress,JmpReloadFunctionAddress));


						//�õ���ȷ����ת��ַ��ֱ��hook�˼ҵ�hook������ͷ����Ȼ����������reload�����ulReloadAddress��ַ������ִ��ʣ�µĴ��룬�������ƹ�hook��
						ulTemp = ulTemp + 0x5;

						HookFunctionByHeaderAddress(
							(DWORD)JmpReloadFunctionAddress,
							ulTemp,
							(PVOID)HookFunctionProcessHookZone,
							&HookFunctionProcessPatchCodeLen,
							&HookFunctionProcessRet
							);
					}
				}
			}
		}

	}__except(EXCEPTION_EXECUTE_HANDLER){

	}
	return;
}