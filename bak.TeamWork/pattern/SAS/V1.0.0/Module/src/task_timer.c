/*
 * File:   task_timer.c
 * Author: river
 * Brief:  ϵͳ��ʱ����Ӳ����ʱ���ص�����ʵ��

 * History:
 * 2012-11-19 created
 * 2012-12-24 ��ҵ�������͵�10ms��ʱ��Ϣ�����ӳ�2s;
 */

#include "includes.h"
#include "task_msg.h"
#include "app_msg.h"
#include "driver.h"
#include "timer7.h"
#include "timer5.h"
#include "device_info.h"
#include "overtime_queue.h"
#include "app_rtc.h"
#include "sys_config.h"
#include "guiwindows.h"

#include "sdk_serial.h"
#include "softdog.h"
#include "DT3102_IO.h"
#include "app_timer.h"

#include "net_rmcpupdate_business.h"
#include "serial_protocol.h"

static SYS_TMR *onesecond_tmr;
static HANDLE tim7_dev; 
static HANDLE tim5_dev;

static volatile	unsigned long sys_tick = 0;


//1s sys timer:ÿ��1���ѯ�豸����״̬��Ϣ�Ƿ��б仯������б仯�������2�����̷߳����豸״̬��Ϣ�仯֪ͨ��
//ͬʱÿ��10�������2�����̷߳�������֪ͨ
static void TaskTimer_1s(void *ptmr, void *parg)
{
	static DeviceStatus lastDev={0};	//��һ�ε�����״̬
	static u8 timeCnt=0;	//10s ����
	TaskMsg msg = {0};
	u32 curDev=Get_Device_Status();
	//��ѯ�豸����״̬��Ϣ�Ƿ��б仯
	if((curDev & 0x154) !=(lastDev.value & 0x154))	//1 0101 0100=0x154
	{
		//�й���״̬�仯
		msg.msg_id=Msg_Device_Fault;
		Task_QPost(&DataManagerMsgQ, &msg);
		debug(Debug_Notify,"device fault change!!!\r\n");
	}
	//�������״̬
	lastDev.value=curDev;
	
	//ÿ��10�������2�����̷߳�������֪ͨ
	if(OverQueue_Get_Size() == 0) 
	{
		if(timeCnt++ > 9) 
		{
			timeCnt=0;
			msg.msg_id=Msg_Device_Heart;
			Task_QPost(&DataManagerMsgQ, &msg);
		} 
	} 
	else 
	{
		timeCnt = 0;
	}
}

static u8 OverTick=0;	//���� ����(0-50)

void Reset_OvertimeTick(void)
{
	OverTick = 0;
}

//10ms timer task routine,��ҵ�����̷߳���10ms��ʱ��Ϣ��
//ͬʱ��ѯ�����Ͷ��ж�ͷԪ���Ƿ��ѳ�ʱ����δ���ͣ�Ȼ�������2�����̷߳������ݳ�ʱ��Ϣ��
//��ģ��ÿ��1���ȡ������RTCʱ�ӣ���GUI�̷߳���1�봰��ˢ����Ϣ
static void TaskTimer_1ms(void)
{
	static u8 SencdTick=0;	//�� ����(0-100)		
	static u8 cnt = 0;
	TaskMsg msg = {0};
	OvertimeData *pCar = NULL;

	//Ӧ�ò�ʱ��tick����
	sys_tick++;

	cnt++;
	//10msִ��һ�ε�ISR
	if(cnt >= 10)
	{
		cnt = 0;

		//������Ź��жϴ���
		SoftWatchDog_ISR();
		
		//�ж����Ͷ��ж�ͷԪ���Ƿ��ѳ�ʱ
		pCar = OverQueue_Get_Head();
		if(pCar != NULL)
		{
			//�������ͻ��Ƶ���Ϣ��Ҫ��ʱ�ط�
			if(pCar->SendMod == SendActiveMod)
			{
				if(pCar->bSend == 1) 
				{
					OverTick++;
					//��ʱ�ط�
					if(OverTick >= 50)
					{
						OverTick = 0;
						msg.msg_id = Msg_Overtime;
						Task_QPost(&DataManagerMsgQ, &msg);			
					} 
				}
				else
				{
					msg.msg_id = Msg_Overtime;
					Task_QPost(&DataManagerMsgQ, &msg);
				}
			}
		}
		
		//1s���ȡrtcʱ��
		if(++SencdTick >= 100)
		{
			SencdTick = 0;	
			//����������ʱ����
			NetUpdate_Overtime_Business();
			//PostWindowMsg(g_pCurWindow,WM_TIMEUPDATE, 0, 0);
		}
	}
}

unsigned long Get_Sys_Tick(void)
{
	unsigned long tmp = 0;
#if	SYS_CRITICAL_METHOD == 3
	SYS_CPU_SR cpu_sr = 0;
#endif

	SYS_ENTER_CRITICAL();
	tmp = sys_tick;
	SYS_EXIT_CRITICAL();
	
	return tmp;
}

void Task_Timer_Create(void)
{
	HVL_ERR_CODE err;

	//����1s��ʱ��
	onesecond_tmr = SysTmrCreate(10,10,SYS_TMR_OPT_PERIODIC,TaskTimer_1s,NULL,"one second",&err);
	
	if(err != SYS_ERR_NONE) {
		debug(Debug_Error,"Create 1s sys timer task failed !\r\n");
	}
	//timer7����2ms��ʱ��
	if(TIM7_Config(TaskTimer_2ms, 20, Mode_100US) == 0) {
		debug(Debug_None, "Timer2 init failed\r\n");	 
		while(1);
	}
	//timer5����10ms��ʱ��
	if(TIM5_Config(TaskTimer_1ms,10, Mode_100US) == 0) {
		debug(Debug_None, "Timer2 init failed\r\n");	 
		while(1);
	}
	
	tim7_dev = DeviceOpen(CHAR_TIM7,0,&err);
	if(err!=HVL_NO_ERR)
	{
		debug(Debug_Error,"open timer 7 error\r\n");
		while(1);
	}	
	tim5_dev = DeviceOpen(CHAR_TIM5,0,&err);
	if(err!=HVL_NO_ERR)
	{
		debug(Debug_Error,"open timer 5 error\r\n");
		while(1);
	}
}


void Task_Timer_Start(void)
{
	u8 err;

	SysTmrStart(onesecond_tmr, &err);				//����1s��ʱ��
	if(err!=SYS_ERR_NONE)
	{
		debug(Debug_Error,"start system timer error!!!\r\n");
	}
	if(DeviceIoctl(tim7_dev,TIM7_CMDTimOpen,0)==0)	//����2ms��ʱ��
	{
		debug(Debug_None,"start timer 7 error!!!\r\n");
		//while(1);
	}
	if(DeviceIoctl(tim5_dev,TIM5_CMDTimOpen,0)==0)	//����10ms��ʱ��
	{
		debug(Debug_None,"start timer 5 error!!!\r\n");
		//while(1);
	}
	CSysTime_Init();		//app rtcģ���ʼ��
}	

