#include "TIM6_init.h"	  
#include "static_init.h"						//Get_SyS_CLK

static u32 TIM6_Count = 0;					//������
static u8 TIM6_CntFlag = 0;					//������������־
static TIM6_fun RealTim6fun = 0;		//��ʱ��ִ�к���ָ��

/****************************************************************************
* ��	�ƣ�void TIM6_TimeOpenOrClose(const u8 Flag)
* ��	�ܣ����ض�ʱ��
* ��ڲ�����const u8 Flag		��ʱ������	1�� 0��
* ���ڲ�������
* ˵	������
****************************************************************************/
void TIM6_TimeOpenOrClose(const u8 Flag)
{
	Flag > 0 ? TIM_Cmd(TIM6, ENABLE) : TIM_Cmd(TIM6, DISABLE);
}

/****************************************************************************
* ��	�ƣ�void TIM6_ITOpenOrClose(const u8 Flag)
* ��	�ܣ����ض�ʱ���ж�
* ��ڲ�����const u8 Flag		��ʱ���жϿ���	1�� 0��
* ���ڲ�������
* ˵	�����жϿ�����ʱֵ����
****************************************************************************/
void TIM6_ITOpenOrClose(const u8 Flag)
{
	Flag > 0 ? TIM_ITConfig(TIM6, TIM_FLAG_Update, ENABLE) : TIM_ITConfig(TIM6, TIM_FLAG_Update, DISABLE);
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
	TIM6_ITOpenOrClose(1);
	TIM6_TimeOpenOrClose(1);
	
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
	TIM6_ITOpenOrClose(0);
	TIM6_TimeOpenOrClose(0);
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
* ��	�ƣ�void TIM6_CntFlagOnOff(const u8 OnOff)
* ��	�ܣ������رռ�����
* ��ڲ�����const u8 OnOff			������رռ�����	1���� 0�ر�
* ���ڲ�������
* ˵	������
****************************************************************************/
void TIM6_CntFlagOnOff(const u8 OnOff)
{
	OnOff > 0 ? (TIM6_CntFlag = 1) : (TIM6_CntFlag = 0);		//��ʱֵ��λ
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
	TIM_ClearITPendingBit(TIM6, TIM_FLAG_Update);	//�������жϱ�־
	if(TIM6_CntFlag == 1)
	{
		if(TIM6_Count == 0xFFFFFFFF)
			TIM6_Count = 0;							//�������ֵ���»ص�0��ʼ����
		TIM6_Count++;									//��ʱֵ����
	}
	
	if(RealTim6fun != 0)								//���ִ�к�����ָ��
		RealTim6fun();										//ִ��ִ�к���
}

/****************************************************************************
* ��	�ƣ�u8 TIM6_Config(const TIM6_fun Cfun,const u16 TimeUS,const u8 mode)
* ��	�ܣ���ʱ������
* ��ڲ�����const TIM6_fun Cfun		�ص�����
						const u16 TimeUS	��Ҫ��ʱ�ĵ�λ����	΢�뵥λ
						const u8 mode			ģʽ 0΢��	1����	2��
* ���ڲ�����u8	�����Ƿ�ɹ�		1�ɹ� 0ʧ��
* ˵	������
****************************************************************************/
u8 TIM6_Config(const TIM6_fun Cfun,const u16 TimeUS,const u8 mode)				//Ĭ��Ϊ�����ж�
{
	TIM_TimeBaseInitTypeDef  T;							//������ʱ������
	NVIC_InitTypeDef N;   									//�����жϹ������

	u32 Period_t = 0;												//�����ö�ʱֵ
	u16 Prescaler_t = 0;										//�����÷�Ƶϵ��
	u16 TimeUS_t = TimeUS;									//��ʱʱ��
		
	if(TimeUS_t < 1)
		TimeUS_t = 2;													//��ֹPeriod_t������Ϊ0
	
	switch(mode)														//��ʱģʽ����
	{
		case Mode_1US:
		{
			Period_t = TimeUS_t - 1;																	//�����ö�ʱֵ
			Prescaler_t = Get_SyS_CLK(SYSCLK) / 2 / 1000000 - 1;			//�����÷�Ƶϵ��
			break;
		}
		case Mode_10US:
		{
			Period_t = TimeUS_t - 1;																	//�����ö�ʱֵ
			Prescaler_t = Get_SyS_CLK(SYSCLK) / 2 / 100000 - 1;				//�����÷�Ƶϵ��
			break;
		}
		case Mode_100US:
		{
			Period_t = TimeUS_t - 1;																	//�����ö�ʱֵ
			Prescaler_t = Get_SyS_CLK(SYSCLK) / 2 / 10000 - 1;				//�����÷�Ƶϵ��
			break;
		}
		case Mode_500US:
		{
			Period_t = TimeUS_t - 1;																	//�����ö�ʱֵ
			Prescaler_t = Get_SyS_CLK(SYSCLK) / 2 / 2000 - 1;					//�����÷�Ƶϵ��
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
	return 1;
}

/*
if(TIM6_Config(0,10,Mode_100US) == 0)							//Ĭ��Ϊ�����ж�
		DebugPf(0,"��ʱ��6����ʧ��\r\n");
	
TIM6_FlagOnOff(1);					//��ʱֵ��λ
TIM6_ResetCount();
TIM6_TimeISR_Open();
*/
