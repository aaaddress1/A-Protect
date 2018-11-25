#include "stdafx.h"
#include "3600safe.h"
#include "SubModule.h"
#include "afxdialogex.h"

#define DLLMODULE_MAX_COLUMN 5

HWND hWnd;

wchar_t	DllModuleStr[DLLMODULE_MAX_COLUMN][260]  = {_T("��ַ"),	_T("��С"),			_T("DLLģ��·��"),_T("ϵͳ��������"),_T("��֤�Ƿ�ϵͳԭ���ļ�")};										 
int		DllModuleWidth[DLLMODULE_MAX_COLUMN]=	{100,80,200,100,140};

wchar_t	HandleStr[DLLMODULE_MAX_COLUMN][260]  = {_T("������ʹ���"),_T("�������"),	_T("���������ļ�"),_T("���"),_T("�������")};										 
int		HandleWidth[DLLMODULE_MAX_COLUMN]=	{100,70,300,70,70};

#define PROCESS_THREAD_MAX_COLUMN 10

wchar_t	ProcessThreadStr[PROCESS_THREAD_MAX_COLUMN][260]  = {_T("�߳�Id"),	_T("ETHREAD"),			_T("TEB"),	_T("�߳����"),		
	_T("HideDebug"),_T("���ȼ�"),_T("�л�����"),_T("ģ��"),_T("�߳�״̬"),_T("��֤�Ƿ�ϵͳԭ���ļ�")};										 
int		ProcessThreadWidth[PROCESS_THREAD_MAX_COLUMN]=	{50, 70, 70, 80, 70,60,60,150,70,140};

void QueryProcessThread(HWND m_hWnd,ULONG ID,CClrListCtrl *m_list);
void KillProcessThread(HWND hWnd,CClrListCtrl *m_list,HANDLE ulCommand);

VOID EnumModule(HWND m_hWnd,ULONG ID,CClrListCtrl *m_list);
void UnLoadDLLModule(HWND m_hWnd,CClrListCtrl *m_list);
void UnLoadDLLModuleAndDelete(HWND m_hWnd,CClrListCtrl *m_list);
void CopyDLLDataToClipboard(HWND m_hWnd,CClrListCtrl *m_list);
void LookupDLLInServices(HWND m_hWnd,ULONG ID,CClrListCtrl *m_list);

VOID QueryProcessHandle(HWND m_hWnd,ULONG ID,CClrListCtrl *m_list);

IMPLEMENT_DYNAMIC(CSubModule, CDialogEx)

CSubModule::CSubModule(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSubModule::IDD, pParent)
{
	m_Subcase=0;
	m_SubItem=0;
}

CSubModule::~CSubModule()
{
}

void CSubModule::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_SubList);
}


BEGIN_MESSAGE_MAP(CSubModule, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_CLOSE, &CSubModule::OnBnClickedBtnClose)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CSubModule::OnNMRClickList1)

	ON_COMMAND(ID_UnLoadDLLModule, &CSubModule::OnUnLoadDLLModule)
	ON_COMMAND(ID_UnLoadDLLModuleAndDeleteFile, &CSubModule::OnUnLoadDLLModuleAndDelete)
	ON_COMMAND(ID_CopyDLLDataToClipboard, &CSubModule::OnCopyDLLDataToClipboard)
	ON_COMMAND(ID_LookupDLLInServices, &CSubModule::OnLookupDLLInServices)

	ON_COMMAND(ID_ProcessThreadList, &CSubModule::OnProcessThreadList)
	ON_COMMAND(ID_KillSelectProcessThread, &CSubModule::OnKillProcessThread)
	ON_COMMAND(ID_SuspendSelectProcessThread, &CSubModule::OnSuspendProcessThread)
	ON_COMMAND(ID_ResumeSelectProcessThread, &CSubModule::OnResumeProcessThread)


END_MESSAGE_MAP()


// CProcessModule ��Ϣ�������


void CSubModule::OnBnClickedBtnClose()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnOK();
}
DWORD WINAPI QueryThreadFuntion(CClrListCtrl *m_SubList)
{
	QueryProcessThread(hWnd,IDC_DllDebugStatus,m_SubList);
	return 0;
}
DWORD WINAPI QueryHandleFuntion(CClrListCtrl *m_SubList)
{
	DWORD dwReadByte;

	ReadFile((HANDLE)KERNEL_SAFE_MODULE,0,0,&dwReadByte,0);

	QueryProcessHandle(hWnd,IDC_DllDebugStatus,m_SubList);

	ReadFile((HANDLE)NO_KERNEL_SAFE_MODULE,0,0,&dwReadByte,0);
	return 0;
}
DWORD WINAPI QueryDllFuntion(CClrListCtrl *m_SubList)
{
	EnumModule(hWnd,IDC_DllDebugStatus,m_SubList);
	return 0;
}
BOOL CSubModule::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	LONG lStyle;
    lStyle = GetWindowLong(m_SubList.m_hWnd, GWL_STYLE);//��ȡ��ǰ����style
    lStyle &= ~LVS_TYPEMASK; //�����ʾ��ʽλ
    lStyle |= LVS_REPORT; //����style
    SetWindowLong(m_SubList.m_hWnd, GWL_STYLE, lStyle);//����style

    DWORD dwStyle = m_SubList.GetExtendedStyle();
    dwStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;//ѡ��ĳ��ʹ���и�����ֻ������report����listctrl��
    //dwStyle |= LVS_EX_GRIDLINES;//�����ߣ�ֻ������report����listctrl��
    //dwStyle |= LVS_EX_CHECKBOXES;//itemǰ����checkbox�ؼ�
    m_SubList.SetExtendedStyle(dwStyle); //������չ���

	hWnd = m_hWnd;
	switch (m_Subcase)
	{
	case 1:
		for(int Index = 0; Index < DLLMODULE_MAX_COLUMN; Index++)
		{
			m_SubList.InsertColumn(Index, DllModuleStr[Index] ,LVCFMT_LEFT, DllModuleWidth[Index]);
		}

		SetWindowText(L"DLLģ��");
		//EnumModule(m_hWnd,IDC_DllDebugStatus,&m_SubList);
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)QueryDllFuntion,&m_SubList, 0,NULL);
		break;
	case 2:
		for(int Index = 0; Index < DLLMODULE_MAX_COLUMN; Index++)
		{
			m_SubList.InsertColumn(Index, HandleStr[Index] ,LVCFMT_LEFT, HandleWidth[Index]);
		}
		SetWindowText(L"Handle");
		//QueryProcessHandle(m_hWnd,IDC_DllDebugStatus,&m_SubList);
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)QueryHandleFuntion,&m_SubList, 0,NULL);
		break;
	case 3:
		for(int Index = 0; Index < PROCESS_THREAD_MAX_COLUMN; Index++)
		{
			m_SubList.InsertColumn(Index, ProcessThreadStr[Index] ,LVCFMT_LEFT, ProcessThreadWidth[Index]);
		}
		SetWindowText(L"Thread");
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)QueryThreadFuntion,&m_SubList, 0,NULL);
		break;
	default:
		return 0;
	}
// 	m_SubList.InsertColumn( 0, _T("ID"), LVCFMT_CENTER, 40 );//������
// 	m_SubList.InsertColumn( 1, _T("NAME"), LVCFMT_CENTER, 50 );

	/*CString strtemp;
	strtemp.Format(_T("�������ǵ�%d�е�%d��"),
	m_Subcase, m_SubItem);
	MessageBox(strtemp);*/
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CSubModule::OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;

	//�Ҽ��˵�
	DWORD dwPos = GetMessagePos();
    CPoint point( LOWORD(dwPos), HIWORD(dwPos) );
	CMenu menu;

	switch (m_SubItem)
	{
	case 1:
		menu.LoadMenu(IDR_DLLMODULE_MENU);
		break;
	case 3:
		menu.LoadMenu(IDR_PROCESSTHREAD_MENU);
		break;
	default:
		return;
	}
	CMenu* popup = menu.GetSubMenu(0);
    ASSERT( popup != NULL );
    popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this );
}
void CSubModule::OnUnLoadDLLModule()
{
	UnLoadDLLModule(m_hWnd,&m_SubList);
}
void CSubModule::OnUnLoadDLLModuleAndDelete()
{
	UnLoadDLLModuleAndDelete(m_hWnd,&m_SubList);
}
void CSubModule::OnCopyDLLDataToClipboard()
{
	CopyDLLDataToClipboard(m_hWnd,&m_SubList);
}
void CSubModule::OnLookupDLLInServices()
{
	LookupDLLInServices(m_hWnd,IDC_DllDebugStatus,&m_SubList);
}
//***********************************
void CSubModule::OnProcessThreadList()
{
	m_SubList.DeleteAllItems();
	QueryProcessThread(m_hWnd,IDC_DllDebugStatus,&m_SubList);
}
void CSubModule::OnKillProcessThread()
{
	//KillProcessThreadFunction(m_hWnd,IDC_DllDebugStatus,&m_SubList);

	KillProcessThread(hWnd,&m_SubList,(HANDLE)KILL_SYSTEM_THREAD);

	m_SubList.DeleteAllItems();
	QueryProcessThread(m_hWnd,IDC_DllDebugStatus,&m_SubList);
}
void CSubModule::OnSuspendProcessThread()
{
	KillProcessThread(m_hWnd,&m_SubList,(HANDLE)SUSPEND_THREAD);
}
void CSubModule::OnResumeProcessThread()
{
	KillProcessThread(m_hWnd,&m_SubList,(HANDLE)RESUME_THREAD);
}