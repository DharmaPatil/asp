#ifndef _USART6_INIT_H_
#define _USART6_INIT_H_

#ifdef __cplusplus		   		//�����CPP����C���� //��ʼ����
extern "C" {
#endif
	
#include "usart_cfg.h"			//���������ļ�

//��������u8 USART6_Config(USART_STRUCT* Real);					//�����ó�ʼ������
void USART6_Init(const u32 BaudRate_t);				//���ò�����
	
//���ܺ���
void USART6_RX_Buffer_Clear(void);						//��ջ�����

//״̬��ѯ	
u32 LookUSART6_GetBuffCount(void);						//��ȡ�������н��յ������ݸ���
u8 USART6CheckISRSendBusy(void);							//�жϷ���æ״̬���

//��������
u8 USART6_GetByte(u8* Data);									//����������ȡ���ַ�����
u32 USART6_GetString(u8* Data ,const u32 len);			//����������ȡ�ַ�������
u8 USART6_GetByte_WaitTime(u8* Data,const u32 TimeLater);		//ʱ������ȡ�ַ�����

//��������
void USART6_SendByte(const u8 Data);								//���ַ�����
void USART6_SendString(u8* Data,const u32 Len);			//���ַ����
void USART6_DMASendString(const u32 Len);						//DMA��ʽ���ַ����

#ifdef __cplusplus		   		//�����CPP����C���� //��������
}
#endif

#endif

/*
����ʹ�ã�
��1������ͷ�ļ���
#include "usart1_init.h"		//���ں���
#include "usart_cfg.h"			//��������
��2�������շ���������
//���ͻ�����
#define USART1_RX_BUFF_SIZEMAX 128
static u8 USART1RxBuffer[USART1_RX_BUFF_SIZEMAX];	//���ڽ��ջ�����
//���ͻ�����
#define USART1_TX_BUFF_SIZEMAX 128
static u8 USART1TxBuffer[USART1_TX_BUFF_SIZEMAX];	//���ڷ��ͻ�����
��3�����ô��ڡ�
//�������ñ���
USART_STRUCT USART1_Real = 
{
	57600,										//������

	//���ջ�����
	USART1RxBuffer,						//���ջ�����
	USART1_RX_BUFF_SIZEMAX,		//���ջ�������С

	//���ͻ�����
	USART1TxBuffer,						//���ͻ�����
	USART1_TX_BUFF_SIZEMAX,		//���ͻ�������С
};
��4����ʼ�����ڡ�
	if(USART1_Config(&USART1_Real) == 0)		//�����ó�ʼ������
		return 0;	
��5�������շ�Ӧ�á�
*/

/*
ֱ�Ӵӻ�������ȡ���ݣ�
	u8 flag = 0;						//��ȡ�Ƿ����		1����		0��ȡ
	u32 len = 0;						//���յ������ݸ���
	u8 CharVal = 0;					//��ȡ�ĵ��ַ�
	u8 StringVal[5] = {0};	//��ȡ���ַ���
	u32 ISRCnt = 0;					//��ISR���͵ȴ����м���
	u32 DMACnt = 0;					//��DMA���͵ȴ����м���

	if(USART1_Config(&USART1_Real) == 0)		//�����ó�ʼ������
		return 0;		

	while(!flag);											//��ȡ����	�޸ı�־��ȡ����
	flag = 0;

	len = USART1_GetByte(&CharVal);				//����������ȡ���ַ�����
	USART1_SendByte(CharVal);							//��ʾ�ַ�
	len = USART1_GetString(StringVal,3);	//����������ȡ�ַ�������
	USART1_SendString(StringVal,len);			//��ʾ�ַ���
	while(USART1CheckISRSendBusy()) ISRCnt++; 	//�ȴ�ISR�����жϽ����ݷ�����												
	USART1_DMASendString(len);						//DMA��ʾ�ַ���	
	while(USART1CheckDMASendBusy()) DMACnt++; 	//�ȴ�DMA�����жϽ����ݷ�����
*/
