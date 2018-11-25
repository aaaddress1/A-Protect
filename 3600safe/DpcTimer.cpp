#include "stdafx.h"
#include "3600safeview.h"
#include "DpcTimer.h"

void QueryDpcTimer(HWND m_hWnd,ULONG ID,CClrListCtrl *m_list)
{
	DWORD dwReadByte;
	int i=0;
	int ItemNum = m_list->GetItemCount();

	SetDlgItemTextW(m_hWnd,ID,L"����ɨ��DpcTimer�����Ժ�...");

	if (DpcTimer)
	{
		VirtualFree(DpcTimer,(sizeof(MyDpcTimer)+sizeof(MyDpcTimerInfo))*MAX_DPCTIMER_COUNT*2,MEM_RESERVE | MEM_COMMIT);
		DpcTimer = 0;
	}
	DpcTimer = (PMyDpcTimer)VirtualAlloc(0,(sizeof(MyDpcTimer)+sizeof(MyDpcTimerInfo))*MAX_DPCTIMER_COUNT*2,MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (DpcTimer)
	{
		memset(DpcTimer,0,(sizeof(MyDpcTimer)+sizeof(MyDpcTimerInfo))*MAX_DPCTIMER_COUNT*2);

		ReadFile((HANDLE)LIST_DPC_TIMER,DpcTimer,(sizeof(MyDpcTimer)+sizeof(MyDpcTimerInfo))*MAX_DPCTIMER_COUNT*2,&dwReadByte,0);

		for (i=0;i< (int)DpcTimer->ulCount;i++)
		{
			WCHAR lpwzTextOut[100];
			memset(lpwzTextOut,0,sizeof(lpwzTextOut));
			wsprintfW(lpwzTextOut,L"���� %d �����ݣ�����ɨ��� %d �������Ժ�...",DpcTimer->ulCount,i);
			SetDlgItemTextW(m_hWnd,ID,lpwzTextOut);

			WCHAR lpwzKTimer[256] = {0};
			WCHAR lpwzKDpc[256] = {0};
			WCHAR lpwzPeriod[256] = {0};
			WCHAR lpwzDpcRoutineAddress[256] = {0};
			WCHAR lpwzFullSysName[256] = {0};

			memset(lpwzKTimer,0,sizeof(lpwzKTimer));
			memset(lpwzKDpc,0,sizeof(lpwzKDpc));
			memset(lpwzPeriod,0,sizeof(lpwzPeriod));
			memset(lpwzDpcRoutineAddress,0,sizeof(lpwzDpcRoutineAddress));
			memset(lpwzFullSysName,0,sizeof(lpwzFullSysName));

			MultiByteToWideChar(
				CP_ACP,
				0, 
				DpcTimer->MyTimer[i].lpszModule,
				-1, 
				lpwzFullSysName, 
				strlen(DpcTimer->MyTimer[i].lpszModule)
				);
//********************************************************************
			WCHAR lpwzDosFullPath[256];
			WCHAR lpwzWinDir[256];
			WCHAR lpwzSysDisk[256];

			memset(lpwzWinDir,0,sizeof(lpwzWinDir));
			memset(lpwzSysDisk,0,sizeof(lpwzSysDisk));
			memset(lpwzDosFullPath,0,sizeof(lpwzDosFullPath));

			GetWindowsDirectoryW(lpwzWinDir,sizeof(lpwzWinDir));
			memcpy(lpwzSysDisk,lpwzWinDir,4);

			if (wcsstr(lpwzFullSysName,L"\\??\\"))
			{
				//��ʼ����·���Ĵ���
				memset(lpwzDosFullPath,0,sizeof(lpwzDosFullPath));
				wcsncpy(lpwzDosFullPath,lpwzFullSysName+wcslen(L"\\??\\"),wcslen(lpwzFullSysName)-wcslen(L"\\??\\"));
				goto Next;
			}
			if (wcsstr(lpwzFullSysName,L"\\WINDOWS\\system32\\"))
			{
				memset(lpwzDosFullPath,0,sizeof(lpwzDosFullPath));
				wcscat(lpwzDosFullPath,lpwzSysDisk);
				wcscat(lpwzDosFullPath,lpwzFullSysName);
				//MessageBoxW(lpwzDosFullPath,lpwzFullSysName,0);
				goto Next;
			}
			if (wcsstr(lpwzFullSysName,L"\\SystemRoot\\"))
			{
				WCHAR lpwzTemp[256];
				memset(lpwzTemp,0,sizeof(lpwzTemp));
				memset(lpwzDosFullPath,0,sizeof(lpwzDosFullPath));
				wcscat(lpwzTemp,lpwzSysDisk);
				wcscat(lpwzTemp,L"\\WINDOWS\\");
				wcscat(lpwzDosFullPath,lpwzTemp);
				wcsncpy(lpwzDosFullPath+wcslen(lpwzTemp),lpwzFullSysName+wcslen(L"\\SystemRoot\\"),wcslen(lpwzFullSysName) - wcslen(L"\\SystemRoot\\"));
				goto Next;
			}
			if (!wcsstr(lpwzFullSysName,L"\\"))
			{
				memset(lpwzDosFullPath,0,sizeof(lpwzDosFullPath));
				wcscat(lpwzDosFullPath,lpwzSysDisk);
				wcscat(lpwzDosFullPath,L"\\WINDOWS\\system32\\drivers\\");
				wcscat(lpwzDosFullPath,lpwzFullSysName);
				goto Next;
			}
Next:

//********************************************************************
			wsprintfW(lpwzKTimer,L"0x%08X",DpcTimer->MyTimer[i].TimerAddress);
			wsprintfW(lpwzKDpc,L"0x%08X",DpcTimer->MyTimer[i].DpcAddress);
			wsprintfW(lpwzPeriod,L"%d",DpcTimer->MyTimer[i].Period);
			wsprintfW(lpwzDpcRoutineAddress,L"0x%08X",DpcTimer->MyTimer[i].DpcRoutineAddress);

			if (_wcsnicmp(lpwzDosFullPath,L"Unknown",wcslen(lpwzDosFullPath)) == 0)
			{
				m_list->InsertItem(i,lpwzKTimer,RGB(255,20,147));
			}else
			{
				m_list->InsertItem(i,lpwzKTimer,RGB(77,77,77));
			}
			m_list->SetItemText(i,1,lpwzKDpc);
			m_list->SetItemText(i,2,lpwzPeriod);
			m_list->SetItemText(i,3,lpwzDpcRoutineAddress);
			m_list->SetItemText(i,4,lpwzDosFullPath);
		}
	}
	WCHAR lpwzTextOut[100];
	memset(lpwzTextOut,0,sizeof(lpwzTextOut));
	wsprintfW(lpwzTextOut,L"DpcTimerɨ����ϣ����� %d ������",i);
	SetDlgItemTextW(m_hWnd,ID,lpwzTextOut);
}
VOID KillDpcTimerThread(int i)
{
	DWORD dwReadByte;

	ReadFile((HANDLE)KILL_DPC_TIMER,0,DpcTimer->MyTimer[i].TimerAddress,&dwReadByte,0);

	return;
}
void KillDpcTimer(HWND m_hWnd,ULONG ID,CClrListCtrl *m_list)
{
	DWORD dwReadByte;
	CString DpcTimerNum;
	WCHAR lpwzAdd[50];

	POSITION pos = m_list->GetFirstSelectedItemPosition(); //�ж��б�����Ƿ���ѡ����
	int Item = m_list->GetNextSelectedItem(pos); //���б��б�ѡ�����һ������ֵ���浽������

	//ȡ���
	DpcTimerNum.Format(L"%s",m_list->GetItemText(Item,0));

// 	memset(lpwzNum,0,sizeof(lpwzNum));
// 	wcscat(lpwzNum,DpcTimerNum);
// 
// 	MessageBoxW(0,lpwzNum,L"Ŀ��",0);

// 	WideCharToMultiByte (CP_OEMCP,NULL,lpwzNum,-1,lpszNum,wcslen(lpwzNum),NULL,FALSE);

	for (int i = 0;i< (int)DpcTimer->ulCount;i++)
	{
		memset(lpwzAdd,0,sizeof(lpwzAdd));
		wsprintfW(lpwzAdd,L"0x%08X",DpcTimer->MyTimer[i].TimerAddress);
		//MessageBoxW(0,lpwzAdd,DpcTimerNum,0);

		if (_wcsicmp(lpwzAdd,DpcTimerNum) == 0)
		{
// 			MessageBoxW(0,DpcTimerNum,L"ȷ��",0);

			HANDLE hThread = CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)KillDpcTimerThread,(PVOID)i,0,NULL);
			WaitForSingleObject(hThread,-1);
			break;
		}
	}
	//ɾ����ǰѡ���һ��
	m_list->DeleteAllItems();
	QueryDpcTimer(m_hWnd,ID,m_list);
}
void CopyDpcTimerDataToClipboard(HWND m_hWnd,CClrListCtrl *m_list)
{
	CString FunctionName;
	int ItemNum = m_list->GetItemCount();
	POSITION pos = m_list->GetFirstSelectedItemPosition(); //�ж��б�����Ƿ���ѡ����
	int Item = m_list->GetNextSelectedItem(pos); //���б��б�ѡ�����һ������ֵ���浽������

	FunctionName.Format(L"%s",m_list->GetItemText(Item,4));

	WCHAR lpwzFunctionName[260];

	memset(lpwzFunctionName,0,sizeof(lpwzFunctionName));
	wcscat_s(lpwzFunctionName,FunctionName);
	CHAR lpszFunctionName[1024];
	char *lpString = NULL;

	memset(lpwzFunctionName,0,sizeof(lpwzFunctionName));
	memset(lpszFunctionName,0,sizeof(lpszFunctionName));
	wcscat_s(lpwzFunctionName,FunctionName);
	WideCharToMultiByte( CP_ACP,
		0,
		lpwzFunctionName,
		-1,
		lpszFunctionName,
		wcslen(lpwzFunctionName)*2,
		NULL,
		NULL
		);
	lpString = setClipboardText(lpszFunctionName);
	if (lpString)
	{
		MessageBoxW(m_hWnd,L"�����ɹ���",0,MB_ICONWARNING);
	}
}