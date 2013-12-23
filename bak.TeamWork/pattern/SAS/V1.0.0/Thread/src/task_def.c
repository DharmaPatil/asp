/*
 * File:   task_def.c
 * Author: river
 * Brief:  ���������������߳�

 * History:
 * 2012-11-14 �����ļ��������߳�;		river
 * 2012-11-29 ���������߳�, ����ҵ�����	river
 * 2013-1-7   ����test�߳�,�����Ź����ȼ���Ϊ���;ԭ���Ź��̳߳�ʼ����ɾ��
 */

#include "includes.h"
#include "driver.h"

#include "system_init.h"				//System_Init
#include "bsp_init.h"					//Bsp_Init
#include "task_msg.h"
#include "task_timer.h"
#include "sys_config.h"
#include "task_def.h"
#include "dog_init.h"
#include "car_business.h"
#include "led.h"
#include "dt3102_io.h"
#include "sys_param.h"
#include "softdog.h"
#include "ccm_mem.h"

//����1
#include "usart1.h"
#include "usart_cfg.h"

#include "net_rmcpupdate_business.h"
#include "net_business_thread.h"


//�����ջ��С
enum {
	WatchDogStackSize		= 128,
	KeyStackSize			= 160,	//256,
	BusinessStackSize		= 512,
	AlgorimStackSize		= 800,	//1024,
	SerialRecvStackSize  	= 256,	//640,
	DataManagerStackSize	= 256,	//320,
	USBStackSize			= 512,  //128
	GUIStackSize			= 512,  //128
	NetStackSize			= 512,  //256
	NetSendStackSize		= 256,
	DebugStackSize			= 256,
	StartStackSize			= 128,
};

//�����ջ
// __align(8) static SYS_STK WatchDogThreadStack[WatchDogStackSize];
// __align(8) static SYS_STK KeyThreadStack[KeyStackSize];
// __align(8) static SYS_STK BusinessThreadStack[BusinessStackSize];
// __align(8) static SYS_STK AlgorimThreadStack[AlgorimStackSize];
// __align(8) static SYS_STK SerialRecvThreadStack[SerialRecvStackSize];
// __align(8) static SYS_STK DataManagerThreadStack[DataManagerStackSize];
// __align(8) static SYS_STK USBThreadStack[USBStackSize];
// __align(8) static SYS_STK GUIhreadStack[GUIStackSize];
// __align(8) static SYS_STK NetThreadStack[NetStackSize];
// __align(8) static SYS_STK NetSendStack[NetSendStackSize];
// __align(8) static SYS_STK DebugThreadStack[DebugStackSize];
// __align(8) static SYS_STK StartThreadStack[StartStackSize];

static SYS_STK *WatchDogStackBos	= NULL;
static SYS_STK *KeyStackBos			= NULL;
static SYS_STK *BusinessStackBos	= NULL;
static SYS_STK *AlgorimStackBos		= NULL;
static SYS_STK *SerialRecvStackBos	= NULL;
static SYS_STK *DataManagerStackBos	= NULL;
static SYS_STK *USBStackBos			= NULL;
static SYS_STK *GUIStackBos		    = NULL;
static SYS_STK *NetStackBos			= NULL;
static SYS_STK *NetSendStackBos		= NULL;
static SYS_STK *DebugStackBos  		= NULL;
static SYS_STK *StartStackBos  		= NULL;

//�������
static void WatchDog_Thread(void *arg);		 	//���Ź�����
extern void Debug_Thread(void *arg);			//���ڵ�������
extern void Key_Thread(void *arg);				//��������
extern void Business_Thread(void *arg);			//ҵ��������
extern void Algorim_Thread(void *arg);			//�����㷨����
extern void SerialRecv_Thread(void *arg);		//����2��������
extern void DataManager_Thread(void *arg);		//ҵ�����ݹ�������
extern void USB_Thread(void *arg);				//u�̹�������
extern void GUI_Thread(void *arg);				//GUI����
extern void Net_Business_Thread(void *arg);		//��������
extern void Net_Send_Thread(void *arg);
static void Start_Thread(void *arg);			//�����߳�

static void WatchDog_Thread(void *arg)
{
	static BOOL led_flag = FALSE;
	static int W_Handle;
	uint8 err;
	TaskMsg msg = {0};	

	W_Handle = Register_SoftDog(400, SOFTDOG_RUN, "�̵߳���");

	//��������
	while(1) 
	{
		err = Task_QPend(&NetUpdateMsgQ, &msg, sizeof(TaskMsg), 50);		
		if(err == SYS_ERR_NONE) 
		{
			Net_Rmcp_Update(msg.msg_id, (unsigned char *)msg.msg_ctx, (int)msg.msg_len);
		}
        else if(err == SYS_ERR_TIMEOUT)
        {
		    //��ˮ��,��ʾϵͳ�Ƿ񵱻�
		    if(led_flag) 
		    {
			    LED1_OFF;
			    LED2_ON;
		    } 
		    else 
		    {
			    LED1_ON;
			    LED2_OFF;
		    }
		    led_flag = !led_flag;		
        }
        
        //������Ź�ι��
  		if(W_Handle >= 0) SoftDog_Feed(W_Handle);
	}
}

static void Start_Thread(void *arg)
{
	uint8 task_status = 0;
	
	printf(" >>�߳̿�ʼ����!\r\n");	

	Task_Timer_Create();		//������ʱ������	
	
	//������Ź���ʼ��
	if(SoftDog_Init() == 0) 
	{
		debug(Debug_Error, "WatchDog init err\r\n");
		while(1);
	}
	
	task_status |= SysTaskCreate(Key_Thread,		  NULL, KeyStackBos + KeyStackSize - 1,                 KeyPrio);
	task_status |= SysTaskCreate(Business_Thread, 	  NULL, BusinessStackBos + BusinessStackSize - 1,	    BusinessPrio);
	task_status |= SysTaskCreate(Algorim_Thread, 	  NULL, AlgorimStackBos + AlgorimStackSize - 1,	        AlgorimPrio);
	task_status |= SysTaskCreate(SerialRecv_Thread,   NULL, SerialRecvStackBos + SerialRecvStackSize - 1,	SerialRecvPrio);
	task_status |= SysTaskCreate(DataManager_Thread,  NULL, DataManagerStackBos + DataManagerStackSize - 1, DataManagerPrio);
	task_status |= SysTaskCreate(USB_Thread, 		  NULL, USBStackBos + USBStackSize - 1,			        USBPrio);
	task_status |= SysTaskCreate(GUI_Thread, 		  NULL, GUIStackBos + GUIStackSize - 1,			        GUIPrio);
	task_status |= SysTaskCreate(Net_Business_Thread, NULL, NetStackBos + NetStackSize - 1,			        NetPrio);
	task_status |= SysTaskCreate(Net_Send_Thread, 	  NULL, NetSendStackBos + NetSendStackSize - 1,	        NetSendPrio);
	task_status |= SysTaskCreate(Debug_Thread,		  NULL, DebugStackBos + DebugStackSize - 1,  	        DebugPrio);
	task_status |= SysTaskCreate(WatchDog_Thread, 	  NULL, WatchDogStackBos + WatchDogStackSize - 1,       WatchDogPrio);

	if(task_status != SYS_NO_ERR) 
	{
		debug(Debug_None, "�̴߳���ʧ��");
		while(1);						//���񴴽�ʧ��	
	}

	//������ʱ�� 
	Task_Timer_Start();
	
	debug(Debug_None, "�߳̿�ʼ����!\r\n");	
	SysTaskDel(StartPrio);
}	

/////////////////////////////////////////////////////////////////////////////////
//��CCM memory�������ڴ�
//�������ջջ��ָ��ָ���ַ0x10000000��ʼ����CCM MEMORY�ڴ�����
static void Assign_TaskStack_Buffer(void)
{
	WatchDogStackBos	= (SYS_STK *)Ccm_Malloc(WatchDogStackSize * sizeof(SYS_STK));
	KeyStackBos			= (SYS_STK *)Ccm_Malloc(KeyStackSize * sizeof(SYS_STK));
	BusinessStackBos	= (SYS_STK *)Ccm_Malloc(BusinessStackSize * sizeof(SYS_STK));
	AlgorimStackBos		= (SYS_STK *)Ccm_Malloc(AlgorimStackSize * sizeof(SYS_STK));
	SerialRecvStackBos	= (SYS_STK *)Ccm_Malloc(SerialRecvStackSize * sizeof(SYS_STK));
	DataManagerStackBos	= (SYS_STK *)Ccm_Malloc(DataManagerStackSize * sizeof(SYS_STK));
	USBStackBos			= (SYS_STK *)Ccm_Malloc(USBStackSize * sizeof(SYS_STK));
	GUIStackBos			= (SYS_STK *)Ccm_Malloc(GUIStackSize * sizeof(SYS_STK));
	NetStackBos			= (SYS_STK *)Ccm_Malloc(NetStackSize * sizeof(SYS_STK));
	NetSendStackBos		= (SYS_STK *)Ccm_Malloc(NetSendStackSize * sizeof(SYS_STK));
	DebugStackBos		= (SYS_STK *)Ccm_Malloc(DebugStackSize * sizeof(SYS_STK));
	StartStackBos		= (SYS_STK *)Ccm_Malloc(StartStackSize * sizeof(SYS_STK));
}

//���峤��22*1024ʱ������;21ʱ������
//__align(8) char deadstack[1024 * 21] = {0};
int main(void)
{
	SetAppRunAddr(0x8020000);					//������ת��ַ����ͬʱ�޸�IROM1��ַ	
	System_Init();								//ϵͳ��ʼ��
	Bsp_Init();									//������ʼ��
	App_Msg_Init();	
	fLED_IO_Init();		
	Debug_Init();
	Assign_TaskStack_Buffer();
	
	//�������Ź�����/��������
	SysTaskCreate(Start_Thread, NULL, StartStackBos+StartStackSize-1, StartPrio);	

	SysStart();	//��ʼ�������
	return 0;
}		

	
