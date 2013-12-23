#include "usart1_init.h"
#include "stm32f4xx.h"			//u8
#include "String.h"					//memcpy

//���տ��ƶ���
static	u8*			USART1QStart = (u8*)0;		      //��ʼλ��
static	u8*			USART1QEnd = (u8*)0;		        //����λ��
static  u8*			USART1QIn = (u8*)0;				    	//����λ��
static	u8*			USART1QOut = (u8*)0;				    //���λ��
static  u32			USART1QDataCount = 0;           //�ռ������ݸ��� 

//��������
static  USART_STRUCT*		USART1Real = (USART_STRUCT*)0;				//���ڽṹ��ָ��
static 	u8 			USART1_GetChar = 0;					//���յ��ĵ����ַ�				�ж���ʹ��	
static  u32			ISRSendLen = 0;							//�ж���Ҫ���͵����ݸ���
static  u32			ISRSendCnt = 0;							//�жϷ��͸���
static  u8			ISRSendBusyFlag = 0;				//�жϷ���æ״̬	1æ	0��æ

/****************************************************************************
* ��	�ƣ�void USART1_GPIO_Init(void)
* ��	�ܣ��������ų�ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void USART1_GPIO_Init(void)			//�������ų�ʼ��
{
	GPIO_InitTypeDef GPIO_InitStructure;		//�������Žṹ
	
	//�������ŷ���ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);		//Connect PXx to USARTx_Tx  
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);	//Connect PXx to USARTx_Rx

	//���ô��� Tx (PA.9) Ϊ�����������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;					//���ڷ�������
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;			//����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//ת��Ƶ��
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//�������
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;	//����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);						//��ʼ������
    
	// ���ô��� Rx (PA.10) Ϊ��������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				//���ڽ�������
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;			//����ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//ת��Ƶ��
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;	//����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);						//��ʼ������
}

/****************************************************************************
* ��	�ƣ�void USART1_Init(const u32 BaudRate_t)
* ��	�ܣ����ڳ�ʼ��
* ��ڲ�����const u32 BaudRate_t		������
* ���ڲ�������
* ˵	������
****************************************************************************/
void USART1_Init(const u32 BaudRate_t)
{		
	USART_InitTypeDef UART_InitStructure;		//���ڽṹ

	//���ڷ���ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	//���ڳ�ʼ��
	UART_InitStructure.USART_BaudRate            = BaudRate_t;						//������
	UART_InitStructure.USART_WordLength          = USART_WordLength_8b;		//����λ8bit
	UART_InitStructure.USART_StopBits            = USART_StopBits_1;			//ֹͣλ����
	UART_InitStructure.USART_Parity              = USART_Parity_No ;			//��������żЧ��
	UART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//RTS��CTSʹ��(None��ʹ��)
	UART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;		//���ͺͽ���ʹ��
	USART_Init(USART1, &UART_InitStructure);																				//��ʼ������
}

/****************************************************************************
* ��	�ƣ�void USART1_NVIC_Init(void)
* ��	�ܣ������ж��������ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void USART1_NVIC_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 		//�жϿ���������

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;			//�����ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//�����ȼ�����
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//�������ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//�򿪴����ж�
	NVIC_Init(&NVIC_InitStructure);								//��ʼ���ж�������
}

/****************************************************************************
* ��	�ƣ�void USART1_RX_Buffer_init(void)
* ��	�ܣ����ڽ��ճ�ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵	���������ж�����ջ�������
****************************************************************************/
void USART1_RX_Buffer_init(void)
{
	USART1QStart = USART1Real->USARTRxBuffer;						//��ʼλ��
	USART1QEnd = &USART1Real->USARTRxBuffer[USART1Real->USARTRxBufferSize - 1];	//����λ��
	USART1QIn = USART1QStart;														//����λ��=��ʼλ��
	USART1QOut = USART1QStart;													//���λ��=��ʼλ��
	USART1QDataCount = 0;																//�ռ������ݸ���
}

/****************************************************************************
* ��	�ƣ�u8 USART1_Config(USART_STRUCT* Real)
* ��	�ܣ���������
* ��ڲ�����USART_STRUCT* Real		�ṹ������
* ���ڲ�����u8 �Ƿ�ɹ�	1�ɹ�	0ʧ��
* ˵	����Ĭ��Ϊ�����ݽ���					 
****************************************************************************/
u8 USART1_Config(USART_STRUCT* Real)
{
	if(Real == 0)
		return 0;
	
	USART1Real = Real;						//�ṹ���ȡ
	
	USART1_Init(USART1Real->BaudRate);	//���ڳ�ʼ��
	USART1_GPIO_Init();						//�������ų�ʼ��
	USART1_NVIC_Init();						//�жϳ�ʼ��
	USART1_RX_Buffer_init();			//�����ж�����ջ�������

	USART_ClearITPendingBit(USART1, USART_IT_RXNE);				//����ձ�־
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);				//���������ж�
	
	USART_Cmd(USART1, ENABLE);  	//ʹ��ʧ�ܴ�������
	
	return 1;
}

/****************************************************************************
* ��	�ƣ�void USART1_RX_Buffer_Clear(void)	
* ��	�ܣ���ս��ջ�����
* ��ڲ�������
* ���ڲ�������
* ˵	������						
****************************************************************************/
void USART1_RX_Buffer_Clear(void)
{
	USART1QIn = USART1QStart;							//����λ��=��ʼλ��
	USART1QOut = USART1QStart;						//���λ��=��ʼλ��
	USART1QDataCount = 0;									//�ռ������ݸ���
	memset(USART1Real->USARTRxBuffer,0,USART1Real->USARTRxBufferSize);
}

/****************************************************************************
* ��	�ƣ�u32 LookUSART1_GetBuffCount(void)
* ��	�ܣ���ȡ�������н��յ������ݸ���
* ��ڲ�������
* ���ڲ�������
* ˵	������					  
****************************************************************************/
u32 LookUSART1_GetBuffCount(void)
{
	return USART1QDataCount;
}

/****************************************************************************
* ��	�ƣ�u8 USART1_GetByte(u8* Data)	
* ��	�ܣ�����������ȡ���ַ�����
* ��ڲ�����u8* Data ���յ�������
* ���ڲ�����u8	���ճɹ���־
			0 	���ճɹ�
			1 	����ʧ��
* ˵	�����ӽ��ջ������л�ȡ���ݣ���������ȴ�
***************************************************************************/
u8 USART1_GetByte(u8* Data)
{
	if(USART1QDataCount != 0)				//��δ�������ݴ���
	{
		*Data = *USART1QOut;					//�Ӷ�����ȡ����
		USART1QOut++;									//ָ����һ��δ�������ݵ�
		if(USART1QOut > USART1QEnd)		//�������һ���жϵ������ؿ�ʼ��
			USART1QOut = USART1QStart;	
		USART1QDataCount--;						//δ�������ݸ����ݼ�

		return 1;
	}
	return 0;
}

/****************************************************************************
* ��	�ƣ�u8 USART1_GetByte_WaitTime(u8* Data,const u32 TimeLater)	
* ��	�ܣ�u8* Data ���յ�������
			const u32 TimeLater	�ȴ�ʱ�� 7=1us
* ��ڲ�����u8	�Ƿ��н��յ�����
				0 û�н��յ�����
				1 ���յ�����
* ���ڲ��������յ��ĵ��ַ�����
* ˵	�����ӽ��ջ������л�ȡ���ݣ�ʱ���ȴ�
***************************************************************************/
u8 USART1_GetByte_WaitTime(u8* Data,const u32 TimeLater)
{
	u8 flag = 0;					//��ȡ��־
	u32 err = 0;					//�����־
	u8 val = 0;					//��ȡ�����ݴ�ŵ�
	
	while(1)
	{
		if(LookUSART1_GetBuffCount() == 0)	//��������û������
		{	
			err++;
			if(err >= TimeLater)	//10���붼û��ȡ�����ݣ������˳�
				return 0;
		}
		else
		{
			flag = USART1_GetByte(&val);
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
* ��	�ƣ�u32 USART1_GetString(u8* Data ,const u32 len)	
* ��	�ܣ�����������ȡ�ַ�������
* ��ڲ�����u8* Data ���յ�������
			const u32 len	 ��Ҫ���յĳ���
* ���ڲ�����u32	��ȡ�����ݸ���		0��ȡʧ��
* ˵	�����ӽ��ջ������л�ȡ���ݣ���������ȴ� 
  
***************************************************************************/
u32 USART1_GetString(u8* Data ,const u32 len)
{
	u32 i = 0;
	u32 cnt = len;
	
	if(cnt == 0)													//׼������0�ֽ�����
		return 0;														//����ʧ��

	if(USART1QDataCount == 0)					   	//���������û������
		return 0;									   				//����ʧ��

	if(cnt > USART1QDataCount)				   	//Ҫȡ�ĸ�����Ŀǰ���յĸ�������
		cnt = USART1QDataCount;				   		//ȡ�����յ�����������

	if((USART1QOut + cnt - 1) > (USART1QEnd + 1))		//�в��������ڻ�������ǰ��		-1��0��ʼ�洢��
	{
		i = USART1QEnd - USART1QOut + 1;							//��벿�ֵ����ݸ���
		memcpy(Data , USART1QOut , i);								//ȡ��N������
		memcpy(Data + i , USART1QStart , cnt - i);	 	//ȡǰN������
		USART1QOut = USART1QStart + cnt - i;	 				//����ȡ��λ��
	}
	else
	{
		memcpy(Data,USART1QOut,cnt);	 		//ȡN������
		USART1QOut += cnt;								//ָ����һ��δ�������ݵ�
	}

	if(USART1QOut > USART1QEnd)	 				//���λ�ó�������һ����ַ
		USART1QOut = USART1QOut - USART1QEnd + USART1QStart - 1;	   //-1��0��ʼ	//���������ַ
	USART1QDataCount -= cnt;			 			//���������˵����ݸ���
	
	return cnt;								 //���ػ�ȡ�ĸ���
}

/****************************************************************************
* ��	�ƣ�void USART1_SendByte(const u8 Data)
* ��	�ܣ����ַ�����
* ��ڲ�����const u8 Data 	���͵��ַ�����
* ���ڲ�������
* ˵	������				   
****************************************************************************/
void USART1_SendByte(const u8 Data)		   //���ַ��������
{
	USART_SendData(USART1, Data);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

/****************************************************************************
* ��	�ƣ�void USART1_SendString(u8* Data,const u32 Len)
* ��	�ܣ����ַ����
* ��ڲ�����u8* Data 	���͵�����
						const u32 Len				�ַ�����
* ���ڲ�������
* ˵	������					 
****************************************************************************/
void USART1_SendString(u8* Data,const u32 Len)		//���ַ����
{
	if(Len == 0)										//����Ϊ0
		return;
	
	if(ISRSendBusyFlag == 1)				//����æ״̬
		return;
	
	if(Len > USART1Real->USARTTxBufferSize)			//�����������Ƿ��㹻
		return;
	
	memcpy(USART1Real->USARTTxBuffer,Data,Len);	//�������ݵ�������
	
	ISRSendLen = 0;									//�����Ҫ���͵����ݸ���
	ISRSendLen = Len;								//��¼��Ҫ���͵����ݸ���
	ISRSendBusyFlag = 1;						//����æ״̬
	
	USART_SendData(USART1, USART1Real->USARTTxBuffer[0]);	//���͵�һ���ֽ�			
	ISRSendCnt = 1;									//���ͼ���
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);		//���������ж�	
}

/****************************************************************************
* ��	�ƣ�u8 USART1CheckISRSendBusy(void)	
* ��	�ܣ��жϷ���æ״̬���
* ��ڲ�������
* ���ڲ�����u8		1æ	0��æ
* ˵	������һ�εķ����Ƿ���ɣ�æ��ȴ�
****************************************************************************/
u8 USART1CheckISRSendBusy(void)	
{
		return ISRSendBusyFlag;
}

/****************************************************************************
* ��	�ƣ�void USART1_IRQHandler(void)	
* ��	�ܣ��жϻ���
* ��ڲ�������
* ���ڲ�������
* ˵	�������յ������ݴ�����ջ�����
	USART_GetITStatus		���ָ����USART�жϷ������
	USART_GetFlagStatus		���ָ����USART��־λ�������
****************************************************************************/
void USART1_IRQHandler(void)
{	
	//�����жϴ���
	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)		
	{
		//���ݽ��ն���
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);				//��ս����жϱ�־

		USART1_GetChar = USART_ReceiveData(USART1);								//���յ����ַ�����
		
		if(USART1QDataCount < USART1Real->USARTRxBufferSize)			//����ռ�δ��,�������� 
		{
			if(USART1QIn > USART1QEnd)			//����пռ�ɴ����ݣ�������λ����ĩβλ��
				USART1QIn = USART1QStart;			//������λ��������ʼλ��
			*USART1QIn = USART1_GetChar;		//���յ����ַ����ݴ��뻺��������λ��
			USART1QIn++;										//����λ������
			USART1QDataCount++;							//�ռ������ݸ�������	
		}
	}
	
	else if(USART_GetFlagStatus(USART1, USART_IT_ORE) == SET)		//����Ƿ��н������
		USART_ReceiveData(USART1);									//����������־��ֻ���ö����ݵķ�ʽ���������־

	//�����жϴ���
	if(USART_GetITStatus(USART1, USART_IT_TXE) == SET)		//�����ж�
	{		
		if(ISRSendCnt >= ISRSendLen)												//���ݷ�����
		{
			USART_ITConfig(USART1, USART_IT_TXE, DISABLE);		//��ֹ������λ���������ж�
			ISRSendBusyFlag = 0;					//���Ϳ���
		}
		else																								//����δ������
		{
			USART_SendData(USART1, USART1Real->USARTTxBuffer[ISRSendCnt]);
			ISRSendCnt++;
		}		
	}
}
