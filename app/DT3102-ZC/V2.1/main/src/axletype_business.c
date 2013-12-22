#include "includes.h"
#include "dt3102_io.h"
#include "axletype_business.h"
#include "task_msg.h"
#include "app_msg.h"

#define MAX_TYRE_NUM 16		//���г���,������2����
#define MAX_TYRE_MOD 15		//MAX_TYRE_NUM - 1

//===========================================================================================
//��̥��ѭ������
static TyreDevice TyreQueue[MAX_TYRE_NUM] = {0};
static u32 TyreFront = 0;  //��ͷָ��
static u32 TyreRear = 0;   //��βָ��

void TyreQueue_Put(TyreDevice *data)
{
	memcpy(&TyreQueue[TyreRear&MAX_TYRE_MOD], data, sizeof(TyreDevice));
	TyreRear++;
	
 	if(TyreRear - TyreFront > MAX_TYRE_NUM) TyreFront++;
}

int TyreQueue_Get(TyreDevice *data)
{
	if(TyreRear == TyreFront) return -1;

	memcpy(data,&TyreQueue [TyreFront&MAX_TYRE_MOD],sizeof(TyreDevice));
	TyreFront++;

	return 0;
}

u32 TyreQueue_len(void)
{
	return (TyreRear - TyreFront);
}

int TyreQueue_Get_Head(TyreDevice *data)
{
  	if(TyreRear == TyreFront) return -1;
		
	memcpy(data,&TyreQueue[TyreFront&MAX_TYRE_MOD],sizeof(TyreDevice));

	return 0;
}

int TyreQueue_Get_Rear(TyreDevice *data)
{
	int tmp;
	
	if(TyreRear == TyreFront) return -1;

	tmp = TyreRear&MAX_TYRE_MOD;
	if(tmp > 0) tmp -= 1;
		
	memcpy(data,&TyreQueue[tmp],sizeof(TyreDevice));

	return 0;
}

void TyreQueue_Init(void)
{
	TyreRear = 0;
	TyreFront = 0;
}

//===================================================================================
//��̥ʶ�����ϴ��ź�
static volatile char gTyreEvent = 0;		//��̥ʶ���������ź�
static volatile char gTyreDouble = 0;		//��̥ʶ������˫���ź�

char Get_TyreDevice_Event(void)
{
	return gTyreEvent;
}

char Get_TyreDevice_bDouble(void)
{
	return gTyreDouble;
}



//��������Ϣ��ӡ
void Car_Business_WBAxle(void)
{
	TyreDevice device = {0};

	if(TyreQueue_Get_Rear(&device) == 0)
	{
		debug(Debug_Business, "WBAxle Interval=%d, CntPulse=%d, bDouble:%d\r\n", 
			device.nCntInterval, device.nCntPulse, device.bDouble);
	}
}


void AxleType_Business(sAxisType1_State_TypeDef *status)
{
	static BOOL UpFlag = FALSE, TyreValid = FALSE;
	static TyreDevice dTyre = {0};
	static char TyreTypeJump = 0;
// 	TaskMsg msg_info = {0};

	//��¼��̥ʶ������˫���ź����ص�ƽ
	//if(gTyreDouble != status->Bit.SingleOrBoth)
	//{
	//	if(status->Bit.SingleOrBoth == 1) TyreTypeJump = 1;
	//}

	gTyreEvent = status->Bit.EventFlag;
	gTyreDouble = status->Bit.SingleOrBoth;

	//��¼��̥���ʱ��
	if(!UpFlag)
	{
		if(TyreQueue_len() > 0)
		{
			if(TyreQueue [TyreRear&MAX_TYRE_MOD - 1].nCntInterval < 360000000)
				TyreQueue [TyreRear&MAX_TYRE_MOD - 1].nCntInterval++;
		}
	}
	
	//ʶ����������
	if(status->Bit.EventFlag == 1)	
	{
		if(!UpFlag)
		{
			UpFlag = TRUE;
			TyreTypeJump = 0;
			memset(&dTyre, 0, sizeof(TyreDevice));
		}
	}
	else
	{
		//��̥�뿪
		if(UpFlag) 
		{
			UpFlag = FALSE;
			TyreValid = TRUE;

			if(status->Bit.SingleOrBoth == 1)
			{
				TyreTypeJump = 1;
			}			
		}
	}

	if(UpFlag) 
	{
		//��¼ѹ����̥ʶ�����ϵ�ʱ��
		if(dTyre.nCntPulse < 100000000)
			dTyre.nCntPulse++;
	}

	//��̥����Ч, ��������
	if(TyreValid)
	{
		dTyre.bDouble = TyreTypeJump;
		TyreTypeJump = 0;
		TyreQueue_Put(&dTyre);
		TyreValid = FALSE;
//		debug(Debug_Business, "TyreQueue enqueue!\r\n");
// 		msg_info.msg_id = Msg_WBAxle;
// 		Task_QPost(&BusinessMsgQ, &msg_info);
	}
}	


