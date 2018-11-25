#include "stdafx.h"
#include "3600safeview.h"
#include "HipsLog.h"

void GetSaveFileLog(WCHAR *lpwzFilePath)
{
	CString			m_path;
	WCHAR lpwzForMat[256];
	DWORD dwReadByte;

	memset(lpwzFilePath,0,sizeof(lpwzFilePath));
	memset(lpwzForMat,0,sizeof(lpwzForMat));

	//�ָ���PEPROCESS����Ȼ�޷���Ϊ������win7���޷��򿪶Ի���
	ReadFile((HANDLE)EXIT_PROCESS,0,0,&dwReadByte,0);

	CFileDialog dlg( FALSE,L"txt",0, OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY,L"�����ļ�|*.*");
	dlg.m_ofn.lpstrTitle= L"���������־���ļ�";
	if ( dlg.DoModal() == IDOK )
	{
		m_path = dlg.GetPathName();
		wsprintfW(lpwzFilePath,L"%ws",m_path);

// 		wsprintfW(lpwzForMat,L"�����ɹ�����¼�ļ�������:%ws",m_path);
// 		MessageBoxW(lpwzForMat,0,MB_ICONWARNING);
	}
	//�ټ��������Լ��Ľ���
	char lpszInit[4];
	memset(lpszInit,0,sizeof(lpszInit));
	ReadFile((HANDLE)SAFE_SYSTEM,lpszInit,4,&dwReadByte,0);
}
VOID HipsLog(HWND m_hWnd,ULONG ID,CClrListCtrl *m_list,int Type)
{
	DWORD dwReadByte;
	int i=0;
	int ItemNum = m_list->GetItemCount();
	WCHAR lpwzFilePath[256];
	BOOL bIsSaveLogFile = FALSE;

	SetDlgItemText(m_hWnd,ID,L"����ɨ�������־�����Ժ�...");
	LogDefenseInfo = (PLOGDEFENSE)VirtualAlloc(0, (sizeof(LOGDEFENSE)+sizeof(LOGDEFENSE_INFORMATION))*1025,MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (LogDefenseInfo)
	{
		ReadFile((HANDLE)LIST_LOG,LogDefenseInfo,(sizeof(LOGDEFENSE)+sizeof(LOGDEFENSE_INFORMATION))*1025,&dwReadByte,0);

		//������־��
		if (Type == 1)
		{
			GetSaveFileLog(lpwzFilePath);
			bIsSaveLogFile = TRUE;
		}
		if (LogDefenseInfo->ulCount >= 1024)
		{
			if (MessageBoxW(m_hWnd,L"������־�Ѿ����ڴ棬�޷��ڼ�¼���Ƿ񵼳����ļ���",0,MB_YESNO | MB_ICONWARNING) == IDYES)
			{
				GetSaveFileLog(lpwzFilePath);
				bIsSaveLogFile = TRUE;
			}
		}
		for (i=0;i< LogDefenseInfo->ulCount;i++)
		{
			WCHAR lpwzTextOut[100];
			memset(lpwzTextOut,0,sizeof(lpwzTextOut));
			wsprintfW(lpwzTextOut,L"���� %d �����ݣ�����ɨ��� %d �������Ժ�...",LogDefenseInfo->ulCount,i);
			SetDlgItemTextW(m_hWnd,ID,lpwzTextOut);

			WCHAR lpwzType[100];
			WCHAR lpwzPID[100];
			WCHAR lpwzInherited[100];
			WCHAR lpwzProName[256];
			WCHAR lpwzEProcess[100];
			WCHAR lpwzMoreEvents[1024];

			memset(lpwzType,0,sizeof(lpwzType));
			memset(lpwzPID,0,sizeof(lpwzPID));
			memset(lpwzInherited,0,sizeof(lpwzInherited));
			memset(lpwzProName,0,sizeof(lpwzProName));
			memset(lpwzEProcess,0,sizeof(lpwzEProcess));
			memset(lpwzMoreEvents,0,sizeof(lpwzMoreEvents));

			if (LogDefenseInfo->LogDefense[i].Type == 1)
			{
				wcscat(lpwzType,L"���̽���");
				if (LogDefenseInfo->LogDefense[i].ulPID)
					wsprintfW(lpwzMoreEvents,L"����[[%d]��ͼ���� A�ܵ��Է������ѱ���ֹ",LogDefenseInfo->LogDefense[i].ulPID);
				else
					wcscat(lpwzMoreEvents,L"(���˳�)����[Unknown]��ͼ���� A�ܵ��Է������ѱ���ֹ");

			}else if (LogDefenseInfo->LogDefense[i].Type == 2)
			{
				wcscat(lpwzType,L"��������");

				if (LogDefenseInfo->LogDefense[i].ulPID)
					wsprintfW(lpwzMoreEvents,L"����[[%d]��ͼ��������(��ͨ)",LogDefenseInfo->LogDefense[i].ulPID);
				else
					wcscat(lpwzMoreEvents,L"(���˳�)����[Unknown]��ͼ��������(��ͨ)");
			}
			else if (LogDefenseInfo->LogDefense[i].Type == 3)
			{
				wcscat(lpwzType,L"���̴���");

				if (LogDefenseInfo->LogDefense[i].ulPID)
					wsprintfW(lpwzMoreEvents,L"����[%d]�������̣����ɵ�ľ����Ϊ(��������Լ������Ļ�)/(Σ��)",LogDefenseInfo->LogDefense[i].ulPID);
				else
					wcscat(lpwzMoreEvents,L"(���˳�)����[Unknown]�������̣����ɵ�ľ����Ϊ(��������Լ������Ļ�)/(Σ��)");

			}else if (LogDefenseInfo->LogDefense[i].Type == 4)
			{
				wcscat(lpwzType,L"DLLע��");

				if (LogDefenseInfo->LogDefense[i].ulPID)
					wsprintfW(lpwzMoreEvents,L"����[%d]��ͼע��DLLģ�鵽 A�ܵ��Է������ѱ��ܾ�",LogDefenseInfo->LogDefense[i].ulPID);
			}
			else if (LogDefenseInfo->LogDefense[i].Type == 5)
			{
				wcscat(lpwzType,L"ϵͳ���񴴽����޸�");

				if (LogDefenseInfo->LogDefense[i].ulPID)
					wsprintfW(lpwzMoreEvents,L"����[%d]��ͼ����/�޸�һ������:%ws",LogDefenseInfo->LogDefense[i].ulPID,LogDefenseInfo->LogDefense[i].lpwzMoreEvents);
			}
			wsprintfW(lpwzPID,L"%d",LogDefenseInfo->LogDefense[i].ulPID);
			wsprintfW(lpwzInherited,L"%d",LogDefenseInfo->LogDefense[i].ulInheritedFromProcessId);

			//----------------------
// 			WCHAR lpwzWinDir[100];
// 			WCHAR lpwzSysDisk[100];
// 			memset(lpwzWinDir,0,sizeof(lpwzWinDir));
// 			memset(lpwzSysDisk,0,sizeof(lpwzSysDisk));
// 
// 			GetWindowsDirectoryW(lpwzWinDir,sizeof(lpwzWinDir));
// 			memcpy(lpwzSysDisk,lpwzWinDir,6);
// 			wcsncat(lpwzFilePath,lpwzSysDisk,wcslen(lpwzSysDisk));
// 
// 			if (wcsstr(LogDefenseInfo->LogDefense[i].lpwzFilePath,L"\\Device\\HarddiskVolume1\\") != 0)
// 			{
// 				wcsncat(lpwzFilePath+wcslen(lpwzSysDisk),
// 					LogDefenseInfo->LogDefense[i].lpwzFilePath+wcslen(L"\\Device\\HarddiskVolume1\\"),
// 					wcslen(LogDefenseInfo->LogDefense[i].lpwzFilePath)-wcslen(L"\\Device\\HarddiskVolume1\\")
// 					);
// 			}
			if (strlen(LogDefenseInfo->LogDefense[i].lpszProName))
			{
				MultiByteToWideChar(CP_ACP,0,LogDefenseInfo->LogDefense[i].lpszProName,strlen(LogDefenseInfo->LogDefense[i].lpszProName),lpwzProName,sizeof(lpwzProName));
			}else
				wcscat(lpwzProName,L"Unknown");

			WCHAR lpwzCreateProcess[260];
			memset(lpwzCreateProcess,0,sizeof(lpwzCreateProcess));
			wcscat(lpwzCreateProcess,LogDefenseInfo->LogDefense[i].lpwzCreateProcess);

			char lpszDLLPath[256] = {0};
			WCHAR lpwzMd5[256] = {0};
			WCHAR lpwzTrue[256] = {0};

			memset(lpszDLLPath,0,sizeof(lpszDLLPath));
			memset(lpwzMd5,0,sizeof(lpwzMd5));
			memset(lpwzTrue,0,sizeof(lpwzTrue));
			if (LogDefenseInfo->LogDefense[i].Type == 3 ||
				LogDefenseInfo->LogDefense[i].Type == 4)
			{
				WideCharToMultiByte( CP_ACP,
					0,
					lpwzCreateProcess,
					-1,
					lpszDLLPath,
					wcslen(lpwzCreateProcess)*2,
					NULL,
					NULL
					);
				FILE * fp=fopen(lpszDLLPath,"rb");
				if(fp)
				{
					MD5VAL val;
					val = md5File(fp);
					wsprintfW(lpwzMd5,L"%08x%08x%08x%08x",conv(val.a),conv(val.b),conv(val.c),conv(val.d));
					FileVerify(lpszDLLPath,lpwzMd5,lpwzTrue);

					fclose(fp);
				}
			}

			wsprintfW(lpwzEProcess,L"0x%08X",LogDefenseInfo->LogDefense[i].EProcess);

			WCHAR lpwzLog[1024];
			CHAR lpszLog[2024];
			//�����ļ�
			if (bIsSaveLogFile)
			{
				memset(lpwzLog,0,sizeof(lpwzLog));
				memset(lpszLog,0,sizeof(lpszLog));
				wsprintfW(lpwzLog,L"����ID:%ws\r\n������ID:%ws\r\nӳ��·��:%ws\r\nEPROCESS:%ws\r\n�¼�:%ws\r\n�¼���ϸ����:%ws\r\n����������:%ws\r\n��֤�Ƿ�ϵͳԭ���ļ�:%ws\r\n\r\n",
								   lpwzPID,
								   lpwzInherited,
								   lpwzProName,
								   lpwzEProcess,
								   lpwzType,
								   lpwzMoreEvents,
								   lpwzCreateProcess,
								   lpwzTrue);

				WideCharToMultiByte( CP_ACP,
					0,
					lpwzLog,
					-1,
					lpszLog,
					wcslen(lpwzLog)*2,
					NULL,
					NULL
					);
				SaveToFile(lpszLog,lpwzFilePath);
			}
			if (!wcslen(lpwzProName))
			{
				wcscat_s(lpwzProName,L"-");
			}
			if (!wcslen(lpwzMoreEvents))
			{
				wcscat_s(lpwzMoreEvents,L"-");
			}
			if (!wcslen(lpwzCreateProcess))
			{
				wcscat_s(lpwzCreateProcess,L"-");
			}
			if (_wcsnicmp(lpwzTrue,L"�޷�ȷ���ļ���Դ",wcslen(L"�޷�ȷ���ļ���Դ")) == 0)
			{
				m_list->InsertItem(i,lpwzPID,RGB(255,20,147));

			}else
			{
				if (LogDefenseInfo->LogDefense[i].Type == 5)  //�����޸ķ���Ҫͻ��
					m_list->InsertItem(i,lpwzPID,RGB(255,20,147));
				else
					m_list->InsertItem(i,lpwzPID,RGB(77,77,77));
			}

			m_list->SetItemText(i,1,lpwzInherited);
			m_list->SetItemText(i,2,lpwzProName);
			m_list->SetItemText(i,3,lpwzEProcess);
			m_list->SetItemText(i,4,lpwzType);
			m_list->SetItemText(i,5,lpwzMoreEvents);
			m_list->SetItemText(i,6,lpwzCreateProcess);
			m_list->SetItemText(i,7,lpwzTrue);
		}
		VirtualFree(LogDefenseInfo,(sizeof(LOGDEFENSE)+sizeof(LOGDEFENSE_INFORMATION))*1025,MEM_RESERVE | MEM_COMMIT);
	}
	WCHAR lpwzTextOut[100];
	memset(lpwzTextOut,0,sizeof(lpwzTextOut));
	wsprintfW(lpwzTextOut,L"������־ɨ����ϣ����� %d ������",i);
	SetDlgItemTextW(m_hWnd,ID,lpwzTextOut);

	//����ǵ�����־�Ļ����Ͳ���Ҫ�����
	if (Type == 0 &&
		bIsSaveLogFile)
	{
		ReadFile((HANDLE)CLEAR_LIST_LOG,0,0,&dwReadByte,0);
		bIsSaveLogFile = FALSE;
	}
}
void ClearListLog(CClrListCtrl *m_list)
{
	DWORD dwReadByte;
	m_list->DeleteAllItems();
	ReadFile((HANDLE)CLEAR_LIST_LOG,0,0,&dwReadByte,0);
}
void ConnectScan(HWND m_hWnd)
{
	if (MessageBoxW(m_hWnd,L"�˹��ܽ���ϵͳ�������Զ��ռ�ȫ���Ŀ�����Ϊ\r\n\r\nϵͳ�����������鿴������־������\r\n\r\n�Ƿ�������",0,MB_YESNO | MB_ICONWARNING) == IDYES)
	{
		if (InstallDepthServicesScan("A-Protect"))
		{
			DWORD dwReadByte;
			ReadFile((HANDLE)EXIT_PROCESS,0,0,&dwReadByte,0);
			Sleep(2000);
			ShutdownWindows(EWX_REBOOT | EWX_FORCE);
			ExitProcess(0);
		}
	}
}
void SaveToFile(HWND m_hWnd,ULONG ID,CClrListCtrl *m_list)
{
	m_list->DeleteAllItems();
	HipsLog(m_hWnd,ID,m_list,1);
}