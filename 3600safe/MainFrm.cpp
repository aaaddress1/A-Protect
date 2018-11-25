
// MainFrm.cpp : CMainFrame ���ʵ��
//

#include "stdafx.h"
#include "3600safe.h"
#include "AboutDlg.h"
#include "3600safeView.h"
#include "C3600Splash.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	//ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	//ON_COMMAND(ID_Exit, &CMainFrame::OnExit)
	//ON_COMMAND(ID_About, &CMainFrame::OnAbout)
	ON_MESSAGE(WM_SHOWTASK,OnShowTask)
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // ״̬��ָʾ��
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame ����/����

CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;
    C3600Splash wndSplash;                 //���������������ʵ��  
	wndSplash.Create(IDB_SPLASH);  
	wndSplash.CenterWindow();  
	wndSplash.UpdateWindow();          //send WM_PAINT  
	Sleep(2000);  
	wndSplash.DestroyWindow();//���ٳ�ʼ���洰��  

	HICON m_hIcon;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(m_hIcon,TRUE);
	SetWindowTextW(_T("A�ܵ��Է��� v0.2.5"));

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ
	cs.style   &=   ~WS_MAXIMIZEBOX;
	cs.style&=~FWS_ADDTOTITLE;
	cs.style &= ~WS_THICKFRAME;//ʹ���ڲ��������ı��С

	cs.cx = 1700;
	cs.cy = 1700;

	return TRUE;
}

// CMainFrame ���

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame ��Ϣ�������

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* ɨ��˵�*/);
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	return 0;
}

void CMainFrame::OnClose()
{
	if (MessageBoxW(L"�˳�֮��A�ܵ��Է��� ���޷������ں˰�ȫ��ͬʱ�޷�����ľ���ϵͳ�Ĺ�����Ϊ��ȷ��Ҫ�˳���", 0, MB_ICONINFORMATION|MB_YESNO) == IDYES)
	{
		DWORD dwReadByte;
		ReadFile((HANDLE)EXIT_PROCESS,0,0,&dwReadByte,0);
		Sleep(2000);
		//exit(0);
		ExitProcess(0);
	}
}
void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if(nID==SC_MINIMIZE)
	{
		//���̴���
		NOTIFYICONDATA nid;
		nid.cbSize=(DWORD)sizeof(NOTIFYICONDATA);
		nid.hWnd=this->m_hWnd;
		nid.uID=IDR_MAINFRAME;
		nid.uFlags=NIF_INFO|NIF_ICON|NIF_MESSAGE|NIF_TIP;
		nid.uCallbackMessage=WM_SHOWTASK;//�Զ������Ϣ����
		nid.hIcon = AfxGetApp()->LoadIconW(IDR_MAINFRAME);
		lstrcpy(nid.szTip,_T("A�ܵ��Է��� ���ڱ�������ϵͳ..."));
		lstrcpy(nid.szInfoTitle,_T("A�ܵ��Է��� "));
		lstrcpy(nid.szInfo,_T("A�ܵ��Է��� ���ڱ�������ϵͳ..."));
		Shell_NotifyIcon(NIM_ADD,&nid);
		ShowWindow(SW_HIDE);
	}
	else
		CFrameWndEx::OnSysCommand(nID, lParam);
}

LRESULT CMainFrame::OnShowTask(WPARAM wParam,LPARAM lParam)
{
	if((lParam == WM_RBUTTONUP) || (lParam == WM_LBUTTONUP))  
    {  
        ModifyStyleEx(0,WS_EX_TOPMOST);
		NOTIFYICONDATA nid;
		nid.cbSize=(DWORD)sizeof(NOTIFYICONDATA);
		nid.hWnd=this->m_hWnd;
		nid.uID=IDR_MAINFRAME;
		nid.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP;
		nid.uCallbackMessage=WM_SHOWTASK;//�Զ������Ϣ����
		nid.hIcon = AfxGetApp()->LoadIconW(IDR_MAINFRAME);
		lstrcpy(nid.szTip,TEXT("A�ܵ��Է��� "));
		Shell_NotifyIcon(NIM_DELETE,&nid);
        ShowWindow(SW_SHOWDEFAULT);
    } 
	return 0;
}