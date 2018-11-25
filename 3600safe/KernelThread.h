#include "stdafx.h"

typedef struct _KERNEL_INFO{
	ULONG ulHideType;           //0 ����ģ�飬��driver_object����   1 ����ģ��
	ULONG ulStatus;             //�߳�״̬��0���У�1�˳�
	ULONG KernelThread;         //ETHREAD
	ULONG ThreadStart;          //��ʼ
	CHAR lpszThreadModule[256];

}KERNEL_INFO,*PKERNEL_INFO;

typedef struct _KERNEL_THREAD_INFO {          //InlineHook
	ULONG ulCount;
	KERNEL_INFO KernelThreadInfo[1];
} KERNEL_THREAD_INFO, *PKERNEL_THREAD_INFO;

PKERNEL_THREAD_INFO KernelThread;
