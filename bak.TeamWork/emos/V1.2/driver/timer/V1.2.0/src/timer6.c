#include "timer6.h"	   
#include "includes.h"
#include "driver.h"

static u32 TIM6_Count = 0;			//������
static u8 TIM6_CntFlag = 0;			//������������־

static TIM6_fun RealTim6fun = 0;		//��ʱ��7ִ�к���ָ��			����static
static const u8 TIM6_pDisc[] = "Tim6\r\n";		//�豸�����ַ�

#define	TIM6_CLEAR_IT_FLAG()	TIM6->SR = (uint16_t)~TIM_FLAG_Update

/****************************************************************************
* ��	�ƣ�void TIM6_TimeOpen(void)
* ��	�ܣ�������ʱ��
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void TIM6_TimeOpen(void)
{
	TIM_Cmd(TIM6, ENABLE);		//���ù̼��⺯��
}

/****************************************************************************
* ��	�ƣ�void TIM6_TimeClose(void)
* ��	�ܣ��رն�ʱ��
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void TIM6_TimeClose(void)
{
	TIM_Cmd(TIM6, DISABLE);		//���ù̼��⺯��
}

/****************************************************************************
* ��	�ƣ�void TIM6_ITOpen(void)
* ��	�ܣ�������ʱ���ж�
* ��ڲ�������
* ���ڲ�������
* ˵	�����жϿ�����ʱֵ����
****************************************************************************/
void TIM6_ITOpen(void)
{
	TIM_ITConfig(TIM6, TIM_FLAG_Update, ENABLE);	   //��TIM2�ĸ��±��TIM_FLAG_Update�ж�
}

/****************************************************************************
* ��	�ƣ�void TIM6_ITClose(void)
* ��	�ܣ��رն�ʱ���ж�
* ��ڲ�������
* ���ڲ�������
* ˵	�����жϹرռ�ʱֵ��������
****************************************************************************/
void TIM6_ITClose(void)
{
	TIM_ITConfig(TIM6, TIM_FLAG_Update, DISABLE);	   //�ر�TIM2�ĸ��±��TIM_FLAG_Update�ж�
}

/****************************************************************************
* ��	�ƣ�void TIM6_TimeISR_Open(void)
* ��	�ܣ���ʱ�жϿ���
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void TIM6_TimeISR_Open(void)
{
	TIM6_ITOpen();
	TIM6_TimeOpen();
	
	if(TIM6_CntFlag == 1)								//�����Ҫ����������
	{
		if(TIM6_Count == 0)								//��һ�ο�ʼʱ
			TIM6_Count = 1;									//��ʱ�����ȡֵʱ�����һ���жϼ�����û����
	}
}

/****************************************************************************
* ��	�ƣ�void TIM6_TimeISR_Close(void)
* ��	�ܣ���ʱ�жϹر�
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void TIM6_TimeISR_Close(void)
{
	TIM6_ITClose();
	TIM6_TimeClose();
}

/****************************************************************************
* ��	�ƣ�u32 TIM6_GetCount(void)
* ��	�ܣ����ʱ����ʱֵ
* ��ڲ�������
* ���ڲ�����u32 ��ʱ���� (���� / ��)
* ˵	������
****************************************************************************/
u32 TIM6_GetCount(void)
{
	return TIM6_Count;
}

/****************************************************************************
* ��	�ƣ�void TIM6_ResetCount(void)
* ��	�ܣ���ʱֵ��λ��0��ʼ
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void TIM6_ResetCount(void)
{
	TIM6_Count = 0;		 //��ʱֵ��λ
}

/****************************************************************************
* ��	�ƣ�void TIM6_FlagOnOff(u8 OnOff)
* ��	�ܣ������رռ�����
* ��ڲ�����u8 OnOff			������رռ�����	1���� 0�ر�
* ���ڲ�������
* ˵	������
****************************************************************************/
void TIM6_FlagOnOff(u8 OnOff)
{
	if(OnOff > 0)
		TIM6_CntFlag = 1;		 //��ʱֵ��λ
	else
		TIM6_CntFlag = 0;		 //��ʱֵ��λ
}

/****************************************************************************
* ��	�ƣ�void TIM6_Caller(void(*TIM6_fun)(void))
* ��	�ܣ�TIM6�ص�����
* ��ڲ������ص�ִ�к���
* ���ڲ�������
* ˵	������
****************************************************************************/
void TIM6_Caller(void(*TIM6_fun_t)(void))
{
	RealTim6fun = TIM6_fun_t;	//ִ�к���ָ��ָ��ִ�к���
}

/****************************************************************************
* ��	�ƣ�void TIM6_DAC_IRQHandler(void)
* ��	�ܣ���ʱ���жϴ���
* ��ڲ�������
* ���ڲ�������
* ˵	������ʱֵ����
****************************************************************************/
void TIM6_DAC_IRQHandler(void)
{
	SysIntEnter();	//�����жϽ����������ȼ����ȱ���

	//TIM_ClearITPendingBit(TIM6, TIM_FLAG_Update);	//�������жϱ�־
	TIM6_CLEAR_IT_FLAG();
	RealTim6fun();									//ִ��ִ�к���
		
	SysIntExit();   //�˳��жϽ����������ȼ����µ���
}

/****************************************************************************
* ��	�ƣ�static s8 TIM6_Open(u32 lParam)
* ��	�ܣ�ͳһ�ӿ�
* ��ڲ�����u32 lParam		��������
* ���ڲ�����u8						�򿪲�������ֵ	��HVL_ERR_CODE��
* ˵	������
****************************************************************************/
static s8 TIM6_Open(u32 lParam)
{
	return 0;					
}

/****************************************************************************
* ��	�ƣ�static s8 TIM6_Close(void)
* ��	�ܣ�ͳһ�ӿ�
* ��ڲ�������
* ���ڲ�����s8						�رղ�������ֵ	��HVL_ERR_CODE��
* ˵	������
****************************************************************************/
static s8 TIM6_Close(void)
{
	return 0;		
}

/****************************************************************************
* ��	�ƣ�static s32 TIM6_Read(u8* buffer,u32 len,u32* Reallen,u32 lParam) 
* ��	�ܣ����ڶ�
* ��ڲ�����u8* buffer		���ջ�����
						u32 len				ϣ�����յĳ���
						u32* Reallen	ʵ�ʽ��յĳ���
						u32 lParam		����
* ���ڲ�����s32						�����Ƿ�ɹ�		0�ɹ�	1ʧ��
* ˵	������
****************************************************************************/
static s32 TIM6_Read(u8* buffer,u32 len) 
{	
	return TIM6_GetCount();						
}

/****************************************************************************
* ��	�ƣ�static s32 TIM6_Write(u8* buffer,u32 len,u32* Reallen,u32 lParam) 
* ��	�ܣ����ڶ�
* ��ڲ�����u8* buffer		���ջ�����
						u32 len				ϣ�����͵ĳ���
						u32* Reallen	ʵ�ʷ��͵ĳ���
						u32 lParam		����
* ���ڲ�����s32						�����Ƿ�ɹ�		0�ɹ�	1ʧ��
* ˵	������
****************************************************************************/
static s32 TIM6_Write(u8* buffer,u32 len) 
{
	return 0;						
}

/****************************************************************************
* ��	�ƣ�static s32 TIM6_Ioctl(u32 cmd,u32 lParam)
* ��	�ܣ����ڿ���
* ��ڲ�����u32 cmd				����
						u32 lParam		����
* ���ڲ�����s32						��������Ĳ�ͬ���ص�ֵ���岻ͬ
* ˵	����	��
****************************************************************************/
static s32 TIM6_Ioctl(u32 cmd,u32 lParam)
{
	switch(cmd)
	{
		case TIM6_CMDClearCnt:	//��ռ�����
		{
			TIM6_ResetCount();
			return 1;							//�ɹ�
		}
		case TIM6_CMDTimOpen:		//�򿪶�ʱ������Ӧ�ж�
		{
			if(RealTim6fun != 0){				//���ִ�к�����ָ��
				TIM6_TimeISR_Open();
				return 1;							//�ɹ�
			}else return 0;
		}
		case TIM6_CMDTimClose:	//�رն�ʱ�����ر��ж�
		{
			TIM6_TimeISR_Close();
			return 1;							//�ɹ�
		}
		case TIM6_CMDSetCntFlag:	//�رն�ʱ�����ر��ж�
		{
			TIM6_FlagOnOff((u8)lParam);
			return 1;							//�ɹ�
		}
		case TIM6_CMDSetCaller:	//���ûص�����
		{
			TIM6_Caller((TIM6_fun)((u32*)lParam));
			return 1;							//�ɹ�
		}
	}
	return 0;	//��������	
}	

/****************************************************************************
* ��	�ƣ�u8 TIM6_Config(TIM6_fun Cfun,u16 TimeUS,u8 mode)
* ��	�ܣ���ʱ������
* ��ڲ�����TIM6_fun Cfun		�ص�����
						u16 TimeUS	��Ҫ��ʱ�ĵ�λ����	΢�뵥λ
						u8 mode			ģʽ 0΢��	1����	2��
* ���ڲ�����u8	�����Ƿ�ɹ�		1�ɹ� 0ʧ��
* ˵	������
****************************************************************************/
u8 TIM6_Config(TIM6_fun Cfun,u16 TimeUS,u8 mode)				//Ĭ��Ϊ�����ж�
{
	//ע���ñ���
	DEV_REG tim6 = 			//�豸ע����Ϣ��						����static		
	{
		CHAR_TIM6,					//�豸ID��
		0,  								//�Ƿ���			0:���ɹ���ʹ��, 1:�ɹ���ʹ��
		0, 									//���ڹ����豸���򿪴���
		1,									//��������
		1,									//���д����
		(u8*)TIM6_pDisc,					//��������			�Զ���
		20120001,						//�����汾			�Զ���
		(u32*)TIM6_Open,		//�豸�򿪺���ָ��
		(u32*)TIM6_Close, 	//�豸�رպ���ָ��
		(u32*)TIM6_Read,		//�ַ�������
		(u32*)TIM6_Write,		//�ַ�д����
		(u32*)TIM6_Ioctl		//���ƺ���
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
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);	//����ʱ������ʱ��APB1	//APB1Ԥ��ƵΪ��1ʱ,��ʱ��ʱ��Ƶ��Ϊ2��Ƶ��36*2=72MHZ
  TIM_DeInit(TIM6);																			//��λTIM2��ʱ����Ĭ��ֵ
	
	//��ʱʱ�䣺72000000��Ƶ��Ϊ1�룬��ǰ����(TIM_Period+1) * (TIM_Prescaler+1)��Ƶ�ʺ��������ж�
	T.TIM_Period = Period_t;										//��ʱֵ,��10000�η���һ������ж� (100΢��*10000=1��) ��ʱ��Ϊ16λ�����ֵ65535
	T.TIM_Prescaler = Prescaler_t; 							//Ԥ��Ƶϵ��	7200��Ƶ�ʺ�ʱֵ��1	(7200��Ƶ��100΢��)
	
  T.TIM_ClockDivision = 0;										//ʱ�ӷָ� �ڶ�ʱ��Ԥ��Ƶ֮ǰ��һ����ʱ�������õ�����Ϊ0����
  T.TIM_CounterMode = TIM_CounterMode_Up;			//���ϼ���		UP��10000��0�ļ���	DOWN:10000��FFFF
  TIM_TimeBaseInit(TIM6, &T);									//�����������ó�ʼ����ʱ��TIM2
  TIM_ClearFlag(TIM6, TIM_FLAG_Update);				//�������ж�

	//Ƕ�������жϿ���������	  
	N.NVIC_IRQChannel = TIM6_DAC_IRQn;					//���ö�TIMx_IRQn���жϼ���
  N.NVIC_IRQChannelPreemptionPriority = 0;		//���ȼ�
  N.NVIC_IRQChannelSubPriority = 0;						//��Ӧ��
  N.NVIC_IRQChannelCmd = ENABLE;							//�����жϼ���
  NVIC_Init(&N); 															//�����������ó�ʼ���жϿ�����
	
	if(Cfun != 0)																//�ص�������Ϊ��
		TIM6_Caller(Cfun);												//���ûص�����
	
	if(DeviceInstall(&tim6) != HVL_NO_ERR)			//�豸ע��
		return 0;
	TIM6_CntFlag = 1;
	return 1;
}

/*
	u32 LaterTime = 0;

	TIM6_Config(1);							//TIM6	��ʱ��	1=�� 	0=����
	TIM6_ResetCount();					//��ʱֵ��λ
	TIM6_TimeLater_Open();			//��ʼ����
	OSTimeDly(100);							//1S	1��ϵͳʱ��Ϊ10MS
	TIM6_TimeLater_Close();			//��������
	LaterTime = TIM6_GetCount();	//��ȡ��ʱֵ	9S
*/
