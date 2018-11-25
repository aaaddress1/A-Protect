#include "stdafx.h"
#include "3600safeview.h"
#include "Services.h"
#include   "winsvc.h"


VOID QueryServiceRunStatus(WCHAR *lpwzSrvName,WCHAR *lpwzSrvStatus)
{
	LPENUM_SERVICE_STATUS st;//EnumServicesStatus�������ܵ����� 
	ULONG bytesNeeded;
	DWORD nService=0; //�������
	int i=0;

	SC_HANDLE sc = OpenSCManagerW(NULL,NULL,SC_MANAGER_ENUMERATE_SERVICE); 
	if (!sc)
	{
		return;
	}
	//��ȡ��Ҫ�Ļ������ֽ���
	EnumServicesStatusW(sc,SERVICE_WIN32,SERVICE_STATE_ALL, NULL,0,&bytesNeeded,&nService,NULL);
	//���뻺����
	st=(LPENUM_SERVICE_STATUS)malloc(bytesNeeded);
	if (!st)
	{
		CloseServiceHandle(sc);
		return;
	}
	//��ʽö��
	if(EnumServicesStatusW(sc,SERVICE_WIN32,SERVICE_STATE_ALL, (LPENUM_SERVICE_STATUS)st,bytesNeeded,&bytesNeeded,&nService,NULL))
	{
		for( i=0;i<nService;i++)
		{
			if (_wcsnicmp(st[i].lpServiceName,lpwzSrvName,wcslen(lpwzSrvName)) == 0)
			{
				bIsHideServices = TRUE;

				switch(st[i].ServiceStatus.dwCurrentState)
				{
				case(SERVICE_RUNNING):
					wcscat(lpwzSrvStatus,L"������");
					break;
				case(SERVICE_STOPPED):
					wcscat(lpwzSrvStatus,L"��ֹͣ");
					break; 
				case(SERVICE_STOP_PENDING):
					wcscat(lpwzSrvStatus,L"��ֹͣ");
					break;
				case(SERVICE_START_PENDING ):
					wcscat(lpwzSrvStatus,L"������");
					break;
				case(SERVICE_CONTINUE_PENDING):
					wcscat(lpwzSrvStatus,L"������");
					break; 
				case(SERVICE_PAUSE_PENDING ):
					wcscat(lpwzSrvStatus,L"��ͣ��");
					break; 
				case(SERVICE_PAUSED ):
					wcscat(lpwzSrvStatus,L"����ͣ");
					break; 
				default:
					wcscat(lpwzSrvStatus,L"δ֪״̬");
					break; 
				}
				break;
			}
		}
	}
	delete st;
	CloseServiceHandle(sc);
}
//QueryUserAgent(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run","3600safe",lpsz3600safeRunKey);
VOID QueryServiceDescription(WCHAR *lpwzSrvName,WCHAR *lpwzDescription)
{
	SC_HANDLE hService;
	ULONG bytesNeeded;
	LPBYTE descriptionStr=NULL;

	SC_HANDLE sc = OpenSCManagerW(NULL,NULL,SC_MANAGER_ENUMERATE_SERVICE); 
	if (!sc)
	{
		return;
	}
	hService = OpenServiceW(sc,lpwzSrvName,SERVICE_QUERY_CONFIG|SERVICE_ALL_ACCESS);
	if(!hService)
	{
		CloseServiceHandle(sc);
		return;
	}
	QueryServiceConfig2W(hService,SERVICE_CONFIG_DESCRIPTION,NULL,0,&bytesNeeded);
	if (!bytesNeeded)
	{
		CloseServiceHandle(sc);
		return;
	}
	//���뻺����
	descriptionStr = (LPBYTE)malloc(bytesNeeded);
	if (!descriptionStr)
	{
		CloseServiceHandle(hService);
		CloseServiceHandle(sc);
		return;
	}
	//��ʽ��ѯ
	memset(descriptionStr,0,bytesNeeded);
	if(QueryServiceConfig2W(hService,SERVICE_CONFIG_DESCRIPTION,descriptionStr,bytesNeeded,&bytesNeeded))
	{
		if (wcslen((WCHAR *)descriptionStr) > 4)
		{
			memcpy(lpwzDescription,descriptionStr+4,bytesNeeded);
		}

	}
	delete descriptionStr;
	CloseServiceHandle(hService);
	CloseServiceHandle(sc);
	return;

// 	WCHAR lpwzKey[256];
// 
// 	memset(lpwzKey,0,sizeof(lpwzKey));
// 	wsprintfW(lpwzKey,L"SYSTEM\\CurrentControlSet\\services\\%ws",lpwzSrvName);
// 	QueryUserAgent(HKEY_LOCAL_MACHINE,lpwzKey,L"Description",lpwzDescription);
}
VOID QueryServiceRunType(WCHAR *lpwzSrvName,WCHAR *lpwzSrvType)
{
	SC_HANDLE hService;
	ULONG bytesNeeded;
	LPQUERY_SERVICE_CONFIG pServiceConfig;

	SC_HANDLE sc = OpenSCManagerW(NULL,NULL,SC_MANAGER_ENUMERATE_SERVICE); 
	if (!sc)
	{
		return;
	}
	hService = OpenServiceW(sc,lpwzSrvName,SERVICE_QUERY_CONFIG|SERVICE_ALL_ACCESS);
	if(!hService)
	{
		CloseServiceHandle(sc);
		return;
	}

	//��ȡ��Ҫ�Ļ���������
	QueryServiceConfigW(hService,NULL,0,&bytesNeeded);
	//���뻺����
	pServiceConfig=(LPQUERY_SERVICE_CONFIG)malloc(bytesNeeded);
	if (!pServiceConfig)
	{
		CloseServiceHandle(hService);
		CloseServiceHandle(sc);
		return;
	}
	//��ʽ��ѯ
	if(!QueryServiceConfigW(hService,pServiceConfig,bytesNeeded,&bytesNeeded))
	{
		delete pServiceConfig;
		CloseServiceHandle(hService);
		CloseServiceHandle(sc);
		return;
	}
	//������������
	switch(pServiceConfig->dwStartType)
	{ 
	case(SERVICE_AUTO_START):
		wcscat(lpwzSrvType,L"�Զ�");
		break;
	case(SERVICE_BOOT_START):
		wcscat(lpwzSrvType,L"����");
		break;
	case(SERVICE_DEMAND_START):
		wcscat(lpwzSrvType,L"�ֶ�");
		break;
	case(SERVICE_DISABLED):
		wcscat(lpwzSrvType,L"�ѽ���");
		break;
	case(SERVICE_SYSTEM_START):
		wcscat(lpwzSrvType,L"ϵͳ");
		break;
	default:
		wcscat(lpwzSrvType,L"δ֪");
		break;
	}

	delete pServiceConfig;
	CloseServiceHandle(hService);
	CloseServiceHandle(sc);

	return;
}
BOOL IsWin32ApplicationServices(WCHAR *lpwzServiceName)
{
	HKEY hKey = NULL;
	WCHAR lpwzKey[256];
	BOOL bRetOK = FALSE;
	DWORD dwType = 0;
	DWORD dwData = 0;

	memset(lpwzKey,0,sizeof(lpwzKey));
	wsprintfW(lpwzKey,L"SYSTEM\\CurrentControlSet\\Services\\%ws",lpwzServiceName);

	if(RegOpenKeyExW(HKEY_LOCAL_MACHINE,lpwzKey,0,KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
	{
		DWORD dwSize = sizeof(dwData);

		if (RegQueryValueExW(hKey,L"Type", NULL, &dwType, (LPBYTE) &dwData, &dwSize) == ERROR_SUCCESS)
		{
			if (dwType == REG_DWORD)
			{
				if (dwData == 32 ||
					dwData == 16)
				{
					bRetOK = TRUE;
				}
			}
		}
		RegCloseKey(hKey);
	}
	//WCHAR lpwzForMat[256] = {0};
	//wsprintfW(lpwzForMat,L"%ws %d",lpwzServiceName,dwData);
	//MessageBoxW(0,lpwzKey,lpwzForMat,0);
	return bRetOK;
}
VOID QueryServices(HWND m_hWnd,ULONG ID,CClrListCtrl *m_list,int IntType)
{
	DWORD dwReadByte;
	int i=0;
	int ItemNum = m_list->GetItemCount();

	SetDlgItemTextW(m_hWnd,ID,L"����ɨ��ϵͳ�������Ժ�...");

	if (ServicesRegistry)
	{
		VirtualFree(ServicesRegistry,(sizeof(SERVICESREGISTRY)+sizeof(SERVICESREGISTRY_INFORMATION))*1025,MEM_RESERVE | MEM_COMMIT);
		ServicesRegistry = 0;
	}
	ServicesRegistry = (PSERVICESREGISTRY)VirtualAlloc(0, (sizeof(SERVICESREGISTRY)+sizeof(SERVICESREGISTRY_INFORMATION))*1025,MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (ServicesRegistry)
	{
		memset(ServicesRegistry,0,(sizeof(SERVICESREGISTRY)+sizeof(SERVICESREGISTRY_INFORMATION))*1025);
		if (IntType == 0)
		{
			ReadFile((HANDLE)LIST_SERVICES,ServicesRegistry,(sizeof(SERVICESREGISTRY)+sizeof(SERVICESREGISTRY_INFORMATION))*1025,&dwReadByte,0);
		}
		else
		{
			ReadFile((HANDLE)LIST_DEPTH_SERVICES,ServicesRegistry,(sizeof(SERVICESREGISTRY)+sizeof(SERVICESREGISTRY_INFORMATION))*1025,&dwReadByte,0);
			if (ServicesRegistry->ulCount < 10)
			{
				if (MessageBoxW(m_hWnd,L"���ɨ����Ҫ����������������Ƿ������",0,MB_YESNO | MB_ICONWARNING) == IDYES)
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
		}
		for ( i=0;i<=ServicesRegistry->ulCount;i++)
		{
			WCHAR lpwzTextOut[100];
			memset(lpwzTextOut,0,sizeof(lpwzTextOut));
			wsprintfW(lpwzTextOut,L"���� %d �����ݣ�����ɨ��� %d �������Ժ�...",ServicesRegistry->ulCount,i);
			SetDlgItemTextW(m_hWnd,ID,lpwzTextOut);

			WCHAR lpwzSrvName[256];
			WCHAR lpwzSrvStatus[256];
			WCHAR lpwzSrvType[256];
			WCHAR lpwzDescription[256];
			WCHAR lpwzImageName[256];
			WCHAR lpwzDLLPath[256];
			WCHAR lpwzTrue[100];

			memset(lpwzSrvName,0,sizeof(lpwzSrvName));
			memset(lpwzImageName,0,sizeof(lpwzImageName));
			memset(lpwzDLLPath,0,sizeof(lpwzDLLPath));
			memset(lpwzSrvStatus,0,sizeof(lpwzSrvStatus));
			memset(lpwzSrvType,0,sizeof(lpwzSrvType));
			memset(lpwzDescription,0,sizeof(lpwzDescription));
			memset(lpwzTrue,0,sizeof(lpwzTrue));
			wcscat(lpwzTrue,L"-");

			wcscat(lpwzSrvName,ServicesRegistry->SrvReg[i].lpwzSrvName);
			if (wcslen(lpwzSrvName) < 1)
			{
				continue;
			}
			wcscat(lpwzImageName,ServicesRegistry->SrvReg[i].lpwzImageName);
			if (wcslen(lpwzImageName) < 1)
			{
				wcscat(lpwzImageName,L"-");
			}
			wcscat(lpwzDLLPath,ServicesRegistry->SrvReg[i].lpwzDLLPath);
			if (wcslen(lpwzDLLPath) < 10)
			{
				wcscat(lpwzDLLPath,L"-");
				goto Next;
			}
			WCHAR lpwzCheck[100];
			WCHAR lpwzWinDir[256];
			WCHAR lpwzSysDisk[10];
			WCHAR lpwzDosPath[256];

			memset(lpwzTrue,0,sizeof(lpwzTrue));
			memset(lpwzWinDir,0,sizeof(lpwzWinDir));
			memset(lpwzSysDisk,0,sizeof(lpwzSysDisk));
			memset(lpwzCheck,0,sizeof(lpwzCheck));
			memset(lpwzDosPath,0,sizeof(lpwzDosPath));

			wcsncat(lpwzCheck,lpwzDLLPath,wcslen(L"%SystemRoot%"));

			GetWindowsDirectoryW(lpwzWinDir,sizeof(lpwzWinDir));
			wcsncat(lpwzSysDisk,lpwzWinDir,wcslen(lpwzWinDir));

			if (_wcsnicmp(lpwzCheck,L"%SystemRoot%",wcslen(L"%SystemRoot%")) == 0)
			{

				wcscat(lpwzDosPath,lpwzSysDisk);
				wcsncat(lpwzDosPath+wcslen(lpwzSysDisk),lpwzDLLPath+wcslen(L"%SystemRoot%"),wcslen(lpwzDLLPath)-wcslen(L"%SystemRoot%"));
				memset(lpwzDLLPath,0,sizeof(lpwzDLLPath));
				wcscat(lpwzDLLPath,lpwzDosPath);
				//MessageBoxW(lpwzDosPath,lpwzDLLPath,0);
			}
			//MessageBoxW(lpwzDLLPath,lpwzCheck,0);
			//////////////////////////
			//����md5
			char lpszFullPath[5024] = {0};
			WCHAR lpwzFileServices[256] = {0};

			memset(lpszFullPath,0,sizeof(lpszFullPath));
			memset(lpwzFileServices,0,sizeof(lpwzFileServices));

			WideCharToMultiByte( CP_ACP,
				0,
				lpwzDLLPath,
				-1,
				lpszFullPath,
				wcslen(lpwzDLLPath)*2,
				NULL,
				NULL);
			FILE * fp=fopen(lpszFullPath,"rb");
			if(fp)
			{
				MD5VAL val;
				val = md5File(fp);
				wsprintfW(lpwzFileServices,L"%08x%08x%08x%08x",conv(val.a),conv(val.b),conv(val.c),conv(val.d));
				FileVerify(lpszFullPath,lpwzFileServices,lpwzTrue);
				fclose(fp);
			}
Next:
			////////////////////////////
			QueryServiceRunStatus(lpwzSrvName,lpwzSrvStatus);  //��ѯ����״̬
			if (wcslen(lpwzSrvStatus) < 1)
			{
				wcscat(lpwzSrvStatus,L"-");
			}
			if (bIsHideServices == FALSE)
			{
				//�����ע����Ƿ���ϵͳ��������
				//type=16 or type=32
				if (!IsWin32ApplicationServices(lpwzSrvName))
				{
					//����������ķ��񣬾�Ҫ�ָ���־
					bIsHideServices = TRUE;
				}
			}

			QueryServiceRunType(lpwzSrvName,lpwzSrvType);   //��ѯ��������
			if (wcslen(lpwzSrvType) < 1)
			{
				wcscat(lpwzSrvType,L"-");
			}
			memset(lpwzDescription,0,sizeof(lpwzDescription));
			QueryServiceDescription(lpwzSrvName,lpwzDescription);   //��ѯ��������
			if (wcslen(lpwzDescription) < 1)
			{
				wcscat(lpwzDescription,L"-");
			}
			if (wcsstr(lpwzSrvName,L"���´���"))
			{
				m_list->InsertItem(i,lpwzSrvStatus,RGB(255,20,147));
				goto InsertData;
			}
			if (bIsHideServices == FALSE)
			{
				//���ط���
				m_list->InsertItem(i,lpwzSrvStatus,RGB(255,20,147));
				wcscat(lpwzSrvName,L"(�������ط���)");
			}
			else
			{
				if (_wcsnicmp(lpwzSrvStatus,L"������",wcslen(L"������")) == 0)
				{
					m_list->InsertItem(i,lpwzSrvStatus,RGB(238,118,0));

				}else
					m_list->InsertItem(i,lpwzSrvStatus,RGB(77,77,77));
			}
InsertData:
			bIsHideServices = FALSE; //�ָ����ر�־

			m_list->SetItemText(i,1,lpwzSrvType);
			m_list->SetItemText(i,2,lpwzSrvName);
			m_list->SetItemText(i,3,lpwzDescription);
			m_list->SetItemText(i,4,lpwzImageName);
			m_list->SetItemText(i,5,lpwzDLLPath);
			m_list->SetItemText(i,6,lpwzTrue);

		}
		VirtualFree(ServicesRegistry,(sizeof(SERVICESREGISTRY)+sizeof(SERVICESREGISTRY_INFORMATION))*1025,MEM_RESERVE | MEM_COMMIT);
	}
	WCHAR lpwzTextOut[100];
	memset(lpwzTextOut,0,sizeof(lpwzTextOut));
	wsprintfW(lpwzTextOut,L"ϵͳ����ɨ����ϣ����� %d ������",i);
	SetDlgItemTextW(m_hWnd,ID,lpwzTextOut);

}
void GetDepthServicesList(HWND m_hWnd,ULONG ID,CClrListCtrl *m_list,int IntType)
{
	m_list->DeleteAllItems();
	QueryServices(m_hWnd,ID,m_list,1);
}
VOID __stdcall DoStartSvc(WCHAR *lpwzSrvName)
{
	SERVICE_STATUS_PROCESS ssStatus; 
	DWORD dwOldCheckPoint; 
	DWORD dwStartTickCount;
	DWORD dwWaitTime;
	DWORD dwBytesNeeded;
	SC_HANDLE schSCManager;
	SC_HANDLE schService;
	// Get a handle to the SCM database. 

	schSCManager = OpenSCManagerW( 
		NULL,                    // local computer
		NULL,                    // servicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 

	if (NULL == schSCManager) 
	{
		printf("OpenSCManager failed (%d)\n", GetLastError());
		return;
	}

	// Get a handle to the service.

	schService = OpenServiceW( 
		schSCManager,         // SCM database 
		lpwzSrvName,            // name of service 
		SERVICE_ALL_ACCESS);  // full access 

	if (schService == NULL)
	{ 
		printf("OpenService failed (%d)\n", GetLastError()); 
		CloseServiceHandle(schSCManager);
		return;
	}    

	// Check the status in case the service is not stopped. 

	if (!QueryServiceStatusEx( 
		schService,                     // handle to service 
		SC_STATUS_PROCESS_INFO,         // information level
		(LPBYTE) &ssStatus,             // address of structure
		sizeof(SERVICE_STATUS_PROCESS), // size of structure
		&dwBytesNeeded ) )              // size needed if buffer is too small
	{
		printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
		CloseServiceHandle(schService); 
		CloseServiceHandle(schSCManager);
		return; 
	}

	// Check if the service is already running. It would be possible 
	// to stop the service here, but for simplicity this example just returns. 

	if(ssStatus.dwCurrentState != SERVICE_STOPPED && ssStatus.dwCurrentState != SERVICE_STOP_PENDING)
	{
		printf("Cannot start the service because it is already running\n");
		CloseServiceHandle(schService); 
		CloseServiceHandle(schSCManager);
		return; 
	}

	// Save the tick count and initial checkpoint.

	dwStartTickCount = GetTickCount();
	dwOldCheckPoint = ssStatus.dwCheckPoint;

	// Wait for the service to stop before attempting to start it.

	while (ssStatus.dwCurrentState == SERVICE_STOP_PENDING)
	{
		// Do not wait longer than the wait hint. A good interval is 
		// one-tenth of the wait hint but not less than 1 second  
		// and not more than 10 seconds. 

		dwWaitTime = ssStatus.dwWaitHint / 10;

		if( dwWaitTime < 1000 )
			dwWaitTime = 1000;
		else if ( dwWaitTime > 10000 )
			dwWaitTime = 10000;

		Sleep( dwWaitTime );

		// Check the status until the service is no longer stop pending. 

		if (!QueryServiceStatusEx( 
			schService,                     // handle to service 
			SC_STATUS_PROCESS_INFO,         // information level
			(LPBYTE) &ssStatus,             // address of structure
			sizeof(SERVICE_STATUS_PROCESS), // size of structure
			&dwBytesNeeded ) )              // size needed if buffer is too small
		{
			printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
			CloseServiceHandle(schService); 
			CloseServiceHandle(schSCManager);
			return; 
		}

		if ( ssStatus.dwCheckPoint > dwOldCheckPoint )
		{
			// Continue to wait and check.

			dwStartTickCount = GetTickCount();
			dwOldCheckPoint = ssStatus.dwCheckPoint;
		}
		else
		{
			if(GetTickCount()-dwStartTickCount > ssStatus.dwWaitHint)
			{
				printf("Timeout waiting for service to stop\n");
				CloseServiceHandle(schService); 
				CloseServiceHandle(schSCManager);
				return; 
			}
		}
	}

	// Attempt to start the service.

	if (!StartServiceW(
		schService,  // handle to service 
		0,           // number of arguments 
		NULL) )      // no arguments 
	{
		printf("StartService failed (%d)\n", GetLastError());
		CloseServiceHandle(schService); 
		CloseServiceHandle(schSCManager);
		return; 
	}
	else printf("Service start pending...\n"); 

	// Check the status until the service is no longer start pending. 

	if (!QueryServiceStatusEx( 
		schService,                     // handle to service 
		SC_STATUS_PROCESS_INFO,         // info level
		(LPBYTE) &ssStatus,             // address of structure
		sizeof(SERVICE_STATUS_PROCESS), // size of structure
		&dwBytesNeeded ) )              // if buffer too small
	{
		printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
		CloseServiceHandle(schService); 
		CloseServiceHandle(schSCManager);
		return; 
	}

	// Save the tick count and initial checkpoint.

	dwStartTickCount = GetTickCount();
	dwOldCheckPoint = ssStatus.dwCheckPoint;

	while (ssStatus.dwCurrentState == SERVICE_START_PENDING) 
	{ 
		// Do not wait longer than the wait hint. A good interval is 
		// one-tenth the wait hint, but no less than 1 second and no 
		// more than 10 seconds. 

		dwWaitTime = ssStatus.dwWaitHint / 10;

		if( dwWaitTime < 1000 )
			dwWaitTime = 1000;
		else if ( dwWaitTime > 10000 )
			dwWaitTime = 10000;

		Sleep( dwWaitTime );

		// Check the status again. 

		if (!QueryServiceStatusEx( 
			schService,             // handle to service 
			SC_STATUS_PROCESS_INFO, // info level
			(LPBYTE) &ssStatus,             // address of structure
			sizeof(SERVICE_STATUS_PROCESS), // size of structure
			&dwBytesNeeded ) )              // if buffer too small
		{
			printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
			break; 
		}

		if ( ssStatus.dwCheckPoint > dwOldCheckPoint )
		{
			// Continue to wait and check.

			dwStartTickCount = GetTickCount();
			dwOldCheckPoint = ssStatus.dwCheckPoint;
		}
		else
		{
			if(GetTickCount()-dwStartTickCount > ssStatus.dwWaitHint)
			{
				// No progress made within the wait hint.
				break;
			}
		}
	} 

	// Determine whether the service is running.

	if (ssStatus.dwCurrentState == SERVICE_RUNNING) 
	{
		printf("Service started successfully.\n"); 
	}
	else 
	{ 
		printf("Service not started. \n");
		printf("  Current State: %d\n", ssStatus.dwCurrentState); 
		printf("  Exit Code: %d\n", ssStatus.dwWin32ExitCode); 
		printf("  Check Point: %d\n", ssStatus.dwCheckPoint); 
		printf("  Wait Hint: %d\n", ssStatus.dwWaitHint); 
	} 

	CloseServiceHandle(schService); 
	CloseServiceHandle(schSCManager);
}
void StartServices(HWND hWnd,CClrListCtrl *m_list)
{
	CString ServicesName;
	DWORD dwReadByte;


	POSITION pos = m_list->GetFirstSelectedItemPosition(); //�ж��б�����Ƿ���ѡ����
	int Item = m_list->GetNextSelectedItem(pos); //���б��б�ѡ�����һ������ֵ���浽������

	ServicesName.Format(L"%s",m_list->GetItemText(Item,2));
	if (wcsstr(ServicesName,L"�������ط���"))
	{
		MessageBoxW(hWnd,L"����һ�����ط����޷�����",0,MB_ICONWARNING);
		return;
	}
	if (!wcslen(ServicesName))
	{
		return;
	}
	WCHAR lpwzSrvName[100] = {0};
	memset(lpwzSrvName,0,sizeof(lpwzSrvName));
	wcscat(lpwzSrvName,ServicesName);

	DoStartSvc(lpwzSrvName);
	MessageBoxW(hWnd,L"���������ɹ�",0,MB_ICONWARNING);
}
BOOL DoStopAndDeleteSrv(WCHAR *lpwzServiceName,int IntType)
{
	SC_HANDLE hSCManager;
	SC_HANDLE hService;
	SERVICE_STATUS status;

	status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	status.dwCurrentState = SERVICE_STOPPED;
	status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	status.dwWin32ExitCode = 0;
	status.dwServiceSpecificExitCode = 0;
	status.dwCheckPoint = 0;
	status.dwWaitHint = 0;

	hSCManager = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!hSCManager)
		return FALSE;
	//�򿪷���
	if (IntType == 1)
	{
		hService = OpenServiceW(hSCManager, lpwzServiceName, SERVICE_STOP | DELETE);
		if (!hService)
			return FALSE;
	}else
	{
		hService = OpenServiceW(hSCManager, lpwzServiceName, SERVICE_STOP);
		if (!hService)
			return FALSE;
	}
	//ֹͣ����
	ControlService(hService, SERVICE_CONTROL_STOP, &status);
	if (IntType == 1)
	{
		DeleteService(hService);
	}
	if(hService)
		CloseServiceHandle(hService);

	return TRUE;
}
void StopServices(HWND hWnd,CClrListCtrl *m_list)
{
	CString ServicesName;
	DWORD dwReadByte;

	POSITION pos = m_list->GetFirstSelectedItemPosition(); //�ж��б�����Ƿ���ѡ����
	int Item = m_list->GetNextSelectedItem(pos); //���б��б�ѡ�����һ������ֵ���浽������

	ServicesName.Format(L"%s",m_list->GetItemText(Item,2));
	if (wcsstr(ServicesName,L"�������ط���"))
	{
		MessageBoxW(hWnd,L"����һ�����ط����޷�ֹͣ",0,MB_ICONWARNING);
		return;
	}
	if (!wcslen(ServicesName))
	{
		return;
	}
	WCHAR lpwzSrvName[100] = {0};
	memset(lpwzSrvName,0,sizeof(lpwzSrvName));
	wcscat(lpwzSrvName,ServicesName);

	if (DoStopAndDeleteSrv(lpwzSrvName,0) == TRUE)
		MessageBoxW(hWnd,L"����ֹͣ�ɹ�",0,MB_ICONWARNING);
	else
		MessageBoxW(hWnd,L"����ֹͣʧ��",0,MB_ICONWARNING);
}
void DeleteServices(HWND hWnd,CClrListCtrl *m_list)
{
	CString ServicesName;
	DWORD dwReadByte;


	POSITION pos = m_list->GetFirstSelectedItemPosition(); //�ж��б�����Ƿ���ѡ����
	int Item = m_list->GetNextSelectedItem(pos); //���б��б�ѡ�����һ������ֵ���浽������

	ServicesName.Format(L"%s",m_list->GetItemText(Item,2));
	if (wcsstr(ServicesName,L"�������ط���"))
	{
		MessageBoxW(hWnd,L"����һ�����ط����޷�ɾ��",0,MB_ICONWARNING);
		return;
	}
	if (!wcslen(ServicesName))
	{
		return;
	}
	WCHAR lpwzSrvName[100] = {0};
	memset(lpwzSrvName,0,sizeof(lpwzSrvName));
	wcscat(lpwzSrvName,ServicesName);

	if (DoStopAndDeleteSrv(lpwzSrvName,1) == TRUE)
		MessageBoxW(hWnd,L"����ɾ���ɹ�",0,MB_ICONWARNING);
	else
		MessageBoxW(hWnd,L"����ɾ��ʧ��",0,MB_ICONWARNING);
}
//�ֶ�
void ManualServices(HWND hWnd,CClrListCtrl *m_list)
{
	CString ServicesName;
	DWORD dwReadByte;


	POSITION pos = m_list->GetFirstSelectedItemPosition(); //�ж��б�����Ƿ���ѡ����
	int Item = m_list->GetNextSelectedItem(pos); //���б��б�ѡ�����һ������ֵ���浽������

	ServicesName.Format(L"%s",m_list->GetItemText(Item,2));
// 	if (wcsstr(ServicesName,L"�������ط���"))
// 	{
// 		MessageBoxW(L"����һ�����ط����޷�����",0,MB_ICONWARNING);
// 		return;
// 	}
	if (!wcslen(ServicesName))
	{
		return;
	}
	WCHAR lpwzSrvName[100] = {0};
	memset(lpwzSrvName,0,sizeof(lpwzSrvName));
	wcscat(lpwzSrvName,ServicesName);

	ReadFile((HANDLE)CHANG_SERVICES_TYPE_1,lpwzSrvName,wcslen(lpwzSrvName),&dwReadByte,0);
	MessageBoxW(hWnd,L"�����ɹ�",0,MB_ICONWARNING);
}
//�Զ�
void AutoServices(HWND hWnd,CClrListCtrl *m_list)
{
	CString ServicesName;
	DWORD dwReadByte;


	POSITION pos = m_list->GetFirstSelectedItemPosition(); //�ж��б�����Ƿ���ѡ����
	int Item = m_list->GetNextSelectedItem(pos); //���б��б�ѡ�����һ������ֵ���浽������

	ServicesName.Format(L"%s",m_list->GetItemText(Item,2));
	// 	if (wcsstr(ServicesName,L"�������ط���"))
	// 	{
	// 		MessageBoxW(L"����һ�����ط����޷�����",0,MB_ICONWARNING);
	// 		return;
	// 	}
	if (!wcslen(ServicesName))
	{
		return;
	}
	WCHAR lpwzSrvName[100] = {0};
	memset(lpwzSrvName,0,sizeof(lpwzSrvName));
	wcscat(lpwzSrvName,ServicesName);

	ReadFile((HANDLE)CHANG_SERVICES_TYPE_2,lpwzSrvName,wcslen(lpwzSrvName),&dwReadByte,0);
	MessageBoxW(hWnd,L"�����ɹ�",0,MB_ICONWARNING);
}
//����
void DisableServices(HWND hWnd,CClrListCtrl *m_list)
{
	CString ServicesName;
	DWORD dwReadByte;


	POSITION pos = m_list->GetFirstSelectedItemPosition(); //�ж��б�����Ƿ���ѡ����
	int Item = m_list->GetNextSelectedItem(pos); //���б��б�ѡ�����һ������ֵ���浽������

	ServicesName.Format(L"%s",m_list->GetItemText(Item,2));
	// 	if (wcsstr(ServicesName,L"�������ط���"))
	// 	{
	// 		MessageBoxW(L"����һ�����ط����޷�����",0,MB_ICONWARNING);
	// 		return;
	// 	}
	if (!wcslen(ServicesName))
	{
		return;
	}
	WCHAR lpwzSrvName[100] = {0};
	memset(lpwzSrvName,0,sizeof(lpwzSrvName));
	wcscat(lpwzSrvName,ServicesName);

	ReadFile((HANDLE)CHANG_SERVICES_TYPE_3,lpwzSrvName,wcslen(lpwzSrvName),&dwReadByte,0);
	MessageBoxW(hWnd,L"�����ɹ�",0,MB_ICONWARNING);
}