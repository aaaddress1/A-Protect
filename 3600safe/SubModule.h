#pragma once
#include "afxcmn.h"
#include "ClrListCtrl.h"

// CProcessModule �Ի���


class CSubModule : public CDialogEx
{
	DECLARE_DYNAMIC(CSubModule)

public:
	CSubModule(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSubModule();

// �Ի�������
	enum { IDD = IDD_DLG_PROMODULE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CClrListCtrl m_SubList;//Process Module ListView
	afx_msg void OnBnClickedBtnClose();
	virtual BOOL OnInitDialog();
	int m_Subcase;//����listview����Ϣ��
	int m_SubItem;//ѡ�����к�
	afx_msg void OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnUnLoadDLLModule();
	afx_msg void OnUnLoadDLLModuleAndDelete();
	afx_msg void OnCopyDLLDataToClipboard();
	afx_msg void OnLookupDLLInServices();

	afx_msg void OnProcessThreadList();
	afx_msg void OnKillProcessThread();
	afx_msg void OnSuspendProcessThread();
	afx_msg void OnResumeProcessThread();
};
