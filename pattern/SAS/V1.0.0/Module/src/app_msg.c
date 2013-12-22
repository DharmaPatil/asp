#include "includes.h"
#include "task_msg.h"
#include "app_msg.h"
#include "device_info.h"
#include "net_business_thread.h"

#define WaveLen 		4
#define WaveSize		328
#define WavePackLen	20

#define AppNetMsgLen		12
#define AppBusinessMsgLen	32
#define AppDataMsgLen		24

#define MsgQSize sizeof(TaskMsg)

//Ӧ�ò���Ϣ����ȫ�ֱ���
MsgQ NetMsgQ = {0};
MsgQ BusinessMsgQ = {0};
MsgQ DataManagerMsgQ = {0};

//���Ͳ��������ڴ滺���
static  int WaveBuf[WaveLen][WaveSize] = {0};

//��Ϣ�����ڴ滺���
static	int NetMsgQBuf[AppNetMsgLen][MsgQSize] = {0};
static 	int BusinessMsgQBuf[AppBusinessMsgLen][MsgQSize] = {0};
static  int DataMsgQBuf[AppDataMsgLen][MsgQSize] = {0};

//��Ϣ����ָ������
static void*	TaskMsgNetPoint[AppNetMsgLen];	
static void*	TaskMsgBusinessPoint[AppBusinessMsgLen];	 
static void*	TaskMsgDataPoint[AppDataMsgLen];

static SYS_MEM *pWaveMem = NULL;

void App_Msg_Init(void)
{
	uint8 err;

	pWaveMem = SysMemCreate(WaveBuf,WaveLen,WaveSize,&err);

 	Task_Msg_Create(&NetMsgQ,         &TaskMsgNetPoint[0], 		NetMsgQBuf, 		AppNetMsgLen, 		MsgQSize);
 	Task_Msg_Create(&BusinessMsgQ,    &TaskMsgBusinessPoint[0],	BusinessMsgQBuf,	AppBusinessMsgLen,	MsgQSize);
 	Task_Msg_Create(&DataManagerMsgQ, &TaskMsgDataPoint[0], 	DataMsgQBuf,		AppDataMsgLen,		MsgQSize);
}

//���緢�Ͳ��������������Ϣ
//��������ϵͳ����, �ⲿ����
extern void Send_SysParm_Info(char *sbuf);

static char debug_buf[128] = {0};
BOOL Net_Send_Wave(void *buf)
{
	static uint8 cnt = 0;
	static char *pbuf;
	static BOOL reset_flag = TRUE;
	static char *pSysParmBuf = NULL;
	uint8 err;
	TaskMsg msg = {0};
	uint32 device_value = Get_Device_Status();
	uint32 len = 0;

	if((buf == NULL) || (debug_buf == NULL))return FALSE;
	if(pWaveMem == NULL) return FALSE;
	
	if(cnt == 0) 
	{
		pbuf = (char *)SysMemGet(pWaveMem, &err);
		if((err != SYS_ERR_NONE) || (pbuf == NULL)) 
		{
			debug(Debug_Notify, "Wave mem allocated failed! %d\r\n", err);
			return FALSE;
		}

		memset(pbuf, 0, WaveSize);
		Build_NetData_Pack(pbuf, 321, Net_Data_Wet);
	}
	
	memcpy(pbuf + 7 + cnt*20, buf, 16);
	memcpy(pbuf + 7 + cnt*20 + 16, (u8 *)&device_value, 4);

	//��һ����������
	if(reset_flag) 
	{
		//��ָ�뿪��ֻʹ��һ��
		pSysParmBuf = (char *)SysMemGet(pWaveMem, &err);
		if(pSysParmBuf != NULL) 
		{
			Send_SysParm_Info(pSysParmBuf);
		}
		reset_flag = FALSE;			
	} 

	cnt++;
	if(cnt >= 16) {
		cnt = 0;
		
		memset(debug_buf, 0, 128);
		len = debug_get_info(debug_buf);

		//����е�����Ϣ, ���͵�����Ϣ
		if(len > 0) 
		{
			msg.msg_id = Msg_NetDebug;
			msg.msg_ctx = (u32)debug_buf;
			msg.msg_len = len;
			err = Task_QPost(&NetMsgQ, &msg);
		}
		
		//�������ݰ�
		msg.msg_id = Msg_NetSend;
		msg.msg_ctx = (u32)pbuf;
		msg.msg_len = 327;
 		err = Task_QPost(&NetMsgQ, &msg);
 		if(err != SYS_ERR_NONE) 
		{
		    Wave_Mem_Free(pbuf);
 			debug(Debug_Warning, "NetSendPost failed!\r\n");
 			return FALSE;
 		}
	}
	return TRUE;
}
	
uint8 Wave_Mem_Free(void *pblk)
{
	return SysMemPut(pWaveMem, pblk);
}


