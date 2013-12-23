#ifndef _USART_CFG_H_
#define _USART_CFG_H_

#ifdef __cplusplus		   		//�����CPP����C���� //��ʼ����
extern "C" {
#endif

#include "stm32f4xx.h"			//u32		

#define DebugXiang					//�Ƿ����������Ϣ		������� ���β����
	
//USART�豸���ƽṹ��
typedef struct
{
	u32			BaudRate;							//������
	
	//���ջ�����
	u8			*USARTRxBuffer;				//���ջ�����
	u32			USARTRxBufferSize;		//���ջ�������С

	//���ͻ�����
	u8			*USARTTxBuffer;				//���ͻ�����
	u32			USARTTxBufferSize;		//���ͻ�������С
} USART_STRUCT;

#ifdef __cplusplus		   		//�����CPP����C���� //��������
}
#endif

#endif
