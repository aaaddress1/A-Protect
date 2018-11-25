//**************************************************************************
//*
//*          A�ܵ��Է��� website:http://www.3600safe.com/
//*        
//*�ļ�˵����
//*         ɾ���ļ�ģ��
//**************************************************************************
#include "DeleteFile.h"

HANDLE
SkillIoOpenFile(
				IN PCWSTR FileName,
				IN ACCESS_MASK DesiredAccess,
				IN ULONG ShareAccess
				)
{
	NTSTATUS              ntStatus;
	UNICODE_STRING        uniFileName;
	OBJECT_ATTRIBUTES     objectAttributes;
	HANDLE                ntFileHandle;
	IO_STATUS_BLOCK       ioStatus;
	BOOL bInit = FALSE;

	RRtlInitUnicodeString = ReLoadNtosCALL(L"RtlInitUnicodeString",SystemKernelModuleBase,ImageModuleBase);
	RIoCreateFile = ReLoadNtosCALL(L"IoCreateFile",SystemKernelModuleBase,ImageModuleBase);
	if (RRtlInitUnicodeString &&
		RIoCreateFile)
	{
		bInit = TRUE;
	}
	if (!bInit)
		return 0;

	RRtlInitUnicodeString(&uniFileName, FileName);

	InitializeObjectAttributes(&objectAttributes, &uniFileName,
		OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, NULL);

	ntStatus = RIoCreateFile(&ntFileHandle,
		DesiredAccess,
		&objectAttributes,
		&ioStatus,
		0,
		FILE_ATTRIBUTE_NORMAL,
		ShareAccess,
		FILE_OPEN,
		0,
		NULL,
		0,
		0,
		NULL,
		IO_NO_PARAMETER_CHECKING);

	if (!NT_SUCCESS(ntStatus))
	{
		return 0;
	}

	return ntFileHandle;
}

NTSTATUS
SkillSetFileCompletion(
					   IN PDEVICE_OBJECT DeviceObject,
					   IN PIRP Irp,
					   IN PVOID Context
					   )
{
	BOOL bInit = FALSE;

	RKeSetEvent = ReLoadNtosCALL(L"KeSetEvent",SystemKernelModuleBase,ImageModuleBase);
	if (RKeSetEvent)
	{
		bInit = TRUE;
	}
	if (!bInit)
		return STATUS_UNSUCCESSFUL;

	Irp->UserIosb->Status = Irp->IoStatus.Status;
	Irp->UserIosb->Information = Irp->IoStatus.Information;

	RKeSetEvent(Irp->UserEvent, IO_NO_INCREMENT, FALSE);

	IoFreeIrp(Irp);

	return STATUS_MORE_PROCESSING_REQUIRED;
}

BOOLEAN
SKillStripFileAttributes(
						 IN HANDLE    FileHandle
						 )
{
	NTSTATUS          ntStatus = STATUS_SUCCESS;
	PFILE_OBJECT      fileObject;
	PDEVICE_OBJECT    DeviceObject;
	PIRP              Irp;
	KEVENT            event;
	FILE_BASIC_INFORMATION    FileInformation;
	IO_STATUS_BLOCK ioStatus;
	PIO_STACK_LOCATION irpSp;

	BOOL bInit = FALSE;

	RObReferenceObjectByHandle = ReLoadNtosCALL(L"ObReferenceObjectByHandle",SystemKernelModuleBase,ImageModuleBase);
	RKeInitializeEvent = ReLoadNtosCALL(L"KeInitializeEvent",SystemKernelModuleBase,ImageModuleBase);
	RIoAllocateIrp = ReLoadNtosCALL(L"IoAllocateIrp",SystemKernelModuleBase,ImageModuleBase);
	RIoCallDriver = ReLoadNtosCALL(L"IoCallDriver",SystemKernelModuleBase,ImageModuleBase);
	RKeWaitForSingleObject = ReLoadNtosCALL(L"KeWaitForSingleObject",SystemKernelModuleBase,ImageModuleBase);
	if (RObReferenceObjectByHandle &&
		RKeInitializeEvent &&
		RIoAllocateIrp &&
		RIoCallDriver &&
		RKeWaitForSingleObject)
	{
		bInit = TRUE;
	}
	if (!bInit)
		return NULL;

	ntStatus = RObReferenceObjectByHandle(FileHandle,
		DELETE,
		*IoFileObjectType,
		KernelMode,
		&fileObject,
		NULL);

	if (!NT_SUCCESS(ntStatus))
	{
		return FALSE;
	}

	DeviceObject = IoGetRelatedDeviceObject(fileObject);
	Irp = RIoAllocateIrp(DeviceObject->StackSize, TRUE);

	if (Irp == NULL)
	{
		ObDereferenceObject(fileObject);
		return FALSE;
	}

	RKeInitializeEvent(&event, SynchronizationEvent, FALSE);

	memset(&FileInformation,0,0x28);

	FileInformation.FileAttributes = FILE_ATTRIBUTE_NORMAL;
	Irp->AssociatedIrp.SystemBuffer = &FileInformation;
	Irp->UserEvent = &event;
	Irp->UserIosb = &ioStatus;
	Irp->Tail.Overlay.OriginalFileObject = fileObject;
	Irp->Tail.Overlay.Thread = (PETHREAD)KeGetCurrentThread();
	Irp->RequestorMode = KernelMode;

	irpSp = IoGetNextIrpStackLocation(Irp);
	irpSp->MajorFunction = IRP_MJ_SET_INFORMATION;
	irpSp->DeviceObject = DeviceObject;
	irpSp->FileObject = fileObject;
	irpSp->Parameters.SetFile.Length = sizeof(FILE_BASIC_INFORMATION);
	irpSp->Parameters.SetFile.FileInformationClass = FileBasicInformation;
	irpSp->Parameters.SetFile.FileObject = fileObject;

	IoSetCompletionRoutine(
		Irp,
		SkillSetFileCompletion,
		&event,
		TRUE,
		TRUE,
		TRUE);

	RIoCallDriver(DeviceObject, Irp);

	RKeWaitForSingleObject(&event, Executive, KernelMode, TRUE, NULL);

	ObDereferenceObject(fileObject);

	return TRUE;
}


BOOLEAN
SKillDeleteFile(
				IN HANDLE    FileHandle
				)
{
	NTSTATUS          ntStatus = STATUS_SUCCESS;
	PFILE_OBJECT      fileObject;
	PDEVICE_OBJECT    DeviceObject;
	PIRP              Irp;
	KEVENT            event;
	FILE_DISPOSITION_INFORMATION    FileInformation;
	IO_STATUS_BLOCK ioStatus;
	PIO_STACK_LOCATION irpSp;
	PSECTION_OBJECT_POINTERS pSectionObjectPointer;     ////////////////////
	BOOL bInit = FALSE;

	RObReferenceObjectByHandle = ReLoadNtosCALL(L"ObReferenceObjectByHandle",SystemKernelModuleBase,ImageModuleBase);
	RKeInitializeEvent = ReLoadNtosCALL(L"KeInitializeEvent",SystemKernelModuleBase,ImageModuleBase);
	RIoAllocateIrp = ReLoadNtosCALL(L"IoAllocateIrp",SystemKernelModuleBase,ImageModuleBase);
	RIoCallDriver = ReLoadNtosCALL(L"IoCallDriver",SystemKernelModuleBase,ImageModuleBase);
	RKeWaitForSingleObject = ReLoadNtosCALL(L"KeWaitForSingleObject",SystemKernelModuleBase,ImageModuleBase);
	if (RObReferenceObjectByHandle &&
		RKeInitializeEvent &&
		RIoAllocateIrp &&
		RIoCallDriver &&
		RKeWaitForSingleObject)
	{
		bInit = TRUE;
	}
	if (!bInit)
		return NULL;

	SKillStripFileAttributes( FileHandle);          //ȥ��ֻ�����ԣ�����ɾ��ֻ���ļ�

	ntStatus = RObReferenceObjectByHandle(FileHandle,
		DELETE,
		*IoFileObjectType,
		KernelMode,
		&fileObject,
		NULL);

	if (!NT_SUCCESS(ntStatus))
	{
		return FALSE;
	}

	DeviceObject = IoGetRelatedDeviceObject(fileObject);
	Irp = RIoAllocateIrp(DeviceObject->StackSize, TRUE);
	if (Irp == NULL)
	{
		ObDereferenceObject(fileObject);
		return FALSE;
	}

	RKeInitializeEvent(&event, SynchronizationEvent, FALSE);

	FileInformation.DeleteFile = TRUE;

	Irp->AssociatedIrp.SystemBuffer = &FileInformation;
	Irp->UserEvent = &event;
	Irp->UserIosb = &ioStatus;
	Irp->Tail.Overlay.OriginalFileObject = fileObject;
	Irp->Tail.Overlay.Thread = (PETHREAD)KeGetCurrentThread();
	Irp->RequestorMode = KernelMode;

	irpSp = IoGetNextIrpStackLocation(Irp);
	irpSp->MajorFunction = IRP_MJ_SET_INFORMATION;
	irpSp->DeviceObject = DeviceObject;
	irpSp->FileObject = fileObject;
	irpSp->Parameters.SetFile.Length = sizeof(FILE_DISPOSITION_INFORMATION);
	irpSp->Parameters.SetFile.FileInformationClass = FileDispositionInformation;
	irpSp->Parameters.SetFile.FileObject = fileObject;


	IoSetCompletionRoutine(
		Irp,
		SkillSetFileCompletion,
		&event,
		TRUE,
		TRUE,
		TRUE);

	//�ټ������������д��� ��MmFlushImageSection    ����ͨ������ṹ������Ƿ����ɾ���ļ���
	//�����Hook
	ulImageSectionObject = NULL;
	ulDataSectionObject = NULL;
	ulSharedCacheMap = NULL;

	pSectionObjectPointer = NULL;
	pSectionObjectPointer = fileObject->SectionObjectPointer;
	if (RMmIsAddressValid(pSectionObjectPointer))
	{
		ulImageSectionObject = pSectionObjectPointer->ImageSectionObject; // ����֮~~~
		pSectionObjectPointer->ImageSectionObject = 0; //���㣬׼��ɾ��

		ulDataSectionObject = pSectionObjectPointer->DataSectionObject;  //����֮
		pSectionObjectPointer->DataSectionObject = 0;       //���㣬׼��ɾ��

		ulSharedCacheMap = pSectionObjectPointer->SharedCacheMap;
		pSectionObjectPointer->SharedCacheMap = 0;
	}

	//��irpɾ��
	RIoCallDriver(DeviceObject, Irp);

	//�ȴ��������
	RKeWaitForSingleObject(&event, Executive, KernelMode, TRUE, NULL);

	//ɾ���ļ�֮�󣬴ӱ�������������
	pSectionObjectPointer = NULL;
	pSectionObjectPointer = fileObject->SectionObjectPointer;

	if (RMmIsAddressValid(pSectionObjectPointer))
	{
		if (ulImageSectionObject)
			pSectionObjectPointer->ImageSectionObject = ulImageSectionObject; //����������Ȼ����Ŷ

		if (ulDataSectionObject)
			pSectionObjectPointer->DataSectionObject = ulDataSectionObject;

		if (ulSharedCacheMap)
			pSectionObjectPointer->SharedCacheMap = ulSharedCacheMap;
	}

	ObDereferenceObject(fileObject);
	fileObject = NULL;

	return TRUE;
}