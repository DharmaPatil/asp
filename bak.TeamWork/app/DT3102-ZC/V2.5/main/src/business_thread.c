/* 
 * File:    business_thread.c
 * Brief:   ҵ�����߳�
 *
 * History:
 * 1. 2012-11-16 �����ļ�, ����п�������Ŀ��ֲ	    river
 * 2. 2012-11-28 ɾ��SysBusinessArray����,Ч�ʲ���  river
 * 3. 2012-12-4  �޸ĳ����ֶγ���ģʽ 				river
 * 4. 2013-1-9   ���̴�ӡ��Ϣ���¼�����֮���ӡ
 * 5. 2013-1-16  ���ӳ�������Ԫ��sdk��Ӧ��־�Ͷ���ѭ��
 * 6. 2013-3-12  ���ӵ�բ�Զ�����, �ӳ������ɳ�����ֲ
 */
#include "includes.h"
#include "task_msg.h"
#include "app_msg.h"
#include "task_timer.h"
#include "car_queue.h"
#include "overtime_queue.h"
#include "car_business.h"
#include "axle_business.h"
#include "sys_config.h"
#include "weight_param.h"
#include "overtime_queue.h"
#include "axle_business.h"
#include "axletype_business.h"
#include "device_info.h"
#include "wet_algorim.h"
#include "io.h"
#include "softdog.h"
#include "scszc_debug_info.h"
#include "barriergate_ctl.h"
#include "speed_check.h"
#include "debug_info.h"

#define PAY_TIME_OUT_TICK	10000

static volatile int nPayTimeOutTick = 10000;			//������ʼ��Ϊ����ֵ
static volatile int nPayTimeOutThreshold = 10000;

//�Ŷ�ʱ�临λ
void ResetPayTimeOutTick(void)
{
	nPayTimeOutTick = 0;
}

void SetPayTimeOutThreshold(int tick)
{
	nPayTimeOutThreshold = tick;
}

//�ŶӼ�ʱ
void Car_Confirm_Wet(CarInfo *pCar);
void TickPayTimeOut(void)
{
	CarInfo *pCar;

	if(nPayTimeOutTick < nPayTimeOutThreshold)
	{
		nPayTimeOutTick += CAR_TICK;
		if(nPayTimeOutTick == nPayTimeOutThreshold)
		{
			//���߳����У�Ҫֱ�Ӵ������ܷ���Ϣ���������Ϣ���ܲ����Ŷ����
			//�п��ܸշ�����ϢȻ����ClosePayTimeOut()������ClosePayTimeOut()ʧЧ
			//msg.msg_id = Msg_Pay_TimeOut;
			//Task_QPost(&BusinessMsgQ, &msg);

			if(Get_FollowCarMode() == DevideAtOnce) return;	//�ϳӷֳ�ģʽ����Ӧ�û���

			//Ѱ��Ӧ�ô��������ĳ���
			pCar = CarQueue_Get_InScaler_FirstCar();

			//���û�г��ڳ��У��Ƴ�2����
			if(pCar == NULL) nPayTimeOutTick -= 200;

			//ȷ�ϲ��ϴ���������
			Car_Confirm_Wet(pCar);
		}
	}
}

//�رսɷѳ�ʱ
void ClosePayTimeOut(void)
{
	nPayTimeOutTick = PAY_TIME_OUT_TICK;
	nPayTimeOutThreshold = PAY_TIME_OUT_TICK;
}

//�Ƿ񵽴��Ŷ�ʱ��
BOOL IsPayTimeOut(void)
{
	if(nPayTimeOutTick < PAY_TIME_OUT_TICK) return FALSE;

	return TRUE;
}



#define AT_TURN_TICK	1000
int nTurnTick = 1000;	//������ʼ��Ϊ��

//�Ŷ�ʱ�临λ
void ResetTurnTick(void)
{
	nTurnTick = 0;
}

//�ŶӼ�ʱ
void TickTurn(void)
{
	TaskMsg msg = {0};

	if(nTurnTick < AT_TURN_TICK)
	{
		nTurnTick += CAR_TICK;
		if(nTurnTick == AT_TURN_TICK)
		{
			msg.msg_id = Msg_At_Turn;
			Task_QPost(&BusinessMsgQ, &msg);
		}
	}
}

//�Ƿ񵽴��Ŷ�ʱ��
BOOL IsAtTurn(void)
{
	if(nTurnTick < AT_TURN_TICK) return FALSE;

	return TRUE;
}

#define DOWN_ALEX_TIME_GATE	200
int nDownAlexTick = 0;
void ResetDownAlexTick(void)
{
	nDownAlexTick = 0;
}

void TickDownAlex(void)
{
	if(nDownAlexTick <= DOWN_ALEX_TIME_GATE)
	{
		nDownAlexTick++;
	}
}

static void Car_Begin_Comming(CarInfo *pCar, int8 cmd)
{
	if(pCar == NULL) return;

	pCar->cmd = cmd;
	pCar->nScalerKgComing = Get_Static_Weight();
	pCar->nScalerKgDyncComing = Get_Dynamic_Weight();

	//�����ϳӵ��ж�ֻ���ǳ�̨����
	if((pCar->nScalerKgComing < SCALER_MIN_KG) || (pCar->nScalerKgDyncComing < SCALER_MIN_KG))
	{
		debug(Debug_Business, "��������, ��̨����=%d\r\n", Get_Dynamic_Weight());
	}
	else
	{
		debug(Debug_Business, "��������, ��̨����=%d\r\n", pCar->nScalerKgComing);
	}
	FSM_Change_CarState(pCar, stCarComing);
}

static void More_Car_Alarm(void)
{
	return;
}

static void Create_New_Car(int8 cmd) 
{
	CarInfo *newCar = NULL;

	//�³����
	newCar = CarQueue_Create_Car();
	debug(Debug_Business, "**��������\r\n");
	if(newCar != NULL) 
	{
		Car_Begin_Comming(newCar, cmd);
		CarQueue_Add_Tail(newCar);
	} 
	else 
	{
		debug(Debug_Error, "����: ����������!\r\n");
	}
}

void FSM_CarFunction(CarInfo *pCar, TaskMsg *pMsg)
{
	if((pCar == NULL) || (pMsg == NULL))
		return;

	if(pCar->bCarIsDead == TRUE)
		return;
		
	if(pCar->CarFSMFun != NULL)
		pCar->CarFSMFun(pCar, pMsg);
}


/**************************��ʱ�¼�����****************************/
//�������г�����Ӧ��ʱ�¼�����
static void Car_Business_Timer(TaskMsg *msg)
{
	CarInfo *iter;

	iter = CarQueue_Get_Head();

	while(iter != NULL) 
	{
		FSM_CarFunction(iter, msg);
		iter = iter->pNext;
	}	
}

/**************״̬��ϵͳ����******************/
extern int IsAlexHold(void);
extern int Car_Confirm(CarInfo *pCar);

void Sys_Business(TaskMsg *msg)
{
	CarInfo *pCar = NULL;
	AlexRecoder *tmpAxle = NULL;
	static long lc_tick = 0;

	switch(msg->msg_id)
	{
		//ǰ�ظд���
		case Msg_GC1_Enter:
			debug(Debug_Business, ">>�����Ļ����, %ldms\r\n", Get_Sys_Tick());
			msg->msg_id = Msg_LC2_Enter;
		
		//��Ļ�����¼�
		case Msg_LC2_Enter:	
			//��¼��Ļ����ʱ�䣬����Ļ�ͷ�ʱ�����ڵ�ʱ��
			lc_tick = Get_Sys_Tick();
			
			//�����ȴ���ǰ��������������Ϣ
			pCar = CarQueue_Get_Tail();
			FSM_CarFunction(pCar, msg);

			//һ�²�������ᣬ������߳����У��п����㷨�̼߳����ź��ѷ������Ŷӣ���Ҳ�����ͷ�״̬��
			//�Ὣ���һ�������źŶ�ʧ���������У���ʱ��Ī������ᣬ����ɼ������η���Ҳ��������
			//if(IsAlexHold() == 0)
			//{
			//	Clear_Axle_Recoder();
			//}

			//��λ��̥ʶ����
			TyreQueue_Init();
		
			//��������
			Create_New_Car(NormalCarCmd);

			//������ʶ��λ
			//LongCarReset();

			//�೵����
			More_Car_Alarm();
			break;

		//�ϳӶ��ᴥ��
		case Msg_PreAlex_Come:
			pCar = CarQueue_Get_Tail();
			if(pCar == NULL) break;
			
			//���һ������һ������ʱ�����ϵͳ���¼
			//������Ҫֻ��ע�˽���������ʱ�����Щ���ӣ��ڵ������źŻ����⴦��
			if(pCar->nAlexMsgCount == 0)
			{
				//���Զ�ǰһ������������ȷ�ϣ�����ȷ�Ϻ��������Ƿ��ظ���
				Car_Confirm(pCar->pPrev);

				//��λ���¼
				Clear_Axle_Recoder();
			}
			
			//��������������Ϣ
			FSM_CarFunction(pCar, msg);

			break;

		//�Ӽ����¼�
		case Msg_PreAlex_Add:
		case Msg_PreAlex_Remove:
			
			//��ȡ����Ϣ
			tmpAxle = (AlexRecoder *)msg->msg_ctx;
			if(tmpAxle == NULL) break;
			
			//�����һ������ʼ
			pCar = CarQueue_Get_Tail();

			if(pCar == NULL) break;	//����û�оͲ�Ҫ�������ź���

			//������������Ϊ��ʱ�����ź���ǰ����
			if(pCar->nAlexMsgCount == 0)
			{
				//����ʱ������ǰ���ź�
				if(msg->msg_id == Msg_PreAlex_Add)
				{
					//��������ź�����һ����©���ģ��л�����һ����
					pCar = pCar->pPrev;
					debug(Debug_Business, "--ǰ������\r\n");

					if(pCar == NULL) return;
				}
				else
				{
					//����ʱ�����Ǻ��˹�Ļ����Զ������ʱû������Ļ��������û����
					//����������Ϣ
					pCar->nAlexMsgCount++;
					Clear_Axle_Recoder();
					//add_by_StreakMCU	2013-8-25
					//��ֹ����������ʻ�ҳ� 
					barriergate_open();
					//end_add_by_StreakMCU
				}
			}

			//�˳����ź�����Ϣ�������ۺ��ж��ﻹ��һ�ι���
			if(pCar->nAxleNum > 0)
			{
				if(tmpAxle->AlexKgRef*10 < pCar->AxleInfo[pCar->nAxleNum-1].axle_peak)
				{
					debug(Debug_Business, "\r\n>�����˳�cur:%d,ref:%d\r\n", tmpAxle->AlexKgRef, pCar->AxleInfo[pCar->nAxleNum-1].axle_peak);
						
					//�������Ϣ
					msg->msg_id = Msg_None;
					break;
				}
			}

			//��������Ϣ
			Save_Axle_Info(tmpAxle);
			
			//����������������
			FSM_CarFunction(pCar, msg);
			debug(Debug_Business, "**��%d��,����:%d,���ֵ:%d,̥��:%s\r\n", pCar->nAxleNum, tmpAxle->AlexKg, tmpAxle->AlexKgRef, Get_Car_TryeType(tmpAxle->bDouble));
			debug(Debug_Business, "**����:%d\r\n", Get_CarSpeed());
			//����������һ���������һ�����Ѳ��ϣ�����ȷ�����ź���
			if(pCar != CarQueue_Get_Tail())
			{
				Car_Confirm(pCar);
			}

			break;
		
		//ǰ�ظ��ͷ�
		case Msg_GC1_Leave:
			debug(Debug_Business, ">>�����Ļ�ͷ�, %ldms\r\n", Get_Sys_Tick());
			msg->msg_id = Msg_LC2_Leave;	
			
		//��Ļ�ͷ��¼�
		case Msg_LC2_Leave:
		case Msg_Cmd_Force:			
			//�ص�բ
			BarrierGateAction_After_LC2DownOff();
			
			pCar = CarQueue_Get_Tail();
			if(pCar == NULL)
			{
				if(msg->msg_id == Msg_Cmd_Force) AbnormalAckInfo_To_SDK(ForceCarCmd, 0);
				break;
			}

			//���������
			if(msg->msg_id == Msg_Cmd_Force)
			{
				pCar->cmd = ForceCarCmd;
				//�����Ѿ���β
				if(pCar->nStatus > stCarComing)
				{
					AbnormalAckInfo_To_SDK(ForceCarCmd, pCar->CutOffState);
					break;
				}
				msg->msg_id = Msg_LC2_Leave;
			}
			
			if(Get_Sys_Tick() - lc_tick < 100) 
			{
				CarQueue_Car_Kill(pCar);
				debug(Debug_Business, "��Ļ�ڵ�ʱ��<100ms, ����!\r\n");
				break;
			}
			FSM_CarFunction(pCar, msg);
			break;
			
		case Msg_At_Turn:
		case Msg_BackAlex_Come:	
		case Msg_GC2_Enter:
		case Msg_GC2_Leave:
			if(Get_FollowCarMode() == DevideAtOnce)
			{
				//��ģʽ�Ǻ��Ժ�ظ����³���ʶ������Ϣ
				break;
			}
			else
			{
				//��Ϊ���ܴ�����ӽɷѣ���ظ����շ�ͤԶ��֮��������ͣ��һ����
				pCar = CarQueue_Get_OnScaler_FirstCar();
				if(pCar == NULL) break;
				FSM_CarFunction(pCar, msg);
			}
			break;

		case Msg_BackAlex_Add:
			ResetDownAlexTick();
			//������һ��case
		case Msg_BackAlex_Remove:

			if(Get_FollowCarMode() == DevideAtOnce) pCar = NULL;
			else pCar = CarQueue_Get_OnScaler_FirstCar();

			tmpAxle = (AlexRecoder *)msg->msg_ctx;

			//��ֹ��������Ϊ����
			if((msg->msg_id == Msg_BackAlex_Remove) && (nDownAlexTick < DOWN_ALEX_TIME_GATE))
			{
				msg->msg_id = Msg_BackAlex_Add;
				debug(Debug_Business, "��������������ǿ������Ϊ�������\r\n");
			}

			//�˳����ź�����Ϣ
			if((pCar != NULL) && (tmpAxle!= NULL))
			{				
				//֮ǰ�Ѿ����������Ϣ��
				if(tmpAxle->AlexKgRef*10 < pCar->nDownAlexMaxKg)
				{
					debug(Debug_Business, "\r\n>�³������˳�cur:%d,ref:%d\r\n", tmpAxle->AlexKgRef, pCar->nDownAlexMaxKg);
						
					//�������Ϣ
					msg->msg_id = Msg_None;
					break;
				}
			}

			//�ַ���Ϣ
			if(Get_FollowCarMode() == DevideAtOnce)
			{
				//��ģʽ�Ǻ��Ժ�ظ����³���ʶ������Ϣ
				break;
			}
			else
			{
				//��Ϊ���ܴ�����ӽɷѣ���ظ����շ�ͤԶ��֮��������ͣ��һ����
				pCar = CarQueue_Get_OnScaler_FirstCar();
				if(pCar == NULL) break;
				FSM_CarFunction(pCar, msg);
			}
			break;
		
		case Msg_Cmd_PlusSave:
			pCar = CarQueue_Get_Tail();
			if(pCar == NULL)
			{
				AbnormalAckInfo_To_SDK(LongCarCmd, 0);
				break;
			}

			FSM_CarFunction(pCar, msg);	
			break;

		case Msg_Cmd_PayNotify:
			pCar = CarQueue_Get_Head();
			if(pCar != NULL)
			{
				if(Get_FollowCarMode() == DevideAtOnce)
				{
					//CarQueue_Car_Kill(pCar);
					FSM_CarFunction(pCar, msg);
				}
				
				//���ദ��,��ֹ�����쳣
				else if(pCar->nStatus == stCarComing) 
				{
					if(pCar->pNext != NULL)
					{
						FSM_CarFunction(pCar->pNext, msg);
						CarQueue_Car_Kill(pCar);
					}	
				} 
				else 
				{
					FSM_CarFunction(pCar, msg);
				}
			}
			
			break;

		case Msg_Cmd_Repeat:
			pCar = CarQueue_Get_Head();		
			Car_Business_RepeatCmd(pCar, msg);
			break;
			
		case Msg_Tick:			
			//��ʱ��ʱ����
			TickTurn();

			//�ɷѳ�ʱ����
			TickPayTimeOut();

			//����ʶ������ʱ
			TickDownAlex();

			//���ó�����ʶ����
			if(Get_LC_Status(2) == TRUE)
			{
				LongCarRecognize(Get_Static_Weight());
			}
			
			SysTimer_Business();
			Car_Business_Timer(msg);
			break;

		case Msg_WBAxle:
			break;
			
		default:
			break;
	}
		
	CarQueue_Remove_Dead();
}

static int B_Handle;
void Business_Thread(void *arg)
{
	TaskMsg event_msg = {0};
	uint8 err;

	BarrierGate_Reset_Init();
	B_Handle = Register_SoftDog(300, SOFTDOG_RUN, "ҵ���߳�");

	while(1) 
	{
		err = Task_QPend(&BusinessMsgQ, &event_msg, sizeof(TaskMsg), 0);	
		if(err == SYS_ERR_NONE) 
		{			
			if(B_Handle >= 0) SoftDog_Feed(B_Handle);
			
			//�ճӶ�ʱ������
			if(event_msg.msg_id != Msg_Tick)
			{
				Clear_ScalerEmpty_Count();
			}
			
			//��ӡ�¼���Ϣ
			MsgInfo_Print(&event_msg);

			//������
		 	Sys_Business(&event_msg);

			//ĳЩ�¼�����ӡ����������Ϣ
			if((event_msg.msg_id == Msg_Tick)
				|| (event_msg.msg_id == Msg_None) 
				|| (event_msg.msg_id == Msg_GC1_Enter) 
				|| (event_msg.msg_id == Msg_GC1_Leave)				
				|| (event_msg.msg_id == Msg_PreAlex_Come)
				|| (event_msg.msg_id == Msg_PreAlex_Add)
				|| (event_msg.msg_id == Msg_PreAlex_Remove)
				|| (event_msg.msg_id == Msg_BackAlex_Come)
				|| (event_msg.msg_id == Msg_BackAlex_Add)
				|| (event_msg.msg_id == Msg_BackAlex_Remove)) 
			{
				continue;
			}
			
			//��ӡ����������Ϣ
			CarQueue_Print_Info();
		}
	}
}

