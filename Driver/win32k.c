//**************************************************************************
//*
//*          A�ܵ��Է��� website:http://www.3600safe.com/
//*        
//*�ļ�˵����
//*         ��ȡshadowSSDT��ԭʼ��ַ������һ���ڴ�
//**************************************************************************
#include "win32k.h"

//��win32k��base�õ�shadowssdt
//ԭ�������������

BOOL GetOriginalW32pTable(PVOID ImageBase,DWORD *W32pTable,DWORD Win32kBase)
{
	BOOL bRet=FALSE;
	PIMAGE_NT_HEADERS NtHeaders;
	ULONG_PTR EntryPoint;
	DWORD dwEntryPoint;
	DWORD dwCurAddress;
	DWORD dwFunAddress;
	DWORD Length=0;
	PUCHAR pOpcode;
	ULONG ulCount = NULL;
	WIN_VER_DETAIL WinVer;

	if (!ImageBase)
		return FALSE;

	NtHeaders = RtlImageNtHeader(ImageBase);
	if (NtHeaders)
	{
		EntryPoint = NtHeaders->OptionalHeader.AddressOfEntryPoint;
		EntryPoint += (ULONG_PTR)ImageBase;
		dwEntryPoint=(DWORD)EntryPoint;
		/*IDA���������
		68 80 A2 99 BF          push    offset off_BF99A280
		FF 15 58 D4 98 BF       call    ds:KeAddSystemServiceTable
		*/
		//ͨ��call    ds:KeAddSystemServiceTable�Ķ�λ���ö�λӦ�ñȽ�׼ȷ
		
		if (DebugOn)
			KdPrint(("start searching:%08x",dwEntryPoint));

		__try
		{
			for(dwCurAddress = dwEntryPoint; dwCurAddress < dwEntryPoint + 0x1000; dwCurAddress+=Length)
			{
				Length = SizeOfCode((PUCHAR)dwCurAddress, &pOpcode);
				if(*(WORD *)dwCurAddress == 0x15ff )
				{
					ulCount++;
					//KdPrint(("dwCurAddress:%08x",dwCurAddress));

					WinVer = GetWindowsVersion();
					if (WinVer == WINDOWS_VERSION_2K3_SP1_SP2 || 
						WinVer == WINDOWS_VERSION_XP)
					{
						if (ulCount == 5)  // �����call
						{
							//��������غ��ds:KeAddSystemServiceTable��ַ��Ȼ��õ�ַ�д�ŵļ���KeAddSystemServiceTable��ʵ��ڵ�ַ
							if (MmIsAddressValid((dwCurAddress + 2)) &&
								MmIsAddressValid((*(PDWORD)(dwCurAddress + 2)-Win32kBase+(DWORD)ImageBase)) &&
								MmIsAddressValid((dwCurAddress - 4)))
							{
								dwFunAddress = *(PDWORD)(*(PDWORD)(dwCurAddress + 2)-Win32kBase+(DWORD)ImageBase);

								if (DebugOn)
									KdPrint(("found:dwCurAddress:%08x,dwFunAddress:%08x",dwCurAddress,dwFunAddress));

								//���õ�ַ����ں���Win32k�ļ��ص�ַ�����ض�λ
								*W32pTable =(PDWORD) (*(PDWORD)(dwCurAddress - 4) - Win32kBase + (DWORD)ImageBase);
								bRet = TRUE;
								break;
							}
						}
					}else if (WinVer == WINDOWS_VERSION_7)
					{
						if (ulCount == 4)  // ���ĸ�call
						{
							//��������غ��ds:KeAddSystemServiceTable��ַ��Ȼ��õ�ַ�д�ŵļ���KeAddSystemServiceTable��ʵ��ڵ�ַ
							if (MmIsAddressValid((dwCurAddress + 2)) &&
								MmIsAddressValid((*(PDWORD)(dwCurAddress + 2)-Win32kBase+(DWORD)ImageBase)) &&
								MmIsAddressValid((dwCurAddress - 4)))
							{
								dwFunAddress = *(PDWORD)(*(PDWORD)(dwCurAddress + 2)-Win32kBase+(DWORD)ImageBase);

								if (DebugOn)
									KdPrint(("found:dwCurAddress:%08x,dwFunAddress:%08x",dwCurAddress,dwFunAddress));

								//���õ�ַ����ں���Win32k�ļ��ص�ַ�����ض�λ
								*W32pTable =(PDWORD) (*(PDWORD)(dwCurAddress - 4) - Win32kBase + (DWORD)ImageBase);
								bRet = TRUE;
								break;
							}
						}
					}
				}
			}

		}__except(EXCEPTION_EXECUTE_HANDLER){

		}
	}
	return bRet;
}
void FixOriginalW32pTable(PDWORD w32pTable,PVOID ImageBase,DWORD Win32kBase)
{
	PIMAGE_NT_HEADERS NtHeaders;
	DWORD FuctionCount;
	DWORD Index;
	DWORD ImageSize;

 	NtHeaders=RtlImageNtHeader(ImageBase);
 	if (NtHeaders)
 	{
 		ImageSize=NtHeaders->OptionalHeader.SizeOfImage;
 		ImageSize=AlignSize(ImageSize,NtHeaders->OptionalHeader.SectionAlignment);
 	}
	FuctionCount = ShadowSSDTTable[1].TableSize;
	
	if (DebugOn)
		KdPrint(("FuctionCount:%d"));
	
	for (Index=0;Index<FuctionCount;Index++)
	{
		w32pTable[Index]=w32pTable[Index]-Win32kBase+(DWORD)ImageBase;
	}
}
BOOL InitReloadWin32K(PDRIVER_OBJECT pDriverObject,PSERVICE_DESCRIPTOR_TABLE ShadowSSDT,ULONG Win32kBase)
{
	UNICODE_STRING FileName;
	HANDLE hSection;
	PDWORD OriginalKiServiceTable;
	PDWORD CsRootkitOriginalKiServiceTable;
	int i=0;

	if (!PeLoad(L"\\SystemRoot\\System32\\win32k.sys",&Win32kImageModuleBase,pDriverObject,Win32kBase))
	{
		if (DebugOn)
			KdPrint(("Safe->PeLoad failed\n"));
		return FALSE;
	}

	if (DebugOn)
		KdPrint(("Safe->ModuleBase:%08x\r\n",Win32kImageModuleBase));

	OriginalKiServiceTable = ExAllocatePool(NonPagedPool,ShadowSSDT[1].TableSize*sizeof(DWORD));
	if (!OriginalKiServiceTable)
	{
		if (DebugOn)
			KdPrint(("OriginalKiServiceTable Failed\n"));
		return FALSE;
	}
	if(!GetOriginalW32pTable(Win32kImageModuleBase,&OriginalKiServiceTable,Win32kBase))
	{
		if (DebugOn)
			KdPrint(("Safe->Get Original KiServiceTable Failed\n"));

		ExFreePool(OriginalKiServiceTable);
		return FALSE;
	}

	if (DebugOn)
		KdPrint(("Safe->OriginalKiServiceTable %x-%x\n",OriginalKiServiceTable,ShadowSSDT[1].ServiceTable));

	//���ÿһ��ssdt��Ӧ������ַ~
	FixOriginalW32pTable(OriginalKiServiceTable,(DWORD)Win32kImageModuleBase,Win32kBase);

	OriginalShadowServiceDescriptorTable=ExAllocatePool(NonPagedPool,sizeof(SERVICE_DESCRIPTOR_TABLE)*4);
	if (OriginalShadowServiceDescriptorTable == NULL)
	{
		ExFreePool(OriginalKiServiceTable);
		return FALSE;
	}
	RtlZeroMemory(OriginalShadowServiceDescriptorTable,sizeof(SERVICE_DESCRIPTOR_TABLE)*4);
	//����һ���ɾ���ԭʼ��ÿ����������Ӧ��SSDT�����ĵ�ַ������Ч��~
	OriginalShadowServiceDescriptorTable->ServiceTable=(PDWORD)OriginalKiServiceTable;
	OriginalShadowServiceDescriptorTable->CounterTable=ShadowSSDT[1].CounterTable;
	OriginalShadowServiceDescriptorTable->TableSize=ShadowSSDT[1].TableSize;
	OriginalShadowServiceDescriptorTable->ArgumentTable=ShadowSSDT[1].ArgumentTable;

	CsRootkitOriginalKiServiceTable=ExAllocatePool(NonPagedPool,ShadowSSDT[1].TableSize*sizeof(DWORD));
	if (CsRootkitOriginalKiServiceTable==NULL)
	{
		ExFreePool(OriginalShadowServiceDescriptorTable);
		ExFreePool(OriginalKiServiceTable);
		return FALSE;

	}
	RtlZeroMemory(CsRootkitOriginalKiServiceTable,ShadowSSDT[1].TableSize*sizeof(DWORD));

	Safe_ServiceDescriptorShadowSSDTTable=ExAllocatePool(NonPagedPool,sizeof(SERVICE_DESCRIPTOR_TABLE)*4);
	if (Safe_ServiceDescriptorShadowSSDTTable==NULL)
	{
		ExFreePool(OriginalShadowServiceDescriptorTable);
		ExFreePool(CsRootkitOriginalKiServiceTable);
		ExFreePool(OriginalKiServiceTable);
		return FALSE;
	}
	//����һ���ɾ���ԭʼ��ÿ����������Ӧ��SSDT�����ĵ�ַ����ԭʼ����
	RtlZeroMemory(Safe_ServiceDescriptorShadowSSDTTable,sizeof(SERVICE_DESCRIPTOR_TABLE)*4);

	//���ԭʼ������ַ
	for (i=0;i<ShadowSSDT[1].TableSize;i++)
	{
		CsRootkitOriginalKiServiceTable[i] = OriginalShadowServiceDescriptorTable->ServiceTable[i];
	}
	Safe_ServiceDescriptorShadowSSDTTable->ServiceTable = (PDWORD)CsRootkitOriginalKiServiceTable;
	Safe_ServiceDescriptorShadowSSDTTable->CounterTable=ShadowSSDT[1].CounterTable;
	Safe_ServiceDescriptorShadowSSDTTable->TableSize=ShadowSSDT[1].TableSize;
	Safe_ServiceDescriptorShadowSSDTTable->ArgumentTable=ShadowSSDT[1].ArgumentTable;

	//�ͷžͻ�bsod
	//ExFreePool(OriginalKiServiceTable);
	return TRUE;
}
// ULONG SearchShadowSSDT()
// {
// 	char *pfn;
// 	ULONG addr;
// 	int len;
// 	pfn = (char*)KeAddSystemServiceTable;
// 	len = 0;
// 	while ( len < 0x1000 )
// 	{
// 		addr = *(ULONG*)pfn;
// 		if ( MmIsAddressValid((PVOID)addr) ) 
// 		{
// 			if ( *(ULONG*)addr == (ULONG)(KeServiceDescriptorTable->ServiceTable)
// 				&& addr != (ULONG)&KeServiceDescriptorTable )
// 				return addr;
// 		}
// 		++len;
// 		pfn = pfn + 1;
// 	}
// 	return 0;
// }
// BOOL GetShdowSSDTAddr()
// {
// 	ULONG addr;
// 
// 	addr = SearchShadowSSDT();
// 	ShadowSSDTTable = (PSERVICE_DESCRIPTOR_TABLE)addr;
// 	return addr != 0;
// }
NTSTATUS ReloadWin32K()
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	ULONG ulKeAddSystemServiceTable;
	UNICODE_STRING UnicodeFunctionName;
	PULONG p;
	int i = 0;

	if (IsExitProcess(CsrssEProcess))
	{
		KeAttachProcess(CsrssEProcess);

		//ulKeAddSystemServiceTable = GetSystemRoutineAddress(1,L"KeAddSystemServiceTable");
		//ulKeAddSystemServiceTable = (char *)KeAddSystemServiceTable;

		RtlInitUnicodeString(&UnicodeFunctionName,L"KeAddSystemServiceTable");
		ulKeAddSystemServiceTable = (DWORD)MmGetSystemRoutineAddress(&UnicodeFunctionName);
		if (!MmIsAddressValid(ulKeAddSystemServiceTable))
		{
			if (DebugOn)
				KdPrint(("KeAddSystemServiceTable false:%x\n",ulKeAddSystemServiceTable));

			KeDetachProcess();
			return status;
		}

 		p = (PULONG)((ULONG)ulKeAddSystemServiceTable + 0x1a + 0x2);
 		ShadowSSDTTable = (PSERVICE_DESCRIPTOR_TABLE)(PULONG)(*p);

		if (DebugOn)
			KdPrint(("ShadowSSDTTable:%x\n",ShadowSSDTTable[1]));

// 		for (i=0;i<ShadowSSDTTable->TableSize;i++)
// 		{
// 			KdPrint(("%d:%x\n",i,ShadowSSDTTable->ServiceTable[i]));
// 		}

		ulWin32kBase = LookupKernelModuleByName(PDriverObject,"win32k.sys",&ulWin32kSize);
		if (ShadowSSDTTable &&
			ulWin32kBase)
		{
			if (DebugOn)
				KdPrint(("LookupKernelModuleByName success\n"));

			if (InitReloadWin32K(PDriverObject,ShadowSSDTTable,ulWin32kBase))
			{
				if (DebugOn)
					KdPrint(("InitReloadWin32K success"));

				ShadowTable = ShadowSSDTTable[1].ServiceTable;

				KeDetachProcess();
				status = STATUS_SUCCESS;
			}
		}
		KeDetachProcess();
	}
	return status;
}
////////--------------------------