//**************************************************************************
//*
//*          A�ܵ��Է��� website:http://www.3600safe.com/
//*        
//*�ļ�˵����
//*         ö���ں˹�������
//**************************************************************************
#include "KernelFilterDriver.h"

NTSTATUS kernel_filterDriverEnumFunc(PKERNEL_FILTERDRIVER KernelFilterDriver,UNICODE_STRING ustrDirPath,ULONG ulDirType)
{
	HANDLE hDriver;
	NTSTATUS status;
	ULONG ulContext,ulRet=0;
	ULONG ulSize = 0x1000;
	OBJECT_ATTRIBUTES objectAttribute;
	PDEVICE_OBJECT pDeviceObject;
	PDRIVER_OBJECT pDriverObject;
	PDEVICE_OBJECT pAttachObject;
	UNICODE_STRING ustrDriverName;
	PDIRECTORY_BASIC_INFORMATION pDirectoryBasicInformation = NULL,pDirectoryBasicInformation_tmp;
	PLDR_DATA_TABLE_ENTRY pLdr;
	WCHAR DriverName[64]={L'\0'};
	KIRQL oldIrql = NULL; 
	BOOL bInit = FALSE;

// 	if (KeGetCurrentIrql() > PASSIVE_LEVEL)
// 		return STATUS_UNSUCCESSFUL;

	RZwOpenDirectoryObject = ReLoadNtosCALL(L"ZwOpenDirectoryObject",SystemKernelModuleBase,ImageModuleBase);
	RExFreePool = ReLoadNtosCALL(L"ExFreePool",SystemKernelModuleBase,ImageModuleBase);
	RExAllocatePool = ReLoadNtosCALL(L"ExAllocatePool",SystemKernelModuleBase,ImageModuleBase);
	RZwQueryDirectoryObject = ReLoadNtosCALL(L"ZwQueryDirectoryObject",SystemKernelModuleBase,ImageModuleBase);
	RObReferenceObjectByName = ReLoadNtosCALL(L"ObReferenceObjectByName",SystemKernelModuleBase,ImageModuleBase);
	RRtlInitUnicodeString = ReLoadNtosCALL(L"RtlInitUnicodeString",SystemKernelModuleBase,ImageModuleBase);
	if (RZwOpenDirectoryObject &&
		RExFreePool &&
		RExAllocatePool &&
		RZwQueryDirectoryObject &&
		RObReferenceObjectByName &&
		RRtlInitUnicodeString)
	{
		bInit = TRUE;
	}
	if (!bInit)
		return STATUS_UNSUCCESSFUL;

	__try
	{
		//��ʼ���ṹ��
		InitializeObjectAttributes(
			&objectAttribute,
			&ustrDirPath,
			OBJ_CASE_INSENSITIVE,
			NULL,
			NULL
			);
		//������Ŀ¼����
		status=RZwOpenDirectoryObject(
			&hDriver,
			DIRECTORY_QUERY,
			&objectAttribute
			);
		//����ɹ�
		if (!NT_SUCCESS(status))
		{
			KdPrint(("kernel_filterDriverEnumFunc ZwOpenDirectoryObject error!"));
			__leave;
		}
		do 
		{
			if (pDirectoryBasicInformation)
				RExFreePool(pDirectoryBasicInformation);

			//������С����
			ulSize = 2*ulSize;
			//����
			pDirectoryBasicInformation = NULL;
			pDirectoryBasicInformation = (PDIRECTORY_BASIC_INFORMATION)RExAllocatePool(NonPagedPool,ulSize);
			//��������ڴ�ʧ��
			if (!pDirectoryBasicInformation)
			{
				KdPrint(("kernel_filterDriverEnumFunc pDirectoryBasicInformation error!"));
				__leave;
			}
			status = RZwQueryDirectoryObject(
				hDriver,
				pDirectoryBasicInformation,
				ulSize,
				FALSE,
				TRUE,
				&ulContext,
				&ulRet
				);

		} while (status == STATUS_BUFFER_TOO_SMALL || status == STATUS_MORE_ENTRIES);

		//���û�гɹ�
		if (!NT_SUCCESS(status))
		{
			KdPrint(("kernel_filterDriverEnumFunc ZwQueryDirectoryObject error!"));
			__leave;
		}
		pDirectoryBasicInformation_tmp = pDirectoryBasicInformation;

		while (pDirectoryBasicInformation_tmp->ObjectName.Length!=0 && pDirectoryBasicInformation_tmp->ObjectTypeName.Length!=0 )
		{
			__try
			{

				if (ulDirType==0)
				{
					wcscpy(DriverName,L"\\Driver\\");
					wcscat(DriverName,pDirectoryBasicInformation_tmp->ObjectName.Buffer);
					RRtlInitUnicodeString(&ustrDriverName,DriverName);
				}
				else
				{
					wcscpy(DriverName,L"\\FileSystem\\");
					wcscat(DriverName,pDirectoryBasicInformation_tmp->ObjectName.Buffer);
					RRtlInitUnicodeString(&ustrDriverName,DriverName);
				}

				//���ü�����һ
				status = RObReferenceObjectByName(
					&ustrDriverName,
					OBJ_CASE_INSENSITIVE,
					NULL,
					0,
					*IoDriverObjectType,
					KernelMode,
					NULL,
					(PVOID *)&pDriverObject
					);
				//�������ʧ��
				if (!NT_SUCCESS(status))
				{
					pDirectoryBasicInformation_tmp++;
					//KdPrint(("ObReferenceObjectByName failed:%d",RtlNtStatusToDosError(status)));
					continue;
				}
				//���ü�����һ
				ObDereferenceObject(pDriverObject);

				if (!RMmIsAddressValid(pDriverObject) ||
					!RMmIsAddressValid(pDriverObject->DeviceObject))
				{
					//KdPrint(("pDriverObject failed"));
					pDirectoryBasicInformation_tmp++;
					continue;
				}

				pDeviceObject = pDriverObject->DeviceObject;

				while (pDeviceObject)
				{
					// �жϵ�ǰ�豸���Ƿ��й�������
					if (pDeviceObject->AttachedDevice)
					{
						//�����豸����
						pAttachObject = pDeviceObject->AttachedDevice;
						//�����豸ջ
						while (pAttachObject)
						{
							switch (pAttachObject->DeviceType)
							{
							case FILE_DEVICE_DISK:
							case FILE_DEVICE_DISK_FILE_SYSTEM:
							case FILE_DEVICE_FILE_SYSTEM:
							case FILE_DEVICE_KEYBOARD:
							case FILE_DEVICE_MOUSE:
							case FILE_DEVICE_NETWORK:
							case FILE_DEVICE_8042_PORT:
							case FILE_DEVICE_DFS_VOLUME:
							case FILE_DEVICE_DFS_FILE_SYSTEM:
							case FILE_DEVICE_UNKNOWN:	

								//�豸����
								KernelFilterDriver->KernelFilterDriverInfo[IntFilterCount].ulObjType = pAttachObject->DeviceType;
								//�����豸
								KernelFilterDriver->KernelFilterDriverInfo[IntFilterCount].ulAttachDevice = (ULONG)pAttachObject;

								//��������
								if (ValidateUnicodeString(&pAttachObject->DriverObject->DriverName))
								{
									memset(KernelFilterDriver->KernelFilterDriverInfo[IntFilterCount].FileName,0,sizeof(KernelFilterDriver->KernelFilterDriverInfo[IntFilterCount].FileName));

//                                  oldIrql = KeRaiseIrqlToDpcLevel(); 
// 									memcpy(
// 										KernelFilterDriver->KernelFilterDriverInfo[IntFilterCount].FileName,
// 										pAttachObject->DriverObject->DriverName.Buffer,
// 										pAttachObject->DriverObject->DriverName.Length);
//                                  KeLowerIrql(oldIrql);

									SafeCopyMemory(pAttachObject->DriverObject->DriverName.Buffer,
										KernelFilterDriver->KernelFilterDriverInfo[IntFilterCount].FileName,
										pAttachObject->DriverObject->DriverName.Length
										);
								}

								//������������
								if (ValidateUnicodeString(&pAttachObject->DriverObject->DriverName))
								{
									memset(KernelFilterDriver->KernelFilterDriverInfo[IntFilterCount].HostFileName,0,sizeof(KernelFilterDriver->KernelFilterDriverInfo[IntFilterCount].HostFileName));

//									oldIrql = KeRaiseIrqlToDpcLevel(); 
// 									memcpy(
// 										KernelFilterDriver->KernelFilterDriverInfo[IntFilterCount].HostFileName,
// 										pDriverObject->DriverName.Buffer,
// 										pDriverObject->DriverName.Length
// 										);
//									KeLowerIrql(oldIrql);
									SafeCopyMemory(pDriverObject->DriverName.Buffer,
										KernelFilterDriver->KernelFilterDriverInfo[IntFilterCount].HostFileName,
										pDriverObject->DriverName.Length
										);
								}
								//����·��
								pLdr=(PLDR_DATA_TABLE_ENTRY)(pAttachObject->DriverObject->DriverSection);
								if (pLdr!=NULL && pLdr->FullDllName.Buffer!=NULL)
								{
									if (ValidateUnicodeString(&pLdr->FullDllName))
									{
										//·��
										memset(KernelFilterDriver->KernelFilterDriverInfo[IntFilterCount].FilePath,0,sizeof(KernelFilterDriver->KernelFilterDriverInfo[IntFilterCount].FilePath));

//										oldIrql = KeRaiseIrqlToDpcLevel(); 
// 										memcpy(
// 											KernelFilterDriver->KernelFilterDriverInfo[IntFilterCount].FilePath,
// 											pLdr->FullDllName.Buffer,
// 											pLdr->FullDllName.Length
// 											);
//										KeLowerIrql(oldIrql);
										SafeCopyMemory(pLdr->FullDllName.Buffer,
											KernelFilterDriver->KernelFilterDriverInfo[IntFilterCount].FilePath,
											pLdr->FullDllName.Length
											);
									}
								}
								else
								{
									if (ValidateUnicodeString(&pAttachObject->DriverObject->DriverName))
									{
										memset(KernelFilterDriver->KernelFilterDriverInfo[IntFilterCount].FilePath,0,sizeof(KernelFilterDriver->KernelFilterDriverInfo[IntFilterCount].FilePath));

//										oldIrql = KeRaiseIrqlToDpcLevel(); 
// 										memcpy(
// 											KernelFilterDriver->KernelFilterDriverInfo[IntFilterCount].FilePath,
// 											pAttachObject->DriverObject->DriverName.Buffer,
// 											pAttachObject->DriverObject->DriverName.Length
// 											);
//										KeLowerIrql(oldIrql);
										SafeCopyMemory(pAttachObject->DriverObject->DriverName.Buffer,
											KernelFilterDriver->KernelFilterDriverInfo[IntFilterCount].FilePath,
											pAttachObject->DriverObject->DriverName.Length
											);
									}
								}
								IntFilterCount++;
								KernelFilterDriver->ulCount = IntFilterCount;
								break;
							}
							//ָ����һ�������豸
							pAttachObject = pAttachObject->AttachedDevice;
						}
					}
					// �õ������ڴ������ϵ���һ���豸 DEVICE_OBJECT 
					pDeviceObject = pDeviceObject->NextDevice;
				}
A1:
				//ָ���ƶ�����һ���ṹ��
				pDirectoryBasicInformation_tmp++;
				if (!RMmIsAddressValid(pDirectoryBasicInformation_tmp))
				{
					goto A1;
				}

			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				//ָ���ƶ�����һ���ṹ��
A2:
				pDirectoryBasicInformation_tmp++;
				if (!RMmIsAddressValid(pDirectoryBasicInformation_tmp))
				{
					goto A2;
				}
			}
		}
		if (pDirectoryBasicInformation)
			RExFreePool(pDirectoryBasicInformation);

		pDirectoryBasicInformation = NULL;

		return STATUS_SUCCESS;
	}
	__except(EXCEPTION_EXECUTE_HANDLER){

		if (pDirectoryBasicInformation)
			RExFreePool(pDirectoryBasicInformation);

		return STATUS_UNSUCCESSFUL;
	}
	if (pDirectoryBasicInformation)
		RExFreePool(pDirectoryBasicInformation);

	pDirectoryBasicInformation = NULL;

	return STATUS_UNSUCCESSFUL;
}

//ö�ٹ�������
NTSTATUS KernelFilterDriverEnum(PKERNEL_FILTERDRIVER KernelFilterDriver)
{
	NTSTATUS status;
	UNICODE_STRING ustrDirPath;
	int i = 0;
	BOOL bInit = FALSE;
	
	RRtlInitUnicodeString = ReLoadNtosCALL(L"RtlInitUnicodeString",SystemKernelModuleBase,ImageModuleBase);
	if (RRtlInitUnicodeString)
	{
		bInit = TRUE;
	}
	if (!bInit)
		return STATUS_UNSUCCESSFUL;

	RtlInitUnicodeString(&ustrDirPath,L"\\FileSystem");

	IntFilterCount = 0;

	status = kernel_filterDriverEnumFunc(KernelFilterDriver,ustrDirPath,1);

	//IntFilterCount++;

	RtlInitUnicodeString(&ustrDirPath,L"\\Driver");
	status = kernel_filterDriverEnumFunc(KernelFilterDriver,ustrDirPath,0);	
	
	return status;
}
//������1:\\FileSystem\\Ntfs
//      2:�豸����
VOID ClearFilters(PWSTR lpwName,ULONG ulDeviceObject)
{
	UNICODE_STRING TName;
	PDRIVER_OBJECT TDrvObj;
	PDEVICE_OBJECT CurrentDevice;
	NTSTATUS status;

	BOOL bInit = FALSE;

	RRtlInitUnicodeString = ReLoadNtosCALL(L"RtlInitUnicodeString",SystemKernelModuleBase,ImageModuleBase);
	RObReferenceObjectByName = ReLoadNtosCALL(L"ObReferenceObjectByName",SystemKernelModuleBase,ImageModuleBase);
	if (RRtlInitUnicodeString &&
		RObReferenceObjectByName)
	{
		bInit = TRUE;
	}
	if (!bInit)
		return STATUS_UNSUCCESSFUL;

	RRtlInitUnicodeString(&TName, lpwName);
	status = RObReferenceObjectByName(&TName,
		OBJ_CASE_INSENSITIVE,
		NULL,
		0,
		*IoDriverObjectType,
		KernelMode,                   
		NULL,
		&TDrvObj);

	if (!NT_SUCCESS(status)) return ;
	if(!TDrvObj) return ;

	CurrentDevice = TDrvObj->DeviceObject;

	while(CurrentDevice != NULL )
	{
		if (CurrentDevice->AttachedDevice == ulDeviceObject)
		{
			if (DebugOn)
				KdPrint(("AttachedDevice:%08x  NextDevice:%08x",CurrentDevice->AttachedDevice,CurrentDevice->NextDevice));

			CurrentDevice->AttachedDevice = NULL;
			CurrentDevice = CurrentDevice->NextDevice;
			break;
		}
		CurrentDevice = CurrentDevice->NextDevice;
	}
	ObDereferenceObject(TDrvObj);
	return;
}