#include "stdafx.h"
#include "3600safeview.h"
#include "FilterDriver.h"

void QueryFilterDriver(HWND m_hWnd,ULONG ID,CClrListCtrl *m_list)
{
	DWORD dwReadByte;
	int i =0;
	int ItemNum = m_list->GetItemCount();

	SetDlgItemTextW(m_hWnd,ID,L"����ɨ��������������Ժ�...");

	if (KernelFilterDriver)
	{
		VirtualFree(KernelFilterDriver,(sizeof(KERNEL_FILTERDRIVER)+sizeof(KERNEL_FILTERDRIVER_INFO))*1025,MEM_RESERVE | MEM_COMMIT);
		KernelFilterDriver = 0;
	}
	KernelFilterDriver = (PKERNEL_FILTERDRIVER)VirtualAlloc(0,(sizeof(KERNEL_FILTERDRIVER)+sizeof(KERNEL_FILTERDRIVER_INFO))*1025,MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (KernelFilterDriver)
	{
		memset(KernelFilterDriver,0,(sizeof(KERNEL_FILTERDRIVER)+sizeof(KERNEL_FILTERDRIVER_INFO))*1025);

		ReadFile((HANDLE)LIST_KERNEL_FILTER_DRIVER,KernelFilterDriver,(sizeof(KERNEL_FILTERDRIVER)+sizeof(KERNEL_FILTERDRIVER_INFO))*1025,&dwReadByte,0);

		for (i=0;i< KernelFilterDriver->ulCount;i++)
		{
			WCHAR lpwzTextOut[100];
			memset(lpwzTextOut,0,sizeof(lpwzTextOut));
			wsprintfW(lpwzTextOut,L"���� %d �����ݣ�����ɨ��� %d �������Ժ�...",KernelFilterDriver->ulCount,i);
			SetDlgItemTextW(m_hWnd,ID,lpwzTextOut);

			WCHAR lpwzObjType[256];
			WCHAR lpwzAttachDevice[256];
			WCHAR lpwzFileName[256];
			WCHAR lpwzFilePath[256];
			//����
			WCHAR lpwzHostFileName[256];

			memset(lpwzObjType,0,sizeof(lpwzObjType));
			memset(lpwzAttachDevice,0,sizeof(lpwzAttachDevice));
			memset(lpwzFileName,0,sizeof(lpwzFileName));
			memset(lpwzFilePath,0,sizeof(lpwzFilePath));
			memset(lpwzHostFileName,0,sizeof(lpwzHostFileName));

			if (wcsstr(KernelFilterDriver->KernelFilterDriverInfo[i].HostFileName,L"\\FileSystem\\") != NULL)
			{
				wcscat(lpwzObjType,L"File");
			}
			if (wcsstr(KernelFilterDriver->KernelFilterDriverInfo[i].HostFileName,L"\\Driver\\") != NULL)
			{
				wcsncat(lpwzObjType,KernelFilterDriver->KernelFilterDriverInfo[i].HostFileName+wcslen(L"\\Driver\\"),wcslen(KernelFilterDriver->KernelFilterDriverInfo[i].HostFileName)-wcslen(L"\\Driver\\"));
			}
			wsprintfW(lpwzAttachDevice,L"0x%X",KernelFilterDriver->KernelFilterDriverInfo[i].ulAttachDevice);
			wsprintfW(lpwzFileName,L"%ws",KernelFilterDriver->KernelFilterDriverInfo[i].FileName);
			wsprintfW(lpwzFilePath,L"%ws",KernelFilterDriver->KernelFilterDriverInfo[i].FilePath);
			wsprintfW(lpwzHostFileName,L"%ws",KernelFilterDriver->KernelFilterDriverInfo[i].HostFileName);

			m_list->InsertItem(i,lpwzObjType,RGB(77,77,77));
			m_list->SetItemText(i,1,lpwzFileName);
			m_list->SetItemText(i,2,lpwzAttachDevice);
			m_list->SetItemText(i,3,lpwzHostFileName);
			m_list->SetItemText(i,4,lpwzFilePath);
		}
	}
	WCHAR lpwzTextOut[100];
	memset(lpwzTextOut,0,sizeof(lpwzTextOut));
	wsprintfW(lpwzTextOut,L"��������ɨ����ϣ����� %d ������",i);
	SetDlgItemTextW(m_hWnd,ID,lpwzTextOut);

}
void DeleteSelectFilterDriver(HWND hWnd,ULONG ID,CClrListCtrl *m_list)
{
	DWORD dwReadByte;
	CString Num;
	CString FunctionStr;

	POSITION pos = m_list->GetFirstSelectedItemPosition(); //�ж��б�����Ƿ���ѡ����
	int Item = m_list->GetNextSelectedItem(pos); //���б��б�ѡ�����һ������ֵ���浽������

	FunctionStr.Format(L"%s",m_list->GetItemText(Item,3));

	WCHAR lpwzFilter[256];

	memset(lpwzFilter,0,sizeof(lpwzFilter));
	wcscat(lpwzFilter,FunctionStr);
	ReadFile((HANDLE)INIT_KERNEL_FILTER_DRIVER,lpwzFilter,wcslen(lpwzFilter),&dwReadByte,0);

	WCHAR lpwzNum1[20];
	WCHAR lpwzNum[20];
	char lpszNum[20];

	WCHAR lpwzDeviceObj[20];
	//ȡ�豸
	Num.Format(L"%s",m_list->GetItemText(Item,2));

	memset(lpwzNum,0,sizeof(lpwzNum));

	wcscat(lpwzNum,Num);

	for (int i=0;i< KernelFilterDriver->ulCount;i++)
	{
		memset(lpwzDeviceObj,0,sizeof(lpwzDeviceObj));
		wsprintfW(lpwzDeviceObj,L"0x%X",KernelFilterDriver->KernelFilterDriverInfo[i].ulAttachDevice);
		if (_wcsnicmp(lpwzNum,lpwzDeviceObj,wcslen(lpwzDeviceObj)) == 0)
		{
			ReadFile((HANDLE)DELETE_KERNEL_FILTER_DRIVER,0,KernelFilterDriver->KernelFilterDriverInfo[i].ulAttachDevice,&dwReadByte,0);
			break;
		}
	}
	m_list->DeleteAllItems();
	QueryFilterDriver(hWnd,ID,m_list);
}