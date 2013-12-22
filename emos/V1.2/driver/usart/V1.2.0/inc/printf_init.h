#ifndef _PRINTF_INIT_H_	
#define _PRINTF_INIT_H_

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif

#include "stm32f4xx.h"
#include <stdio.h>		   	//printf FILE
#include "includes.h"			
#include "driver.h"				//HANDLE
	
#ifdef _USART1_INIT_H_
#include "usart1_init.h"
#endif

#ifdef _USART2_INIT_H_
#include "usart2_init.h"
#endif

#ifdef _USART3_INIT_H_
#include "usart3_init.h"
#endif

#ifdef _UART4_INIT_H_
#include "uart4_init.h"
#endif

#ifdef _UART5_INIT_H_
#include "uart5_init.h"
#endif

#ifdef _USART6_INIT_H_
#include "usart6_init.h"
#endif

//�����ź�������ӡ��ʾ�ã����ź�����ӡ��ʾ����ο�CommandSendJob.c
#define DebugPf 			printf								//�����ô�ӡ���
#define DebugString 	USART1_SendString			//�����ö��ַ������
#define DebugByte 		USART1_SendByte				//�����õ��ַ����

typedef struct
{
	u8* pData;						//��ӡ����
	u32 pDataLen;					//��ӡ���ݳ���
}PrintfData;						//��ӡ���ݽṹ��

/****************************************************************************
* ��	�ƣ�u8 Get_PrintfPort(void)
* ��	�ܣ���ȡ��ӡ����˿�
* ��ڲ�������
* ���ڲ�����u8 ��ǰ��ӡ����˿ں�	0��ȡʧ��
* ˵	������
****************************************************************************/
u8 Get_PrintfPort(void);   									//��ȡ��ӡ����˿�			(1-6)

/****************************************************************************
* ��	�ƣ�void Set_PrintfPort(u8 Port)
* ��	�ܣ����ô�ӡ����˿�
* ��ڲ�����u8	��ӡ��ʾ�˿ں� ����1 = 1	����2 = 2 .....
* ���ڲ�������
* ˵	������
****************************************************************************/
void Set_PrintfPort(u8 Port);								//����printf����˿�		(1-6)

/****************************************************************************
* ��	�ƣ�void SetPrintfEvent(SYS_EVENT* JOBEvent)
* ��	�ܣ����ô�ӡ�ź���
* ��ڲ�����SYS_EVENT*	��ӡ�ź�����ַ
* ���ڲ�������
* ˵	������
****************************************************************************/
void SetPrintfEvent(SYS_EVENT* JOBEvent);		//���ô�ӡ�ź���	

/****************************************************************************
* ��	�ƣ�u8 WaitPrintfEventAndPrintf(HANDLE USARTx,u32 Times)
* ��	�ܣ��ȴ���ӡ�ź�������ӡ����
* ��ڲ�����HANDLE USARTx				�豸���
						u32 Times						�ȴ�ʱ��10MS
* ���ڲ�����u8	�Ƿ�ɹ�	0ʧ��	1�ɹ�
* ˵	����
typedef struct
{
	u8* pData;						//��ӡ����
	u32 pDataLen;					//��ӡ���ݳ���
}PrintfData;						//��ӡ���ݽṹ��
****************************************************************************/
u8 WaitPrintfEventAndPrintf(HANDLE USARTx,u32 Times);	//�ȴ���ӡ�źŲ���ӡ����

#ifdef __cplusplus		   //�����CPP����C���� //��������
}
#endif

#endif

/*
������
#include "usart1_init.h"
#include "usart_cfg.h"
#include "printf_init.h"

USART_STRUCT usart1cfg = 
{
	57600,																	//������
	USART_WordLength_8b,										//����λ
	USART_StopBits_1,												//ֹͣλ
	USART_Parity_No													//Ч��λ
};

USART1_Config(&usart1cfg);												//���ڳ�ʼ��

Set_PrintfPort(1);						//���ô�ӡ����
printf("���Կ�ʼ\r\n");
*/

/*
JOB1
SYS_EVENT* DefaultJob1Event = (SYS_EVENT*)0;					//��������
if(DefaultJob1Event == (SYS_EVENT*)0)									
		DefaultJob1Event = SysMboxCreate((void *)0);			//���������ź���
SetPrintfEvent(DefaultJob1Event);											//���ô�ӡ�ź���	
if(WaitPrintfEventAndPrintf(USART1_Dev,0) == 0)				//�ȴ���ӡ�źŲ���ӡ����
		while(1);																					//�ȴ���ӡ����
		
JOB2
PrintfData senddata = 																//��ӡ��Ϣ
	{
		"test\r\n",				//���͵�����
		6									//�������ݵĸ���
	};
if(DefaultJob1Event != (SYS_EVENT*)0)
	SysMboxPost(DefaultJob1Event, (void*)&senddata);		//���ͼ���ȴ�������
*/
