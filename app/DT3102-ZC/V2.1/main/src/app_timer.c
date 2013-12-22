#include "app_timer.h"
#include "app_msg.h"
#include "device_info.h"
#include "dt3102_io.h"

typedef struct {
	u8 countTick1:3;	//����������������
	u8 countTick2:3;	//�����ͷŴ�������
	u8 jump:1;	//�����Ļ��һ�ε�״̬ 0-�ͷţ�1-����
}SavedStatus;


//2ms timer task routine
void TaskTimer_2ms(void)
{	
	static SavedStatus LC2_status={0};
	static uint8 cnt = 0;
	TaskMsg msg = {0};
	//��ȡ��һ���豸��Ϣ
	u32 oldDev=Get_Device_Status();
	DeviceStatus newDev={0};
	//������Ϣ����ط�
	sLight_State_TypeDef		lc2_status={0};	//���Ļ
	sLand1_State_TypeDef		gc1_status={0};	//ǰ�ظ�
	sLand2_State_TypeDef 		gc2_status={0};	//��ظ�
	sAxisType1_State_TypeDef 	axle1_status={0};//̥��ʶ����-��˫�֡����ϡ��ź�

	//��ȡ��Ļ���ظС�����ʶ��������״̬
	lc2_status = fRead_Light2State();
	gc1_status = fRead_Land1State();
	gc2_status = fRead_Land2State();
	axle1_status = fRead_AxisType1State();

	newDev.bitValue.LC2_err		= lc2_status.Bit.ErrFlag;
	newDev.bitValue.LC2_status	= lc2_status.Bit.EventFlag;
	newDev.bitValue.GC1_err		= gc1_status.Bit.ErrFlag;
	newDev.bitValue.GC1_status	= gc1_status.Bit.EventFlag;
	newDev.bitValue.GC2_err		= gc2_status.Bit.ErrFlag;
	newDev.bitValue.GC2_status	= gc2_status.Bit.EventFlag;
	newDev.bitValue.tireSen_err	 = axle1_status.Bit.ErrFlag;
	newDev.bitValue.single_double= axle1_status.Bit.SingleOrBoth;
	newDev.bitValue.tireSen_Signal = axle1_status.Bit.EventFlag;
	//����ȫ���豸��Ϣ
	Updata_Device_Status(newDev);
	
	//�Ƚ����ε��豸״̬�仯�����Ͷ�Ӧ��Ϣ����Ļ2/ǰ��1/��ظ�2---�������ͷ�
	//3-lc  5-gc1  7-gc2     ( 1010 1000=0xa8)
	/*------------------lc 2-----------------*/
	if(LC2_status.jump > newDev.bitValue.LC2_status)	//lc�ź������� trigger--release
	{
		if(++LC2_status.countTick1>4)		//����5��(10ms)Ϊ��Ч�ź�
		{
			//����OUT���
			IoLo(OUT4);			
			
			//������Ϣ
			msg.msg_id=Msg_LC2_Leave;
			Task_QPost(&BusinessMsgQ, &msg);
			LC2_status.countTick1=0;
			LC2_status.jump=0;
		}		
	}
	else if(LC2_status.jump < newDev.bitValue.LC2_status)	//lc�ź������� release--trigger
	{
		if(++LC2_status.countTick2>4)		//����5��(10ms)Ϊ��Ч�ź�
		{
			//����OUT���
			IoHi(OUT4);
			
			//������Ϣ
			msg.msg_id=Msg_LC2_Enter;
			Task_QPost(&BusinessMsgQ,&msg);
			LC2_status.countTick2=0;
			LC2_status.jump=1;
		}
	}
	else
	{
		LC2_status.countTick1=LC2_status.countTick2=0;
	}
	/*---------------gc 1----------------*/
	if((oldDev & 0x20) > (newDev.value & 0x20))	//ground 1 sensor release
	{
		msg.msg_id=Msg_GC1_Leave;
		Task_QPost(&BusinessMsgQ,&msg);
	}
	else if((oldDev & 0x20) < (newDev.value & 0x20))	//ground 1 sensor trigger
	{
		msg.msg_id=Msg_GC1_Enter;
		Task_QPost(&BusinessMsgQ,&msg);
	}
	
	/*---------------gc 2----------------*/
	if((oldDev & 0x80) > (newDev.value & 0x80))	//ground 2 sensor release
	{
		msg.msg_id=Msg_GC2_Leave;
		Task_QPost(&BusinessMsgQ,&msg);
	}
	else if((oldDev & 0x80) < (newDev.value & 0x80))	//ground 2 sensor trigger
	{
		msg.msg_id=Msg_GC2_Enter;
		Task_QPost(&BusinessMsgQ,&msg);
	}

	//��ҵ���̷߳���10msʱ��
	if(++cnt >= 5)
	{
		cnt = 0;
		msg.msg_id=Msg_Tick;
		Task_QPost(&BusinessMsgQ, &msg);
	}
#if 0
	//�����ٶȼ���ģ��
	if(Get_System_Mode()<=SCS_ZC_3)
	{
		Check_CarSpeed();
	}
#endif
}

