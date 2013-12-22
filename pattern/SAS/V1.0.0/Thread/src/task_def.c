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
#include "overtime_queue.h"
#include "dt3102_io.h"
#include "sys_param.h"
#include "softdog.h"

//����1
#include "usart1.h"
#include "usart_cfg.h"

//static HANDLE dog_handle;

struct _ThreadArray
{
	void (*ThreadFun)(void *arg);
	void *arg;
	SYS_STK *pstack;
	int8 prio;
};


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
__align(8) static SYS_STK WatchDogThreadStack[WatchDogStackSize];
__align(8) static SYS_STK KeyThreadStack[KeyStackSize];
__align(8) static SYS_STK BusinessThreadStack[BusinessStackSize];
__align(8) static SYS_STK AlgorimThreadStack[AlgorimStackSize];
__align(8) static SYS_STK SerialRecvThreadStack[SerialRecvStackSize];
__align(8) static SYS_STK DataManagerThreadStack[DataManagerStackSize];
__align(8) static SYS_STK USBThreadStack[USBStackSize];
__align(8) static SYS_STK GUIhreadStack[GUIStackSize];
__align(8) static SYS_STK NetThreadStack[NetStackSize];
__align(8) static SYS_STK NetSendStack[NetSendStackSize];
__align(8) static SYS_STK DebugThreadStack[DebugStackSize];
__align(8) static SYS_STK StartThreadStack[StartStackSize];

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

//��������
static const struct _ThreadArray ThreadArray[] = {
	{Start_Thread,				NULL, 	&StartThreadStack[StartStackSize-1],			StartPrio},
	{Key_Thread,				NULL, 	&KeyThreadStack[KeyStackSize-1], 				KeyPrio},
	{Business_Thread, 			NULL, 	&BusinessThreadStack[BusinessStackSize-1],		BusinessPrio},
	{Algorim_Thread, 			NULL,	&AlgorimThreadStack[AlgorimStackSize-1],		AlgorimPrio},
	{SerialRecv_Thread, 		NULL, 	&SerialRecvThreadStack[SerialRecvStackSize-1],	SerialRecvPrio},
	{DataManager_Thread,		NULL, 	&DataManagerThreadStack[DataManagerStackSize-1],DataManagerPrio},
	{USB_Thread, 				NULL, 	&USBThreadStack[USBStackSize-1],				USBPrio},
	{GUI_Thread, 				NULL, 	&GUIhreadStack[GUIStackSize-1],					GUIPrio},
	{Net_Business_Thread,   	NULL, 	&NetThreadStack[NetStackSize-1],			    NetPrio},
	{Net_Send_Thread, 			NULL, 	&NetSendStack[NetSendStackSize-1],				NetSendPrio},
	{Debug_Thread,				NULL, 	&DebugThreadStack[DebugStackSize-1],			DebugPrio},
	{WatchDog_Thread, 			NULL, 	&WatchDogThreadStack[WatchDogStackSize-1],		WatchDogPrio},
};

static void WatchDog_Thread(void *arg)
{
	static BOOL led_flag = FALSE;
	static int W_Handle;

	W_Handle = Register_SoftDog(200, SOFTDOG_RUN, "�̵߳���");
	
 	while(1) 
 	{
		SysTimeDly(50);			//��ʱ500ms Ӳ�����Ź�ʱ�� 1s ~ 2.25s
		
		//������Ź�ι��
  		if(W_Handle >= 0) SoftDog_Feed(W_Handle);
		
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
}

static void Start_Thread(void *arg)
{
	uint8 task_status = 0;
	int32 i = 0;

	Task_Timer_Create();		//������ʱ������	
	
	//������Ź���ʼ��
	if(SoftDog_Init() == 0) 
	{
		debug(Debug_Error, "WatchDog init err\r\n");
		while(1);
	}
	
	//������������
	for(i = 1; i < sizeof(ThreadArray)/sizeof(ThreadArray[0]); i++) 
	{	
		task_status |= SysTaskCreate(ThreadArray[i].ThreadFun, ThreadArray[i].arg,		
										ThreadArray[i].pstack, ThreadArray[i].prio);

		if(task_status != SYS_NO_ERR) break;
	}
										
	if(task_status != SYS_NO_ERR) 
	{
		debug(Debug_None, "�̴߳���ʧ��,id:%d\n", i);
		while(1);						//���񴴽�ʧ��	
	}

	//������ʱ�� 
	Task_Timer_Start();
	
	debug(Debug_None, "�߳̿�ʼ����!\r\n");	
	SysTaskDel(StartPrio);
}	

//���峤��22*1024ʱ������;21ʱ������
//__align(8) char deadstack[1024 * 21] = {0};
int main(void)
{
	SetAppRunAddr(0x8000000);					//������ת��ַ����ͬʱ�޸�IROM1��ַ	
	System_Init();								//ϵͳ��ʼ��
	Bsp_Init();									//������ʼ��
	App_Msg_Init();	
	fLED_IO_Init();	
	OverQueue_Init();		
	Debug_Init();
	
	//�������Ź�����/��������
	SysTaskCreate(ThreadArray[0].ThreadFun, ThreadArray[0].arg,	
					ThreadArray[0].pstack,	 ThreadArray[0].prio);	

	printf("�߳̿�ʼ����!\r\n");	
	SysStart();	//��ʼ�������
	return 0;
}		

	
