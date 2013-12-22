#include "IO.h"
#include "dt3102_io.h"
#include "C2.h"
#include "sys_param.h"

static void DT3102_OutputInit(void)
{
	IoSetMode(OUT1, IO_MODE_OUTPUT);		//1
	IoSetSpeed(OUT1, IO_SPEED_50M);
	IoPushPull(OUT1); 	
	IoSetMode(OUT2, IO_MODE_OUTPUT);		//2
	IoSetSpeed(OUT2, IO_SPEED_50M);
	IoPushPull(OUT2); 	
	IoSetMode(OUT3, IO_MODE_OUTPUT);		//3
	IoSetSpeed(OUT3, IO_SPEED_50M);
	IoPushPull(OUT3); 	
	IoSetMode(OUT4, IO_MODE_OUTPUT);		//4
	IoSetSpeed(OUT4, IO_SPEED_50M);
	IoPushPull(OUT4); 	
}
static void DT3102_InputInit(void)
{	//�������Ѿ�����������MCU�ڲ�����
	IoSetMode(IN1, IO_MODE_INPUT);			//1
	IoSetSpeed(IN1, IO_SPEED_50M);	
	IoSetMode(IN2, IO_MODE_INPUT);			//2
	IoSetSpeed(IN2, IO_SPEED_50M);	
	IoSetMode(IN3, IO_MODE_INPUT);			//3
	IoSetSpeed(IN3, IO_SPEED_50M);	
	IoSetMode(IN4, IO_MODE_INPUT);			//4
	IoSetSpeed(IN4, IO_SPEED_50M);	
	IoSetMode(IN5, IO_MODE_INPUT);			//5
	IoSetSpeed(IN5, IO_SPEED_50M);	
	IoSetMode(IN6, IO_MODE_INPUT);			//6
	IoSetSpeed(IN6, IO_SPEED_50M);	
	IoSetMode(IN7, IO_MODE_INPUT);			//7
	IoSetSpeed(IN7, IO_SPEED_50M);	
	IoSetMode(IN8, IO_MODE_INPUT);			//8
	IoSetSpeed(IN8, IO_SPEED_50M);	
	IoSetMode(IN9, IO_MODE_INPUT);			//9
	IoSetSpeed(IN9, IO_SPEED_50M);	
	IoSetMode(IN10, IO_MODE_INPUT);			//10
	IoSetSpeed(IN10, IO_SPEED_50M);	
}

//=====================================================================
//�����ƽ����
/*
 #define	LIGHT2_NOEVENT 		1			//�źŵ�ƽ		���źű�ʶ
 #define	LIGHT2_ERROR 		0			//���ϵ�ƽ		���ϱ�ʶ
 #define	LAND1_NOEVENT		1			//�źŵ�ƽ		���źű�ʶ
 #define	LAND1_ERROR		0			//���ϵ�ƽ		���ϱ�ʶ
 #define	LAND2_NOEVENT		1			//�źŵ�ƽ		���źű�ʶ
 #define	LAND2_ERROR		0			//���ϵ�ƽ		���ϱ�ʶ
 #define	AXIS_TYPE1_NOEVENT	0			//�źŵ�ƽ		���źű�ʶ
 #define	AXIS_TYPE1_SINGLE	0			//��˫�ֵ�ƽ	���ֱ�ʶ
 #define	AXIS_TYPE1_ERROR	1			//���ϵ�ƽ		���ϱ�ʶ
*/

static DeviceSignal DT3102Signal = {0};

static void DT3102_DeviceSignal_DefaultInit(void)
{
	DT3102Signal.Signal.LC1EventSignal 	= 0;
	DT3102Signal.Signal.LC1ErrSignal		= 0;
	
	DT3102Signal.Signal.LC2EventSignal 	= 0;
	DT3102Signal.Signal.LC2ErrSignal		= 0;
	
	DT3102Signal.Signal.GC1EventSignal		= 0;
	DT3102Signal.Signal.GC1ErrSignal		= 0;
	
	DT3102Signal.Signal.GC2EventSignal		= 0;
	DT3102Signal.Signal.GC2ErrSignal		= 0;

	DT3102Signal.Signal.TyreEventSignal	= 1;
	DT3102Signal.Signal.TyreDoubleSignal	= 1;
	DT3102Signal.Signal.TyreErrSignal		= 1;
}

u16 Get_Device_Signal(void)
{
	return DT3102Signal.value;
}

BOOL Set_Device_Signal(u16 value)
{
	if(Param_Write(GET_U16_OFFSET(ParamDeviceSignal), &value, sizeof(unsigned short)))
	{
		DT3102Signal.value = value;
		return TRUE;
	}

	return FALSE;
}

void fDT3102_IO_Init(void)
{
	uint16 tmp = 0xffff;
	
	DT3102_OutputInit();
	DT3102_InputInit();

	Param_Read(GET_U16_OFFSET(ParamDeviceSignal), &tmp, sizeof(unsigned short));
	if((tmp & 0xf800) != 0)		//��ƽ����ʹ�������ֽڵĵ�11λ����λ��ʼ��Ϊ0������λ������0ʱ�����¼���Ĭ�ϵ�ƽ
	{
		//����Ĭ�ϵ�ƽ
		DT3102_DeviceSignal_DefaultInit();
	}
	else
	{
		DT3102Signal.value = tmp;
	}
}

//���ظ�1״̬
sLand1_State_TypeDef fRead_Land1State(void)
{
	sLand1_State_TypeDef sTmp;
	if( DT3102Signal.Signal.GC1EventSignal == GetIo(LAND1_SIGN) )
		sTmp.Bit.EventFlag = 1;		//��������ظ�1
	else
		sTmp.Bit.EventFlag = 0;
	if( DT3102Signal.Signal.GC1ErrSignal == GetIo(LAND1_ERR) )
		sTmp.Bit.ErrFlag = 1;		//����ظ�1����
	else
		sTmp.Bit.ErrFlag = 0;
	
	return sTmp;
}
//���ظ�2״̬
sLand2_State_TypeDef fRead_Land2State(void)
{
	sLand2_State_TypeDef sTmp;
	if( DT3102Signal.Signal.GC2EventSignal == GetIo(LAND2_SIGN) )
		sTmp.Bit.EventFlag = 1;		//��������ظ�2
	else
		sTmp.Bit.EventFlag = 0;
	if( DT3102Signal.Signal.GC2ErrSignal == GetIo(LAND2_ERR) )
		sTmp.Bit.ErrFlag = 1;		//����ظ�2����
	else
		sTmp.Bit.ErrFlag = 0;
	
	return sTmp;
}
//����Ļ2״̬
sLight_State_TypeDef fRead_Light2State(void)
{
	sLight_State_TypeDef sTmp;
	if( DT3102Signal.Signal.LC2EventSignal == GetIo(LIGHT2_SIGN) )
		sTmp.Bit.EventFlag = 1;		//���������Ļ
	else
		sTmp.Bit.EventFlag = 0;
	if( DT3102Signal.Signal.LC2ErrSignal == GetIo(LIGHT2_ERR) )
		sTmp.Bit.ErrFlag = 1;		//�����Ļ����
	else 
		sTmp.Bit.ErrFlag = 0;
	
	return sTmp;
}
//������ʶ����1״̬
sAxisType1_State_TypeDef fRead_AxisType1State(void)
{
	sAxisType1_State_TypeDef sTmp;
	if( DT3102Signal.Signal.TyreEventSignal == GetIo(AXIS_TYPE1_SIGN) )
		sTmp.Bit.EventFlag = 1;		//�����������ʶ����
	else
		sTmp.Bit.EventFlag = 0;
	if( DT3102Signal.Signal.TyreDoubleSignal == GetIo(AXIS_TYPE1_JUDGE) )
		sTmp.Bit.SingleOrBoth = 1;	//�����˫��
	else
		sTmp.Bit.SingleOrBoth = 0;
	if( DT3102Signal.Signal.TyreErrSignal == GetIo(AXIS_TYPE1_ERR) )
		sTmp.Bit.ErrFlag = 1;		//�������ʶ��������
	else
		sTmp.Bit.ErrFlag = 0;
	
	return sTmp;
}

static void fRoadGate_Close(void)
{
	IoHi(ROAD_GATE1);
	IoLo(ROAD_GATE2);
	SysTimeDly(ROAD_GATE_DLY_TIME);
	IoHi(ROAD_GATE2);
}
static void fRoadGate_Open(void)
{
	IoHi(ROAD_GATE2);
	IoLo(ROAD_GATE1);
	SysTimeDly(ROAD_GATE_DLY_TIME);
	IoHi(ROAD_GATE1);
}
/***********************************************************************
 * ������ƺ���
 * �������ƣ�fPeripheral_Control
 * �������ܣ��������裬���ؿ��ƽ��
 * ������	  STATUS status:����ֵ(0�����̵����̵ƣ���բ����;1�����̵�����ƣ���բ�ر�) 
 *  		  uint8 type��1�����ƺ��̵�;2�����Ƶ�բ
 * ����ֵ��  s8 �������Ƿ�ɹ�(1���ɹ���-1��ʧ��)
 * ��ע:
   1.1��·��  �̵� �ߵ�ƽ���� ��� �͵�ƽ����
   1.0��·��  �̵� �͵�ƽ���� ��� �ߵ�ƽ����
 ***********************************************************************/
s8 fPeripheral_Control(u8 type , u8 status)
{
	if( TrafficSignal == type )				//���ƺ��̵�
	{
		if( 0 == status )		//�̵���
			IoHi(TRAFFIC_LIGHT);
		else					//�����
			IoLo(TRAFFIC_LIGHT);
		return 1;
	}
	else if( BarrierGate == type )		//���Ƶ�բ
	{
		if( 0 == status )		//������բ
			fRoadGate_Open();
		else if ( 1 == status )	//�رյ�բ
			fRoadGate_Close();
		return 1;
	}
	return 0;
}

//ROAD_GATE2_PIN ��բ��
//ROAD_GATE1_PIN ��բ��
void BarrierGate_JDQ_Control(u8 port, u8 status)
{
	if(port == BarrierGateOpenPort)
	{
		if(status == PortNormalOpen) 
			IoHi(ROAD_GATE1);
		else if(status == PortNormalClose) 
			IoLo(ROAD_GATE1);
	}
	else if(port == BarrierGateClosePort)
	{
		if(status == PortNormalOpen) 
			IoHi(ROAD_GATE2);
		else if(status == PortNormalClose) 
			IoLo(ROAD_GATE2);
	}
}


