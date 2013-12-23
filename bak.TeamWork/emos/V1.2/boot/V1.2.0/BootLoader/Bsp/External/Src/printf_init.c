#include "printf_init.h"

//Printf�����
static USART_TypeDef* USARTNow = (USART_TypeDef*)0;	//��ǰ���������ָ��

/****************************************************************************
* ��	�ƣ�u8 Get_PrintfPort(void)
* ��	�ܣ���ȡ��ӡ����˿�
* ��ڲ�������
* ���ڲ�����u8 ��ǰ��ӡ����˿ں�	0��ȡʧ��
* ˵	������
****************************************************************************/
u8 Get_PrintfPort(void)
{
	if(USARTNow == ((USART_TypeDef*) USART1_BASE))
		return 1;
	else if(USARTNow == ((USART_TypeDef*)USART2_BASE))
		return 2;
	else if(USARTNow == ((USART_TypeDef*)USART3_BASE))
		return 3;
	else if(USARTNow == ((USART_TypeDef*)UART4_BASE))
		return 4;
	else if(USARTNow == ((USART_TypeDef*)UART5_BASE))
		return 5;
	else if(USARTNow == ((USART_TypeDef*)USART6_BASE))
		return 6;
	else
		return 0;
}

/****************************************************************************
* ��	�ƣ�void Set_PrintfPort(const u8 Port)
* ��	�ܣ����ô�ӡ����˿�
* ��ڲ�����const u8	��ӡ��ʾ�˿ں� ����1 = 1	����2 = 2 .....
* ���ڲ�������
* ˵	������
****************************************************************************/
void Set_PrintfPort(const u8 Port)
{
	switch(Port)
	{
		case 1:
		{
			USARTNow = ((USART_TypeDef*)USART1_BASE);
			break;
		}
		case 2:
		{
			USARTNow = ((USART_TypeDef*)USART2_BASE);
			break;
		}
		case 3:
		{
			USARTNow = ((USART_TypeDef*)USART3_BASE);
			break;
		}
		case 4:
		{
			USARTNow = ((USART_TypeDef*)UART4_BASE);
			break;
		}
		case 5:
		{
			USARTNow = ((USART_TypeDef*)UART5_BASE);
			break;
		}
		case 6:
		{
			USARTNow = ((USART_TypeDef*)USART6_BASE);
			break;
		}
		default:
		{
			USARTNow = ((USART_TypeDef*)USART1_BASE);
			break;
		}
	}
}

#pragma import(__use_no_semihosting)                          
struct __FILE 
{ 
	int handle; 
};
 
FILE __stdout;
         
_sys_exit(const int x) 
{ 
	int i = x;
	i = i; 
}

/****************************************************************************
* ��	�ƣ�int fputc(const int ch, FILE *f)
* ��	�ܣ��ض���fputc������printf�������ն˴�ӡ���
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
int fputc(const int ch, FILE *f)
{   
	while (USART_GetFlagStatus(USARTNow,USART_FLAG_TXE) == RESET);	//�ȴ��������,��һ���ַ�������TC���������ж� 
	USARTNow->DR = (u8)ch;      
	while((USARTNow->SR & USART_FLAG_TC) == 0);
	return ch;
}

/****************************************************************************
* ��	�ƣ�int fgetc(FILE *f)
* ��	�ܣ��ض���fgetc������printf�������ն���������
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
int fgetc(FILE *f)
{
	while (USART_GetFlagStatus(USARTNow, USART_FLAG_RXNE) == RESET);
	return (int)USART_ReceiveData(USARTNow);
}

#ifdef _COMMANDSENDJOB_H_
/****************************************************************************
* ��	�ƣ�u8 SendToPrintf(u8* ShowChar,const u32 Len)
* ��	�ܣ�����Ҫ��ӡ���������ͨ���ź�����������ӡ����߳�
* ��ڲ�����u8* ShowChar	��Ҫ��ӡ���ַ�
						const u32 Len			�ַ�����
* ���ڲ�����u8	�Ƿ�ɹ�	0ʧ��	1�ɹ�
* ˵	������
****************************************************************************/
u8 SendToPrintf(u8* ShowChar,const u32 Len)
{
	OS_ERR err;					//��ȡ�������

	if(Len <= 0)				//���ͳ��ȱ��� 
		return 0;

	OSQPost((OS_Q*)GetCSQEvent(),
		(void*)ShowChar,
		(OS_MSG_SIZE)Len,
		OS_OPT_POST_FIFO,
		&err);
	if(err != OS_ERR_NONE) 
		return 0;
		
	return 1;	
}
#endif