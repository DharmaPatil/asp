#include "usart3_init.h"
#include "stm32f4xx.h"			//u8
#include "String.h"					//memcpy

//���տ��ƶ���
static	u8*			USART3QStart = (u8*)0;		      //��ʼλ��
static	u8*			USART3QEnd = (u8*)0;		        //����λ��
static  u8*			USART3QIn = (u8*)0;				    	//����λ��
static	u8*			USART3QOut = (u8*)0;				    //���λ��
static  u32			USART3QDataCount = 0;           //�ռ������ݸ��� 

//��������
static  USART_STRUCT*		USART3Real = (USART_STRUCT*)0;				//���ڽṹ��ָ��
static 	u8 			USART3_GetChar = 0;					//���յ��ĵ����ַ�				�ж���ʹ��	
static  u32			ISRSendLen = 0;							//�ж���Ҫ���͵����ݸ���
static  u32			ISRSendCnt = 0;							//�жϷ��͸���
static  u8			ISRSendBusyFlag = 0;				//�жϷ���æ״̬	1æ	0��æ

/****************************************************************************
* ��	�ƣ�void USART3_GPIO_Init(void)
* ��	�ܣ��������ų�ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void USART3_GPIO_Init(void)			//�������ų�ʼ��
{
	GPIO_InitTypeDef GPIO_InitStructure;		//�������Žṹ
	
	//�������ŷ���ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_USART3);		//Connect PXx to USARTx_Tx  
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART3);	//Connect PXx to USARTx_Rx

	//���ô��� Tx (PC.10) Ϊ�����������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				//���ڷ�������
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;			//����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//ת��Ƶ��
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//�������
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;	//����������
  GPIO_Init(GPIOC, &GPIO_InitStructure);						//��ʼ������
    
	// ���ô��� Rx (PC.11) Ϊ��������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				//���ڽ�������
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;			//����ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//ת��Ƶ��
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;	//����������
  GPIO_Init(GPIOC, &GPIO_InitStructure);						//��ʼ������
}

/****************************************************************************
* ��	�ƣ�void USART3_Init(const u32 BaudRate_t)
* ��	�ܣ����ڳ�ʼ��
* ��ڲ�����const u32 BaudRate_t		������
* ���ڲ�������
* ˵	������
****************************************************************************/
void USART3_Init(const u32 BaudRate_t)
{		
	USART_InitTypeDef UART_InitStructure;		//���ڽṹ

	//���ڷ���ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	//���ڳ�ʼ��
	UART_InitStructure.USART_BaudRate            = BaudRate_t;						//������
	UART_InitStructure.USART_WordLength          = USART_WordLength_8b;		//����λ8bit
	UART_InitStructure.USART_StopBits            = USART_StopBits_1;		//ֹͣλ����
	UART_InitStructure.USART_Parity              = USART_Parity_No ;		//��������żЧ��
	UART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//RTS��CTSʹ��(None��ʹ��)
	UART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;	//���ͺͽ���ʹ��
	USART_Init(USART3, &UART_InitStructure);	//��ʼ������
}

/****************************************************************************
* ��	�ƣ�void USART3_NVIC_Init(void)
* ��	�ܣ������ж��������ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void USART3_NVIC_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 		//�жϿ���������

	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;			//�����ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//�����ȼ�����
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//�������ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//�򿪴����ж�
	NVIC_Init(&NVIC_InitStructure);								//��ʼ���ж�������
}

/****************************************************************************
* ��	�ƣ�void USART3_RX_Buffer_init(void)
* ��	�ܣ����ڽ��ճ�ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵	���������ж�����ջ�������
****************************************************************************/
void USART3_RX_Buffer_init(void)
{
	USART3QStart = USART3Real->USARTRxBuffer;						//��ʼλ��
	USART3QEnd = &USART3Real->USARTRxBuffer[USART3Real->USARTRxBufferSize - 1];	//����λ��
	USART3QIn = USART3QStart;														//����λ��=��ʼλ��
	USART3QOut = USART3QStart;													//���λ��=��ʼλ��
	USART3QDataCount = 0;																//�ռ������ݸ���
}

/****************************************************************************
* ��	�ƣ�u8 USART3_Config(USART_STRUCT* Real)
* ��	�ܣ���������
* ��ڲ�����USART_STRUCT* Real		�ṹ������
* ���ڲ�����u8 �Ƿ�ɹ�	1�ɹ�	0ʧ��
* ˵	����Ĭ��Ϊ�����ݽ���					 
****************************************************************************/
u8 USART3_Config(USART_STRUCT* Real)
{
	if(Real == 0)
		return 0;
	
	USART3Real = Real;						//�ṹ���ȡ
	
	USART3_Init(USART3Real->BaudRate);	//���ڳ�ʼ��
	USART3_GPIO_Init();						//�������ų�ʼ��
	USART3_NVIC_Init();						//�жϳ�ʼ��
	USART3_RX_Buffer_init();			//�����ж�����ջ�������

	USART_ClearITPendingBit(USART3, USART_IT_RXNE);				//����ձ�־
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);				//���������ж�
	
	USART_Cmd(USART3, ENABLE);  	//ʹ��ʧ�ܴ�������
	
	return 1;
}

/****************************************************************************
* ��	�ƣ�void USART3_RX_Buffer_Clear(void)	
* ��	�ܣ���ս��ջ�����
* ��ڲ�������
* ���ڲ�������
* ˵	������						
****************************************************************************/
void USART3_RX_Buffer_Clear(void)
{
	USART3QIn = USART3QStart;							//����λ��=��ʼλ��
	USART3QOut = USART3QStart;						//���λ��=��ʼλ��
	USART3QDataCount = 0;									//�ռ������ݸ���
	memset(USART3Real->USARTRxBuffer,0,USART3Real->USARTRxBufferSize);
}

/****************************************************************************
* ��	�ƣ�u32 LookUSART3_GetBuffCount(void)
* ��	�ܣ���ȡ�������н��յ������ݸ���
* ��ڲ�������
* ���ڲ�������
* ˵	������					  
****************************************************************************/
u32 LookUSART3_GetBuffCount(void)
{
	return USART3QDataCount;
}

/****************************************************************************
* ��	�ƣ�u8 USART3_GetByte(u8* Data)	
* ��	�ܣ�����������ȡ���ַ�����
* ��ڲ�����u8* Data ���յ�������
* ���ڲ�����u8	���ճɹ���־
			0 	���ճɹ�
			1 	����ʧ��
* ˵	�����ӽ��ջ������л�ȡ���ݣ���������ȴ�
***************************************************************************/
u8 USART3_GetByte(u8* Data)
{
	if(USART3QDataCount != 0)				//��δ�������ݴ���
	{
		*Data = *USART3QOut;					//�Ӷ�����ȡ����
		USART3QOut++;									//ָ����һ��δ�������ݵ�
		if(USART3QOut > USART3QEnd)		//�������һ���жϵ������ؿ�ʼ��
			USART3QOut = USART3QStart;	
		USART3QDataCount--;						//δ�������ݸ����ݼ�

		return 1;
	}
	return 0;
}

/****************************************************************************
* ��	�ƣ�u8 USART3_GetByte_WaitTime(u8* Data,const u32 TimeLater)	
* ��	�ܣ�u8* Data ���յ�������
			const u32 TimeLater	�ȴ�ʱ�� 7=1us
* ��ڲ�����u8	�Ƿ��н��յ�����
				0 û�н��յ�����
				1 ���յ�����
* ���ڲ��������յ��ĵ��ַ�����
* ˵	�����ӽ��ջ������л�ȡ���ݣ�ʱ���ȴ�
***************************************************************************/
u8 USART3_GetByte_WaitTime(u8* Data,const u32 TimeLater)
{
	u8 flag = 0;					//��ȡ��־
	u32 err = 0;					//�����־
	u8 val = 0;					//��ȡ�����ݴ�ŵ�
	
	while(1)
	{
		if(LookUSART3_GetBuffCount() == 0)	//��������û������
		{	
			err++;
			if(err >= TimeLater)	//10���붼û��ȡ�����ݣ������˳�
				return 0;
		}
		else
		{
			flag = USART3_GetByte(&val);
			if (flag == 1)
			{
				*Data = val;
				return 1;
			}
			else
				return 0;	
		}
	}
}

/****************************************************************************
* ��	�ƣ�u32 USART3_GetString(u8* Data ,const u32 len)	
* ��	�ܣ�����������ȡ�ַ�������
* ��ڲ�����u8* Data ���յ�������
			const u32 len	 ��Ҫ���յĳ���
* ���ڲ�����u32	��ȡ�����ݸ���		0��ȡʧ��
* ˵	�����ӽ��ջ������л�ȡ���ݣ���������ȴ� 
  
***************************************************************************/
u32 USART3_GetString(u8* Data ,const u32 len)
{
	u32 i = 0;
	u32 cnt = len;
	
	if(cnt == 0)													//׼������0�ֽ�����
		return 0;														//����ʧ��

	if(USART3QDataCount == 0)					   	//���������û������
		return 0;									   				//����ʧ��

	if(cnt > USART3QDataCount)				   	//Ҫȡ�ĸ�����Ŀǰ���յĸ�������
		cnt = USART3QDataCount;				   		//ȡ�����յ�����������

	if((USART3QOut + cnt - 1) > (USART3QEnd + 1))		//�в��������ڻ�������ǰ��		-1��0��ʼ�洢��
	{
		i = USART3QEnd - USART3QOut + 1;							//��벿�ֵ����ݸ���
		memcpy(Data , USART3QOut , i);								//ȡ��N������
		memcpy(Data + i , USART3QStart , cnt - i);	 	//ȡǰN������
		USART3QOut = USART3QStart + cnt - i;	 				//����ȡ��λ��
	}
	else
	{
		memcpy(Data,USART3QOut,cnt);	 		//ȡN������
		USART3QOut += cnt;								//ָ����һ��δ�������ݵ�
	}

	if(USART3QOut > USART3QEnd)	 				//���λ�ó�������һ����ַ
		USART3QOut = USART3QOut - USART3QEnd + USART3QStart - 1;	   //-1��0��ʼ	//���������ַ
	USART3QDataCount -= cnt;			 			//���������˵����ݸ���
	
	return cnt;								 //���ػ�ȡ�ĸ���
}

/****************************************************************************
* ��	�ƣ�void USART3_SendByte(const u8 Data)
* ��	�ܣ����ַ�����
* ��ڲ�����const u8 Data 	���͵��ַ�����
* ���ڲ�������
* ˵	������				   
****************************************************************************/
void USART3_SendByte(const u8 Data)		   //���ַ��������
{
	USART_SendData(USART3, Data);
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
}

/****************************************************************************
* ��	�ƣ�void USART3_SendString(u8* Data,const u32 Len)
* ��	�ܣ����ַ����
* ��ڲ�����u8* Data 	���͵�����
						const u32 Len				�ַ�����
* ���ڲ�������
* ˵	������					 
****************************************************************************/
void USART3_SendString(u8* Data,const u32 Len)		//���ַ����
{
	if(Len == 0)										//����Ϊ0
		return;
	
	if(ISRSendBusyFlag == 1)				//����æ״̬
		return;
	
	if(Len > USART3Real->USARTTxBufferSize)			//�����������Ƿ��㹻
		return;
	
	memcpy(USART3Real->USARTTxBuffer,Data,Len);	//�������ݵ�������
	
	ISRSendLen = 0;									//�����Ҫ���͵����ݸ���
	ISRSendLen = Len;								//��¼��Ҫ���͵����ݸ���
	ISRSendBusyFlag = 1;						//����æ״̬
	
	USART_SendData(USART3, USART3Real->USARTTxBuffer[0]);	//���͵�һ���ֽ�			
	ISRSendCnt = 1;									//���ͼ���
	USART_ITConfig(USART3, USART_IT_TXE, ENABLE);		//���������ж�	
}

/****************************************************************************
* ��	�ƣ�u8 USART3CheckISRSendBusy(void)	
* ��	�ܣ��жϷ���æ״̬���
* ��ڲ�������
* ���ڲ�����u8		1æ	0��æ
* ˵	������һ�εķ����Ƿ���ɣ�æ��ȴ�
****************************************************************************/
u8 USART3CheckISRSendBusy(void)	
{
		return ISRSendBusyFlag;
}

/****************************************************************************
* ��	�ƣ�void USART3_IRQHandler(void)	
* ��	�ܣ��жϻ���
* ��ڲ�������
* ���ڲ�������
* ˵	�������յ������ݴ�����ջ�����
	USART_GetITStatus		���ָ����USART�жϷ������
	USART_GetFlagStatus		���ָ����USART��־λ�������
****************************************************************************/
void USART3_IRQHandler(void)
{	
	//�����жϴ���
	if(USART_GetITStatus(USART3, USART_IT_RXNE) == SET)		
	{
		//���ݽ��ն���
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);				//��ս����жϱ�־

		USART3_GetChar = USART_ReceiveData(USART3);								//���յ����ַ�����
		
		if(USART3QDataCount < USART3Real->USARTRxBufferSize)			//����ռ�δ��,�������� 
		{
			if(USART3QIn > USART3QEnd)			//����пռ�ɴ����ݣ�������λ����ĩβλ��
				USART3QIn = USART3QStart;			//������λ��������ʼλ��
			*USART3QIn = USART3_GetChar;		//���յ����ַ����ݴ��뻺��������λ��
			USART3QIn++;										//����λ������
			USART3QDataCount++;							//�ռ������ݸ�������	
		}
	}
	
	else if(USART_GetFlagStatus(USART3, USART_IT_ORE) == SET)		//����Ƿ��н������
		USART_ReceiveData(USART3);									//����������־��ֻ���ö����ݵķ�ʽ���������־

	//�����жϴ���
	if(USART_GetITStatus(USART3, USART_IT_TXE) == SET)		//�����ж�
	{		
		if(ISRSendCnt >= ISRSendLen)												//���ݷ�����
		{
			USART_ITConfig(USART3, USART_IT_TXE, DISABLE);		//��ֹ������λ���������ж�
			ISRSendBusyFlag = 0;					//���Ϳ���
		}
		else																								//����δ������
		{
			USART_SendData(USART3, USART3Real->USARTTxBuffer[ISRSendCnt]);
			ISRSendCnt++;
		}		
	}
}
