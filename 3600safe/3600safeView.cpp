
// 3600safeView.cpp : CMy3600safeView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "3600safe.h"
#endif

#include "3600safeDoc.h"
#include "3600safeView.h"
#include "AboutDlg.h"
#include "SubModule.h"
#include "uninstall360.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern char lpOpenUrl[260];

BOOL Install(HWND hwndDlg);
BOOL WINAPI EnableDebugPriv(LPCTSTR name);//����Ȩ����
BOOL InstallDepthServicesScan(CHAR * serviceName);
BOOL UninstallDepthServicesScan(CHAR * serviceName);
VOID QueryInternetInfo(HWND hWnd);
BOOL IsWindows7();
BOOL QueryUserAgent(HKEY HKey,char *lpSubKey,char *lpValueName,char *OutBuffer);
//*********************************************************
//SSDT
//*********************************************************
#define   SSDT_MAX_COLUMN 7
wchar_t	SSDTStr[SSDT_MAX_COLUMN][260]  = {_T("ID"),	_T("��ǰ��ַ"),			_T("������"),	_T("�ں�ģ��"),		
	_T("ģ���ַ"),	_T("ģ���С"),	_T("Hook����")};										 
int		   SSDTWidth[SSDT_MAX_COLUMN]= {40, 90, 130, 200, 80, 80, 80};
VOID QuerySSDTHook(HWND m_hWnd,ULONG ID,int IntType,CClrListCtrl *m_list);
VOID UnHookSSDT(HWND m_hWnd,CClrListCtrl *m_list);
VOID UnHookSSDTAll(HWND m_hWnd,CClrListCtrl *m_list);
VOID CopySSDTDataToClipboard(HWND hWnd,CClrListCtrl *m_list);
//*********************************************************
//ShadowSSDT
//*********************************************************
#define	  SHADOW_MAX_COLUMN	7

wchar_t	ShadowSSDTStr[SHADOW_MAX_COLUMN][260]  = {_T("ID"),	_T("��ǰ��ַ"),			_T("������"),	_T("�ں�ģ��"),		
	_T("ģ���ַ"),	_T("ģ���С"),	_T("Hook����")};							 
int		ShadowSSDTWidth[SHADOW_MAX_COLUMN]=	{40, 90, 130, 200, 80, 80, 80};
VOID QueryShadowSSDTHook(HWND m_hWnd,ULONG ID,int IntType,CClrListCtrl *m_list);
VOID UnHookShadowSSDT(HWND m_hWnd,CClrListCtrl *m_list);
VOID UnHookShadowSSDTAll(HWND m_hWnd,CClrListCtrl *m_list);
VOID CopyShadowSSDTDataToClipboard(HWND hWnd,CClrListCtrl *m_list);
//*********************************************************
//KernelHook
//*********************************************************
#define   KERNEL_HOOK_MAX_COLUMN 7

wchar_t	HookStr[KERNEL_HOOK_MAX_COLUMN][260]  = {_T("���ҹ���ַ"),	_T("�ҹ�����"),			_T("hook��ת��ַ"),	_T("����ģ��"),		
	_T("ģ���ַ"),	_T("ģ���С"),	_T("Hook����")};										 
int		HookWidth[KERNEL_HOOK_MAX_COLUMN]= {80, 120, 90, 180, 80, 80,80};

VOID QueryKernelHook(HWND m_hWnd,ULONG ID,CClrListCtrl *m_list);
VOID UnHookKernelHookSelect(HWND m_hWnd,CClrListCtrl *m_list);
VOID ByPassKernelHook(HWND m_hWnd,CClrListCtrl *m_list);
void CopyKernelHookDataToClipboard(HWND m_hWnd,CClrListCtrl *m_list);
//*********************************************************
//objecthook
//**********************************************************
#define	  OBJECT_HOOK_MAX_COLUMN	7

wchar_t	OBJStr[OBJECT_HOOK_MAX_COLUMN][260]  = {_T("��ǰ������ַ"),	_T("ԭʼ������ַ"),			_T("������"),	_T("Object����"),		
	_T("ObjectType��ַ"),	_T("��ǰ������ַ����ģ��"),	_T("Hook����")};										 
int		OBJWidth[OBJECT_HOOK_MAX_COLUMN]=	{90, 90,110, 90, 100, 130, 90};
VOID QueryObjectHook(HWND m_hWnd,ULONG ID,CClrListCtrl *m_list);
//***********************************************************************
//kernel module
//***********************************************************************
#define KERNEL_MODULE_MAX_COLUMN 6

wchar_t	ModuleStr[KERNEL_MODULE_MAX_COLUMN][260]  = {_T("����ַ"),_T("��С"),	_T("����ӳ��"),	_T("����·��"),		
	_T("���ش���"),	_T("��֤�Ƿ�ϵͳԭ���ļ�")};										 
int		MWidth[KERNEL_MODULE_MAX_COLUMN]=	{80,80, 80, 230, 60, 150};
VOID QueryKernelModule(HWND m_hWnd,ULONG ID,CClrListCtrl *m_list,int IntLookType);
void OnDumpmemorydatatofile(HWND hWnd,CClrListCtrl *m_list);
void CopyKernelModuleDataToClipboard(HWND m_hWnd,CClrListCtrl *m_list);
//***********************************************************************
//kernel thread
//***********************************************************************
#define KTHREAD_MAX_COLUMN 5
wchar_t	KernelThreadStr[KTHREAD_MAX_COLUMN][260]  = {_T("ETHREAD"),	_T("�̺߳�����ַ"),		
	_T("�߳�ģ��"),	_T("�߳�״̬"),		_T("�Ƿ�����")};										 
int		KernelThreadWidth[KTHREAD_MAX_COLUMN]=	{80, 100, 280, 80, 70};
void QueryKernelThread(HWND m_hWnd,ULONG ID,CClrListCtrl *m_list);
void ClearKernelThreadLog(CClrListCtrl *m_list);
//***********************************************************************
//system thread
//***********************************************************************
#define STHREAD_MAX_COLUMN 10

wchar_t	SystemThreadStr[STHREAD_MAX_COLUMN][260]  = {_T("�߳�Id"),	_T("ETHREAD"),			_T("TEB"),	_T("�߳����"),		
	_T("HideDebug"),_T("���ȼ�"),_T("�л�����"),_T("ģ��"),_T("�߳�״̬"),_T("��֤�Ƿ�ϵͳԭ���ļ�")};										 
int		SystemThreadWidth[STHREAD_MAX_COLUMN]=	{50, 70, 70, 80, 70,60,60,150,70,140};
void QuerySystemThread(HWND m_hWnd,ULONG ID,CClrListCtrl *m_list);
void KillSystemThread(HWND hWnd,ULONG ID,CClrListCtrl *m_list);
void SuspendThread(HWND hWnd,CClrListCtrl *m_list);
void ResumeThread(HWND hWnd,CClrListCtrl *m_list);
//***********************************************************************
//Dpc timer
//***********************************************************************
#define DPCTIMER_MAX_COLUMN 5

wchar_t	DpcTimerStr[DPCTIMER_MAX_COLUMN][260]  = {_T("KTIMER"),_T("KDPC"),_T("��������"),_T("�������"),_T("��������ģ��")};										 
int		DpcTimerWidth[DPCTIMER_MAX_COLUMN]=	{80, 80, 70, 80, 350};

void QueryDpcTimer(HWND m_hWnd,ULONG ID,CClrListCtrl *m_list);
void KillDpcTimer(HWND hWnd,ULONG ID,CClrListCtrl *m_list);
void CopyDpcTimerDataToClipboard(HWND m_hWnd,CClrListCtrl *m_list);
//***********************************************************************
//SystemNotify
//***********************************************************************
#define SYSTEMNOTIFY_MAX_COLUMN 4

wchar_t	SystemNotifyStr[SYSTEMNOTIFY_MAX_COLUMN][260]  = {_T("�ص����"),_T("�ص�����"),_T("����"),_T("�������ģ��")};					 
int		SystemNotifyWidth[SYSTEMNOTIFY_MAX_COLUMN]=	{80, 80, 190, 350};

void QuerySystemNotify(HWND m_hWnd,ULONG ID,CClrListCtrl *m_list);
void KillSystemNotify(HWND hWnd,ULONG ID,CClrListCtrl *m_list);
void CopySystemNotifyDataToClipboard(HWND m_hWnd,CClrListCtrl *m_list);
//***************************************************************************
//FilterDriver
//***************************************************************************
#define	  FILTER_MAX_COLUMN	5

wchar_t	GLStr[FILTER_MAX_COLUMN][260]  = {_T("����"),	_T("����������"),			_T("�豸"),	_T("��������������"),		
	_T("ģ��·��")};										 
int		GLWidth[FILTER_MAX_COLUMN]=	{90, 120, 90, 120, 280};
void QueryFilterDriver(HWND m_hWnd,ULONG ID,CClrListCtrl *m_list);
void DeleteSelectFilterDriver(HWND hWnd,ULONG ID,CClrListCtrl *m_list);
//***************************************************************************
//NTFS/FSD
//***************************************************************************

#define	  FSD_MAX_COLUMN	8

wchar_t	FSDStr[FSD_MAX_COLUMN][260]  = {_T("�������"),	_T("��������"),			_T("��ǰ��ַ"),	_T("ԭʼ��ַ"),		
	_T("��ǰ��������ģ��"),_T("��ַ"),_T("��С"),	_T("Hook����")};										 
int		FSDWidth[FSD_MAX_COLUMN]=	{70, 130, 80, 80, 120, 80,60,70};
void QueryNtfsFsdHook(HWND m_hWnd,ULONG ID,CClrListCtrl *m_list);
void FsdHookResetOne(HWND hWnd,ULONG ID,CClrListCtrl *m_list);
void CopyFsdDataToClipboard(HWND hWnd,CClrListCtrl *m_list);
//***************************************************************************
//tcpip
//***************************************************************************
#define  TCPIP_MAX_COLUMN  8

wchar_t	TcpStr[TCPIP_MAX_COLUMN][260]  = {_T("�������"),	_T("��������"),			_T("��ǰ��ַ"),	_T("ԭʼ��ַ"),		
	_T("��ǰ��������ģ��"),_T("��ַ"),_T("��С"),	_T("Hook����")};										 
int		TcpWidth[TCPIP_MAX_COLUMN]=	{70, 130, 80, 80, 140, 80,60,70};
void QueryTcpipHook(HWND m_hWnd,ULONG ID,CClrListCtrl *m_list);
void TcpipHookResetOne(HWND hWnd,ULONG ID,CClrListCtrl *m_list);
void CopyTcpipDataToClipboard(HWND hWnd,CClrListCtrl *m_list);
//***************************************************************************
//Nsiproxy
//***************************************************************************
#define  NSIPROXY_MAX_COLUMN  8

wchar_t	NsipStr[NSIPROXY_MAX_COLUMN][260]  = {_T("�������"),	_T("��������"),			_T("��ǰ��ַ"),	_T("ԭʼ��ַ"),		
	_T("��ǰ��������ģ��"),_T("��ַ"),_T("��С"),	_T("Hook����")};										 
int		NsipWidth[NSIPROXY_MAX_COLUMN]=	{70, 130, 80, 80, 140, 80,60,70};
void QueryNsipHook(HWND m_hWnd,ULONG ID,CClrListCtrl *m_list);
void NsipHookResetOne(HWND hWnd,ULONG ID,CClrListCtrl *m_list);
void CopyNsipDataToClipboard(HWND hWnd,CClrListCtrl *m_list);
//***************************************************************************
//Kbdclass
//***************************************************************************
#define  KBDCLASS_MAX_COLUMN  8

wchar_t	KbdclassStr[KBDCLASS_MAX_COLUMN][260]  = {_T("�������"),	_T("��������"),			_T("��ǰ��ַ"),	_T("ԭʼ��ַ"),		
	_T("��ǰ��������ģ��"),_T("��ַ"),_T("��С"),	_T("Hook����")};										 
int		KbdclassWidth[KBDCLASS_MAX_COLUMN]=	{70, 130, 80, 80, 140, 80,60,70};
void QueryKbdclassHook(HWND m_hWnd,ULONG ID,CClrListCtrl *m_list);
void KbdclassHookResetOne(HWND hWnd,ULONG ID,CClrListCtrl *m_list);
void CopyKbdclassDataToClipboard(HWND hWnd,CClrListCtrl *m_list);
//***************************************************************************
//Mouclass
//***************************************************************************
#define  MOUCLASS_MAX_COLUMN  8

wchar_t	MouclassStr[MOUCLASS_MAX_COLUMN][260]  = {_T("�������"),	_T("��������"),			_T("��ǰ��ַ"),	_T("ԭʼ��ַ"),		
	_T("��ǰ��������ģ��"),_T("��ַ"),_T("��С"),	_T("Hook����")};										 
int		MouclassWidth[MOUCLASS_MAX_COLUMN]=	{70, 130, 80, 80, 140, 80,60,70};
void QueryMouclassHook(HWND m_hWnd,ULONG ID,CClrListCtrl *m_list);
void MouclassHookResetOne(HWND hWnd,ULONG ID,CClrListCtrl *m_list);
void CopyMouclassDataToClipboard(HWND hWnd,CClrListCtrl *m_list);
//***************************************************************************
//Aatapi
//***************************************************************************
#define  ATAPI_MAX_COLUMN  8

wchar_t	AtapiStr[ATAPI_MAX_COLUMN][260]  = {_T("�������"),	_T("��������"),			_T("��ǰ��ַ"),	_T("ԭʼ��ַ"),		
	_T("��ǰ��������ģ��"),_T("��ַ"),_T("��С"),	_T("Hook����")};										 
int		AtapiWidth[ATAPI_MAX_COLUMN]=	{70, 130, 80, 80, 140, 80,60,70};
void QueryAtapiHook(HWND m_hWnd,ULONG ID,CClrListCtrl *m_list);
void AtapiHookResetOne(HWND hWnd,ULONG ID,CClrListCtrl *m_list);
void CopyAtapiDataToClipboard(HWND hWnd,CClrListCtrl *m_list);
//***************************************************************************
//process
//***************************************************************************
#define	 PROCESS_MAX_COLUMN	7

wchar_t	ProcessStr[PROCESS_MAX_COLUMN][260]  = {_T("����PID"),	_T("������PID"),			_T("������"),	_T("����·��"),		
	_T("EPROCESS"),	_T("�û�/�ں� ����"),	_T("��֤�Ƿ�ϵͳԭ���ļ�")};										 
int		Width[PROCESS_MAX_COLUMN]=	{50, 70, 80, 200, 80, 100, 140};

VOID QuerySystemProcess(HWND m_hWnd,ULONG ID,CClrListCtrl *m_list);
void KillProcess(HWND hWnd,ULONG ID,CClrListCtrl *m_list);
void  KillProcessDeleteFile(HWND hWnd,ULONG ID,CClrListCtrl *m_list);
void ProcessVerify(HWND hWnd,CClrListCtrl *m_list);
void CopyProcessDataToClipboard(HWND hWnd,CClrListCtrl *m_list);
//***************************************************************************
//services
//***************************************************************************
#define  SERVICES_MAX_COLUMN  7
wchar_t		    ServiceStr[SERVICES_MAX_COLUMN][260]  = {_T("״̬"),			_T("��������"),_T("������"),		_T("����"),		
	_T("ӳ��·��"),	_T("��̬���ӿ�"),	_T("��֤�Ƿ�ϵͳԭ���ļ�(��̬���ӿ�)")};										 
int				SrcWidth[SERVICES_MAX_COLUMN]=	{50, 70,80, 100,120, 120, 140};
VOID QueryServices(HWND m_hWnd,ULONG ID,CClrListCtrl *m_list,int IntType);
void GetDepthServicesList(HWND m_hWnd,ULONG ID,CClrListCtrl *m_list,int IntType);
void StartServices(HWND hWnd,CClrListCtrl *m_list);
void StopServices(HWND hWnd,CClrListCtrl *m_list);
void DeleteServices(HWND hWnd,CClrListCtrl *m_list);
void ManualServices(HWND hWnd,CClrListCtrl *m_list);
void AutoServices(HWND hWnd,CClrListCtrl *m_list);
void DisableServices(HWND hWnd,CClrListCtrl *m_list);
//***************************************************************************
//tcpview
//***************************************************************************
#define  TCPVIEW_MAX_COLUMN 7

wchar_t	TCPStr[TCPVIEW_MAX_COLUMN][260]  = {_T("Э��"),	_T("���ص�ַ"),			_T("Զ�̵�ַ"),	_T("����״̬"),		
	_T("PID"),	_T("����·��"),	_T("��֤�Ƿ�ϵͳԭ���ļ�")};										 
int		TCWidth[TCPVIEW_MAX_COLUMN]=	{50, 100, 100, 80, 40, 180, 150};
VOID Tcpview(HWND m_hWnd,ULONG ID,CClrListCtrl *m_list);
void TcpProKill(HWND m_hWnd,ULONG ID,CClrListCtrl *m_list);
//***************************************************************************
//HipsLog
//***************************************************************************
#define	 HIPSLOG_MAX_COLUMN	8

wchar_t	StealStr[HIPSLOG_MAX_COLUMN][260]  = {_T("����ID"),_T("������ID"),_T("ӳ��·��"),		
	_T("EPROCESS"),	_T("�¼�"),_T("�¼���ϸ����"),_T("����������/���޸ķ���"),_T("��֤�Ƿ�ϵͳԭ���ļ�")};										 
int				SCWidth[HIPSLOG_MAX_COLUMN]=	{50, 60, 70, 70,70, 150,200,100};
VOID HipsLog(HWND m_hWnd,ULONG ID,CClrListCtrl *m_list,int Type);
void ClearListLog(CClrListCtrl *m_list);
void ConnectScan(HWND m_hWnd);
void SaveToFile(HWND m_hWnd,ULONG ID,CClrListCtrl *m_list);
//***************************************************************************
IMPLEMENT_DYNCREATE(CMy3600safeView, CFormView)

BEGIN_MESSAGE_MAP(CMy3600safeView, CFormView)
//	ON_WM_CONTEXTMENU()
//	ON_WM_RBUTTONUP()
// 	ON_WM_SYSCOMMAND()
// 	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CMy3600safeView::OnTvnSelchangedTree1)
	ON_NOTIFY(NM_RCLICK, IDC_LIST2, &CMy3600safeView::OnNMRClickList2)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST2, &CMy3600safeView::OnLvnItemchangedList2)
	
	ON_COMMAND(ID_About, &CMy3600safeView::OnAbout)
	ON_COMMAND(ID_Exit, &CMy3600safeView::OnClose)
	ON_COMMAND(IDC_OpenURL, &CMy3600safeView::OnOpenUrl)
	ON_COMMAND(ID_OpenKernelType, &CMy3600safeView::OnOpenKernelType)
	ON_COMMAND(ID_ShutdownKernelType, &CMy3600safeView::OnShutdownKernelType)
	ON_COMMAND(ID_WindowsOverhead, &CMy3600safeView::OnWindowsOverhead)
	ON_COMMAND(ID_CancelTheOverhead, &CMy3600safeView::OnCancelTheOverhead)
	ON_COMMAND(ID_ProtectProcess, &CMy3600safeView::OnProtectProcess)
	ON_COMMAND(ID_UnProtectProcess, &CMy3600safeView::OnUnProtectProcess)
	ON_COMMAND(ID_Booting, &CMy3600safeView::OnBooting)
	ON_COMMAND(ID_CancelBooting, &CMy3600safeView::OnCancelBooting)

	ON_COMMAND(ID_SSDTHook, &CMy3600safeView::SSDTHook)
	ON_COMMAND(ID_SSDTAllHook, &CMy3600safeView::SSDTAllHook)
	ON_COMMAND(ID_UnHookSSDTHookSelect, &CMy3600safeView::UnHookSSDTHookSelect)
	ON_COMMAND(ID_UnHookSSDTHookAll, &CMy3600safeView::UnHookSSDTHookAll)
	ON_COMMAND(ID_CopySSDTDataToClipboard, &CMy3600safeView::OnCopySSDTDataToClipboard)

	ON_COMMAND(ID_ShadowSSDTHook, &CMy3600safeView::ShadowSSDTHook)
	ON_COMMAND(ID_ShadowSSDTAllHook, &CMy3600safeView::ShadowSSDTAllHook)
	ON_COMMAND(ID_UnHookShadowSSDTHookSelect, &CMy3600safeView::UnHookShadowSSDTHookSelect)
	ON_COMMAND(ID_UnHookShadowSSDTHookAll, &CMy3600safeView::UnHookShadowSSDTHookAll)
	ON_COMMAND(ID_CopyShadowSSDTDataToClipboard, &CMy3600safeView::OnCopyShadowSSDTDataToClipboard)

	ON_COMMAND(ID_KernelHook, &CMy3600safeView::OnKernelHook)
	ON_COMMAND(ID_UnHookKernelHookSelect, &CMy3600safeView::OnUnHookKernelHookSelect)
	ON_COMMAND(ID_ByPassKernelHook, &CMy3600safeView::OnByPassKernelHook)
	ON_COMMAND(ID_CopyKernelHookDataToClipboard, &CMy3600safeView::OnCopyKernelHookDataToClipboard)

	ON_COMMAND(ID_KernelModule, &CMy3600safeView::OnKernelModule)
	ON_COMMAND(ID_KernelModule1, &CMy3600safeView::OnKernelModule1)
	ON_COMMAND(ID_DumpMemoryDataToFile, &CMy3600safeView::OnDumpMemoryDataToFile)
	ON_COMMAND(ID_CopyKernelModuleDataToClipboard, &CMy3600safeView::OnCopyKernelModuleDataToClipboard)

	ON_COMMAND(ID_KernelThreadList, &CMy3600safeView::OnKernelThreadList)
	ON_COMMAND(ID_ClearKernelThreadLog, &CMy3600safeView::OnClearKernelThreadLog)

	ON_COMMAND(ID_SystemThreadList, &CMy3600safeView::OnSystemThreadList)
	ON_COMMAND(ID_KillSelectThread, &CMy3600safeView::OnKillSystemThread)
	ON_COMMAND(ID_SuspendSelectThread, &CMy3600safeView::OnSuspendThread)
	ON_COMMAND(ID_ResumeSelectThread, &CMy3600safeView::OnResumeThread)

	ON_COMMAND(ID_FilterDriverList, &CMy3600safeView::OnFilterDriverList)
	ON_COMMAND(ID_DeleteSelectFilterDriver, &CMy3600safeView::OnDeleteSelectFilterDriver)

	ON_COMMAND(ID_FsdHookList, &CMy3600safeView::OnFsdHookList)
	ON_COMMAND(ID_FsdHookResetOne, &CMy3600safeView::OnFsdHookResetOne)
	ON_COMMAND(ID_CopyFsdDataToClipboard, &CMy3600safeView::OnCopyFsdDataToClipboard)

	ON_COMMAND(ID_TcpipHookList, &CMy3600safeView::OnTcpipHookList)
	ON_COMMAND(ID_TcpipHookResetOne, &CMy3600safeView::OnTcpipHookResetOne)
	ON_COMMAND(ID_CopyTcpipDataToClipboard, &CMy3600safeView::OnCopyTcpipDataToClipboard)

	ON_COMMAND(ID_NsipHookList, &CMy3600safeView::OnNsipHookList)
	ON_COMMAND(ID_NsipHookResetOne, &CMy3600safeView::OnNsipHookResetOne)
	ON_COMMAND(ID_CopyNsipDataToClipboard, &CMy3600safeView::OnCopyNsipDataToClipboard)

	ON_COMMAND(ID_KbdclassHookList, &CMy3600safeView::OnKbdclassHookList)
	ON_COMMAND(ID_KbdclassHookResetOne, &CMy3600safeView::OnKbdclassHookResetOne)
	ON_COMMAND(ID_CopyKbdclassDataToClipboard, &CMy3600safeView::OnCopyKbdclassDataToClipboard)

	ON_COMMAND(ID_MouclassHookList, &CMy3600safeView::OnMouclassHookList)
	ON_COMMAND(ID_MouclassHookResetOne, &CMy3600safeView::OnMouclassHookResetOne)
	ON_COMMAND(ID_CopyMouclassDataToClipboard, &CMy3600safeView::OnCopyMouclassDataToClipboard)

	ON_COMMAND(ID_AtapiHookList, &CMy3600safeView::OnAtapiHookList)
	ON_COMMAND(ID_AtapiHookResetOne, &CMy3600safeView::OnAtapiHookResetOne)
	ON_COMMAND(ID_CopyAtapiDataToClipboard, &CMy3600safeView::OnCopyAtapiDataToClipboard)

	ON_COMMAND(ID_DpcTimerList, &CMy3600safeView::OnDpcTimerList)
	ON_COMMAND(ID_KillDpcTimer, &CMy3600safeView::OnKillDpcTimer)
	ON_COMMAND(ID_CopyDpcTimerDataToClipboard, &CMy3600safeView::OnCopyDpcTimerDataToClipboard)

	ON_COMMAND(ID_SystemNotifyList, &CMy3600safeView::OnSystemNotifyList)
	ON_COMMAND(ID_KillSystemNotify, &CMy3600safeView::OnKillSystemNotify)
	ON_COMMAND(ID_CopySystemNotifyDataToClipboard, &CMy3600safeView::OnCopySystemNotifyDataToClipboard)

	ON_COMMAND(ID_Process, &CMy3600safeView::OnQueryProcess)
	ON_COMMAND(ID_ProcessModule, &CMy3600safeView::OnProcessmodule)
	ON_COMMAND(ID_ProcessThread, &CMy3600safeView::OnProcessThread)
	ON_COMMAND(ID_ProcessHandle, &CMy3600safeView::OnProcessHandle)
	
	ON_COMMAND(ID_KillProcess, &CMy3600safeView::OnKillProcess)
	ON_COMMAND(ID_KillProcessDeleteFile, &CMy3600safeView::OnKillProcessDeleteFile) 
	ON_COMMAND(ID_ProcessVerify, &CMy3600safeView::OnProcessVerify)
	ON_COMMAND(ID_CopyProcessDataToClipboard, &CMy3600safeView::OnCopyProcessDataToClipboard)

	ON_COMMAND(ID_GetServicesList, &CMy3600safeView::OnGetServicesList)
	ON_COMMAND(ID_RestartToGetServicesList, &CMy3600safeView::OnGetDepthServicesList)
	ON_COMMAND(ID_StartServices, &CMy3600safeView::OnStartServices)
	ON_COMMAND(ID_StopServices, &CMy3600safeView::OnStopServices)
	ON_COMMAND(ID_DeleteServices, &CMy3600safeView::OnDeleteServices)
	ON_COMMAND(ID_ManualServices, &CMy3600safeView::OnManualServices)
	ON_COMMAND(ID_AutoServices, &CMy3600safeView::OnAutoServices)
	ON_COMMAND(ID_DisableServices, &CMy3600safeView::OnDisableServices)

	ON_COMMAND(ID_Tcpview, &CMy3600safeView::OnMsgTcpView)
	ON_COMMAND(ID_KillTcpProcess, &CMy3600safeView::OnMsgTcpProKill)

	ON_COMMAND(ID_HipsLog, &CMy3600safeView::OnListLog)
	ON_COMMAND(ID_ClearListLog, &CMy3600safeView::OnClearListLog)
	ON_COMMAND(ID_ConnectScan, &CMy3600safeView::OnConnectScan)
	ON_COMMAND(ID_SaveToFile, &CMy3600safeView::OnSaveToFile)

	ON_BN_CLICKED(IDC_DeleteFile, &CMy3600safeView::OnBnClickedQzdeletefile)
	ON_BN_CLICKED(IDC_DisLoadDriver, &CMy3600safeView::OnBnClickedDisdriver)
	ON_BN_CLICKED(IDC_DisCreateProcess, &CMy3600safeView::OnBnClickedDisprocess)
	ON_BN_CLICKED(IDC_DisCreateFile, &CMy3600safeView::OnBnClickedDisfile)
	ON_BN_CLICKED(IDC_ShutdownSystem, &CMy3600safeView::OnBnClickedShutdownSystem)
	ON_BN_CLICKED(IDC_DisSrvReset, &CMy3600safeView::OnBnClickedServicesReset)
	ON_BN_CLICKED(IDC_DisKernelThread, &CMy3600safeView::OnBnClickedDisKernelThread)
	ON_BN_CLICKED(IDC_Uninstall360, &CMy3600safeView::OnUninstall360)

	ON_WM_CREATE()
END_MESSAGE_MAP()

// CMy3600safeView ����/����

CMy3600safeView::CMy3600safeView()
	: CFormView(CMy3600safeView::IDD)
{
	// TODO: �ڴ˴���ӹ������
	m_case=0;
}

CMy3600safeView::~CMy3600safeView()
{
}

void CMy3600safeView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_TreeCtrl);
	DDX_Control(pDX, IDC_LIST2, m_ListCtrl);
}

BOOL CMy3600safeView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CFormView::PreCreateWindow(cs);
}

void CMy3600safeView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
    GetParentFrame()->RecalcLayout();  
    ResizeParentToFit();  
 
	if (GetSystemDefaultLCID() != 2052)
	{
		MessageBoxW(L"\"A�ܵ��Է���\" Ŀǰ��֧������ϵͳ��\r\n",0,MB_ICONWARNING);
		exit(0);
	}
	//����ΪTreeͼ�괦��
	CMy3600safeApp *imgApp=(CMy3600safeApp*)AfxGetApp();
	ImgList.Create(18,18,ILC_COLOR32,2,9);
	ImgList.Add(imgApp->LoadIconW(IDR_My3600safeTYPE));
	ImgList.Add(imgApp->LoadIconW(IDI_ICON1));
	ImgList.Add(imgApp->LoadIconW(IDI_ICON2));
	ImgList.Add(imgApp->LoadIconW(IDI_ICON3));
	ImgList.Add(imgApp->LoadIconW(IDI_ICON4));
	ImgList.Add(imgApp->LoadIconW(IDI_ICON5));
	ImgList.Add(imgApp->LoadIconW(IDI_ICON6));
	ImgList.Add(imgApp->LoadIconW(IDI_ICON7));
	ImgList.Add(imgApp->LoadIconW(IDI_ICON8));
	ImgList.Add(imgApp->LoadIconW(IDI_ICON9));
	m_TreeCtrl.SetImageList(&ImgList,TVSIL_NORMAL);


	HTREEITEM hKernelHook=m_TreeCtrl.InsertItem(_T("�ں�Hook"),0,0,TVI_ROOT,TVI_LAST);//��һ������Ϊδѡ��ʱͼƬID���ڶ�������Ϊѡ��ʱͼƬ���֡�
	HTREEITEM hKernelModule=m_TreeCtrl.InsertItem(_T("�ں�ģ��"),1,1,TVI_ROOT,TVI_LAST);
	HTREEITEM hKernelThread=m_TreeCtrl.InsertItem(_T("�ں���Ϣ"),2,2,TVI_ROOT,TVI_LAST);
	HTREEITEM hKernelFilterDriver=m_TreeCtrl.InsertItem(_T("��������"),3,3,TVI_ROOT,TVI_LAST);
	HTREEITEM hDispatch=m_TreeCtrl.InsertItem(_T("Dispatch����"),4,4,TVI_ROOT,TVI_LAST);
	HTREEITEM hProcess=m_TreeCtrl.InsertItem(_T("ϵͳ����"),5,5,TVI_ROOT,TVI_LAST);
	HTREEITEM hServices=m_TreeCtrl.InsertItem(_T("ϵͳ����"),6,6,TVI_ROOT,TVI_LAST);
	HTREEITEM hTcpView=m_TreeCtrl.InsertItem(_T("��������"),7,7,TVI_ROOT,TVI_LAST);
	HTREEITEM hHips=m_TreeCtrl.InsertItem(_T("��������"),8,8,TVI_ROOT,TVI_LAST);
	//HTREEITEM hRing3Memory=m_TreeCtrl.InsertItem(_T("Ӧ�ò�ȫ��"),9,9,TVI_ROOT,TVI_LAST);
	///////////////////////////////////////////////////////////////////////////////////
	HTREEITEM subSSDT=m_TreeCtrl.InsertItem(_T("SSDT"),0,0,hKernelHook,TVI_LAST);
	HTREEITEM subShadowSSDT=m_TreeCtrl.InsertItem(_T("ShadowSSDT"),0,0,hKernelHook,TVI_LAST);
	HTREEITEM subNtosHook=m_TreeCtrl.InsertItem(_T("NtosHook"),0,0,hKernelHook,TVI_LAST);
	HTREEITEM subObjectHook=m_TreeCtrl.InsertItem(_T("ObjectHook"),0,0,hKernelHook,TVI_LAST);

	/////////////////////////////////////////////////////////////////////////
	HTREEITEM subKernelThread=m_TreeCtrl.InsertItem(_T("�ں��߳�"),2,2,hKernelThread,TVI_LAST);
	HTREEITEM subSystemThread=m_TreeCtrl.InsertItem(_T("ϵͳ�߳�"),2,2,hKernelThread,TVI_LAST);
 	HTREEITEM subDpcTimer=m_TreeCtrl.InsertItem(_T("Dpc��ʱ��"),2,2,hKernelThread,TVI_LAST);
 	HTREEITEM subSystemProc=m_TreeCtrl.InsertItem(_T("ϵͳ�ص�"),2,2,hKernelThread,TVI_LAST);
	////////////////////////////////////////////////////////////////////////
	HTREEITEM subDispatchNtfs=m_TreeCtrl.InsertItem(_T("Ntfs/Fsd"),4,4,hDispatch,TVI_LAST);

	if (IsWindows7())
	{
		HTREEITEM subDispatchNsiproxy=m_TreeCtrl.InsertItem(_T("Nsiproxy"),4,4,hDispatch,TVI_LAST);
	}else
		HTREEITEM subDispatchTcpip=m_TreeCtrl.InsertItem(_T("Tcpip"),4,4,hDispatch,TVI_LAST);

	HTREEITEM subDispatchKbdclass=m_TreeCtrl.InsertItem(_T("����Kbdclass"),4,4,hDispatch,TVI_LAST);
	HTREEITEM subDispatchMouclass=m_TreeCtrl.InsertItem(_T("���Mouclass"),4,4,hDispatch,TVI_LAST);

	if (!IsWindows7())
		HTREEITEM subDispatchAtapi=m_TreeCtrl.InsertItem(_T("Atapi"),4,4,hDispatch,TVI_LAST);

	//HTREEITEM subDispatchAcpi=m_TreeCtrl.InsertItem(_T("Acpi"),4,4,hDispatch,TVI_LAST);
	///////////////////////////////////////////////////////////////////////
// 	HTREEITEM subHandle=m_TreeCtrl.InsertItem(_T("���ռ��"),9,9,hRing3Memory,TVI_LAST);
// 	HTREEITEM subMessageHook=m_TreeCtrl.InsertItem(_T("��Ϣ����"),9,9,hRing3Memory,TVI_LAST);
	///////////////////////////////////////////////////////////////////////
	LONG lStyle;
    lStyle = GetWindowLong(m_ListCtrl.m_hWnd, GWL_STYLE);//��ȡ��ǰ����style
    lStyle &= ~LVS_TYPEMASK; //�����ʾ��ʽλ
    lStyle |= LVS_REPORT; //����style
    SetWindowLong(m_ListCtrl.m_hWnd, GWL_STYLE, lStyle);//����style

    DWORD dwStyle = m_ListCtrl.GetExtendedStyle();
    dwStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;//ѡ��ĳ��ʹ���и�����ֻ������report����listctrl��
    //dwStyle |= LVS_EX_GRIDLINES;//�����ߣ�ֻ������report����listctrl��
    //dwStyle |= LVS_EX_CHECKBOXES;//itemǰ����checkbox�ؼ�
    m_ListCtrl.SetExtendedStyle(dwStyle); //������չ���
	//m_ListCtrl.InsertColumn( 0, _T("ID"), LVCFMT_CENTER, 40 );//������
	//m_ListCtrl.InsertColumn( 1, _T("NAME"), LVCFMT_CENTER, 50 );
	m_ListCtrl.SetExtendedStyle(m_ListCtrl.GetExtendedStyle()|LVS_EX_SUBITEMIMAGES);

	EnableDebugPriv(SE_DEBUG_NAME);
	Install(m_hWnd);

	HANDLE m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)QueryInternetInfo,m_hWnd, 0,NULL);
	CloseHandle(m_hThread);

	//���ڶ���
	SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);

	char lpsz3600safeRunKey[100] = {0};

	memset(lpsz3600safeRunKey,0,sizeof(lpsz3600safeRunKey));
	QueryUserAgent(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run","A-Protect",lpsz3600safeRunKey);
	if (strstr(lpsz3600safeRunKey,"\\") == 0)
	{
		UninstallDepthServicesScan("A-Protect");
	}
	//��ʼ����ʱ��ö�ٽ���
	for(int Index = 0; Index < PROCESS_MAX_COLUMN; Index++)
	{
		m_ListCtrl.InsertColumn(Index, ProcessStr[Index] ,LVCFMT_LEFT, Width[Index]);
	}
	m_case=12;
	QuerySystemProcess(m_hWnd,IDC_DebugStatus,&m_ListCtrl);

		//����Ϊ����
	/*CImageList ImageListBmp;
	ImageListBmp.Create(16,16, ILC_COLOR32, 2, 1);
	ImageListBmp.Add(imgApp->LoadIconW(IDI_ICON5));
	m_ListCtrl.SetImageList(&ImgList,LVSIL_SMALL);
	int i;
	i=m_ListCtrl.InsertItem(1,_T("fdsfsdfds"));
	m_ListCtrl.SetItem(i, 3, LVIF_IMAGE, NULL, 1, 0, 0, 0);
	m_ListCtrl.SetItemState(i,INDEXTOSTATEIMAGEMASK(1),LVIS_STATEIMAGEMASK);*/
}



// CMy3600safeView ���

#ifdef _DEBUG
void CMy3600safeView::AssertValid() const
{
	CFormView::AssertValid();
}

void CMy3600safeView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CMy3600safeDoc* CMy3600safeView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMy3600safeDoc)));
	return (CMy3600safeDoc*)m_pDocument;
}
#endif //_DEBUG


// CMy3600safeView ��Ϣ�������

void CMy3600safeView::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
	HTREEITEM hTree=m_TreeCtrl.GetSelectedItem();
	if(!m_TreeCtrl.ItemHasChildren(hTree))
	{
		//���listview
		//SSDT
		for(int Index = 0; Index < SSDT_MAX_COLUMN; Index++)
		{
			m_ListCtrl.DeleteColumn(0);
		}
		//shadowSSDT
		for(int Index = 0; Index < SHADOW_MAX_COLUMN; Index++)
		{
			m_ListCtrl.DeleteColumn(1);
		}
		//kernel HOOK
		for(int Index = 0; Index < KERNEL_HOOK_MAX_COLUMN; Index++)
		{
			m_ListCtrl.DeleteColumn(2);
		}
		//object hook
		for(int Index = 0; Index < OBJECT_HOOK_MAX_COLUMN; Index++)
		{
			m_ListCtrl.DeleteColumn(3);
		}
		//kernel module
		for(int Index = 0; Index < KERNEL_MODULE_MAX_COLUMN; Index++)
		{
			m_ListCtrl.DeleteColumn(4);
		}
		//kernel thread
		for(int Index = 0; Index < KTHREAD_MAX_COLUMN; Index++)
		{
			m_ListCtrl.DeleteColumn(5);
		}
		for(int Index = 0; Index < STHREAD_MAX_COLUMN; Index++)
		{
			m_ListCtrl.DeleteColumn(6);
		}
		for(int Index = 0; Index < FILTER_MAX_COLUMN; Index++)
		{
			m_ListCtrl.DeleteColumn(7);
		}
		for(int Index = 0; Index < FSD_MAX_COLUMN; Index++)
		{
			m_ListCtrl.DeleteColumn(8);
		}
		for(int Index = 0; Index < TCPIP_MAX_COLUMN; Index++)
		{
			m_ListCtrl.DeleteColumn(9);
		}
		for(int Index = 0; Index < NSIPROXY_MAX_COLUMN; Index++)
		{
			m_ListCtrl.DeleteColumn(10);
		}
		////
		for(int Index = 0; Index < PROCESS_MAX_COLUMN; Index++)
		{
			m_ListCtrl.DeleteColumn(11);
		}
		////
		for(int Index = 0; Index < SERVICES_MAX_COLUMN; Index++)
		{
			m_ListCtrl.DeleteColumn(12);
		}
		for(int Index = 0; Index < TCPVIEW_MAX_COLUMN; Index++)
		{
			m_ListCtrl.DeleteColumn(13);
		}
		for(int Index = 0; Index < HIPSLOG_MAX_COLUMN; Index++)
		{
			m_ListCtrl.DeleteColumn(14);
		}
		for(int Index = 0; Index < KBDCLASS_MAX_COLUMN; Index++)
		{
			m_ListCtrl.DeleteColumn(15);
		}
		for(int Index = 0; Index < MOUCLASS_MAX_COLUMN; Index++)
		{
			m_ListCtrl.DeleteColumn(16);
		}
		for(int Index = 0; Index < ATAPI_MAX_COLUMN; Index++)
		{
			m_ListCtrl.DeleteColumn(17);
		}
		for(int Index = 0; Index < DPCTIMER_MAX_COLUMN; Index++)
		{
			m_ListCtrl.DeleteColumn(5);
		}
		for(int Index = 0; Index < SYSTEMNOTIFY_MAX_COLUMN; Index++)
		{
			m_ListCtrl.DeleteColumn(4);
		}
		m_ListCtrl.DeleteAllItems();

		if(m_TreeCtrl.GetItemText(hTree)==_T("SSDT"))
		{
			for(int Index = 0; Index < SSDT_MAX_COLUMN; Index++)
			{
				m_ListCtrl.InsertColumn(Index, SSDTStr[Index] ,LVCFMT_CENTER, SSDTWidth[Index]);
			}
			m_case=1;
			QuerySSDTHook(m_hWnd,IDC_DebugStatus,0,&m_ListCtrl);

		}else if(m_TreeCtrl.GetItemText(hTree)==_T("ShadowSSDT"))
		{
			for(int Index = 0; Index < SHADOW_MAX_COLUMN; Index++)
			{
				m_ListCtrl.InsertColumn(Index, ShadowSSDTStr[Index] ,LVCFMT_CENTER, ShadowSSDTWidth[Index]);
			}
			m_case=2;
			QueryShadowSSDTHook(m_hWnd,IDC_DebugStatus,0,&m_ListCtrl);
		}
		else if(m_TreeCtrl.GetItemText(hTree)==_T("NtosHook"))
		{
			for(int Index = 0; Index < KERNEL_HOOK_MAX_COLUMN; Index++)
			{
				m_ListCtrl.InsertColumn(Index, HookStr[Index] ,LVCFMT_CENTER, HookWidth[Index]);
			}
			m_case=3;
			QueryKernelHook(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
		}
		else if(m_TreeCtrl.GetItemText(hTree)==_T("ObjectHook"))
		{
			for(int i = 0; i < OBJECT_HOOK_MAX_COLUMN; i++)
			{
				m_ListCtrl.InsertColumn(i,OBJStr[i], LVCFMT_LEFT,OBJWidth[i]);
			}
			m_case=4;
			QueryObjectHook(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
		}
		else if(m_TreeCtrl.GetItemText(hTree)==_T("�ں�ģ��"))
		{
			for(int Index = 0; Index < KERNEL_MODULE_MAX_COLUMN; Index++)
			{
				m_ListCtrl.InsertColumn(Index, ModuleStr[Index] ,LVCFMT_LEFT, MWidth[Index]);
			}
			m_case=5;
			QueryKernelModule(m_hWnd,IDC_DebugStatus,&m_ListCtrl,0);
		}
		else if(m_TreeCtrl.GetItemText(hTree)==_T("�ں��߳�"))
		{
			for(int i = 0; i < KTHREAD_MAX_COLUMN; i++)
			{
				m_ListCtrl.InsertColumn(i,KernelThreadStr[i], LVCFMT_LEFT,KernelThreadWidth[i]);
			}
			m_case=6;
			QueryKernelThread(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
		}
		else if(m_TreeCtrl.GetItemText(hTree)==_T("ϵͳ�߳�"))
		{
			for(int i = 0; i < STHREAD_MAX_COLUMN; i++)
			{
				m_ListCtrl.InsertColumn(i,SystemThreadStr[i], LVCFMT_LEFT,SystemThreadWidth[i]);
			}
			m_case=7;
			QuerySystemThread(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
		}
		else if(m_TreeCtrl.GetItemText(hTree)==_T("��������"))
		{
			for(int i = 0; i < FILTER_MAX_COLUMN; i++)
			{
				m_ListCtrl.InsertColumn(i,GLStr[i], LVCFMT_LEFT,GLWidth[i]);
			}
			m_case=8;
			QueryFilterDriver(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
		}
		else if(m_TreeCtrl.GetItemText(hTree)==_T("Ntfs/Fsd"))
		{
			for(int i = 0; i < FSD_MAX_COLUMN; i++)
			{
				m_ListCtrl.InsertColumn(i,FSDStr[i], LVCFMT_LEFT,FSDWidth[i]);
			}
			m_case=9;
			QueryNtfsFsdHook(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
		}
		else if(m_TreeCtrl.GetItemText(hTree)==_T("Tcpip"))
		{
			for(int Index = 0; Index < TCPIP_MAX_COLUMN; Index++)
			{
				m_ListCtrl.InsertColumn(Index, TcpStr[Index] ,LVCFMT_LEFT, TcpWidth[Index]);
			}
			m_case=10;
			QueryTcpipHook(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
		}
		else if(m_TreeCtrl.GetItemText(hTree)==_T("Nsiproxy"))
		{
			for(int i = 0; i < NSIPROXY_MAX_COLUMN; i++)
			{
				m_ListCtrl.InsertColumn(i,NsipStr[i], LVCFMT_LEFT,NsipWidth[i]);
			}
			m_case=11;
			QueryNsipHook(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
		}
		else if(m_TreeCtrl.GetItemText(hTree)==_T("ϵͳ����"))
		{
			for(int Index = 0; Index < PROCESS_MAX_COLUMN; Index++)
			{
				m_ListCtrl.InsertColumn(Index, ProcessStr[Index] ,LVCFMT_LEFT, Width[Index]);
			}
			m_case=12;
			QuerySystemProcess(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
		}
		else if(m_TreeCtrl.GetItemText(hTree)==_T("ϵͳ����"))
		{
			for(int Index = 0; Index < SERVICES_MAX_COLUMN; Index++)
			{
				m_ListCtrl.InsertColumn(Index, ServiceStr[Index] ,LVCFMT_LEFT, SrcWidth[Index]);
			}
			m_case=13;
			QueryServices(m_hWnd,IDC_DebugStatus,&m_ListCtrl,0);
		}
		else if(m_TreeCtrl.GetItemText(hTree)==_T("��������"))
		{
			for(int Index = 0; Index < TCPVIEW_MAX_COLUMN; Index++)
			{
				m_ListCtrl.InsertColumn(Index, TCPStr[Index] ,LVCFMT_LEFT, TCWidth[Index]);
			}
			m_case=14;
			Tcpview(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
		}
		else if(m_TreeCtrl.GetItemText(hTree)==_T("��������"))
		{
			for(int Index = 0; Index < HIPSLOG_MAX_COLUMN; Index++)
			{
				m_ListCtrl.InsertColumn(Index, StealStr[Index] ,LVCFMT_LEFT, SCWidth[Index]);
			}
			m_case=15;
			HipsLog(m_hWnd,IDC_DebugStatus,&m_ListCtrl,0);
		}
		else if(m_TreeCtrl.GetItemText(hTree)==_T("����Kbdclass"))
		{
			for(int Index = 0; Index < KBDCLASS_MAX_COLUMN; Index++)
			{
				m_ListCtrl.InsertColumn(Index, KbdclassStr[Index] ,LVCFMT_CENTER, KbdclassWidth[Index]);
			}
			m_case=16;
			QueryKbdclassHook(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
		}
		else if(m_TreeCtrl.GetItemText(hTree)==_T("���Mouclass"))
		{
			for(int Index = 0; Index < MOUCLASS_MAX_COLUMN; Index++)
			{
				m_ListCtrl.InsertColumn(Index, MouclassStr[Index] ,LVCFMT_CENTER, MouclassWidth[Index]);
			}
			m_case=17;
			QueryMouclassHook(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
		}
		else if(m_TreeCtrl.GetItemText(hTree)==_T("Atapi"))
		{
			for(int Index = 0; Index < ATAPI_MAX_COLUMN; Index++)
			{
				m_ListCtrl.InsertColumn(Index, AtapiStr[Index] ,LVCFMT_CENTER, AtapiWidth[Index]);
			}
			m_case=18;
			QueryAtapiHook(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
		}
		else if(m_TreeCtrl.GetItemText(hTree)==_T("Dpc��ʱ��"))
		{
			for(int Index = 0; Index < DPCTIMER_MAX_COLUMN; Index++)
			{
				m_ListCtrl.InsertColumn(Index, DpcTimerStr[Index] ,LVCFMT_CENTER, DpcTimerWidth[Index]);
			}
			m_case=19;
			QueryDpcTimer(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
		}
		else if(m_TreeCtrl.GetItemText(hTree)==_T("ϵͳ�ص�"))
		{
			for(int Index = 0; Index < SYSTEMNOTIFY_MAX_COLUMN; Index++)
			{
				m_ListCtrl.InsertColumn(Index, SystemNotifyStr[Index] ,LVCFMT_CENTER, SystemNotifyWidth[Index]);
			}
			m_case=20;
			QuerySystemNotify(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
		}
	}
}
void CMy3600safeView::OnNMRClickList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;

	DWORD dwPos = GetMessagePos();
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	m_iItem=pNMListView->iItem;//��0��ʼ����
    CPoint point( LOWORD(dwPos), HIWORD(dwPos) );
	CMenu menu;

	switch (m_case)
	{
	case 1:
		menu.LoadMenu( IDR_SSDT_MENU );
		break;
	case 2:
		menu.LoadMenu(IDR_SHADOWSSDT_MENU);
		break;
	case 3:
		menu.LoadMenu(IDR_KERNELHOOK_MENU);
		break;
	case 5:
		menu.LoadMenu(IDR_KERNELMODULE_MENU);
		break;
	case 6:
		menu.LoadMenu(IDR_KERNELTHREAD_MENU);
		break;
	case 7:
		menu.LoadMenu(IDR_SYSTEMTHREAD_MENU);
		break;
	case 8:
		menu.LoadMenu(IDR_FILTERDRIVER_MENU);
		break;
	case 9:
		menu.LoadMenu(IDR_FSDHOOK_MENU);
		break; 
	case 10:
		menu.LoadMenu(IDR_TCPIPHOOK_MENU);
		break; 
	case 11:
		menu.LoadMenu(IDR_NSIPHOOK_MENU);
		break; 
	case 12:
		menu.LoadMenu(IDR_PROCESS_MENU);
		break; 
	case 13:
		menu.LoadMenu(IDR_SERVICES_MENU);
		break; 
	case 14:
		menu.LoadMenu(IDR_TCPVIEW_MENU);
		break; 
	case 15:
		menu.LoadMenu(IDR_HIPSLOG_MENU);
		break; 
	case 16:
		menu.LoadMenu(IDR_KBDCLASSHOOK_MENU);
		break; 
	case 17:
		menu.LoadMenu(IDR_MOUCLASSHOOK_MENU);
		break; 
	case 18:
		menu.LoadMenu(IDR_ATAPIHOOK_MENU);
		break; 
	case 19:
		menu.LoadMenu(IDR_DPCTIMER_MENU);
		break; 
	case 20:
		menu.LoadMenu(IDR_SYSTEMNOTIFY_MENU);
		break; 
	default:
		return;
	}
    CMenu* popup = menu.GetSubMenu(0);
    ASSERT( popup != NULL );
    popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this );
}
void CMy3600safeView::OnAbout()
{
	// TODO: �ڴ���������������
 	CAboutDlg	Dlg;
 	Dlg.DoModal();
}
void CMy3600safeView::OnOpenUrl()
{
	char szCommand[256] = {0};
	char lpszWinDir[256] = {0};
	char lpszSysDisk[10] = {0};

	if (strlen(lpOpenUrl) > 0)
	{
		// 		MessageBoxA(0,lpOpenUrl,0,0);
		// 		ShellExecuteA(0,0,lpOpenUrl,0,0,SW_SHOW);
		GetWindowsDirectoryA(lpszWinDir,sizeof(lpszWinDir));
		memcpy(lpszSysDisk,lpszWinDir,2);

		wsprintfA(szCommand,"%s\\Program Files\\Internet Explorer\\IEXPLORE.EXE %s",lpszSysDisk,lpOpenUrl);

		//MessageBoxA(0,szCommand,0,0);
		WinExec(szCommand,SW_SHOW);
	}
}

void CMy3600safeView::OnBooting()
{
	char lpszRun[256] = {0};
	char lpszModuleFile[256] = {0};
	DWORD dwReadByte;

	if (InstallDepthServicesScan("A-Protect"))
	{
		ReadFile((HANDLE)PROTECT_DRIVER_FILE,0,0,&dwReadByte,0);

		//���ÿ���������˵��exeפ����Ҫ��������
		ReadFile((HANDLE)PROTECT_PROCESS,0,0,&dwReadByte,0);

		//����ע���run������
		GetModuleFileNameA(NULL,lpszModuleFile,sizeof(lpszModuleFile));
		wsprintfA(lpszRun,"reg add HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run /v A-Protect /t REG_SZ /d \"%s\" /f",lpszModuleFile);
		WinExec(lpszRun,SW_HIDE);

		MessageBoxW(L"�����ɹ���",0,MB_ICONWARNING);
	}
}
void CMy3600safeView::OnCancelBooting()
{
	DWORD dwReadByte;
	if (UninstallDepthServicesScan("A-Protect"))
	{
		ReadFile((HANDLE)UNPROTECT_DRIVER_FILE,0,0,&dwReadByte,0);

		ReadFile((HANDLE)UNPROTECT_PROCESS,0,0,&dwReadByte,0);

		SHDeleteValueA(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run","A-Protect");  //ɾ������run��
		MessageBoxW(L"�����ɹ���",0,MB_ICONWARNING);
	}
}
void CMy3600safeView::OnClose()
{
	if (MessageBoxW(L"�˳�֮�󣬡�A�ܵ��Է��������޷������ں˰�ȫ��ͬʱ�޷�����ľ���ϵͳ�Ĺ�����Ϊ��ȷ��Ҫ�˳���", 0, MB_ICONINFORMATION|MB_YESNO) == IDYES)
	{
		DWORD dwReadByte;
		ReadFile((HANDLE)EXIT_PROCESS,0,0,&dwReadByte,0);
		Sleep(2000);
		//exit(0);
		ExitProcess(0);
	}
}
void CMy3600safeView::OnOpenKernelType()
{
	// TODO: �ڴ���������������
	DWORD dwReadByte;
	ReadFile((HANDLE)KERNEL_SAFE_MODULE,0,0,&dwReadByte,0);
	AfxMessageBox(L"����\"�ں˰�ȫģʽ\"�ɹ�!");
}
void CMy3600safeView::OnShutdownKernelType()
{
	// TODO: �ڴ���������������
	DWORD dwReadByte;
	ReadFile((HANDLE)NO_KERNEL_SAFE_MODULE,0,0,&dwReadByte,0);
	AfxMessageBox(L"�ر�\"�ں˰�ȫģʽ\"�ɹ�!");
}
void CMy3600safeView::OnWindowsOverhead()
{
	// TODO: �ڴ���������������
	SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	AfxMessageBox(L"���ô��ڶ��óɹ�!");
}
void CMy3600safeView::OnCancelTheOverhead()
{
	// TODO: �ڴ���������������
	SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	AfxMessageBox(L"ȡ�����ڶ��óɹ�!");
}
void CMy3600safeView::OnProtectProcess()
{
	// TODO: �ڴ���������������
	DWORD dwReadByte;
	ReadFile((HANDLE)PROTECT_PROCESS,0,0,&dwReadByte,0);
	AfxMessageBox(L"������A�ܵ��Է�����������̱����ɹ�!");
}
void CMy3600safeView::OnUnProtectProcess()
{
	// TODO: �ڴ���������������
	DWORD dwReadByte;
	ReadFile((HANDLE)UNPROTECT_PROCESS,0,0,&dwReadByte,0);
	AfxMessageBox(L"�رա�A�ܵ��Է�����������̱���!");
}
//*********************************************************
//SSDT
//*********************************************************
void CMy3600safeView::SSDTHook()
{
	m_ListCtrl.DeleteAllItems();
	QuerySSDTHook(m_hWnd,IDC_DebugStatus,0,&m_ListCtrl);
}
void CMy3600safeView::SSDTAllHook()
{
	m_ListCtrl.DeleteAllItems();
	QuerySSDTHook(m_hWnd,IDC_DebugStatus,1,&m_ListCtrl);
}
void CMy3600safeView::UnHookSSDTHookSelect()
{
	UnHookSSDT(m_hWnd,&m_ListCtrl);
}
void CMy3600safeView::UnHookSSDTHookAll()
{
	UnHookSSDTAll(m_hWnd,&m_ListCtrl);
}
void CMy3600safeView::OnCopySSDTDataToClipboard()
{
	CopySSDTDataToClipboard(m_hWnd,&m_ListCtrl);
}
//*********************************************************
//ShadowSSDT
//*********************************************************
void CMy3600safeView::ShadowSSDTHook()
{
	m_ListCtrl.DeleteAllItems();
	QueryShadowSSDTHook(m_hWnd,IDC_DebugStatus,0,&m_ListCtrl);
}
void CMy3600safeView::ShadowSSDTAllHook()
{
	m_ListCtrl.DeleteAllItems();
	QueryShadowSSDTHook(m_hWnd,IDC_DebugStatus,1,&m_ListCtrl);
}
void CMy3600safeView::UnHookShadowSSDTHookSelect()
{
	UnHookShadowSSDT(m_hWnd,&m_ListCtrl);
}
void CMy3600safeView::UnHookShadowSSDTHookAll()
{
	UnHookShadowSSDTAll(m_hWnd,&m_ListCtrl);
}
void CMy3600safeView::OnCopyShadowSSDTDataToClipboard()
{
	CopyShadowSSDTDataToClipboard(m_hWnd,&m_ListCtrl);
}
//******************************************************************
//kernel hook
//******************************************************************
void CMy3600safeView::OnKernelHook()
{
	m_ListCtrl.DeleteAllItems();
	QueryKernelHook(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
}
void CMy3600safeView::OnUnHookKernelHookSelect()
{
	UnHookKernelHookSelect(m_hWnd,&m_ListCtrl);
}
void CMy3600safeView::OnByPassKernelHook()
{
	ByPassKernelHook(m_hWnd,&m_ListCtrl);
}
void CMy3600safeView::OnCopyKernelHookDataToClipboard()
{
	CopyKernelHookDataToClipboard(m_hWnd,&m_ListCtrl);
}
//******************************************************************
//kernelmodule
//******************************************************************
void CMy3600safeView::OnKernelModule()
{
	m_ListCtrl.DeleteAllItems();
	QueryKernelModule(m_hWnd,IDC_DebugStatus,&m_ListCtrl,0);
}
void CMy3600safeView::OnKernelModule1()
{
	m_ListCtrl.DeleteAllItems();
	QueryKernelModule(m_hWnd,IDC_DebugStatus,&m_ListCtrl,1);
}
void CMy3600safeView::OnDumpMemoryDataToFile()
{
	OnDumpmemorydatatofile(m_hWnd,&m_ListCtrl);
}
void CMy3600safeView::OnCopyKernelModuleDataToClipboard()
{
	CopyKernelModuleDataToClipboard(m_hWnd,&m_ListCtrl);
}
//**********************************************************************
//kernel thread
//******************************************************************
void CMy3600safeView::OnKernelThreadList()
{
	m_ListCtrl.DeleteAllItems();
	QueryKernelThread(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
}
void CMy3600safeView::OnClearKernelThreadLog()
{
	ClearKernelThreadLog(&m_ListCtrl);
}
//***********************************************************************
//system thread
//******************************************************************
void CMy3600safeView::OnSystemThreadList()
{
	m_ListCtrl.DeleteAllItems();
	QuerySystemThread(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
}
void CMy3600safeView::OnKillSystemThread()
{
	KillSystemThread(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
};
void CMy3600safeView::OnSuspendThread()
{
	SuspendThread(m_hWnd,&m_ListCtrl);
	m_ListCtrl.DeleteAllItems();
	QuerySystemThread(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
}
void CMy3600safeView::OnResumeThread()
{
	ResumeThread(m_hWnd,&m_ListCtrl);
	m_ListCtrl.DeleteAllItems();
	QuerySystemThread(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
}
//***********************************************************************
//kernel filter driver
//******************************************************************
void CMy3600safeView::OnFilterDriverList()
{
	m_ListCtrl.DeleteAllItems();
	QueryFilterDriver(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
}
void CMy3600safeView::OnDeleteSelectFilterDriver()
{
	DeleteSelectFilterDriver(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
}
//************************************************************************
//ntfs fsd
//******************************************************************
void CMy3600safeView::OnFsdHookList()
{
	m_ListCtrl.DeleteAllItems();
	QueryNtfsFsdHook(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
}
void CMy3600safeView::OnFsdHookResetOne()
{
	FsdHookResetOne(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
}
void CMy3600safeView::OnCopyFsdDataToClipboard()
{
	CopyFsdDataToClipboard(m_hWnd,&m_ListCtrl);
}
//************************************************************************
//tcpip
//******************************************************************
void CMy3600safeView::OnTcpipHookList()
{
	m_ListCtrl.DeleteAllItems();
	QueryTcpipHook(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
}
void CMy3600safeView::OnTcpipHookResetOne()
{
	TcpipHookResetOne(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
}
void CMy3600safeView::OnCopyTcpipDataToClipboard()
{
	CopyTcpipDataToClipboard(m_hWnd,&m_ListCtrl);
}
//************************************************************************
//nsiproxy
//******************************************************************
void CMy3600safeView::OnNsipHookList()
{
	m_ListCtrl.DeleteAllItems();
	QueryNsipHook(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
}
void CMy3600safeView::OnNsipHookResetOne()
{
	NsipHookResetOne(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
}
void CMy3600safeView::OnCopyNsipDataToClipboard()
{
	CopyNsipDataToClipboard(m_hWnd,&m_ListCtrl);
}
//************************************************************************
//process
//************************************************************************
void CMy3600safeView::OnQueryProcess()
{
	m_ListCtrl.DeleteAllItems();
	QuerySystemProcess(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
}
WCHAR lpwzNum[50];  //ȫ�ֱ���
void CMy3600safeView::OnProcessmodule()
{
	CString PidNum;

	POSITION pos = m_ListCtrl.GetFirstSelectedItemPosition(); //�ж��б�����Ƿ���ѡ����
	int Item = m_ListCtrl.GetNextSelectedItem(pos); //���б��б�ѡ�����һ������ֵ���浽������
	PidNum.Format(L"%s",m_ListCtrl.GetItemText(Item,0));
	memset(lpwzNum,0,sizeof(lpwzNum));
	wcscat(lpwzNum,PidNum);

	CSubModule m_Module;
	m_Module.m_Subcase = 1;//���ֽ���ģ�顢����Hook�����̹��ӵȡ�
	m_Module.m_SubItem = 1;//m_iItem;//��¼���Ҽ�������һ��
	m_Module.DoModal();
}
void CMy3600safeView::OnProcessHandle()
{
	CString PidNum;

	POSITION pos = m_ListCtrl.GetFirstSelectedItemPosition(); //�ж��б�����Ƿ���ѡ����
	int Item = m_ListCtrl.GetNextSelectedItem(pos); //���б��б�ѡ�����һ������ֵ���浽������
	PidNum.Format(L"%s",m_ListCtrl.GetItemText(Item,0));
	memset(lpwzNum,0,sizeof(lpwzNum));
	wcscat(lpwzNum,PidNum);

	CSubModule m_Module;
	m_Module.m_Subcase = 2;//���ֽ���ģ�顢����Hook�����̹��ӵȡ�
	m_Module.m_SubItem = 2;//m_iItem;//��¼���Ҽ�������һ��
	m_Module.DoModal();
}
void CMy3600safeView::OnProcessThread()
{
	CString EProcess;

	POSITION pos = m_ListCtrl.GetFirstSelectedItemPosition(); //�ж��б�����Ƿ���ѡ����
	int Item = m_ListCtrl.GetNextSelectedItem(pos); //���б��б�ѡ�����һ������ֵ���浽������
	EProcess.Format(L"%s",m_ListCtrl.GetItemText(Item,4));

	memset(lpwzNum,0,sizeof(lpwzNum));
	wcscat(lpwzNum,EProcess);

	CSubModule m_Module;
	m_Module.m_Subcase = 3;//���ֽ���ģ�顢����Hook�����̹��ӵȡ�
	m_Module.m_SubItem = 3;//m_iItem;//��¼���Ҽ�������һ��

	m_Module.DoModal();
}
void CMy3600safeView::OnKillProcess()
{
	KillProcess(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
}
void CMy3600safeView::OnKillProcessDeleteFile()
{
	KillProcessDeleteFile(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
}
void CMy3600safeView::OnProcessVerify()
{
	ProcessVerify(m_hWnd,&m_ListCtrl);
}
void CMy3600safeView::OnCopyProcessDataToClipboard()
{
	CopyProcessDataToClipboard(m_hWnd,&m_ListCtrl);
}
//************************************************************************
//services
//************************************************************************
void CMy3600safeView::OnGetServicesList()
{
	m_ListCtrl.DeleteAllItems();
	QueryServices(m_hWnd,IDC_DebugStatus,&m_ListCtrl,0);
}
void CMy3600safeView::OnGetDepthServicesList()
{
	m_ListCtrl.DeleteAllItems();
	GetDepthServicesList(m_hWnd,IDC_DebugStatus,&m_ListCtrl,1);
}
void CMy3600safeView::OnStartServices()
{
	StartServices(m_hWnd,&m_ListCtrl);
}
void CMy3600safeView::OnStopServices()
{
	StopServices(m_hWnd,&m_ListCtrl);
}
void CMy3600safeView::OnDeleteServices()
{
	m_ListCtrl.DeleteAllItems();
	DeleteServices(m_hWnd,&m_ListCtrl);
}
void CMy3600safeView::OnManualServices()
{
	ManualServices(m_hWnd,&m_ListCtrl);
}
void CMy3600safeView::OnAutoServices()
{
	AutoServices(m_hWnd,&m_ListCtrl);
}
void CMy3600safeView::OnDisableServices()
{
	DisableServices(m_hWnd,&m_ListCtrl);
}
//************************************************************************
//tcpview
//************************************************************************
void CMy3600safeView::OnMsgTcpView()
{
	m_ListCtrl.DeleteAllItems();
	Tcpview(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
}
void CMy3600safeView::OnMsgTcpProKill()
{
	TcpProKill(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
}
//************************************************************************
//HipsLog
//************************************************************************
void CMy3600safeView::OnListLog()
{
	m_ListCtrl.DeleteAllItems();
	HipsLog(m_hWnd,IDC_DebugStatus,&m_ListCtrl,0);
}
void CMy3600safeView::OnClearListLog()
{
	ClearListLog(&m_ListCtrl);
}
void CMy3600safeView::OnConnectScan()
{
	ConnectScan(m_hWnd);
}
void CMy3600safeView::OnSaveToFile()
{
	SaveToFile(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
}
//************************************************************************
//Kbdclass
//******************************************************************
void CMy3600safeView::OnKbdclassHookList()
{
	m_ListCtrl.DeleteAllItems();
	QueryKbdclassHook(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
}
void CMy3600safeView::OnKbdclassHookResetOne()
{
	KbdclassHookResetOne(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
}
void CMy3600safeView::OnCopyKbdclassDataToClipboard()
{
	CopyKbdclassDataToClipboard(m_hWnd,&m_ListCtrl);
}
//************************************************************************
//Mouclass
//******************************************************************
void CMy3600safeView::OnMouclassHookList()
{
	m_ListCtrl.DeleteAllItems();
	QueryMouclassHook(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
}
void CMy3600safeView::OnMouclassHookResetOne()
{
	MouclassHookResetOne(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
}
void CMy3600safeView::OnCopyMouclassDataToClipboard()
{
	CopyMouclassDataToClipboard(m_hWnd,&m_ListCtrl);
}
//************************************************************************
//Atapi
//******************************************************************
void CMy3600safeView::OnAtapiHookList()
{
	m_ListCtrl.DeleteAllItems();
	QueryAtapiHook(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
}
void CMy3600safeView::OnAtapiHookResetOne()
{
	AtapiHookResetOne(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
}
void CMy3600safeView::OnCopyAtapiDataToClipboard()
{
	CopyAtapiDataToClipboard(m_hWnd,&m_ListCtrl);
}
//************************************************************************
//DpcTimer
//******************************************************************
void CMy3600safeView::OnDpcTimerList()
{
	m_ListCtrl.DeleteAllItems();
	QueryDpcTimer(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
}
void CMy3600safeView::OnKillDpcTimer()
{
	KillDpcTimer(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
}
void CMy3600safeView::OnCopyDpcTimerDataToClipboard()
{
	CopyDpcTimerDataToClipboard(m_hWnd,&m_ListCtrl);
}
//************************************************************************
//SystemNotify
//******************************************************************
void CMy3600safeView::OnSystemNotifyList()
{
	m_ListCtrl.DeleteAllItems();
	QuerySystemNotify(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
}
void CMy3600safeView::OnKillSystemNotify()
{
	KillSystemNotify(m_hWnd,IDC_DebugStatus,&m_ListCtrl);
}
void CMy3600safeView::OnCopySystemNotifyDataToClipboard()
{
	CopySystemNotifyDataToClipboard(m_hWnd,&m_ListCtrl);
}
//************************************************************************
void CMy3600safeView::OnLvnItemchangedList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
}
////////////////////////////////////////////////////////////////////
void CMy3600safeView::OnBnClickedQzdeletefile()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//AfxMessageBox(_T("������������"));
	DWORD dwReadByte;
	WCHAR lpwzDeletedFile[256];
	CString			m_path;

	//�ָ���PEPROCESS����Ȼ�޷���Ϊ������win7���޷��򿪶Ի���
	ReadFile((HANDLE)EXIT_PROCESS,0,0,&dwReadByte,0);
	//Sleep(3000);

	//ǿ��ɾ���ļ���ʱ��Ҫ�л����ں˰�ȫģʽȥ
	ReadFile((HANDLE)KERNEL_SAFE_MODULE,0,0,&dwReadByte,0);

	CFileDialog dlg( TRUE,L"*.*",0, 0,L"�����ļ�|*.*");
	dlg.m_ofn.lpstrTitle= L"����ɾ���ļ�";
	if ( dlg.DoModal() == IDOK )
	{
		m_path = dlg.GetPathName();

		memset(lpwzDeletedFile,0,sizeof(lpwzDeletedFile));
		wsprintfW(lpwzDeletedFile,L"\\??\\%ws",m_path);

		ReadFile((HANDLE)DELETE_FILE,lpwzDeletedFile,wcslen(lpwzDeletedFile),&dwReadByte,0);
		if (GetFileAttributesW(m_path) == INVALID_FILE_ATTRIBUTES)
		{
			MessageBoxW(L"�ļ�ɾ���ɹ���",0,MB_ICONWARNING);
		}
	}
	//�ټ��������Լ��Ľ���
	char lpszInit[4];
	memset(lpszInit,0,sizeof(lpszInit));
	ReadFile((HANDLE)SAFE_SYSTEM,lpszInit,4,&dwReadByte,0);
}

BOOL bDisLoadDriver = TRUE;
void CMy3600safeView::OnBnClickedDisdriver()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//AfxMessageBox(_T("������������"));
	DWORD dwReadByte;

	if (bDisLoadDriver == TRUE)
	{
		ReadFile((HANDLE)DIS_LOAD_DRIVER,0,0,&dwReadByte,0);
		bDisLoadDriver = FALSE;
		SetDlgItemText(IDC_DisLoadDriver,L"�����������");

	}else
	{
		ReadFile((HANDLE)LOAD_DRIVER,0,0,&dwReadByte,0);
		bDisLoadDriver = TRUE;
		SetDlgItemText(IDC_DisLoadDriver,L"��ֹ��������");
	}
}

BOOL bDisCreateProcess = TRUE;

void CMy3600safeView::OnBnClickedDisprocess()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//AfxMessageBox(_T("������������"));
	DWORD dwReadByte;

	if (bDisCreateProcess == TRUE)
	{
		ReadFile((HANDLE)DIS_CREATE_PROCESS,0,0,&dwReadByte,0);
		bDisCreateProcess = FALSE;
		SetDlgItemText(IDC_DisCreateProcess,L"����������");

	}else
	{
		ReadFile((HANDLE)CREATE_PROCESS,0,0,&dwReadByte,0);
		bDisCreateProcess = TRUE;
		SetDlgItemText(IDC_DisCreateProcess,L"��ֹ��������");
	}

}

BOOL bDisWriteFile = TRUE;

void CMy3600safeView::OnBnClickedDisfile()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//AfxMessageBox(_T("������������"));
	DWORD dwReadByte;

	if (bDisWriteFile == TRUE)
	{
		ReadFile((HANDLE)DIS_WRITE_FILE,0,0,&dwReadByte,0);
		bDisWriteFile = FALSE;
		SetDlgItemText(IDC_DisCreateFile,L"�������ļ�");

	}else
	{
		ReadFile((HANDLE)WRITE_FILE,0,0,&dwReadByte,0);
		bDisWriteFile = TRUE;
		SetDlgItemText(IDC_DisCreateFile,L"��ֹ�����ļ�");
	}
}
void CMy3600safeView::OnBnClickedShutdownSystem()
{
	DWORD dwReadByte;

	ReadFile((HANDLE)SHUT_DOWN_SYSTEM,0,0,&dwReadByte,0);
}
BOOL bDisResetServices = TRUE;
void CMy3600safeView::OnBnClickedServicesReset()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//AfxMessageBox(_T("������������"));
	DWORD dwReadByte;

	if (bDisResetServices == TRUE)
	{
		ReadFile((HANDLE)DIS_RESET_SRV,0,0,&dwReadByte,0);
		bDisResetServices = FALSE;
		SetDlgItemText(IDC_DisSrvReset,L"��������д");

	}else
	{
		ReadFile((HANDLE)RESET_SRV,0,0,&dwReadByte,0);
		bDisResetServices = TRUE;
		SetDlgItemText(IDC_DisSrvReset,L"��ֹ�����д");
	}
}
BOOL bDisKernelThread = TRUE;
void CMy3600safeView::OnBnClickedDisKernelThread()
{
	DWORD dwReadByte;

	if (bDisKernelThread == TRUE)
	{
		ReadFile((HANDLE)DIS_KERNEL_THREAD,0,0,&dwReadByte,0);
		bDisKernelThread = FALSE;
		SetDlgItemText(IDC_DisKernelThread,L"�����ں��߳�");

	}else
	{
		ReadFile((HANDLE)KERNEL_THREAD,0,0,&dwReadByte,0);
		bDisKernelThread = TRUE;
		SetDlgItemText(IDC_DisKernelThread,L"��ֹ�ں��߳�");
	}
}
void ReadDeskPath(LPCSTR DaskPath)
{
	CHAR path[255];
	ZeroMemory(path,255);
	SHGetSpecialFolderPathA(0,path,CSIDL_DESKTOPDIRECTORY,0);
	lstrcatA((char *)DaskPath,path);
	return;
}
void CMy3600safeView::OnUninstall360()
{
	DWORD dwReadByte;

	HKEY hkRoot2 = HKEY_LOCAL_MACHINE;
	HKEY hkRoot3 = HKEY_LOCAL_MACHINE;
	BOOL bFound360 = FALSE;

	if(RegOpenKeyExW(hkRoot2,L"SYSTEM\\CurrentControlSet\\Services\\zhudongfangyu", 0, KEY_QUERY_VALUE, &hkRoot2) == ERROR_SUCCESS ||
		RegOpenKeyExW(hkRoot3,L"SYSTEM\\CurrentControlSet\\Services\\360rp", 0, KEY_QUERY_VALUE, &hkRoot3) == ERROR_SUCCESS)
	{
		bFound360 = TRUE;
	}
	if (!bFound360)
	{
		MessageBoxW(L"��ϲ����ϵͳ��û�з���360������Ҫһ��ж�أ�",0,MB_ICONINFORMATION);
		return;
	}
	RegCloseKey(hkRoot2);
	RegCloseKey(hkRoot3);

	if (MessageBoxW(L"��A�ܵ��Է�������Ȼ�ṩһ��ж��360�Ĺ��ܣ�����������ж�ز��������û�������ʶȥ�����һ��ж��360�������ж�ء�\r\n\r\n3600safe�ٴ�ȷ���Ƿ�Ҫһ��ж��360��\r\n",0,MB_ICONINFORMATION|MB_YESNO) == IDYES)
	{
		//��ʼж��360
		SetDlgItemText(IDC_Uninstall360,L"����ж��...");

		//ȥ������
		ReadFile((HANDLE)UNPROTECT_360SAFE,0,0,&dwReadByte,0);
		Uninstall360();

		//�����ݷ�ʽ
		char lpszDesk[260] = {0};
		char lpsz360lnk[260] = {0};

		memset(lpszDesk,0,260);
		memset(lpsz360lnk,0,260);
		ReadDeskPath(lpszDesk);

		wsprintfA(lpsz360lnk,"%s\\360��ȫ��ʿ.lnk",lpszDesk);
		DeleteFileA(lpsz360lnk);

		memset(lpsz360lnk,0,260);
		wsprintfA(lpsz360lnk,"%s\\360����ܼ�.lnk",lpszDesk);
		DeleteFileA(lpsz360lnk);

		memset(lpsz360lnk,0,260);
		wsprintfA(lpsz360lnk,"%s\\360ɱ��.lnk",lpszDesk);
		DeleteFileA(lpsz360lnk);

		SetDlgItemText(IDC_Uninstall360,L"һ��ж��360");
		if (MessageBoxW(L"һ��ж����ϣ������360�ļ������´�ϵͳ����ʱ�Զ�ɾ����\r\n\r\n�Ƿ�����������",0,MB_ICONINFORMATION | MB_YESNO) == IDYES)
		{
			ReadFile((HANDLE)SHUT_DOWN_SYSTEM,0,0,&dwReadByte,0);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////