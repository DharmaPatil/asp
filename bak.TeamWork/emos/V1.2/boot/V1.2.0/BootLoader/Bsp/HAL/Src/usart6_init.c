#include "usart6_init.h"
#include "stm32f4xx.h"			//u8
#include "String.h"					//memcpy

//���տ��ƶ���
static	u8*			USART6QStart = (u8*)0;		      //��ʼλ��
static	u8*			USART6QEnd = (u8*)0;		        //����λ��
static  u8*			USART6QIn = (u8*)0;				    	//����λ��
static	u8*			USART6QOut = (u8*)0;				    //���λ��
static  u32			USART6QDataCount = 0;           //�ռ������ݸ��� 

//��������
static  USART_STRUCT*		USART6Real = (USART_STRUCT*)0;				//���ڽṹ��ָ��
static 	u8 			USART6_GetChar = 0;					//���յ��ĵ����ַ�				�ж���ʹ��	
static  u32			ISRSendLen = 0;							//�ж���Ҫ���͵����ݸ���
static  u32			ISRSendCnt = 0;							//�жϷ��͸���
static  u8			ISRSendBusyFlag = 0;				//�жϷ���æ״̬	1æ	0��æ

/****************************************************************************
* ��	�ƣ�void USART6_GPIO_Init(void)
* ��	�ܣ��������ų�ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void USART6_GPIO_Init(void)			//�������ų�ʼ��
{
	GPIO_InitTypeDef GPIO_InitStructure;		//�������Žṹ
	
	//�������ŷ���ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);		//Connect PXx to USARTx_Tx  
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);		//Connect PXx to USARTx_Rx

	//���ô��� Tx (PC.6) Ϊ�����������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;					//���ڷ�������
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;			//����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//ת��Ƶ��
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//�������
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;	//����������
  GPIO_Init(GPIOC, &GPIO_InitStructure);						//��ʼ������
    
	// ���ô��� Rx (PC.7) Ϊ��������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;					//���ڽ�������
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;			//����ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//ת��Ƶ��
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;	//����������
  GPIO_Init(GPIOC, &GPIO_InitStructure);						//��ʼ������
}

/****************************************************************************
* ��	�ƣ�void USART6_Init(const u32 BaudRate_t)
* ��	�ܣ����ڳ�ʼ��
* ��ڲ�����const u32 BaudRate_t		������
* ���ڲ�������
* ˵	������
****************************************************************************/
void USART6_Init(const u32 BaudRate_t)
{		
	USART_InitTypeDef UART_InitStructure;		//���ڽṹ

	//���ڷ���ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);

	//���ڳ�ʼ��
	UART_InitStructure.USART_BaudRate            = BaudRate_t;						//������
	UART_InitStructure.USART_WordLength          = USART_WordLength_8b;		//����λ8bit
	UART_InitStructure.USART_StopBits            = USART_StopBits_1;			//ֹͣλ����
	UART_InitStructure.USART_Parity              = USART_Parity_No ;			//��������żЧ��
	UART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//RTS��CTSʹ��(None��ʹ��)
	UART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;		//���ͺͽ���ʹ��
	USART_Init(USART6, &UART_InitStructure);																				//��ʼ������
}

/****************************************************************************
* ��	�ƣ�void USART6_NVIC_Init(void)
* ��	�ܣ������ж��������ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void USART6_NVIC_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 		//�жϿ���������

	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;			//�����ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//�����ȼ�����
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//�������ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//�򿪴����ж�
	NVIC_Init(&NVIC_InitStructure);								//��ʼ���ж�������
}

/****************************************************************************
* ��	�ƣ�void USART6_RX_Buffer_init(void)
* ��	�ܣ����ڽ��ճ�ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵	���������ж�����ջ�������
****************************************************************************/
void USART6_RX_Buffer_init(void)
{
	USART6QStart = USART6Real->USARTRxBuffer;						//��ʼλ��
	USART6QEnd = &USART6Real->USARTRxBuffer[USART6Real->USARTRxBufferSize - 1];	//����λ��
	USART6QIn = USART6QStart;														//����λ��=��ʼλ��
	USART6QOut = USART6QStart;													//���λ��=��ʼλ��
	USART6QDataCount = 0;																//�ռ������ݸ���
}

/****************************************************************************
* ��	�ƣ�u8 USART6_Config(USART_STRUCT* Real)
* ��	�ܣ���������
* ��ڲ�����USART_STRUCT* Real		�ṹ������
* ���ڲ�����u8 �Ƿ�ɹ�	1�ɹ�	0ʧ��
* ˵	����Ĭ��Ϊ�����ݽ���					 
****************************************************************************/
u8 USART6_Config(USART_STRUCT* Real)
{
	if(Real == 0)
		return 0;
	
	USART6Real = Real;						//�ṹ���ȡ
	
	USART6_Init(USART6Real->BaudRate);	//���ڳ�ʼ��
	USART6_GPIO_Init();						//�������ų�ʼ��
	USART6_NVIC_Init();						//�жϳ�ʼ��
	USART6_RX_Buffer_init();			//�����ж�����ջ�������

	USART_ClearITPendingBit(USART6, USART_IT_RXNE);				//����ձ�־
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);				//���������ж�
	
	USART_Cmd(USART6, ENABLE);  	//ʹ��ʧ�ܴ�������
	
	return 1;
}

/****************************************************************************
* ��	�ƣ�void USART6_RX_Buffer_Clear(void)	
* ��	�ܣ���ս��ջ�����
* ��ڲ�������
* ���ڲ�������
* ˵	������						
****************************************************************************/
void USART6_RX_Buffer_Clear(void)
{
	USART6QIn = USART6QStart;							//����λ��=��ʼλ��
	USART6QOut = USART6QStart;						//���λ��=��ʼλ��
	USART6QDataCount = 0;									//�ռ������ݸ���
	memset(USART6Real->USARTRxBuffer,0,USART6Real->USARTRxBufferSize);
}

/****************************************************************************
* ��	�ƣ�u32 LookUSART6_GetBuffCount(void)
* ��	�ܣ���ȡ�������н��յ������ݸ���
* ��ڲ�������
* ���ڲ�������
* ˵	������					  
****************************************************************************/
u32 LookUSART6_GetBuffCount(void)
{
	return USART6QDataCount;
}

/****************************************************************************
* ��	�ƣ�u8 USART6_GetByte(u8* Data)	
* ��	�ܣ�����������ȡ���ַ�����
* ��ڲ�����u8* Data ���յ�������
* ���ڲ�����u8	���ճɹ���־
			0 	���ճɹ�
			1 	����ʧ��
* ˵	�����ӽ��ջ������л�ȡ���ݣ���������ȴ�
***************************************************************************/
u8 USART6_GetByte(u8* Data)
{
	if(USART6QDataCount != 0)				//��δ�������ݴ���
	{
		*Data = *USART6QOut;					//�Ӷ�����ȡ����
		USART6QOut++;									//ָ����һ��δ�������ݵ�
		if(USART6QOut > USART6QEnd)		//�������һ���жϵ������ؿ�ʼ��
			USART6QOut = USART6QStart;	
		USART6QDataCount--;						//δ�������ݸ����ݼ�

		return 1;
	}
	return 0;
}

/****************************************************************************
* ��	�ƣ�u8 USART6_GetByte_WaitTime(u8* Data,const u32 TimeLater)	
* ��	�ܣ�u8* Data ���յ�������
			const u32 TimeLater	�ȴ�ʱ�� 7=1us
* ��ڲ�����u8	�Ƿ��н��յ�����
				0 û�н��յ�����
				1 ���յ�����
* ���ڲ��������յ��ĵ��ַ�����
* ˵	�����ӽ��ջ������л�ȡ���ݣ�ʱ���ȴ�
***************************************************************************/
u8 USART6_GetByte_WaitTime(u8* Data,const u32 TimeLater)
{
	u8 flag = 0;					//��ȡ��־
	u32 err = 0;					//�����־
	u8 val = 0;					//��ȡ�����ݴ�ŵ�
	
	while(1)
	{
		if(LookUSART6_GetBuffCount() == 0)	//��������û������
		{	
			err++;
			if(err >= TimeLater)	//10���붼û��ȡ�����ݣ������˳�
				return 0;
		}
		else
		{
			flag = USART6_GetByte(&val);
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
* ��	�ƣ�u32 USART6_GetString(u8* Data ,const u32 len)	
* ��	�ܣ�����������ȡ�ַ�������
* ��ڲ�����u8* Data ���յ�������
			const u32 len	 ��Ҫ���յĳ���
* ���ڲ�����u32	��ȡ�����ݸ���		0��ȡʧ��
* ˵	�����ӽ��ջ������л�ȡ���ݣ���������ȴ� 
  
***************************************************************************/
u32 USART6_GetString(u8* Data ,const u32 len)
{
	u32 i = 0;
	u32 cnt = len;
	
	if(cnt == 0)													//׼������0�ֽ�����
		return 0;														//����ʧ��

	if(USART6QDataCount == 0)					   	//���������û������
		return 0;									   				//����ʧ��

	if(cnt > USART6QDataCount)				   	//Ҫȡ�ĸ�����Ŀǰ���յĸ�������
		cnt = USART6QDataCount;				   		//ȡ�����յ�����������

	if((USART6QOut + cnt - 1) > (USART6QEnd + 1))		//�в��������ڻ�������ǰ��		-1��0��ʼ�洢��
	{
		i = USART6QEnd - USART6QOut + 1;							//��벿�ֵ����ݸ���
		memcpy(Data , USART6QOut , i);								//ȡ��N������
		memcpy(Data + i , USART6QStart , cnt - i);	 	//ȡǰN������
		USART6QOut = USART6QStart + cnt - i;	 				//����ȡ��λ��
	}
	else
	{
		memcpy(Data,USART6QOut,cnt);	 		//ȡN������
		USART6QOut += cnt;								//ָ����һ��δ�������ݵ�
	}

	if(USART6QOut > USART6QEnd)	 				//���λ�ó�������һ����ַ
		USART6QOut = USART6QOut - USART6QEnd + USART6QStart - 1;	   //-1��0��ʼ	//���������ַ
	USART6QDataCount -= cnt;			 			//���������˵����ݸ���
	
	return cnt;								 //���ػ�ȡ�ĸ���
}

/****************************************************************************
* ��	�ƣ�void USART6_SendByte(const u8 Data)
* ��	�ܣ����ַ�����
* ��ڲ�����const u8 Data 	���͵��ַ�����
* ���ڲ�������
* ˵	������				   
****************************************************************************/
void USART6_SendByte(const u8 Data)		   //���ַ��������
{
	USART_SendData(USART6, Data);
	while(USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET);
}

/****************************************************************************
* ��	�ƣ�void USART6_SendString(u8* Data,const u32 Len)
* ��	�ܣ����ַ����
* ��ڲ�����u8* Data 	���͵�����
						const u32 Len				�ַ�����
* ���ڲ�������
* ˵	������					 
****************************************************************************/
void USART6_SendString(u8* Data,const u32 Len)		//���ַ����
{
	if(Len == 0)										//����Ϊ0
		return;
	
	if(ISRSendBusyFlag == 1)				//����æ״̬
		return;
	
	if(Len > USART6Real->USARTTxBufferSize)			//�����������Ƿ��㹻
		return;
	
	memcpy(USART6Real->USARTTxBuffer,Data,Len);	//�������ݵ�������
	
	ISRSendLen = 0;									//�����Ҫ���͵����ݸ���
	ISRSendLen = Len;								//��¼��Ҫ���͵����ݸ���
	ISRSendBusyFlag = 1;						//����æ״̬
	
	USART_SendData(USART6, USART6Real->USARTTxBuffer[0]);	//���͵�һ���ֽ�			
	ISRSendCnt = 1;									//���ͼ���
	USART_ITConfig(USART6, USART_IT_TXE, ENABLE);		//���������ж�	
}

/****************************************************************************
* ��	�ƣ�u8 USART6CheckISRSendBusy(void)	
* ��	�ܣ��жϷ���æ״̬���
* ��ڲ�������
* ���ڲ�����u8		1æ	0��æ
* ˵	������һ�εķ����Ƿ���ɣ�æ��ȴ�
****************************************************************************/
u8 USART6CheckISRSendBusy(void)	
{
		return ISRSendBusyFlag;
}

/****************************************************************************
* ��	�ƣ�void USART6_IRQHandler(void)	
* ��	�ܣ��жϻ���
* ��ڲ�������
* ���ڲ�������
* ˵	�������յ������ݴ�����ջ�����
	USART_GetITStatus		���ָ����USART�жϷ������
	USART_GetFlagStatus		���ָ����USART��־λ�������
****************************************************************************/
void USART6_IRQHandler(void)
{	
	//�����жϴ���
	if(USART_GetITStatus(USART6, USART_IT_RXNE) == SET)		
	{
		//���ݽ��ն���
		USART_ClearITPendingBit(USART6, USART_IT_RXNE);				//��ս����жϱ�־

		USART6_GetChar = USART_ReceiveData(USART6);								//���յ����ַ�����
		
		if(USART6QDataCount < USART6Real->USARTRxBufferSize)			//����ռ�δ��,�������� 
		{
			if(USART6QIn > USART6QEnd)			//����пռ�ɴ����ݣ�������λ����ĩβλ��
				USART6QIn = USART6QStart;			//������λ��������ʼλ��
			*USART6QIn = USART6_GetChar;		//���յ����ַ����ݴ��뻺��������λ��
			USART6QIn++;										//����λ������
			USART6QDataCount++;							//�ռ������ݸ�������	
		}
	}
	
	else if(USART_GetFlagStatus(USART6, USART_IT_ORE) == SET)		//����Ƿ��н������
		USART_ReceiveData(USART6);									//����������־��ֻ���ö����ݵķ�ʽ���������־

	//�����жϴ���
	if(USART_GetITStatus(USART6, USART_IT_TXE) == SET)		//�����ж�
	{		
		if(ISRSendCnt >= ISRSendLen)												//���ݷ�����
		{
			USART_ITConfig(USART6, USART_IT_TXE, DISABLE);		//��ֹ������λ���������ж�
			ISRSendBusyFlag = 0;					//���Ϳ���
		}
		else																								//����δ������
		{
			USART_SendData(USART6, USART6Real->USARTTxBuffer[ISRSendCnt]);
			ISRSendCnt++;
		}		
	}
}
