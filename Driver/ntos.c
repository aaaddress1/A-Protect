//**************************************************************************
//*
//*          A�ܵ��Է��� website:http://www.3600safe.com/
//*        
//*�ļ�˵����
//*         Hook KiFastCallEntryģ��/������ؽ���ģ��
//**************************************************************************
#include "ntos.h"

/*
BOOL GetOriginalKiServiceTable(BYTE *ModuleBase,DWORD ExistImageBase,DWORD *OriginalKiServiceTable)
{
	PIMAGE_DOS_HEADER ImageDosHeader;
	PIMAGE_NT_HEADERS ImageNtHeaders;
	DWORD KeServiceDescriptorTableRva;
	PIMAGE_BASE_RELOCATION ImageBaseReloc=NULL;
	DWORD RelocSize;
	int ItemCount,Index;
	int Type;
	PDWORD RelocAddress;
	DWORD RvaData;
	DWORD count=0;
	WORD *TypeOffset;


// 	ImageDosHeader=(PIMAGE_DOS_HEADER)ModuleBase;
// 	if (ImageDosHeader->e_magic!=IMAGE_DOS_SIGNATURE)
// 	{
// 		return FALSE;
// 	}
// 	ImageNtHeaders=(PIMAGE_NT_HEADERS)(ModuleBase+ImageDosHeader->e_lfanew);
// 	if (ImageNtHeaders->Signature!=IMAGE_NT_SIGNATURE)
// 	{
// 		return FALSE;
// 	}
	ImageNtHeaders = RtlImageNtHeader(ModuleBase);
	if (!ImageNtHeaders)
		return FALSE;

	KeServiceDescriptorTableRva=(DWORD)MiFindExportedRoutine(ModuleBase,TRUE,"KeServiceDescriptorTable",0);
	if (!KeServiceDescriptorTableRva)
		return FALSE;

	KeServiceDescriptorTableRva = KeServiceDescriptorTableRva-(DWORD)ModuleBase;

	if (DebugOn)
		KdPrint(("KeServiceDescriptorTable:%X\n",KeServiceDescriptorTableRva));

	ImageBaseReloc=RtlImageDirectoryEntryToData(ModuleBase,TRUE,IMAGE_DIRECTORY_ENTRY_BASERELOC,&RelocSize);
	if (!ImageBaseReloc)
		return FALSE;
	
	if (DebugOn)
		KdPrint(("get x IMAGE_DIRECTORY_ENTRY_BASERELOC ok\n"));

	while (ImageBaseReloc->SizeOfBlock)
	{  
		count++;
		ItemCount=(ImageBaseReloc->SizeOfBlock-sizeof(IMAGE_BASE_RELOCATION))/2;
		for (Index=0;Index<ItemCount;Index++)
		{
			TypeOffset = (WORD*)((DWORD)ImageBaseReloc + sizeof(IMAGE_BASE_RELOCATION));
			Type = TypeOffset[Index]>>12;
			if (Type == IMAGE_REL_BASED_HIGHLOW)
			{
				if (MmIsAddressValid(((DWORD)(TypeOffset[Index]&0x0fff)+ImageBaseReloc->VirtualAddress+(DWORD)ModuleBase)))
				{
					RelocAddress = (PDWORD)((DWORD)(TypeOffset[Index]&0x0fff)+ImageBaseReloc->VirtualAddress+(DWORD)ModuleBase);

					if (MmIsAddressValid(RelocAddress))
					{
						RvaData = *RelocAddress-ExistImageBase;

						if (RvaData==KeServiceDescriptorTableRva)
						{
							if(*(USHORT*)((DWORD)RelocAddress-2)==0x05c7)
							{

								*OriginalKiServiceTable=*(DWORD*)((DWORD)RelocAddress+4)-ExistImageBase+(DWORD)ModuleBase;
								if (DebugOn)
									KdPrint(("Find OriginalImage ssdt :%X\n",*OriginalKiServiceTable));
								return TRUE;
							}
						}
					}
				}
			}

		}
		ImageBaseReloc=(PIMAGE_BASE_RELOCATION)((DWORD)ImageBaseReloc+ImageBaseReloc->SizeOfBlock);
	}
	if (DebugOn)
		KdPrint(("count:%d\n",count));
	return FALSE;
}*/
BOOL GetOriginalKiServiceTable(BYTE *NewImageBase,DWORD ExistImageBase,DWORD *NewKiServiceTable)
{
	PIMAGE_DOS_HEADER ImageDosHeader;
	PIMAGE_NT_HEADERS ImageNtHeaders;
	DWORD KeServiceDescriptorTableRva;
	PIMAGE_BASE_RELOCATION ImageBaseReloc=NULL;
	DWORD RelocSize;
	int ItemCount,Index;
	int Type;
	PDWORD RelocAddress;
	DWORD RvaData;
	DWORD count=0;
	WORD *TypeOffset;


	ImageDosHeader=(PIMAGE_DOS_HEADER)NewImageBase;
	if (ImageDosHeader->e_magic!=IMAGE_DOS_SIGNATURE)
	{
		return FALSE;
	}
	ImageNtHeaders=(PIMAGE_NT_HEADERS)(NewImageBase+ImageDosHeader->e_lfanew);
	if (ImageNtHeaders->Signature!=IMAGE_NT_SIGNATURE)
	{
		return FALSE;
	}
	KeServiceDescriptorTableRva=(DWORD)MiFindExportedRoutine(NewImageBase,TRUE,"KeServiceDescriptorTable",0);
	if (KeServiceDescriptorTableRva==0)
	{
		return FALSE;
	}

	KeServiceDescriptorTableRva=KeServiceDescriptorTableRva-(DWORD)NewImageBase;
	if (DebugOn)
		KdPrint(("KeServiceDescriptorTable:%X\n",KeServiceDescriptorTableRva));
	ImageBaseReloc=RtlImageDirectoryEntryToData(NewImageBase,TRUE,IMAGE_DIRECTORY_ENTRY_BASERELOC,&RelocSize);
	if (ImageBaseReloc==NULL)
	{
		return FALSE;
	}
	if (DebugOn)
		KdPrint(("get x IMAGE_DIRECTORY_ENTRY_BASERELOC ok\n"));
	while (ImageBaseReloc->SizeOfBlock)
	{  
		count++;
		ItemCount=(ImageBaseReloc->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION))/2;
		TypeOffset=(WORD*)((DWORD)ImageBaseReloc+sizeof(IMAGE_BASE_RELOCATION));
		for (Index=0;Index<ItemCount;Index++)
		{
			Type=TypeOffset[Index]>>12;
			if (Type==3)
			{
				RelocAddress=(PDWORD)((DWORD)(TypeOffset[Index]&0x0fff)+ImageBaseReloc->VirtualAddress+(DWORD)NewImageBase);
				RvaData=*RelocAddress-ExistImageBase;

				if (RvaData==KeServiceDescriptorTableRva)
				{
					if(*(USHORT*)((DWORD)RelocAddress-2)==0x05c7)
					{

						*NewKiServiceTable=*(DWORD*)((DWORD)RelocAddress+4)-ExistImageBase+(DWORD)NewImageBase;
						if (DebugOn)
							KdPrint(("Find OriginalImage ssdt :%X\n",*NewKiServiceTable));
						return TRUE;
					}
				}

			}

		}
		ImageBaseReloc=(PIMAGE_BASE_RELOCATION)((DWORD)ImageBaseReloc+ImageBaseReloc->SizeOfBlock);
	}
	if (DebugOn)
		KdPrint(("count:%d\n",count));

	return FALSE;
}
VOID FixOriginalKiServiceTable(PDWORD OriginalKiServiceTable,DWORD ModuleBase,DWORD ExistImageBase)
{
	DWORD FuctionCount;
	DWORD Index;
	FuctionCount=KeServiceDescriptorTable->TableSize;
	if (DebugOn)
		KdPrint(("ssdt funcion count:%X---KiServiceTable:%X\n",FuctionCount,KeServiceDescriptorTable->ServiceTable));	
	for (Index=0;Index<FuctionCount;Index++)
	{
		OriginalKiServiceTable[Index]=OriginalKiServiceTable[Index]-ExistImageBase+ModuleBase;
	}
}
BOOL InitSafeOperationModule(PDRIVER_OBJECT pDriverObject,WCHAR *SystemModulePath,ULONG KernelModuleBase)
{
	UNICODE_STRING FileName;
	HANDLE hSection;
	PDWORD FixdOriginalKiServiceTable;
	PDWORD CsRootkitOriginalKiServiceTable;
	int i=0;

	if (DebugOn)
		KdPrint(("Safe->Get System Kernel Module Info %ws:%08x\r\n",SystemModulePath,KernelModuleBase));

	if (DebugOn)
		KdPrint(("Safe->DriverObject:%08x\r\n",pDriverObject));

	//�Լ�peload һ��ntos*�������ͽ���˸�������ȫ����ĳ�ͻ��~

	if (!PeLoad(SystemModulePath,&ImageModuleBase,pDriverObject,KernelModuleBase))
	{
		if (DebugOn)
			KdPrint(("Safe->PeLoad failed\n"));
		return FALSE;
	}

	if (DebugOn)
		KdPrint(("Safe->ModuleBase:%08x\r\n",ImageModuleBase));

	OriginalKiServiceTable = ExAllocatePool(NonPagedPool,KeServiceDescriptorTable->TableSize*sizeof(DWORD));
	if (!OriginalKiServiceTable)
	{
		if (DebugOn)
			KdPrint(("OriginalKiServiceTable Failed\n"));
		return FALSE;
	}
	if(!GetOriginalKiServiceTable(ImageModuleBase,KernelModuleBase,&OriginalKiServiceTable))
	{
		if (DebugOn)
			KdPrint(("Safe->Get Original KiServiceTable Failed\n"));

		ExFreePool(OriginalKiServiceTable);

		return FALSE;
	}
	if (DebugOn)
		KdPrint(("Safe->OriginalKiServiceTable %X\n",OriginalKiServiceTable));

	//���ÿһ��ssdt��Ӧ������ַ~����ĵ�ַ��reload��
    FixOriginalKiServiceTable((PDWORD)OriginalKiServiceTable,(DWORD)ImageModuleBase,KernelModuleBase);

	OriginalServiceDescriptorTable=ExAllocatePool(NonPagedPool,sizeof(SERVICE_DESCRIPTOR_TABLE)*4);
	if (OriginalServiceDescriptorTable == NULL)
	{
		ExFreePool(OriginalKiServiceTable);
		return FALSE;
	}
	RtlZeroMemory(OriginalServiceDescriptorTable,sizeof(SERVICE_DESCRIPTOR_TABLE)*4);
	//����һ���ɾ���ԭʼ��ÿ����������Ӧ��SSDT�����ĵ�ַ������Ч��~
	OriginalServiceDescriptorTable->ServiceTable = (PDWORD)OriginalKiServiceTable;
	OriginalServiceDescriptorTable->CounterTable = KeServiceDescriptorTable->CounterTable;
	OriginalServiceDescriptorTable->TableSize    = KeServiceDescriptorTable->TableSize;
	OriginalServiceDescriptorTable->ArgumentTable = KeServiceDescriptorTable->ArgumentTable;

	CsRootkitOriginalKiServiceTable = ExAllocatePool(NonPagedPool,KeServiceDescriptorTable->TableSize*sizeof(DWORD));
	if (CsRootkitOriginalKiServiceTable==NULL)
	{
		ExFreePool(OriginalServiceDescriptorTable);
		ExFreePool(OriginalKiServiceTable);
		return FALSE;

	}
	RtlZeroMemory(CsRootkitOriginalKiServiceTable,KeServiceDescriptorTable->TableSize*sizeof(DWORD));

	Safe_ServiceDescriptorTable = ExAllocatePool(NonPagedPool,sizeof(SERVICE_DESCRIPTOR_TABLE)*4);
	if (Safe_ServiceDescriptorTable == NULL)
	{
		ExFreePool(OriginalServiceDescriptorTable);
		ExFreePool(CsRootkitOriginalKiServiceTable);
		ExFreePool(OriginalKiServiceTable);
		return FALSE;
	}
	//����һ���ɾ���ԭʼ��ÿ����������Ӧ��SSDT�����ĵ�ַ����ԭʼ����
	RtlZeroMemory(Safe_ServiceDescriptorTable,sizeof(SERVICE_DESCRIPTOR_TABLE)*4);
	
	//���ԭʼ������ַ
	for (i=0;i<KeServiceDescriptorTable->TableSize;i++)
	{
		CsRootkitOriginalKiServiceTable[i] = OriginalServiceDescriptorTable->ServiceTable[i];
	}
	Safe_ServiceDescriptorTable->ServiceTable = (PDWORD)CsRootkitOriginalKiServiceTable;
	Safe_ServiceDescriptorTable->CounterTable = KeServiceDescriptorTable->CounterTable;
	Safe_ServiceDescriptorTable->TableSize = KeServiceDescriptorTable->TableSize;
	Safe_ServiceDescriptorTable->ArgumentTable = KeServiceDescriptorTable->ArgumentTable;

	//�ͷžͻ�bsod
	//ExFreePool(OriginalKiServiceTable);
	return TRUE;
}
///////////////////////////////////////////////////
__declspec(naked) VOID KiFastCallEntryHookZone()
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
		jmp [KiFastCallEntryRet];

	}
}
///////////////////////////////////////////////////
__declspec(naked) VOID KiFastCallEntryTempHookZone()
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
		jmp [KiFastCallEntryTempRet];

	}
}

PSERVICE_DESCRIPTOR_TABLE __stdcall FakeServiceDescriptorTable(PSERVICE_DESCRIPTOR_TABLE ServiceDescriptorTable,DWORD Index)
{
	ULONG ulEProcess;
	ULONG ulRetOK;
	int i=0;

	if (!RMmIsAddressValid ||
		!RPsGetCurrentProcess)
	{
		goto _FunRet;
	}
	//---------------------------------------------------
	//sreach hide process demo
	//---------------------------------------------------
	if (bIsInitSuccess)
	{
		ulEProcess = NULL;
		ulEProcess = RPsGetCurrentProcess();
		if (RMmIsAddressValid(ulEProcess))
		{
			ulRetOK = IsHideProcess(ulEProcess,HideProcessInfo);
			if (ulRetOK == 8)
			{
				//�������ؽ��̣�����
			}
		}
	}
	//--------------------------------------------------------
	//SSDT
	//-------------------------------------------------------
	if (ServiceDescriptorTable->ServiceTable == KeServiceDescriptorTable->ServiceTable)
	{
		if (RMmIsAddressValid(Safe_ServiceDescriptorTable))
		{
			if (Safe_ServiceDescriptorTable->TableSize > Index &&
				Safe_ServiceDescriptorTable->TableSize == ServiceDescriptorTable->TableSize)
			{
				if (RMmIsAddressValid(Safe_ServiceDescriptorTable->ServiceTable[Index]) &&
					(ULONG)Safe_ServiceDescriptorTable->ServiceTable[Index] >= ulMyDriverBase &&
					(ULONG)Safe_ServiceDescriptorTable->ServiceTable[Index] <= ulMyDriverBase + ulMyDriverSize)
				{
					//��ǰ����ŵĺ�����hook�������й��˺����ı�~
					return Safe_ServiceDescriptorTable;
				}
			}
		}
		//����ú���û�б�hook���ͷ��ظɾ��ı�����ĵ�ַ����Ч����û�б��������hook����Ŷ���������������hook�Ͳ�������~����Ϊ��������������Խ���xuetr
		//����Ҳ���Է���ServiceDescriptorTable���������������hook�ʹ�����
		//�������ַ���ֵ�����վ���������� -_-!!
		//�������ں˰�ȫģʽ������һ������һ��ԭʼ�ɾ���SSDT��~~�����κ�hook��

		//��3600safe���е�ʱ�򣬲ſ����ں˰�ȫģʽ
		if (bIsInitSuccess &&
			bKernelSafeModule &&
			RMmIsAddressValid(OriginalServiceDescriptorTable) &&
			OriginalServiceDescriptorTable->TableSize == ServiceDescriptorTable->TableSize)
		{
			return OriginalServiceDescriptorTable;
		}

	}
	if (bInitWin32K &&
		RMmIsAddressValid(ShadowTable))
	{
		if (ServiceDescriptorTable->ServiceTable == ShadowTable)    //ShadowSSDT
		{
			//KdPrint(("ShadowSSDT:%08x",ServiceDescriptorTable->ServiceTable[Index]));
			if (RMmIsAddressValid(Safe_ServiceDescriptorShadowSSDTTable))
			{
				if (Safe_ServiceDescriptorShadowSSDTTable->TableSize > Index &&
					Safe_ServiceDescriptorShadowSSDTTable->TableSize == ServiceDescriptorTable->TableSize)
				{
					if (RMmIsAddressValid(Safe_ServiceDescriptorShadowSSDTTable->ServiceTable[Index]) &&
						Safe_ServiceDescriptorShadowSSDTTable->ServiceTable[Index] >= ulMyDriverBase &&
						Safe_ServiceDescriptorShadowSSDTTable->ServiceTable[Index] <= ulMyDriverBase + ulMyDriverSize)
					{
						//��ǰ����ŵĺ�����hook�������й��˺����ı�~
						return Safe_ServiceDescriptorShadowSSDTTable;
					}
				}
			}
			if (bIsInitSuccess &&
				bKernelSafeModule &&
				RMmIsAddressValid(OriginalShadowServiceDescriptorTable) &&
				OriginalShadowServiceDescriptorTable->TableSize == ServiceDescriptorTable->TableSize)
			{
				return OriginalShadowServiceDescriptorTable;
			}
		}
	}

_FunRet:
	//����ssdt������ԭʼ~
	return ServiceDescriptorTable;
}
__declspec(naked) VOID KiFastCallEntryHookProc()
{
	_asm
	{
		push eax;
		push ecx;
		push edx;

		push eax;
		push edi;
		call FakeServiceDescriptorTable;
		mov edi,eax;

		pop edx;
		pop ecx;
		pop eax;
		jmp [KiFastCallEntryHookZone];
	}
}
/*
    win xp:
	8053e632 8bf2            mov     esi,edx
	8053e634 8b5f0c          mov     ebx,dword ptr [edi+0Ch]
	8053e637 33c9            xor     ecx,ecx
	8053e639 8a0c18          mov     cl,byte ptr [eax+ebx]
	8053e63c 8b3f            mov     edi,dword ptr [edi]
	8053e63e 8b1c87          mov     ebx,dword ptr [edi+eax*4]

	CodeInfo->LineCount=2;
	CodeInfo->CodeLine[0].CodeLength=2;
	CodeInfo->CodeLine[0].Code[0]=0x33;
	CodeInfo->CodeLine[0].Code[1]=0xC9;

	CodeInfo->CodeLine[1].CodeLength=3;
	CodeInfo->CodeLine[1].Code[0]=0x8A;
	CodeInfo->CodeLine[1].Code[1]=0x0C;
	CodeInfo->CodeLine[1].Code[2]=0x18;


	win 7:
	83c593ce 64ff05b0060000  inc     dword ptr fs:[6B0h]
	83c593d5 8bf2            mov     esi,edx
	83c593d7 33c9            xor     ecx,ecx
	83c593d9 8b570c          mov     edx,dword ptr [edi+0Ch]
	83c593dc 8b3f            mov     edi,dword ptr [edi]
	83c593de 8a0c10          mov     cl,byte ptr [eax+edx]
	83c593e1 8b1487          mov     edx,dword ptr [edi+eax*4]
	83c593e4 2be1            sub     esp,ecx
	83c593e6 c1e902          shr     ecx,2

	CodeInfo->LineCount=2;
	CodeInfo->CodeLine[0].CodeLength=2;
	CodeInfo->CodeLine[0].Code[0]=0x33;
	CodeInfo->CodeLine[0].Code[1]=0xC9;

	CodeInfo->CodeLine[1].CodeLength=3;
	CodeInfo->CodeLine[1].Code[0]=0x8B;
	CodeInfo->CodeLine[1].Code[1]=0x57;
	CodeInfo->CodeLine[1].Code[2]=0x0C;
*/
BOOL HookKiFastCallEntry()
{
	DWORD KiFastCallEntry;
	DWORD dwReloadKiFastCallEntry;
	int CodeInfoLength;
	PCODE_INFO CodeInfo;
	int PatchCodeLength;
	WIN_VER_DETAIL WinVer;
	BOOL bRetOK = FALSE;

	_asm
	{
		pushad;
		mov ecx, 0x176;
		rdmsr;
		mov KiFastCallEntry, eax;
		popad;
	}
	if (DebugOn)
		KdPrint(("Safe->KiFastCallEntry:0x%08X\n",KiFastCallEntry));

	CodeInfoLength=sizeof(CODE_INFO)+sizeof(CODE_LINE);
	CodeInfo=ExAllocatePool(NonPagedPool,CodeInfoLength);
	if (CodeInfo==NULL)
	{
		return bRetOK;
	}
	RtlZeroMemory(CodeInfo,CodeInfoLength);

	WinVer = GetWindowsVersion();
	switch(WinVer)
	{
	case WINDOWS_VERSION_XP:
	case WINDOWS_VERSION_2K3_SP1_SP2:
		CodeInfo->LineCount=1;
		CodeInfo->CodeLine[0].CodeLength=2;
		CodeInfo->CodeLine[0].Code[0]=0x33;
		CodeInfo->CodeLine[0].Code[1]=0xC9;

		CodeInfo->CodeLine[1].CodeLength=3;
		CodeInfo->CodeLine[1].Code[0]=0x8A;
		CodeInfo->CodeLine[1].Code[1]=0x0C;
		CodeInfo->CodeLine[1].Code[2]=0x18;
		break;
	case WINDOWS_VERSION_7:
		CodeInfo->LineCount=1;
		CodeInfo->CodeLine[0].CodeLength=2;
		CodeInfo->CodeLine[0].Code[0]=0x33;
		CodeInfo->CodeLine[0].Code[1]=0xC9;

		CodeInfo->CodeLine[1].CodeLength=3;
		CodeInfo->CodeLine[1].Code[0]=0x8B;
		CodeInfo->CodeLine[1].Code[1]=0x57;
		CodeInfo->CodeLine[1].Code[2]=0x0C;
		break;
	}
	//��hook KiFastCallEntryͷ������ת�� dwReloadKiFastCallEntry
	dwReloadKiFastCallEntry = KiFastCallEntry - SystemKernelModuleBase + ImageModuleBase;
	if (!RMmIsAddressValid(dwReloadKiFastCallEntry))
	{
		return FALSE;
	}
	bRetOK = HookFunctionByHeaderAddress(
		dwReloadKiFastCallEntry,
		KiFastCallEntry,
		KiFastCallEntryTempHookZone,
		&KiFastCallEntryTempPatchCodeLength,
		&KiFastCallEntryTempRet
		);
	if (bRetOK)
	{
		bRetOK = FALSE;

		//��hook dwReloadKiFastCallEntry����ñ�hook����

		if(HookFunctionMiddle((BYTE*)dwReloadKiFastCallEntry,4096,(DWORD)KiFastCallEntryHookProc,CodeInfo,KiFastCallEntryHookZone,&PatchCodeLength,&KiFastCallEntryRet))
		{
			memcpy(ByteKiFastCallEntryBak,(PVOID)KiFastCallEntry,5);  //������ת��ַ
			memcpy(ByteReloadKiFastCallEntryBak,(PVOID)dwReloadKiFastCallEntry,5);  //������ת��ַ
			bRetOK = TRUE;
		}
	}
/*
	if(HookFunctionMiddle((BYTE*)KiFastCallEntry,4096,(DWORD)KiFastCallEntryHookProc,CodeInfo,KiFastCallEntryHookZone,&PatchCodeLength,&KiFastCallEntryRet))
	{
		//����hook�ĵ�ַ
		//ulKiFastCallEntryHookCheck = (ULONG)KiFastCallEntryRet-PatchCodeLength;
		//memcpy(ByteHookCheck,(PVOID)ulKiFastCallEntryHookCheck,5);  //������ת��ַ

		//KdPrint(("hook ok��%08x\n",ulKiFastCallEntryHookCheck));
		bRetOK = TRUE;
	}
*/
	ExFreePool(CodeInfo);
	return bRetOK;
}
ULONG ReLoadNtosCALL(WCHAR *lpwzFuncTion,ULONG ulOldNtosBase,ULONG ulReloadNtosBase)
{
	UNICODE_STRING UnicodeFunctionName;
	ULONG ulOldFunctionAddress;
	ULONG ulReloadFunctionAddress;
	int index=0;
	PIMAGE_DOS_HEADER pDosHeader;
	PIMAGE_NT_HEADERS NtDllHeader;

	IMAGE_OPTIONAL_HEADER opthdr;
	DWORD* arrayOfFunctionAddresses;
	DWORD* arrayOfFunctionNames;
	WORD* arrayOfFunctionOrdinals;
	DWORD functionOrdinal;
	DWORD Base, x, functionAddress,position;
	char* functionName;
	IMAGE_EXPORT_DIRECTORY *pExportTable;
	ULONG ulNtDllModuleBase;

	UNICODE_STRING UnicodeFunction;
	UNICODE_STRING UnicodeExportTableFunction;
	ANSI_STRING ExportTableFunction;

	__try
	{
		if (RRtlInitUnicodeString &&
			RRtlCompareUnicodeString &&
			RMmGetSystemRoutineAddress &&
			RMmIsAddressValid)
		{
			RRtlInitUnicodeString(&UnicodeFunctionName,lpwzFuncTion);
			ulOldFunctionAddress = (DWORD)RMmGetSystemRoutineAddress(&UnicodeFunctionName);
			ulReloadFunctionAddress = ulOldFunctionAddress - ulOldNtosBase + ulReloadNtosBase;
			if (RMmIsAddressValid(ulReloadFunctionAddress))
			{
				return ulReloadFunctionAddress;
			}
			//�ӵ��������ȡ
			ulNtDllModuleBase = ulReloadNtosBase;
			pDosHeader = (PIMAGE_DOS_HEADER)ulReloadNtosBase;
			if (pDosHeader->e_magic!=IMAGE_DOS_SIGNATURE)
			{
				KdPrint(("failed to find NtHeader\r\n"));
				return NULL;
			}
			NtDllHeader=(PIMAGE_NT_HEADERS)(ULONG)((ULONG)pDosHeader+pDosHeader->e_lfanew);
			if (NtDllHeader->Signature!=IMAGE_NT_SIGNATURE)
			{
				KdPrint(("failed to find NtHeader\r\n"));
				return NULL;
			}
			opthdr = NtDllHeader->OptionalHeader;
			pExportTable =(IMAGE_EXPORT_DIRECTORY*)((BYTE*)ulNtDllModuleBase + opthdr.DataDirectory[ IMAGE_DIRECTORY_ENTRY_EXPORT]. VirtualAddress); //�õ�������
			arrayOfFunctionAddresses = (DWORD*)( (BYTE*)ulNtDllModuleBase + pExportTable->AddressOfFunctions);  //��ַ��
			arrayOfFunctionNames = (DWORD*)((BYTE*)ulNtDllModuleBase + pExportTable->AddressOfNames);         //��������
			arrayOfFunctionOrdinals = (WORD*)((BYTE*)ulNtDllModuleBase + pExportTable->AddressOfNameOrdinals);

			Base = pExportTable->Base;

			for(x = 0; x < pExportTable->NumberOfFunctions; x++) //��������������ɨ��
			{
				functionName = (char*)( (BYTE*)ulNtDllModuleBase + arrayOfFunctionNames[x]);
				functionOrdinal = arrayOfFunctionOrdinals[x] + Base - 1; 
				functionAddress = (DWORD)((BYTE*)ulNtDllModuleBase + arrayOfFunctionAddresses[functionOrdinal]);
				RtlInitAnsiString(&ExportTableFunction,functionName);
				RtlAnsiStringToUnicodeString(&UnicodeExportTableFunction,&ExportTableFunction,TRUE);
				
				RRtlInitUnicodeString(&UnicodeFunction,lpwzFuncTion);
				if (RRtlCompareUnicodeString(&UnicodeExportTableFunction,&UnicodeFunction,TRUE) == 0)
				{
					RtlFreeUnicodeString(&UnicodeExportTableFunction);
					return functionAddress;
				}
				RtlFreeUnicodeString(&UnicodeExportTableFunction);
			}
			return NULL;
		}
		RtlInitUnicodeString(&UnicodeFunctionName,lpwzFuncTion);
		ulOldFunctionAddress = (DWORD)MmGetSystemRoutineAddress(&UnicodeFunctionName);
		ulReloadFunctionAddress = ulOldFunctionAddress - ulOldNtosBase + ulReloadNtosBase;

		//KdPrint(("%ws:%08x:%08x",lpwzFuncTion,ulOldFunctionAddress,ulReloadFunctionAddress));

		if (MmIsAddressValid(ulReloadFunctionAddress))
		{
			return ulReloadFunctionAddress;
		}
// 		

	}__except(EXCEPTION_EXECUTE_HANDLER){
		KdPrint(("EXCEPTION_EXECUTE_HANDLER"));
	}
	return NULL;
}
NTSTATUS ReLoadNtos(PDRIVER_OBJECT   DriverObject,DWORD RetAddress)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	ULONG ulKeAddSystemServiceTable;
	PULONG p;

	if (DebugOn)
		KdPrint(("ret address:%X\n",RetAddress));

	//Ϊ��¼���ؽ��̽�����
	HideProcessInfo = (PPROCESSINFO)ExAllocatePool(NonPagedPool,(sizeof(PROCESSINFO)+sizeof(SAFESYSTEM_PROCESS_INFORMATION))*120);
	if (!HideProcessInfo)
	{
		KdPrint(("Init ProcessInfo failed"));
		return status;
	}
	memset(HideProcessInfo,0,(sizeof(PROCESSINFO)+sizeof(SAFESYSTEM_PROCESS_INFORMATION))*120);

	if (!GetSystemKernelModuleInfo(
		&SystemKernelFilePath,
		&SystemKernelModuleBase,
		&SystemKernelModuleSize
		))
	{
		KdPrint(("Get System Kernel Module failed"));
		ExFreePool(HideProcessInfo);
		return status;
	}
	if (DebugOn)
		KdPrint(("%S,%X\n",SystemKernelFilePath,SystemKernelModuleBase));

	//Ϊ��¼��־����һ����
	LogDefenseInfo = (PLOGDEFENSE)ExAllocatePool(NonPagedPool,(sizeof(LOGDEFENSE)+sizeof(LOGDEFENSE_INFORMATION))*1024);
	if (!LogDefenseInfo)
	{
		KdPrint(("Init Log Defense Info failed"));
		ExFreePool(HideProcessInfo);
		return status;
	}
	memset(LogDefenseInfo,0,(sizeof(LOGDEFENSE)+sizeof(LOGDEFENSE_INFORMATION))*1024);

	if (InitSafeOperationModule(
		DriverObject,
		SystemKernelFilePath,
		SystemKernelModuleBase
		))
	{
		KdPrint(("Init Ntos module success\r\n"));

// 		if (ReloadWin32K() != STATUS_SUCCESS)
// 		{
// 			KdPrint(("Init Win32K failed"));
// 			ExFreePool(HideProcessInfo);
// 			ExFreePool(LogDefenseInfo);
// 			return status;
// 		}

		RRtlInitUnicodeString = NULL;
		RMmGetSystemRoutineAddress = NULL;
		RMmIsAddressValid = NULL;
		RRtlCompareUnicodeString = NULL;
		RPsGetCurrentProcess = NULL;

		status = STATUS_UNSUCCESSFUL;

		RRtlInitUnicodeString = ReLoadNtosCALL(L"RtlInitUnicodeString",SystemKernelModuleBase,ImageModuleBase);
		RRtlCompareUnicodeString = ReLoadNtosCALL(L"RtlCompareUnicodeString",SystemKernelModuleBase,ImageModuleBase);
		RMmGetSystemRoutineAddress = ReLoadNtosCALL(L"MmGetSystemRoutineAddress",SystemKernelModuleBase,ImageModuleBase);
		RMmIsAddressValid = ReLoadNtosCALL(L"MmIsAddressValid",SystemKernelModuleBase,ImageModuleBase);
		RPsGetCurrentProcess = ReLoadNtosCALL(L"PsGetCurrentProcess",SystemKernelModuleBase,ImageModuleBase);
		if (!RRtlInitUnicodeString ||
			!RRtlCompareUnicodeString ||
			!RMmGetSystemRoutineAddress ||
			!RMmIsAddressValid ||
			!RPsGetCurrentProcess)
		{
			KdPrint(("Init NtosCALL failed"));
			ExFreePool(HideProcessInfo);
			ExFreePool(LogDefenseInfo);
			return status;
		}
		//������ؽ���
		if (HookKiFastCallEntry())
		{
			KdPrint(("Init A-Protect kernel module success\r\n"));
			status = STATUS_SUCCESS;
		}
		else
			KdPrint(("Init A-Protect kernel module failed"));
	}
	return status;
}

