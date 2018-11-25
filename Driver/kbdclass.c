//**************************************************************************
//*
//*          A�ܵ��Է��� website:http://www.3600safe.com/
//*        
//*�ļ�˵����
//*         ö�ټ��̵ĸ�������
//**************************************************************************
#include "Kbdclass.h"

VOID SetKbdclassHook(ULONG ulNumber,ULONG ulRealDispatch)
{
	PDRIVER_OBJECT PKbdclassDriverObject = NULL;
	ULONG ulReloadDispatch;

	if (DebugOn)
		KdPrint(("%d:%08x\r\n",ulNumber,ulRealDispatch));

	if (RMmIsAddressValid(PKbdclassDriverObjectBakup))
	{
		PKbdclassDriverObject = PKbdclassDriverObjectBakup;
		PKbdclassDriverObject->MajorFunction[ulNumber] = ulRealDispatch;   //�ָ�hook

		if (GetWindowsVersion() == WINDOWS_VERSION_7 ||
			GetWindowsVersion() == WINDOWS_VERSION_XP)
		{
			ulReloadDispatch = ulRealDispatch - ulKbdclassModuleBase + ulReLoadKbdclassModuleBase;

			//�ָ� fsd inline hook
			if (GetFunctionCodeSize(ulRealDispatch) != GetFunctionCodeSize(ulReloadDispatch))
			{
				return;
			}
			if (memcmp(ulRealDispatch,ulReloadDispatch,GetFunctionCodeSize(ulRealDispatch)) == NULL)
			{
				return;
			}
			__asm
			{
				cli
					push eax
					mov eax,cr0
					and eax,not 0x10000
					mov cr0,eax
					pop eax
			}
			memcpy(ulRealDispatch,ulReloadDispatch,GetFunctionCodeSize(ulRealDispatch));
			__asm
			{
				push eax
					mov eax,cr0
					or eax,0x10000
					mov cr0,eax
					pop eax
					sti
			}
		}
	}
}
//�������ṹ������ring3
VOID FixFixKbdclass(PKBDCLASSDISPATCHBAKUP KbdclassDispatchBakUp,PDRIVER_OBJECT PKbdclassDriverObject,int i,ULONG ulReal_Dispatch,WCHAR *lpwzDispatchName,ULONG Dispatch)
{
	ULONG ulCurrentKbdclassDispatch;
	ULONG ulHookModuleBase;
	ULONG ulHookModuleSize;
	BOOL bIsHooked = FALSE;
	ULONG ulReloadKbdclassDispatch;

	KbdclassDispatchBakUp->KbdclassDispatch[i].ulKbdclassDispatch = ulReal_Dispatch;
	KbdclassDispatchBakUp->KbdclassDispatch[i].ulNumber = Dispatch;

	memset(KbdclassDispatchBakUp->KbdclassDispatch[i].lpwzKbdclassDispatchName,0,sizeof(KbdclassDispatchBakUp->KbdclassDispatch[0].lpwzKbdclassDispatchName));
	wcsncpy(KbdclassDispatchBakUp->KbdclassDispatch[i].lpwzKbdclassDispatchName,lpwzDispatchName,wcslen(lpwzDispatchName));

	ulCurrentKbdclassDispatch = PKbdclassDriverObject->MajorFunction[Dispatch];

	if (DebugOn)
		KdPrint(("ulCurrentKbdclassDispatch:%08x-%08x",ulCurrentKbdclassDispatch,PKbdclassDriverObject));

	if (ulCurrentKbdclassDispatch == ulReal_Dispatch)
	{
		bIsHooked = TRUE;

		WinVer = GetWindowsVersion();
		if (WinVer == WINDOWS_VERSION_2K3_SP1_SP2)
		{
			goto _FunRet;
		}
		ulReloadKbdclassDispatch = ulReal_Dispatch - ulKbdclassModuleBase + ulReLoadKbdclassModuleBase;

		//����Ƿ�inline hook
		if (GetFunctionCodeSize(ulReal_Dispatch) == GetFunctionCodeSize(ulReloadKbdclassDispatch) &&
			memcmp(ulReal_Dispatch,ulReloadKbdclassDispatch,GetFunctionCodeSize(ulReal_Dispatch)) != NULL)
		{
			KbdclassDispatchBakUp->KbdclassDispatch[i].Hooked = 2; // inline hook

			//�򵥵Ĵ���һ��ͷ5�ֽڵ�hook�ļ��
			ulCurrentKbdclassDispatch = *(PULONG)(ulReal_Dispatch+1)+(ULONG)(ulReal_Dispatch+5);
			//������ǿ�ͷjmp hook����д��ԭʼ��ַ
			if (!RMmIsAddressValid(ulCurrentKbdclassDispatch))
			{
				ulCurrentKbdclassDispatch = ulReal_Dispatch;
			}
		}
	}
_FunRet:
	if (!bIsHooked)
	{
		KbdclassDispatchBakUp->KbdclassDispatch[i].Hooked = 1;  // hook
	}
	memset(KbdclassDispatchBakUp->KbdclassDispatch[i].lpszBaseModule,0,sizeof(KbdclassDispatchBakUp->KbdclassDispatch[0].lpszBaseModule));

	if (!IsAddressInSystem(
		ulCurrentKbdclassDispatch,
		&ulHookModuleBase,
		&ulHookModuleSize,
		KbdclassDispatchBakUp->KbdclassDispatch[i].lpszBaseModule))
	{
		strcat(KbdclassDispatchBakUp->KbdclassDispatch[i].lpszBaseModule,"Unknown3");
	}
	KbdclassDispatchBakUp->KbdclassDispatch[i].ulCurrentKbdclassDispatch = ulCurrentKbdclassDispatch;
	KbdclassDispatchBakUp->KbdclassDispatch[i].ulModuleSize = ulHookModuleSize;
	KbdclassDispatchBakUp->KbdclassDispatch[i].ulModuleBase = ulHookModuleBase;

}
NTSTATUS ReLoadKbdclass(PDRIVER_OBJECT DriverObject,PKBDCLASSDISPATCHBAKUP KbdclassDispatchBakUp,int Type)
{
	PDRIVER_OBJECT PKbdclassDriverObject = NULL;
	PUCHAR i;
	WIN_VER_DETAIL WinVer;
	BOOL bInit = FALSE;
	PUCHAR KbdclassDriverEntry;
	PUCHAR ulJmpAddress,ulAddress;
	ULONG ulDispatch;
	ULONG ulDriverEntryToDispatchCodeOffset;
	ULONG ulOffset;
	BOOL bIsReLoadSuccess = FALSE;
	UNICODE_STRING UnicodeModule;
	HANDLE hSection;
	ULONG ulModuleBase;

	//��ȡdriverobject
	if (GetDriverObject(L"\\Driver\\Kbdclass",&PKbdclassDriverObject) == STATUS_SUCCESS)
	{
		PKbdclassDriverObjectBakup = PKbdclassDriverObject;

		ulKbdclassModuleBase = PKbdclassDriverObject->DriverStart;
		ulKbdclassModuleSize = PKbdclassDriverObject->DriverSize;

		WinVer = GetWindowsVersion();
		if (WinVer == WINDOWS_VERSION_2K3_SP1_SP2)
		{
			goto _FunMapFile;
		}
		//reload
		if (PeLoad(
			L"\\SystemRoot\\system32\\drivers\\Kbdclass.sys",
			&ulReLoadKbdclassModuleBase,
			DriverObject,
			ulKbdclassModuleBase
			))
		{
			bIsReLoadSuccess = TRUE;
		}
_FunMapFile:
		if (!bIsReLoadSuccess)
		{
			RtlInitUnicodeString(&UnicodeModule,L"\\SystemRoot\\system32\\drivers\\kbdclass.sys");
			hSection = MapFileAsSection(&UnicodeModule,&ulModuleBase);
			if (hSection)
			{
				ulReLoadKbdclassModuleBase = ulModuleBase;
				ZwClose(hSection);
			}
		}
		if (GetDriverEntryPoint(ulReLoadKbdclassModuleBase,&KbdclassDriverEntry))
		{
			/*
			IRP_MJ_CREATE		0xF875FDD0		-		0xF875FDD0		C:\WINDOWS\system32\DRIVERS\kbdclass.sys
			IRP_MJ_CLOSE		0xF875FFE0		-		0xF875FFE0		C:\WINDOWS\system32\DRIVERS\kbdclass.sys
			IRP_MJ_READ C:\WINDOWS\system32\DRIVERS\kbdclass.sys
			IRP_MJ_FLUSH_BUFFERS		0xF875FD4A		-		0xF875FD4A		C:\WINDOWS\system32\DRIVERS\kbdclass.sys
			IRP_MJ_DEVICE_CONTROL	
			IRP_MJ_INTERNAL_DEVICE_CONTROL		0xF8761386		-		0xF8761386		C:\WINDOWS\system32\DRIVERS\kbdclass.sys
			IRP_MJ_CLEANUP		0xF875FD06		-		0xF875FD06		C:\WINDOWS\system32\DRIVERS\kbdclass.sys
			IRP_MJ_POWER		0xF8762180		-		0xF8762180		C:\WINDOWS\system32\DRIVERS\kbdclass.sys
			IRP_MJ_SYSTEM_CONTROL		0xF8761842		-		0xF8761842		C:\WINDOWS\system32\DRIVERS\kbdclass.sys
			IRP_MJ_PNP_POWER		0xF876078A		-		0xF876078A		C:\WINDOWS\system32\DRIVERS\kbdclass.sys
			*/
			/*
			8105e610 8bff            mov     edi,edi
			8105e612 55              push    ebp
			8105e613 8bec            mov     ebp,esp
			8105e615 a12c4095f8      mov     eax,dword ptr ds:[F895402Ch]
			8105e61a 85c0            test    eax,eax
			8105e61c b940bb0000      mov     ecx,0BB40h
			8105e621 7404            je      8105e627
			8105e623 3bc1            cmp     eax,ecx
			8105e625 7523            jne     8105e64a
			8105e627 8b15ec3e95f8    mov     edx,dword ptr ds:[0F8953EECh]
			8105e62d b82c4095f8      mov     eax,0F895402Ch
			8105e632 c1e808          shr     eax,8
			8105e635 3302            xor     eax,dword ptr [edx]
			8105e637 25ffff0000      and     eax,0FFFFh
			8105e63c a32c4095f8      mov     dword ptr ds:[F895402Ch],eax
			8105e641 7507            jne     8105e64a
			8105e643 8bc1            mov     eax,ecx
			8105e645 a32c4095f8      mov     dword ptr ds:[F895402Ch],eax
			8105e64a f7d0            not     eax
			8105e64c a3284095f8      mov     dword ptr ds:[F8954028h],eax
			8105e651 5d              pop     ebp
			8105e652 e9d9f9ffff      jmp     8105e030          <-----��ȡ����ĵ�ַ ��driverentry(xp)
			*/
			for (i=(ULONG)KbdclassDriverEntry;i < (ULONG)KbdclassDriverEntry+0x1000;i++)
			{
				if (*i == 0xe9)
				{
					ulJmpAddress = *(PULONG)(i+1)+(ULONG)(i+5);
					if (MmIsAddressValid(ulJmpAddress))
					{
						if (DebugOn)
							KdPrint(("i:%08x,DriverEntry:%08x\n",i,ulJmpAddress));
						bInit = TRUE;
						break;
					}
				}
			}
			if (!bInit)
			{
				return;
			}
			WinVer = GetWindowsVersion();
			switch (WinVer)
			{
			case WINDOWS_VERSION_XP:
				ulDriverEntryToDispatchCodeOffset = 0x2c2;   //Ӳ�����ˣ�xp
				ulOffset = 0;
				break;
			case WINDOWS_VERSION_7:
				ulDriverEntryToDispatchCodeOffset = 0x2BE;   //Ӳ�����ˣ�win7
				ulOffset = 0;
				break;
			case WINDOWS_VERSION_2K3_SP1_SP2:
				ulDriverEntryToDispatchCodeOffset = 0x2c2;   //Ӳ�����ˣ�2003
				ulOffset = (ULONG)PKbdclassDriverObject->DriverStart - 0x10000;
				break;
			}
			ulDispatch = ulJmpAddress + ulDriverEntryToDispatchCodeOffset;
			if (DebugOn)
				KdPrint(("ulAddress:%08x\r\n",ulDispatch));

			ulReal_KBDCLASS_IRP_MJ_CREATE = *(PULONG)(ulDispatch+3) + ulOffset;
			ulReal_KBDCLASS_IRP_MJ_CLOSE = *(PULONG)(ulDispatch+0xA) + ulOffset;
			ulReal_KBDCLASS_IRP_MJ_READ = *(PULONG)(ulDispatch+0x11) + ulOffset;
			ulReal_KBDCLASS_IRP_MJ_FLUSH_BUFFERS = *(PULONG)(ulDispatch+0x18) + ulOffset;
			ulReal_KBDCLASS_IRP_MJ_DEVICE_CONTROL = *(PULONG)(ulDispatch+0x1F) + ulOffset;
			ulReal_KBDCLASS_IRP_MJ_INTERNAL_DEVICE_CONTROL = *(PULONG)(ulDispatch+0x26) + ulOffset;
			ulReal_KBDCLASS_IRP_MJ_CLEANUP = *(PULONG)(ulDispatch+0x30) + ulOffset;
			ulReal_KBDCLASS_IRP_MJ_PNP_POWER = *(PULONG)(ulDispatch+0x3A) + ulOffset;
			ulReal_KBDCLASS_IRP_MJ_POWER = *(PULONG)(ulDispatch+0x44) + ulOffset;
			ulReal_KBDCLASS_IRP_MJ_SYSTEM_CONTROL = *(PULONG)(ulDispatch+0x4E) + ulOffset;

			if (DebugOn)
				KdPrint(("%08x\r\n"
				"%08x\r\n"
				"%08x\r\n"
				"%08x\r\n"
				"%08x\r\n"
				"%08x\r\n"
				"%08x\r\n"
				"%08x\r\n"
				"%08x\r\n"
				"%08x\r\n",
				ulReal_KBDCLASS_IRP_MJ_CREATE,
				ulReal_KBDCLASS_IRP_MJ_CLOSE,
				ulReal_KBDCLASS_IRP_MJ_READ,
				ulReal_KBDCLASS_IRP_MJ_FLUSH_BUFFERS,
				ulReal_KBDCLASS_IRP_MJ_DEVICE_CONTROL,
				ulReal_KBDCLASS_IRP_MJ_INTERNAL_DEVICE_CONTROL,
				ulReal_KBDCLASS_IRP_MJ_CLEANUP,
				ulReal_KBDCLASS_IRP_MJ_POWER,
				ulReal_KBDCLASS_IRP_MJ_SYSTEM_CONTROL,
				ulReal_KBDCLASS_IRP_MJ_PNP_POWER
				));
			if (Type == 1)
			{
				//���ṹ
				FixFixKbdclass(KbdclassDispatchBakUp,PKbdclassDriverObjectBakup,0,ulReal_KBDCLASS_IRP_MJ_CREATE,L"IRP_MJ_CREATE",IRP_MJ_CREATE);
				FixFixKbdclass(KbdclassDispatchBakUp,PKbdclassDriverObjectBakup,1,ulReal_KBDCLASS_IRP_MJ_CLOSE,L"IRP_MJ_CLOSE",IRP_MJ_CLOSE);
				FixFixKbdclass(KbdclassDispatchBakUp,PKbdclassDriverObjectBakup,2,ulReal_KBDCLASS_IRP_MJ_READ,L"IRP_MJ_READ",IRP_MJ_READ);
				FixFixKbdclass(KbdclassDispatchBakUp,PKbdclassDriverObjectBakup,3,ulReal_KBDCLASS_IRP_MJ_FLUSH_BUFFERS,L"IRP_MJ_FLUSH_BUFFERS",IRP_MJ_FLUSH_BUFFERS);
				FixFixKbdclass(KbdclassDispatchBakUp,PKbdclassDriverObjectBakup,4,ulReal_KBDCLASS_IRP_MJ_DEVICE_CONTROL,L"IRP_MJ_DEVICE_CONTROL",IRP_MJ_DEVICE_CONTROL);
				FixFixKbdclass(KbdclassDispatchBakUp,PKbdclassDriverObjectBakup,5,ulReal_KBDCLASS_IRP_MJ_INTERNAL_DEVICE_CONTROL,L"IRP_MJ_INTERNAL_DEVICE_CONTROL",IRP_MJ_INTERNAL_DEVICE_CONTROL);
				FixFixKbdclass(KbdclassDispatchBakUp,PKbdclassDriverObjectBakup,6,ulReal_KBDCLASS_IRP_MJ_CLEANUP,L"IRP_MJ_CLEANUP",IRP_MJ_CLEANUP);
				FixFixKbdclass(KbdclassDispatchBakUp,PKbdclassDriverObjectBakup,7,ulReal_KBDCLASS_IRP_MJ_POWER,L"IRP_MJ_POWER",IRP_MJ_POWER);
				FixFixKbdclass(KbdclassDispatchBakUp,PKbdclassDriverObjectBakup,8,ulReal_KBDCLASS_IRP_MJ_SYSTEM_CONTROL,L"IRP_MJ_SYSTEM_CONTROL",IRP_MJ_SYSTEM_CONTROL);
				FixFixKbdclass(KbdclassDispatchBakUp,PKbdclassDriverObjectBakup,9,ulReal_KBDCLASS_IRP_MJ_PNP_POWER,L"IRP_MJ_PNP_POWER",IRP_MJ_PNP_POWER);
				KbdclassDispatchBakUp->ulCount = 10;
				return STATUS_SUCCESS;
			}
			//���е��ö���reload
			switch (WinVer)
			{
			case WINDOWS_VERSION_XP:
			case WINDOWS_VERSION_7:
				PKbdclassDriverObjectBakup->MajorFunction[IRP_MJ_CREATE] = ulReal_KBDCLASS_IRP_MJ_CREATE - ulKbdclassModuleBase + ulReLoadKbdclassModuleBase;
				PKbdclassDriverObjectBakup->MajorFunction[IRP_MJ_CLOSE] = ulReal_KBDCLASS_IRP_MJ_CLOSE - ulKbdclassModuleBase + ulReLoadKbdclassModuleBase;
				PKbdclassDriverObjectBakup->MajorFunction[IRP_MJ_READ] = ulReal_KBDCLASS_IRP_MJ_READ - ulKbdclassModuleBase + ulReLoadKbdclassModuleBase;
				PKbdclassDriverObjectBakup->MajorFunction[IRP_MJ_FLUSH_BUFFERS] = ulReal_KBDCLASS_IRP_MJ_FLUSH_BUFFERS - ulKbdclassModuleBase + ulReLoadKbdclassModuleBase;
				PKbdclassDriverObjectBakup->MajorFunction[IRP_MJ_DEVICE_CONTROL] = ulReal_KBDCLASS_IRP_MJ_DEVICE_CONTROL - ulKbdclassModuleBase + ulReLoadKbdclassModuleBase;
				PKbdclassDriverObjectBakup->MajorFunction[IRP_MJ_INTERNAL_DEVICE_CONTROL] = ulReal_KBDCLASS_IRP_MJ_INTERNAL_DEVICE_CONTROL - ulKbdclassModuleBase + ulReLoadKbdclassModuleBase;
				PKbdclassDriverObjectBakup->MajorFunction[IRP_MJ_CLEANUP] = ulReal_KBDCLASS_IRP_MJ_CLEANUP - ulKbdclassModuleBase + ulReLoadKbdclassModuleBase;
				PKbdclassDriverObjectBakup->MajorFunction[IRP_MJ_POWER] = ulReal_KBDCLASS_IRP_MJ_POWER - ulKbdclassModuleBase + ulReLoadKbdclassModuleBase;
				PKbdclassDriverObjectBakup->MajorFunction[IRP_MJ_SYSTEM_CONTROL] = ulReal_KBDCLASS_IRP_MJ_SYSTEM_CONTROL - ulKbdclassModuleBase + ulReLoadKbdclassModuleBase;
				PKbdclassDriverObjectBakup->MajorFunction[IRP_MJ_PNP_POWER] = ulReal_KBDCLASS_IRP_MJ_PNP_POWER - ulKbdclassModuleBase + ulReLoadKbdclassModuleBase;
				break;
			case WINDOWS_VERSION_2K3_SP1_SP2:
				PKbdclassDriverObjectBakup->MajorFunction[IRP_MJ_CREATE] = ulReal_KBDCLASS_IRP_MJ_CREATE;
				PKbdclassDriverObjectBakup->MajorFunction[IRP_MJ_CLOSE] = ulReal_KBDCLASS_IRP_MJ_CLOSE;
				PKbdclassDriverObjectBakup->MajorFunction[IRP_MJ_READ] = ulReal_KBDCLASS_IRP_MJ_READ;
				PKbdclassDriverObjectBakup->MajorFunction[IRP_MJ_FLUSH_BUFFERS] = ulReal_KBDCLASS_IRP_MJ_FLUSH_BUFFERS;
				PKbdclassDriverObjectBakup->MajorFunction[IRP_MJ_DEVICE_CONTROL] = ulReal_KBDCLASS_IRP_MJ_DEVICE_CONTROL;
				PKbdclassDriverObjectBakup->MajorFunction[IRP_MJ_INTERNAL_DEVICE_CONTROL] = ulReal_KBDCLASS_IRP_MJ_INTERNAL_DEVICE_CONTROL;
				PKbdclassDriverObjectBakup->MajorFunction[IRP_MJ_CLEANUP] = ulReal_KBDCLASS_IRP_MJ_CLEANUP;
				PKbdclassDriverObjectBakup->MajorFunction[IRP_MJ_POWER] = ulReal_KBDCLASS_IRP_MJ_POWER;
				PKbdclassDriverObjectBakup->MajorFunction[IRP_MJ_SYSTEM_CONTROL] = ulReal_KBDCLASS_IRP_MJ_SYSTEM_CONTROL;
				PKbdclassDriverObjectBakup->MajorFunction[IRP_MJ_PNP_POWER] = ulReal_KBDCLASS_IRP_MJ_PNP_POWER;
				break;
			}
		}
	}
	return STATUS_SUCCESS;
}
NTSTATUS ReLoadKbdclassFree()
{
	WIN_VER_DETAIL WinVer;

	WinVer = GetWindowsVersion();
	if (WinVer == WINDOWS_VERSION_2K3_SP1_SP2)
		return STATUS_UNSUCCESSFUL;

	if (RMmIsAddressValid(PKbdclassDriverObjectBakup))
	{
		if (ulReal_KBDCLASS_IRP_MJ_CREATE &&
			ulReal_KBDCLASS_IRP_MJ_CLOSE &&
			ulReal_KBDCLASS_IRP_MJ_READ &&
			ulReal_KBDCLASS_IRP_MJ_FLUSH_BUFFERS &&
			ulReal_KBDCLASS_IRP_MJ_DEVICE_CONTROL &&
			ulReal_KBDCLASS_IRP_MJ_INTERNAL_DEVICE_CONTROL &&
			ulReal_KBDCLASS_IRP_MJ_CLEANUP &&
			ulReal_KBDCLASS_IRP_MJ_POWER &&
			ulReal_KBDCLASS_IRP_MJ_SYSTEM_CONTROL &&
			ulReal_KBDCLASS_IRP_MJ_PNP_POWER)
		{
			PKbdclassDriverObjectBakup->MajorFunction[IRP_MJ_CREATE] = ulReal_KBDCLASS_IRP_MJ_CREATE;
			PKbdclassDriverObjectBakup->MajorFunction[IRP_MJ_CLOSE] = ulReal_KBDCLASS_IRP_MJ_CLOSE;
			PKbdclassDriverObjectBakup->MajorFunction[IRP_MJ_READ] = ulReal_KBDCLASS_IRP_MJ_READ;
			PKbdclassDriverObjectBakup->MajorFunction[IRP_MJ_FLUSH_BUFFERS] = ulReal_KBDCLASS_IRP_MJ_FLUSH_BUFFERS;
			PKbdclassDriverObjectBakup->MajorFunction[IRP_MJ_DEVICE_CONTROL] = ulReal_KBDCLASS_IRP_MJ_DEVICE_CONTROL;
			PKbdclassDriverObjectBakup->MajorFunction[IRP_MJ_INTERNAL_DEVICE_CONTROL] = ulReal_KBDCLASS_IRP_MJ_INTERNAL_DEVICE_CONTROL;
			PKbdclassDriverObjectBakup->MajorFunction[IRP_MJ_CLEANUP] = ulReal_KBDCLASS_IRP_MJ_CLEANUP;
			PKbdclassDriverObjectBakup->MajorFunction[IRP_MJ_POWER] = ulReal_KBDCLASS_IRP_MJ_POWER;
			PKbdclassDriverObjectBakup->MajorFunction[IRP_MJ_SYSTEM_CONTROL] = ulReal_KBDCLASS_IRP_MJ_SYSTEM_CONTROL;
			PKbdclassDriverObjectBakup->MajorFunction[IRP_MJ_PNP_POWER] = ulReal_KBDCLASS_IRP_MJ_PNP_POWER;
		}
	}
	return STATUS_SUCCESS;
}