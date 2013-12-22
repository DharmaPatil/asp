#include "beep_init.h"
#include "includes.h"					//INT16U
#include "driver.h"

#define USER_BEEP_Port		GPIOE								//���������Ŷ˿�
#define USER_BEEP_Pin			GPIO_Pin_4					//����������

//BEEP�豸���ƽṹ��
typedef struct
{
	GPIO_TypeDef*		BEEP_Port;							//���Ŷ˿ں�
	uint16_t				BEEP_Pin;								//���ź�
} BEEPCTL_STRUCT;

static BEEPCTL_STRUCT BEEP_Real = {(GPIO_TypeDef*)0,0};			//��������������ʵ��				����static
static u8 BeepFlag = 0;										//����or�ر�Beep��־ 0�ر�		1����		����static
static u32 Bcount = 0;										//���д���
static u32 Btime = 0;											//��������

/****************************************************************************
* ��	�ƣ�void SetBeepFlag(u8 on)
* ��	�ܣ�����beep�������Ŀ���
* ��ڲ�����u8 on		0 �ر�	1 ����
* ���ڲ�������
* ˵	������
****************************************************************************/
void SetBeepFlag(u8 on)
{
	if(on > 1) 
		on = 1;
	BeepFlag = on;		//����������
}

/****************************************************************************
* ��	�ƣ�u8 LookBeepFlag(void)
* ��	�ܣ��鿴����������
* ��ڲ�������
* ���ڲ�����u8		0 �ر�	1 ����
* ˵	������
****************************************************************************/
u8 LookBeepFlag(void)
{
	return BeepFlag;		//����������
}

/****************************************************************************
* ��	�ƣ�void Beep(u32 count,u16 time)
* ��	�ܣ�����������
* ��ڲ�����u16 count 	��������		16λ����
						u16 time 		����10MS		16λ���������65535���룬��655.3��
* ���ڲ�������
* ˵	������
****************************************************************************/
void Beep(u32 count,u16 time)
{
	u32 i;							//�ۼ���
	
	if(BeepFlag != 1)		//����������
	{
			GPIO_SetBits(BEEP_Real.BEEP_Port, BEEP_Real.BEEP_Pin);				//������1
			SysTimeDly(Btime);			//10���뼶��ʱ
			return;
	}
	else								//����
	{
		Bcount = count;			//�������
		Btime = time;				//��������
		
		for(i = 0;i < Bcount;i++)
		{
			GPIO_ResetBits(BEEP_Real.BEEP_Port, BEEP_Real.BEEP_Pin);			//������0
			SysTimeDly(Btime);			//10���뼶��ʱ
			
			GPIO_SetBits(BEEP_Real.BEEP_Port, BEEP_Real.BEEP_Pin);				//������1
			SysTimeDly(Btime);			//10���뼶��ʱ
			
		}
	}
}

/****************************************************************************
* ��	�ƣ�void Beep_Init(BEEPCTL_STRUCT Beep_t)
* ��	�ܣ�Beepģ��������ʼ��
* ��ڲ�����BEEPCTL_STRUCT Beep_t		��������
* ���ڲ�������
* ˵	������
****************************************************************************/
void Beep_Init(BEEPCTL_STRUCT Beep_t)
{
	GPIO_InitTypeDef GPIO_InitStructure;		//�������Žṹ

	BEEP_Real.BEEP_Port = Beep_t.BEEP_Port;		//��ȡ�˿ں�
	BEEP_Real.BEEP_Pin = Beep_t.BEEP_Pin;			//��ȡ���ź�
	
	//�������Ŷ˿ںŴ���Ӧ�Ķ˿�ʱ��
	if(BEEP_Real.BEEP_Port == GPIOA)
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	else if(BEEP_Real.BEEP_Port == GPIOB)
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	else if(BEEP_Real.BEEP_Port == GPIOC)
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	else if(BEEP_Real.BEEP_Port == GPIOD)
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	else if(BEEP_Real.BEEP_Port == GPIOE)
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	else if(BEEP_Real.BEEP_Port == GPIOF)
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	else if(BEEP_Real.BEEP_Port == GPIOG)
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	else if(BEEP_Real.BEEP_Port == GPIOH)
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);
	else if(BEEP_Real.BEEP_Port == GPIOI)
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE);

	GPIO_InitStructure.GPIO_Pin   = BEEP_Real.BEEP_Pin;	//��������������
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;			//��������Ϊ���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//Ƶ��50MHz
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL; 	//�����ڲ�������
	GPIO_Init(BEEP_Real.BEEP_Port, &GPIO_InitStructure);
}

/****************************************************************************
* ��	�ƣ�void Beep_Config(void)
* ��	�ܣ�ֻ��Ϊ������װBeep_init,�����Ź̶�����
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void Beep_Config(void)
{
	BEEPCTL_STRUCT Beep_t =
	{
		USER_BEEP_Port,		//���Ŷ˿ں�
		USER_BEEP_Pin			//���ź�
	};

	Beep_Init(Beep_t);
}
