#ifndef		_DT3102_IO_H_
#define		_DT3102_IO_H_

#include "includes.h"
#include "IO.h"
/*
		IN1				�ظ�1�¼�״̬			PF0
		IN2				�ظ�1����״̬			PF1
		IN3				��Ļ2�¼�״̬			PF2
		IN4				��Ļ2����״̬			PF3
		IN5				�ظ�2�¼�״̬			PF4
		IN6				�ظ�2����״̬			PF5
		
		IN8				����ʶ����1��˫������	PF7
		IN9				����ʶ����1�¼�״̬		PF8
		IN10			����ʶ����1����״̬		PF9
		
		OUT1			��բͨ��1				PE0
		OUT2			��բͨ��2				PE1
		OUT3			���̵�					PE2
*/
//IO-PORT
#define	IN1_PORT	GPIOF
#define	IN2_PORT	GPIOF
#define	IN3_PORT	GPIOF
#define	IN4_PORT	GPIOF
#define	IN5_PORT	GPIOF
#define	IN6_PORT	GPIOF
#define	IN7_PORT	GPIOF
#define	IN8_PORT	GPIOF
#define	IN9_PORT	GPIOF
#define	IN10_PORT	GPIOF
#define	OUT1_PORT	GPIOE
#define	OUT2_PORT	GPIOE
#define	OUT3_PORT	GPIOE
#define	OUT4_PORT	GPIOE
//IO-PIN
#define	IN1_PIN		GPIO_Pin_0
#define	IN2_PIN		GPIO_Pin_1
#define	IN3_PIN		GPIO_Pin_2
#define	IN4_PIN		GPIO_Pin_3
#define	IN5_PIN		GPIO_Pin_4
#define	IN6_PIN		GPIO_Pin_5
#define	IN7_PIN		GPIO_Pin_6
#define	IN8_PIN		GPIO_Pin_7
#define	IN9_PIN		GPIO_Pin_8
#define	IN10_PIN	GPIO_Pin_9
#define	OUT1_PIN	GPIO_Pin_0
#define	OUT2_PIN	GPIO_Pin_1
#define	OUT3_PIN	GPIO_Pin_2
#define	OUT4_PIN	GPIO_Pin_3
//��Ļ2
#define	LIGHT2_SIGN_PORT	IN3_PORT
#define	LIGHT2_SIGN_PIN	IN3_PIN
#define	LIGHT2_ERR_PORT	IN4_PORT
#define	LIGHT2_ERR_PIN		IN4_PIN
//�ظ�1
#define	LAND1_SIGN_PORT	IN1_PORT
#define	LAND1_SIGN_PIN		IN1_PIN
#define	LAND1_ERR_PORT		IN2_PORT
#define	LAND1_ERR_PIN		IN2_PIN
//�ظ�2
#define	LAND2_SIGN_PORT	IN5_PORT
#define	LAND2_SIGN_PIN		IN5_PIN
#define	LAND2_ERR_PORT		IN6_PORT
#define	LAND2_ERR_PIN		IN6_PIN
//����ʶ����1
#define	AXIS_TYPE1_JUDGE_PORT	IN8_PORT	
#define	AXIS_TYPE1_JUDGE_PIN	IN8_PIN
#define	AXIS_TYPE1_SIGN_PORT	IN9_PORT
#define	AXIS_TYPE1_SIGN_PIN		IN9_PIN
#define	AXIS_TYPE1_ERR_PORT		IN10_PORT
#define	AXIS_TYPE1_ERR_PIN		IN10_PIN
//��բ�����̵�
#define	ROAD_GATE_DLY_TIME		60			//��բ������ʱʱ��
#define	ROAD_GATE1_PORT			OUT1_PORT
#define	ROAD_GATE1_PIN			OUT1_PIN
#define	ROAD_GATE2_PORT			OUT2_PORT
#define	ROAD_GATE2_PIN			OUT2_PIN
#define	TRAFFIC_LIGHT_PORT		OUT3_PORT
#define	TRAFFIC_LIGHT_PIN		OUT3_PIN

//�豸��ƽ�ṹ
typedef union _DeviceSignal
{
	struct
	{
		u8	LC1EventSignal 	:	1;	//��Ļ1�źŵ�ƽ
		u8	LC1ErrSignal		:	1;	//��Ļ1���ϵ�ƽ
		u8	LC2EventSignal 	:	1;	//��Ļ2�źŵ�ƽ
		u8	LC2ErrSignal		:	1;	//��Ļ2���ϵ�ƽ
		u8	GC1EventSignal 	:	1;	//�ظ�1�źŵ�ƽ
		u8	GC1ErrSignal 		:	1;	//�ظ�1���ϵ�ƽ
		u8	GC2EventSignal 	:	1;	//�ظ�2�źŵ�ƽ
		u8	GC2ErrSignal 		:	1;	//�ظ�2���ϵ�ƽ
		u8	TyreEventSignal 	:	1;	//�����źŵ�ƽ
		u8	TyreErrSignal 		:	1;	//������ϵ�ƽ
		u8	TyreDoubleSignal	:	1;	//˫�ֵ�ƽ
	}Signal;
	u16 value;
}DeviceSignal;

u16 Get_Device_Signal(void);
BOOL Set_Device_Signal(u16 value);


//��Ļ״̬�ṹ��
typedef	union sLight_State_TypeDef
{
	u8	u8Data;
	struct
	{
		u8	EventFlag : 1;		//�¼�λ	0-���¼�	1-��Ļ����
		u8	ErrFlag : 	1;		//����λ	0-��Ļ����	1-��Ļ����
	}Bit;
}sLight_State_TypeDef;			
//�ظ�״̬�ṹ��
typedef	union sLand1_State_TypeDef
{
	u8	u8Data;
	struct
	{
		u8	EventFlag : 1;		//�¼�λ	0-���¼�	1-�ظд���
		u8	ErrFlag : 	1;		//����λ	0-�ظ�����	1-�ظй���
	}Bit;
}sLand1_State_TypeDef;
typedef	union sLand2_State_TypeDef
{
	u8	u8Data;
	struct
	{
		u8	EventFlag : 1;		//�¼�λ	0-���¼�	1-�ظд���
		u8	ErrFlag : 	1;		//����λ	0-�ظ�����	1-�ظй���
	}Bit;
}sLand2_State_TypeDef;
//����ʶ����״̬�ṹ��
typedef union sAxisType1_State_TypeDef
{
	u8 u8Data;
	struct
	{
		u8	EventFlag :	1;	//�¼�λ			0-���¼�	1-����ʶ��������
		u8	ErrFlag	:	1;		//����λ			0-�ظ�����	1-����ʶ��������
		u8	SingleOrBoth :	1;	//��˫�ֱ�־λ		0-����		1-˫��
	}Bit;
}sAxisType1_State_TypeDef;

void fDT3102_IO_Init(void);							//����10·����4·����ĳ�ʼ��
sLand1_State_TypeDef fRead_Land1State(void);		//��ȡ�ظ�1״̬
sLand2_State_TypeDef fRead_Land2State(void);		//��ȡ�ظ�2״̬
sLight_State_TypeDef fRead_Light2State(void);		//��ȡ��Ļ2״̬
sAxisType1_State_TypeDef fRead_AxisType1State(void);//��ȡ����ʶ����1״̬

enum 
{
	TrafficSignal = 1,		//���̵�
	BarrierGate				//��բ
};

//��բ����
enum
{
	BarrierGateOpenPort = 1,
	BarrierGateClosePort,
};

//��բ��ƽ����
#define	PortNormalOpen	1
#define PortNormalClose 0

s8 fPeripheral_Control(u8 type , u8 status);		//��բ�����̵ƿ���
void BarrierGate_JDQ_Control(u8 port, u8 status);	//��բ���ſ���

#endif
