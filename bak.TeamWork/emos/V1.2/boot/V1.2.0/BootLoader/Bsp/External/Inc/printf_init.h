#ifndef _PRINTF_INIT_H_	
#define _PRINTF_INIT_H_

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif

#include "stm32f4xx.h"
#include <stdio.h>		   	//printf
	
//#include "usart1_init.h"
// #include "usart2_init.h"
 #include "usart3_init.h"
// #include "uart4_init.h"
// #include "uart5_init.h"
// #include "usart6_init.h"

#define PRINTFPRIO		10																						//�����ô�ӡ�ȼ�

#define DebugString 	USART3_SendString															//�����ö��ַ������
#define DebugByte 		USART3_SendByte																//�����õ��ַ����
	
//�����ź�������ӡ��ʾ�ã����ź�����ӡ��ʾ����ο�CommandSendJob.c
#define DebugPf(x,y) 	if(x < PRINTFPRIO)	printf(y);								//�����ô�ӡ���
#define DebugPfEx(x,y,z) 	if(x < PRINTFPRIO)	printf(y,z);					//�����ô�ӡ���������

u8 Get_PrintfPort(void);   																					//��ȡ��ӡ����˿�			(1-6)
void Set_PrintfPort(const u8 Port);																	//����printf����˿�		(1-6)

#ifdef _COMMANDSENDJOB_H_				//���ź�������ӡ��ʾ�ã�������ο�CommandSendJob.c
u8 SendToPrintf(u8* ShowChar,const u32 Len);				//���ʹ�ӡ��������ӡ�߳�
#endif

#ifdef __cplusplus		   //�����CPP����C���� //��������
}
#endif

#endif

/*
������
	#include "printf_init.h"		//printf����

	//���ڳ�ʼ����

	Set_PrintfPort(1);								//����printf�˿�			
	printf("���\r\n");								//����printf
	printf("1sssss2\r\n");
*/
