#include "timer5.h"	   
#include "includes.h"
#include "driver.h"

static u32 TIM5_Count = 0;			//������
static u8 TIM5_CntFlag = 0;			//������������־

static TIM5_fun RealTim5fun = 0;		//��ʱ��5ִ�к���ָ��			����static
static u8 TIM5_pDisc[] = "LuoHuaiXiang_Tim5\r\n";		//�豸�����ַ�

/****************************************************************************
* ��	�ƣ�void TIM5_TimeOpen(void)
* ��	�ܣ�������ʱ��
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void TIM5_TimeOpen(void)
{
	TIM_Cmd(TIM5, ENABLE);		//���ù̼��⺯��
}

/****************************************************************************
* ��	�ƣ�void TIM5_TimeClose(void)
* ��	�ܣ��رն�ʱ��
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void TIM5_TimeClose(void)
{
	TIM_Cmd(TIM5, DISABLE);		//���ù̼��⺯��
}

/****************************************************************************
* ��	�ƣ�void TIM5_ITOpen(void)
* ��	�ܣ�������ʱ���ж�
* ��ڲ�������
* ���ڲ�������
* ˵	�����жϿ�����ʱֵ����
****************************************************************************/
void TIM5_ITOpen(void)
{
	TIM_ITConfig(TIM5, TIM_FLAG_Update, ENABLE);	   //��TIM5�ĸ��±��TIM_FLAG_Update�ж�
}

/****************************************************************************
* ��	�ƣ�void TIM7_ITClose(void)
* ��	�ܣ��رն�ʱ���ж�
* ��ڲ�������
* ���ڲ�������
* ˵	�����жϹرռ�ʱֵ��������
****************************************************************************/
void TIM5_ITClose(void)
{
	TIM_ITConfig(TIM5, TIM_FLAG_Update, DISABLE);	   //�ر�TIM5�ĸ��±��TIM_FLAG_Update�ж�
}

/****************************************************************************
* ��	�ƣ�void TIM5_TimeISR_Open(void)
* ��	�ܣ���ʱ�жϿ���
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void TIM5_TimeISR_Open(void)
{
	TIM5_ITOpen();
	TIM5_TimeOpen();
	
	if(TIM5_CntFlag == 1)								//�����Ҫ����������
	{
		if(TIM5_Count == 0)								//��һ�ο�ʼʱ
			TIM5_Count = 1;									//��ʱ�����ȡֵʱ�����һ���жϼ�����û����
	}
}

/****************************************************************************
* ��	�ƣ�void TIM5_TimeISR_Close(void)
* ��	�ܣ���ʱ�жϹر�
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void TIM5_TimeISR_Close(void)
{
	TIM5_ITClose();
	TIM5_TimeClose();
}

/****************************************************************************
* ��	�ƣ�u32 TIM5_GetCount(void)
* ��	�ܣ����ʱ����ʱֵ
* ��ڲ�������
* ���ڲ�����u32 ��ʱ���� (���� / ��)
* ˵	������
****************************************************************************/
u32 TIM5_GetCount(void)
{
	return TIM5_Count;
}

/****************************************************************************
* ��	�ƣ�void TIM5_ResetCount(void)
* ��	�ܣ���ʱֵ��λ��0��ʼ
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void TIM5_ResetCount(void)
{
	TIM5_Count = 0;		 //��ʱֵ��λ
}

/****************************************************************************
* ��	�ƣ�void TIM5_FlagOnOff(u8 OnOff)
* ��	�ܣ������رռ�����
* ��ڲ�����u8 OnOff			������رռ�����	1���� 0�ر�
* ���ڲ�������
* ˵	������
****************************************************************************/
void TIM5_FlagOnOff(u8 OnOff)
{
	if(OnOff > 0)
		TIM5_CntFlag = 1;		 //��ʱֵ��λ
	else
		TIM5_CntFlag = 0;		 //��ʱֵ��λ
}

/****************************************************************************
* ��	�ƣ�void TIM5_Caller(void(*TIM7_fun)(void))
* ��	�ܣ�TIM7�ص�����
* ��ڲ������ص�ִ�к���
* ���ڲ�������
* ˵	������
****************************************************************************/
void TIM5_Caller(void(*TIM5_fun_t)(void))
{
	RealTim5fun = TIM5_fun_t;	//ִ�к���ָ��ָ��ִ�к���
}

/****************************************************************************
* ��	�ƣ�void TIM5_DAC_IRQHandler(void)
* ��	�ܣ���ʱ���жϴ���
* ��ڲ�������
* ���ڲ�������
* ˵	������ʱֵ����
****************************************************************************/
void TIM5_IRQHandler(void)
{
	SysIntEnter();	//�����жϽ����������ȼ����ȱ���

	TIM_ClearITPendingBit(TIM5, TIM_FLAG_Update);	//�������жϱ�־
	if(TIM5_CntFlag == 1)
	{
		if(TIM5_Count == 0xFFFFFFFF)
			TIM5_Count = 0;							//�������ֵ���»ص�0��ʼ����
		TIM5_Count++;									//��ʱֵ����
	}
	
	if(RealTim5fun != 0)								//���ִ�к�����ָ��
		RealTim5fun();										//ִ��ִ�к���
		
	SysIntExit();   //�˳��жϽ����������ȼ����µ���
}

/****************************************************************************
* ��	�ƣ�static s8 TIM5_Open(u32 lParam)
* ��	�ܣ�ͳһ�ӿ�
* ��ڲ�����u32 lParam		��������
* ���ڲ�����u8						�򿪲�������ֵ	��HVL_ERR_CODE��
* ˵	������
****************************************************************************/
static s8 TIM5_Open(u32 lParam)
{
	return 0;					
}

/****************************************************************************
* ��	�ƣ�static s8 TIM5_Close(void)
* ��	�ܣ�ͳһ�ӿ�
* ��ڲ�������
* ���ڲ�����s8						�رղ�������ֵ	��HVL_ERR_CODE��
* ˵	������
****************************************************************************/
static s8 TIM5_Close(void)
{
	return 0;		
}

/****************************************************************************
* ��	�ƣ�static s32 TIM5_Read(u8* buffer,u32 len,u32* Reallen,u32 lParam) 
* ��	�ܣ����ڶ�
* ��ڲ�����u8* buffer		���ջ�����
						u32 len				ϣ�����յĳ���
						u32* Reallen	ʵ�ʽ��յĳ���
						u32 lParam		����
* ���ڲ�����s32						�����Ƿ�ɹ�		0�ɹ�	1ʧ��
* ˵	������
****************************************************************************/
static s32 TIM5_Read(u8* buffer,u32 len,u32* Reallen,u32 lParam) 
{
	*Reallen = TIM5_GetCount();
	return 1;						
}

/****************************************************************************
* ��	�ƣ�static s32 TIM5_Write(u8* buffer,u32 len,u32* Reallen,u32 lParam) 
* ��	�ܣ����ڶ�
* ��ڲ�����u8* buffer		���ջ�����
						u32 len				ϣ�����͵ĳ���
						u32* Reallen	ʵ�ʷ��͵ĳ���
						u32 lParam		����
* ���ڲ�����s32						�����Ƿ�ɹ�		0�ɹ�	1ʧ��
* ˵	������
****************************************************************************/
static s32 TIM5_Write(u8* buffer,u32 len,u32* Reallen,u32 lParam) 
{
	return 0;						
}

/****************************************************************************
* ��	�ƣ�static s32 TIM7_Ioctl(u32 cmd,u32 lParam)
* ��	�ܣ����ڿ���
* ��ڲ�����u32 cmd				����
						u32 lParam		����
* ���ڲ�����s32						��������Ĳ�ͬ���ص�ֵ���岻ͬ
* ˵	����	��
****************************************************************************/
static s32 TIM5_Ioctl(u32 cmd,u32 lParam)
{
	switch(cmd)
	{
		case TIM5_CMDClearCnt:	//��ռ�����
		{
			TIM5_ResetCount();
			return 1;							//�ɹ�
		}
		case TIM5_CMDTimOpen:		//�򿪶�ʱ������Ӧ�ж�
		{
			TIM5_TimeISR_Open();
			return 1;							//�ɹ�
		}
		case TIM5_CMDTimClose:	//�رն�ʱ�����ر��ж�
		{
			TIM5_TimeISR_Close();
			return 1;							//�ɹ�
		}
		case TIM5_CMDSetCntFlag:	//�رն�ʱ�����ر��ж�
		{
			TIM5_FlagOnOff((u8)lParam);
			return 1;							//�ɹ�
		}
		case TIM5_CMDSetCaller:	//���ûص�����
		{
			TIM5_Caller((TIM5_fun)((u32*)lParam));
			return 1;							//�ɹ�
		}
	}
	return 0;	//��������	
}	

/****************************************************************************
* ��	�ƣ�u8 TIM5_Config(TIM5_fun Cfun,u16 TimeUS,u8 mode)
* ��	�ܣ���ʱ������
* ��ڲ�����TIM7_fun Cfun		�ص�����
						u16 TimeUS	��Ҫ��ʱ�ĵ�λ����	΢�뵥λ
						u8 mode			ģʽ 0΢��	1����	2��
* ���ڲ�����u8	�����Ƿ�ɹ�		1�ɹ� 0ʧ��
* ˵	������
****************************************************************************/
u8 TIM5_Config(TIM5_fun Cfun,u16 TimeUS,u8 mode)				//Ĭ��Ϊ�����ж�
{
	//ע���ñ���
	DEV_REG tim5 = 			//�豸ע����Ϣ��						����static		
	{
		CHAR_TIM5,					//�豸ID��
		0,  								//�Ƿ���			0:���ɹ���ʹ��, 1:�ɹ���ʹ��
		0, 									//���ڹ����豸���򿪴���
		1,									//��������
		1,									//���д����
		TIM5_pDisc,					//��������			�Զ���
		20120001,						//�����汾			�Զ���
		(u32*)TIM5_Open,		//�豸�򿪺���ָ��
		(u32*)TIM5_Close, 	//�豸�رպ���ָ��
		(u32*)TIM5_Read,		//�ַ�������
		(u32*)TIM5_Write,		//�ַ�д����
		(u32*)TIM5_Ioctl		//���ƺ���
	};
	
	TIM_TimeBaseInitTypeDef  T;							//������ʱ������
	NVIC_InitTypeDef N;   									//�����жϹ������

	u32 Period_t = 0;												//�����ö�ʱֵ
	u16 Prescaler_t = 0;										//�����÷�Ƶϵ��
	
	if(TimeUS < 1)
		TimeUS = 2;														//��ֹPeriod_t������Ϊ0
	
	switch(mode)														//��ʱģʽ����
	{
		case Mode_1US:
		{
			Period_t = TimeUS - 1;																				//�����ö�ʱֵ
			Prescaler_t = SYS_CPU_SysTickClkFreq() / 2 / 1000000 - 1;			//�����÷�Ƶϵ��
			break;
		}
		case Mode_10US:
		{
			Period_t = TimeUS - 1;																				//�����ö�ʱֵ
			Prescaler_t = SYS_CPU_SysTickClkFreq() / 2 / 100000 - 1;			//�����÷�Ƶϵ��
			break;
		}
		case Mode_100US:
		{
			Period_t = TimeUS - 1;																				//�����ö�ʱֵ
			Prescaler_t = SYS_CPU_SysTickClkFreq() / 2 / 10000 - 1;				//�����÷�Ƶϵ��
			break;
		}
		case Mode_500US:
		{
			Period_t = TimeUS - 1;																				//�����ö�ʱֵ
			Prescaler_t = SYS_CPU_SysTickClkFreq() / 2 / 2000 - 1;				//�����÷�Ƶϵ��
			break;
		}
		default:
			return 0;														//ģʽ����
	}
	
	//��ʱ������
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);	//����ʱ������ʱ��APB1	//APB1Ԥ��ƵΪ��1ʱ,��ʱ��ʱ��Ƶ��Ϊ2��Ƶ��36*2=72MHZ
  TIM_DeInit(TIM5);																			//��λTIM2��ʱ����Ĭ��ֵ
	
	//��ʱʱ�䣺72000000��Ƶ��Ϊ1�룬��ǰ����(TIM_Period+1) * (TIM_Prescaler+1)��Ƶ�ʺ��������ж�
	T.TIM_Period = Period_t;										//��ʱֵ,��10000�η���һ������ж� (100΢��*10000=1��) ��ʱ��Ϊ16λ�����ֵ65535
	T.TIM_Prescaler = Prescaler_t; 							//Ԥ��Ƶϵ��	7200��Ƶ�ʺ�ʱֵ��1	(7200��Ƶ��100΢��)
	
  T.TIM_ClockDivision = 0;										//ʱ�ӷָ� �ڶ�ʱ��Ԥ��Ƶ֮ǰ��һ����ʱ�������õ�����Ϊ0����
  T.TIM_CounterMode = TIM_CounterMode_Up;			//���ϼ���		UP��10000��0�ļ���	DOWN:10000��FFFF
  TIM_TimeBaseInit(TIM5, &T);									//�����������ó�ʼ����ʱ��TIM2
  TIM_ClearFlag(TIM5, TIM_FLAG_Update);				//�������ж�

	//Ƕ�������жϿ���������	  
	N.NVIC_IRQChannel = TIM5_IRQn;							//���ö�TIMx_IRQn���жϼ���
  N.NVIC_IRQChannelPreemptionPriority = 0;		//���ȼ�
  N.NVIC_IRQChannelSubPriority = 0;						//��Ӧ��
  N.NVIC_IRQChannelCmd = ENABLE;							//�����жϼ���
  NVIC_Init(&N); 															//�����������ó�ʼ���жϿ�����
	
	if(Cfun != 0)																//�ص�������Ϊ��
		TIM5_Caller(Cfun);												//���ûص�����
	
	if(DeviceInstall(&tim5) != HVL_NO_ERR)			//�豸ע��
		return 0;
	
	return 1;
}

/*
	u32 LaterTime = 0;

	TIM7_Config(1);							//TIM7	��ʱ��	1=�� 	0=����
	TIM7_ResetCount();					//��ʱֵ��λ
	TIM7_TimeLater_Open();			//��ʼ����
	OSTimeDly(100);							//1S	1��ϵͳʱ��Ϊ10MS
	TIM7_TimeLater_Close();			//��������
	LaterTime = TIM7_GetCount();	//��ȡ��ʱֵ	9S
*/
