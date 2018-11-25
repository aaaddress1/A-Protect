
// 3600safeView.h : CMy3600safeView ��Ľӿ�
//

#pragma once
#include "3600safeDoc.h"
#include "resource.h"
#include "afxcmn.h"
#include "ClrListCtrl.h"

class CMy3600safeView : public CFormView
{
public: // �������л�����
	CMy3600safeView();
	DECLARE_DYNCREATE(CMy3600safeView)

public:
	enum{ IDD = IDD_MY3600SAFE_FORM };

// ����
public:
	CMy3600safeDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual void OnInitialUpdate(); // ������һ�ε���

// ʵ��
public:
	virtual ~CMy3600safeView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnFilePrintPreview();
	DECLARE_MESSAGE_MAP()
public:
	HICON m_hIcon;
	CTreeCtrl m_TreeCtrl;
	CClrListCtrl m_ListCtrl;
	afx_msg void OnOpenKernelType();
	afx_msg void OnWindowsOverhead();
	afx_msg void OnCancelTheOverhead();
	afx_msg void OnShutdownKernelType();
	afx_msg void OnProtectProcess();
	afx_msg void OnUnProtectProcess();
	afx_msg void OnBooting();
	afx_msg void OnCancelBooting();
	afx_msg void OnOpenUrl();
	afx_msg void OnClose();
	afx_msg void OnAbout();

	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void SSDTHook();
	afx_msg void SSDTAllHook();
	afx_msg void UnHookSSDTHookSelect();
	afx_msg void UnHookSSDTHookAll();
	afx_msg void OnCopySSDTDataToClipboard();

	afx_msg void ShadowSSDTHook();
	afx_msg void ShadowSSDTAllHook();
	afx_msg void UnHookShadowSSDTHookSelect();
	afx_msg void UnHookShadowSSDTHookAll();
	afx_msg void OnCopyShadowSSDTDataToClipboard();

	afx_msg void OnKernelHook();
	afx_msg void OnUnHookKernelHookSelect();
	afx_msg void OnByPassKernelHook();
	afx_msg void OnCopyKernelHookDataToClipboard();

	afx_msg void OnKernelModule();
	afx_msg void OnKernelModule1();
	afx_msg void OnDumpMemoryDataToFile();
	afx_msg void OnCopyKernelModuleDataToClipboard();

	afx_msg void OnKernelThreadList();
	afx_msg void OnClearKernelThreadLog();

	afx_msg void OnSystemThreadList();
	afx_msg void OnKillSystemThread();
	afx_msg void OnSuspendThread();
	afx_msg void OnResumeThread();

	afx_msg void OnFilterDriverList();
	afx_msg void OnDeleteSelectFilterDriver();

	afx_msg void OnFsdHookList();
	afx_msg void OnFsdHookResetOne();
	afx_msg void OnCopyFsdDataToClipboard();

	afx_msg void OnTcpipHookList();
	afx_msg void OnTcpipHookResetOne();
	afx_msg void OnCopyTcpipDataToClipboard();

	afx_msg void OnNsipHookList();
	afx_msg void OnNsipHookResetOne();
	afx_msg void OnCopyNsipDataToClipboard();

	afx_msg void OnKbdclassHookList();
	afx_msg void OnKbdclassHookResetOne();
	afx_msg void OnCopyKbdclassDataToClipboard();

	afx_msg void OnMouclassHookList();
	afx_msg void OnMouclassHookResetOne();
	afx_msg void OnCopyMouclassDataToClipboard();

	afx_msg void OnAtapiHookList();
	afx_msg void OnAtapiHookResetOne();
	afx_msg void OnCopyAtapiDataToClipboard();

	afx_msg void OnDpcTimerList();
	afx_msg void OnKillDpcTimer();
	afx_msg void OnCopyDpcTimerDataToClipboard();

	afx_msg void OnSystemNotifyList();
	afx_msg void OnKillSystemNotify();
	afx_msg void OnCopySystemNotifyDataToClipboard();

	afx_msg void OnQueryProcess();
	afx_msg void OnProcessmodule();
	afx_msg void OnProcessHandle();
	afx_msg void OnProcessThread();
	afx_msg void OnKillProcess();
	afx_msg void OnKillProcessDeleteFile();
	afx_msg void OnProcessVerify();
	afx_msg void OnCopyProcessDataToClipboard();

	afx_msg void OnGetServicesList();
	afx_msg void OnGetDepthServicesList();
	afx_msg void OnStartServices();
	afx_msg void OnStopServices();
	afx_msg void OnDeleteServices();
	afx_msg void OnManualServices();
	afx_msg void OnAutoServices();
	afx_msg void OnDisableServices();

	afx_msg void OnMsgTcpView();
	afx_msg void OnMsgTcpProKill();

	afx_msg void OnListLog();
	afx_msg void GetSaveFileLog(WCHAR *lpwzFilePath);
	afx_msg void OnClearListLog();
	afx_msg void OnConnectScan();
	afx_msg void OnSaveToFile();


	afx_msg void OnBnClickedQzdeletefile();
	afx_msg void OnBnClickedDisdriver();
	afx_msg void OnBnClickedDisprocess();
	afx_msg void OnBnClickedDisfile();
	afx_msg void OnBnClickedShutdownSystem();
	afx_msg void OnBnClickedServicesReset();
	afx_msg void OnBnClickedDisKernelThread();
	afx_msg void OnUninstall360();


	int m_case;
	CImageList ImgList;
	afx_msg void OnLvnItemchangedList2(NMHDR *pNMHDR, LRESULT *pResult);

	int m_iItem;//������¼�ڵڼ����ϵ����ģ�Ҳ�ɸ�Ϊ�ڼ��е�����
};

#ifndef _DEBUG  // 3600safeView.cpp �еĵ��԰汾
inline CMy3600safeDoc* CMy3600safeView::GetDocument() const
   { return reinterpret_cast<CMy3600safeDoc*>(m_pDocument); }
#endif

