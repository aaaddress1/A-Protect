#include "stdafx.h"

#define MAX_DPCTIMER_COUNT 250

typedef struct _MyDpcTimerInfo{
	ULONG  TimerAddress;    //KTIMER�ṹ��ַ
	ULONG  Period;        //ѭ�����
	ULONG  DpcAddress;      //DPC�ṹ��ַ
	ULONG  DpcRoutineAddress;  //���̵�ַ
	char lpszModule[260];
}MyDpcTimerInfo,*PMyDpcTimerInfo;

typedef struct _MyDpcTimer{
	ULONG  ulCount;
	MyDpcTimerInfo MyTimer[1];
}MyDpcTimer,*PMyDpcTimer;

PMyDpcTimer DpcTimer;

//CString DpcTimerNum;

CHAR* setClipboardText(CHAR* str);