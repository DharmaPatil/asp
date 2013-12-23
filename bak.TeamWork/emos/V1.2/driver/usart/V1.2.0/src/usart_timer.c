#include "usart_timer.h"
#include "stm32f4xx.h"			//stm32
#include "C2.h"							//SYS_EVENT
#include "usart1.h"
#include "usart6.h"
#define usart1_Activate			1				//1����		0����
#define usart2_Activate			0				//1����		0����
#define usart3_Activate			0				//1����		0����
#define uart4_Activate			0				//1����		0����

#if(usart1_Activate)
extern void USART1_SetStatus(const u8 Status);											//���ô��ڴ���״̬
extern SYS_EVENT* GetUSART1_ReceiveFinishEvent(void);								//��ȡ��������ź���
extern u8 LookUSART1_SrcStatus(void);																//��ȡ���ڴ���Դ״̬
extern u8 LookUSART1_WorkStatusVal(void);														//�鿴״̬�ź��������󴮿ڵĹ���״ֵ̬
#endif

#if(usart2_Activate)
extern void USART2_SetStatus(const u8 Status);											//���ô��ڴ���״̬
extern SYS_EVENT* GetUSART2_ReceiveFinishEvent(void);								//��ȡ��������ź���
extern u8 LookUSART2_SrcStatus(void);																//��ȡ���ڴ���Դ״̬
extern u8 LookUSART2_WorkStatusVal(void);														//�鿴״̬�ź��������󴮿ڵĹ���״ֵ̬
#endif

#if(usart3_Activate)
extern void USART3_SetStatus(const u8 Status);											//���ô��ڴ���״̬
extern SYS_EVENT* GetUSART3_ReceiveFinishEvent(void);								//��ȡ��������ź���
extern u8 LookUSART3_SrcStatus(void);																//��ȡ���ڴ���Դ״̬
extern u8 LookUSART3_WorkStatusVal(void);														//�鿴״̬�ź��������󴮿ڵĹ���״ֵ̬
#endif

#if(uart4_Activate)
extern void UART4_SetStatus(const u8 Status);												//���ô��ڴ���״̬
extern SYS_EVENT* GetUART4_ReceiveFinishEvent(void);								//��ȡ��������ź���
extern u8 LookUART4_SrcStatus(void);																//��ȡ���ڴ���Դ״̬
extern u8 LookUART4_WorkStatusVal(void);														//�鿴״̬�ź��������󴮿ڵĹ���״ֵ̬
#endif

#define TIM3_FullOut	0			//��ʱ�������⿪��
#define USARTSTATUS_General_Bak								0x00		//ͨ��״̬		//�Ѿ���usart_cfg.h�ж�����
#define USARTSTATUS_Bag_Bak										0x08		//������			//�Ѿ���usart_cfg.h�ж�����

/****************************************************************************
* ��	�ƣ�void TIM3_Init(void)
* ��	�ܣ���ʱ����ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵	����Get_SyS_CLK���м��ϵͳ��Ƶ

3�ֽ�ʱ���	  �ο�����USART1_Real.TimeVal��ʱֵ
115200	= 1000000	/	11520 = 86us*3 		= 258us		/	20us	=	����12.9��	
57600		= 1000000	/	5760 	= 173us*3 	= 519us		/	20us	=	����25��
38400		= 1000000	/	3840 	= 260us*3 	= 781us		/	20us	=	����39��
19200		= 1000000	/	1920 	= 520us*3 	= 1560us	/	20us	=	����78��
9600		= 1000000	/	960 	= 1041us*3 	= 3123us	/	20us	=	����156��
****************************************************************************/
void TIM3_Init(void)
{
	TIM_TimeBaseInitTypeDef  T;	//������ʱ������

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	  	//TIM3ʹ��APB1ʱ������
	TIM_DeInit(TIM3);																					//��λTIM3��ʱ����Ĭ��ֵ

	T.TIM_Period = 65535;											//���������ֵ�����65535����ֵ���ܸ��ģ�������Ƚϵ��趨ֵһ�£�����Ƚϼ��������ҲΪ65535
	T.TIM_Prescaler = (SYS_CPU_SysTickClkFreq() / 2) / 50000 - 1; 	//Ԥ��Ƶϵ����ϵͳÿ��168,000,000�Σ�������2��Ƶ����ÿ��84,000,000�Σ���ϵͳƵ�����Ǵ˴�Ϊ2�룬168,000,000/100,000=1680(20us)����ÿ�μ���ʱ��Ϊ20us��ÿ����TIM_Period���ʱ��0�ؼ�
	T.TIM_ClockDivision = 0;									//ʱ�ӷָ� �ڶ�ʱ��Ԥ��Ƶ֮ǰ��һ����ʱ�������õ�����Ϊ0����
  T.TIM_CounterMode = TIM_CounterMode_Up;		//���ϼ���		UP��0��10000�ļ���	DOWN:FFFF��10000 
  TIM_TimeBaseInit(TIM3, &T);								//�����������ó�ʼ����ʱ��TIM3

  TIM_ClearFlag(TIM3, TIM_FLAG_Update);			//�������ж�
  TIM_SetCounter(TIM3,0);										//���ü�������ֵΪ0	//Ŀǰ��δ���øö�ʱ��
}

/****************************************************************************
* ��	�ƣ�void TIM3_OC(void)
* ��	�ܣ���ʱ������ȽϹ��ܳ�ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/ 
void TIM3_OC(void)
{
	TIM_OCInitTypeDef O;  	//��������Ƚ����ñ���
	
  O.TIM_OCMode = TIM_OCMode_Timing; 			  		//���ó�����Ƚϼ�ʱģʽ
  O.TIM_OutputState = TIM_OutputState_Disable;  //��ֹOC����Ƚ�
  O.TIM_OCPolarity = TIM_OCPolarity_Low; 		  	//����Ƚϼ�Ϊ��

	O.TIM_Pulse = 0;				//��ʼ������Ƚ�ͨ��ֵ	���65535�������TIM_Periodһ��

	TIM_OC1Init(TIM3, &O); 	//��ʼ������Ƚ�ͨ��1
	TIM_OC2Init(TIM3, &O); 	//��ʼ������Ƚ�ͨ��2
	TIM_OC3Init(TIM3, &O); 	//��ʼ������Ƚ�ͨ��3
	TIM_OC4Init(TIM3, &O); 	//��ʼ������Ƚ�ͨ��4	
}

/****************************************************************************
* ��	�ƣ�void TIM3_IC(void)
* ��	�ܣ���ʱ�����벶���ܳ�ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵	������δʵ��
****************************************************************************/ 
void TIM3_IC(void)
{
	TIM_ICInitTypeDef I;  													//��������Ƚ����ñ���
	
	//���벶������	TIMx_CCRx
	I.TIM_Channel = TIM_Channel_1; 				  				//ʹ�����벶��ͨ��
	I.TIM_ICPolarity = TIM_ICPolarity_Rising; 	  	//���벶����������Ч
	I.TIM_ICSelection = TIM_ICSelection_DirectTI; 	//IC*��������ѡ�����IC1/IC2�в�ͬ�Ķ���
	I.TIM_ICPrescaler = TIM_ICPSC_DIV1; 		  			//����ģʽ�£�����˿��ϵ�ÿһ�α��ض�����һ�β���
	I.TIM_ICFilter = 0; 						  							//�������Ƶ�ʣ����TIM*_CCMR->IC*F˵��
	TIM_ICInit(TIM3, &I); 						  						//��ʼ������
}

/****************************************************************************
* ��	�ƣ�void TIM3_NVIC_Init(void)
* ��	�ܣ���ʱ���ж��������ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void TIM3_NVIC_Init(void)
{
	NVIC_InitTypeDef N;
		  
	N.NVIC_IRQChannel = TIM3_IRQn;						//���ö�TIM3_IRQn���жϼ���
  N.NVIC_IRQChannelPreemptionPriority = 0;	//���ȼ�
  N.NVIC_IRQChannelSubPriority = 0;					//��Ӧ��
  N.NVIC_IRQChannelCmd = ENABLE;						//�����жϼ���
  NVIC_Init(&N); 														//�����������ó�ʼ���жϿ�����
}

/****************************************************************************
* ��	�ƣ�void TIM3_Config(void)
* ��	�ܣ����ö�ʱ��
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/	 
void usart_timer_init(void)
{
	TIM3_Init();			   	//��ʱ����ʼ��
	TIM3_OC();						//����Ƚϳ�ʼ��
//	TIM3_IC();					//���벶���ʼ��	��δʵ��
	TIM3_NVIC_Init();			//�ж��������ʼ��

	//��������ж�	���.����Ƚ�ͨ��1.����Ƚ�ͨ��2.����Ƚ�ͨ��3.����Ƚ�ͨ��4
	TIM_ClearITPendingBit(TIM3 , TIM_FLAG_Update | TIM_FLAG_CC1 | TIM_FLAG_CC2 | TIM_FLAG_CC3 | TIM_FLAG_CC4 | TIM_FLAG_Trigger);
	
	if(TIM3_FullOut)
		TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);	  //��������ж�

	TIM_Cmd(TIM3, ENABLE);	  //����TIM3������	(�������ϵ���ͣʱ��������Ȼ�����)
}

/****************************************************************************
* ��	�ƣ�void TIM3_IRQHandler(void)
* ��	�ܣ���ʱ��3�ж�
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/	 
void TIM3_IRQHandler(void)		//TIM3�жϴ��� �������������������ͣ����ͣ
{
	SysIntEnter();																			//�����жϽ����������ȼ����ȱ���

	if(TIM_GetITStatus(TIM3, TIM_FLAG_Update) != RESET) //����ж�
 	{
		TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);	 			//�ر��ж�
		TIM_ClearITPendingBit(TIM3, TIM_FLAG_Update);			//����жϱ�־
	}

	if(TIM_GetITStatus(TIM3, TIM_FLAG_CC1) != RESET)	 	//����Ƚ�ͨ��1�ж�
 	{	
		TIM_ITConfig(TIM3, TIM_IT_CC1, DISABLE);					//�ر��ж�
		TIM_ClearITPendingBit(TIM3, TIM_FLAG_CC1);				//����жϱ�־
		usart1_timer_isr();
	}
	
	if(TIM_GetITStatus(TIM3, TIM_FLAG_CC2) != RESET)	 								//����Ƚ�ͨ��1�ж�
	{
		TIM_ITConfig(TIM3, TIM_IT_CC2, DISABLE);												//�ر��ж�
		TIM_ClearITPendingBit(TIM3, TIM_FLAG_CC2);											//����жϱ�־
		usart6_timer_isr();
 	}

	if(TIM_GetITStatus(TIM3, TIM_FLAG_CC3) != RESET)	 //����Ƚ�ͨ��3�ж�
 	{
		TIM_ITConfig(TIM3, TIM_IT_CC3, DISABLE);	//�ر��ж�
		TIM_ClearITPendingBit(TIM3, TIM_FLAG_CC3);	//����жϱ�־
 	}
	
	if(TIM_GetITStatus(TIM3, TIM_FLAG_CC4) != RESET)	 //����Ƚ�ͨ��4�ж�
 	{
		TIM_ITConfig(TIM3, TIM_IT_CC4, DISABLE);	//�ر��ж�
		TIM_ClearITPendingBit(TIM3, TIM_FLAG_CC4);	//����жϱ�־
 	}

	SysIntExit();	//�˳��жϽ����������ȼ����µ���
}
