// C3600Splash.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "3600safe.h"
#include "C3600Splash.h"


// C3600Splash

IMPLEMENT_DYNAMIC(C3600Splash, CWnd)

C3600Splash::C3600Splash()
{

}

C3600Splash::~C3600Splash()
{
}


// C3600Splash ��Ϣ�������

BEGIN_MESSAGE_MAP(C3600Splash, CWnd)  
    ON_WM_PAINT()  
    ON_WM_TIMER()  
END_MESSAGE_MAP() 

// CWzdSplash ��Ϣ�������  
void C3600Splash::Create(UINT nBitmapID)  
{  
    m_bitmap.LoadBitmap(nBitmapID);  
    BITMAP bitmap;  
    m_bitmap.GetBitmap(&bitmap);  
    //CreateEx(0,AfxRegisterWndClass(0),"",WS_POPUP|WS_VISIBLE|WS_BORDER,0,0,bitmap.bmWidth,bitmap.bmHeight,NULL,0);  
    CreateEx(0,  
            AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW)),  
            NULL, WS_POPUP | WS_VISIBLE, 0, 0, bitmap.bmWidth, bitmap.bmHeight, NULL , NULL);  
}  
void C3600Splash::OnPaint()  
{  
    // TODO: �ڴ˴������Ϣ����������  
    // ��Ϊ��ͼ��Ϣ���� CWnd::OnPaint()  
    CPaintDC dc(this); // device context forpainting  
    BITMAP bitmap;  
    m_bitmap.GetBitmap(&bitmap);  
    CDC dcComp;  
    dcComp.CreateCompatibleDC(&dc);  
    dcComp.SelectObject(&m_bitmap);  
    // draw bitmap  
    dc.BitBlt(0,0,bitmap.bmWidth,bitmap.bmHeight,&dcComp,0,0,SRCCOPY);  
}  
void C3600Splash::OnTimer(UINT_PTR nIDEvent)  
{  
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ  
    //CWnd::OnTimer(nIDEvent);  
    DestroyWindow(); //���ٳ�ʼ���洰��  
}  