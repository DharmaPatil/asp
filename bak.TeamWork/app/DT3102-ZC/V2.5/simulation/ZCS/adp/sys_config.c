#include "includes.h"
#include <stdarg.h>
#include "GuiWindows.h"
#include "sys_config.h"
#include "app_rtc.h"
#include "car_queue.h"
#include "..\ZcsAdp.h"
#include "overtime_queue.h"
#include "car_business.h"
#include "rmcp.h"

CWindow		gWD_Main;
CWindow*		g_pCurWindow;
OvertimeData gMainCarInfo;
MsgQ MsgBusiness;
MsgQ DataManagerMsgQ;
MsgQ BusinessMsgQ;
MsgQ NetMsgQ;

//��Ϣ˵��
static const char* strMsgArr[] = 
{
	"��Ч",				//msgNone = 0,
	"��λ",					//msgReset,
	"ˢ��",					//msgRefresh
	"�ظ�1����",			//msgInCoilEnter, //����ظд���
	"�ظ�1�ͷ�",			//msgInCoilLeave,
	"�ظ�2����",		
	"�ظ�2�ͷ�",		
	"��Ļ1����",			
	"��Ļ1�ͷ�",		
	"��Ļ2����",		
	"��Ļ2�ͷ�",	
	"����",
	"����",					//msgInAlexAdd,
	"����",					//msgInAlexRemove,
	"��������",					//msgOutAlexEnter,
	"�������",					//msgOutAlexEnter,
	"��������",					//msgOutAlexEnter,
	"ǰ���뿪",
	"���²ɼ�",
	"�����ֶ�",
	"�ɷ�֪ͨ",				//msgCmdDelCar,
	"ǿ�Ʋɼ�",
	"����"					//msgTick,
};

//����״̬˵��
static const char* strCarStateArr[] = 
{
	"��Ч",				//stCarNone
	"����",				//stCarComing
	"����",				//stCarEnter
	"����",				//stCarInScaler
	"�ɷ�",				//stCarWaitPay
	"����",				//stLongCarInScaler, //�����ڳ�̨�ϣ�ͷ���Ѿ�������ظ�
	"�뿪",				//stCarLeaving
	"��ӽɷ�",			//stCarFarPay
	"ɾ��"				//stDead,		
};

static const char *strCarLevel[] = 
{
	"��Ч",
	"����",
	"�����Զ�",
	"��������",
	"�����³�",
	"�����ֶ�",
	"����"
};

static const char *strTryeType[] = 
{
	"����",
	"˫��",
	"��Ч"
};

void MsgPush(TaskMsg msg);
uint8 Task_QPost(MsgQ *pMsgQ, void *pMsg)
{
	if((pMsgQ == &MsgBusiness) || (pMsgQ == &BusinessMsgQ))
	{
		MsgPush(*(TaskMsg*)pMsg);
	}

	return 1;
}

TaskMsg* MsgPull();
uint8 Task_QPend(MsgQ *pMsgQ, void *pMsg, int len, uint16 timeout)
{
	TaskMsg* pMsgTmp;
	pMsgTmp = MsgPull();
	if(pMsgTmp == NULL) return 0;


	memcpy(pMsg, pMsgTmp, len);

	return 1;
}


BOOL Get_BGFlag(void)
{
	return FALSE;
}

void Sys_Business(TaskMsg *msg);
void Business_Thread_Adp(void *arg)
{
	TaskMsg event_msg = {0};
	uint8 ret;

	while(1)
	{
		ret = Task_QPend(&MsgBusiness, &event_msg, sizeof(event_msg), 0);	
		if(ret != 0) 
		{
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
		else
		{
			break;
		}
	}

}

gWetParam gWetPar;
double g_fTimerTick = 0.0f;

static volatile LongCarMod LongCarMode = AutoLongCarMode;			//��������ģʽ

static char debug_buff[128] = {0};
int8 DebugLevel = 5;
void debug(int8 level, const char *fmt, ...)
{
	uint32 len = 0, rlen = 0;
	

	if(level <= DebugLevel) 
	{
		va_list ap;
		memset(debug_buff, 0, 128);
		va_start(ap,fmt);
		vsnprintf(debug_buff,127,fmt,ap);
		len = strlen(debug_buff);
		OutPut(debug_buff);
		va_end(ap);
	}
}

void BusinessTimerReset()
{
	g_fTimerTick = 0;
}

//����10ms��ʱ
void BusinessTimerTick()
{
	static int nTimeTick = 0;
	static u16 cnt = 0;
	static BOOL flag = FALSE;

	TaskMsg msg = {0};

	g_fTimerTick += 1.25f;
	nTimeTick++;
	if(nTimeTick >= 8)
	{
		nTimeTick = 0;

		//ҵ���߳�10ms��ʱ��Ϣ,������ʱ2s
		if(!flag) 
		{
			cnt++;
			if(cnt > 200) 
			{
				cnt = 0;
				flag = TRUE;
			}
		}

		if(flag) 
		{
			msg.msg_id = Msg_Tick;
			Task_QPost(&MsgBusiness, &msg);
		}
	}
}

//������Ļ�¼�
static int nLC2State;
void LC2MakeMsg(int nState)
{
	static int nPreState = 0;
	static int nStateTick = 0;
	TaskMsg msg = {0};

	nLC2State = nState;

	if(nPreState != nState)
	{
		nStateTick++;
		
		if(nStateTick > 10)
		{
			nStateTick = 0;
			nPreState = nState;

			if(nState > 0)
			{
				msg.msg_id = Msg_LC2_Enter;
			}
			else
			{
				msg.msg_id = Msg_LC2_Leave;
			}
			Task_QPost(&MsgBusiness, &msg);
		}
	}
	else
	{
		nStateTick = 0;
	}
}


//�ظ�1
//void GC1MakeMsg(int nState)
//{
//	static int nPreState = 0;
//	TaskMsg msg = {0};
//
//	if(nPreState != nState)
//	{
//		if(nState > 0)
//		{
//			msg.msg_id = Msg_GC1_Enter;
//		}
//		else
//		{
//			msg.msg_id = Msg_GC1_Leave;
//		}
//		Task_QPost(&MsgBusiness, &msg);
//	}
//
//	nPreState = nState;
//}

void GC1MakeMsg(int nState)
{
	static int nPreState = 0;
	TaskMsg msg = {0};

	if(nPreState != nState)
	{
		if(nState > 0)
		{
			msg.msg_id = Msg_Cmd_PayNotify;
		}


		Task_QPost(&MsgBusiness, &msg);
	}

	nPreState = nState;
}

//�ظ�2
void GC2MakeMsg(int nState)
{
	static int nPreState = 0;
	TaskMsg msg = {0};

	if(nPreState != nState)
	{
		if(nState > 0)
		{
			msg.msg_id = Msg_GC2_Enter;
		}
		else
		{
			msg.msg_id = Msg_GC2_Leave;
		}
		Task_QPost(&MsgBusiness, &msg);
	}

	nPreState = nState;
}

SysMod Get_System_Mode()
{
	return (SysMod)SCS_ZC_7;
}

int Updata_AD_Status(u8 chanel,u8 status)
{
	return 0;
}



int8 Get_Debug_Level(void)
{
	return 1;
}

//������ʶ����״̬
void Update_AxleDevice_Status(DeviceStatus status)
{

}

int Param_Write(uint32 nAddr, char* pBuff, int nLen)
{
	return 1;
}

void Update_Scaler_WetPar(WetStatue *wet)
{

}

BOOL Net_Send_Wave(void *buf)
{
	return 0;
}

unsigned char Get_NetStatus(void)
{
	return 0;
}

unsigned long Get_Sys_Tick(void)
{
	return (unsigned long)g_fTimerTick;
}

uint8 GuiMsgQueuePost(GuiMsgInfo* pGuiMsgInfo)
{
	return 0;
}

/****************************************
 *��	����no��Ϊ1(ǰ��Ļ)��2(���Ļ)
 * ����ֵ��true������false�ͷ�
 * ˵	������ȡ��Ļ����״̬
 ****************************************/
BOOL Get_LC_Status(u8 no)
{
	return (nLC2State > 0) ? 1 : 0;
}

static uint16 CarSpeed = 30;

/* �ٶȻ�ȡ���� */
uint16 Get_CarSpeed(void)
{
	if(CarSpeed < 20)
		CarSpeed = 20;

	if(CarSpeed > 800)
		CarSpeed = 300;

	return CarSpeed;
}

uint16 Get_Speed_Length(void)
{
	return 1500;
}


BOOL Get_TrafficSignal_Flag(void)		//return ture: �к��̵� FALSE: û��
{
	return 0;
}

const char *Get_Car_StatusName(int32 id)
{
	if(id >= sizeof(strCarStateArr)/sizeof(strCarStateArr[0]))	
		id = 0;
		
	return strCarStateArr[id];
}

const char *Get_Car_LevelName(uint8 id)
{
	if(id > 0)
		id = id - LevelMultiCar + 1;

	if(id >= sizeof(strCarLevel)/sizeof(strCarLevel[0]))	
		id = 0;	

	return strCarLevel[id];
}

const char *Get_Car_TryeType(uint8 nCount)
{
	if(nCount > 2) nCount = 2;

	return strTryeType[nCount];
}

void Update_System_Time(void)
{

}


int32 Get_Static_Weight(void);
static char strInfo[1024];
static char* pStrInfo = (char*)strInfo;

void CarQueue_Print_Info(void)
{
	CarInfo* pIter = NULL;
	int nStrIndex = 0;
	int i = 0;

	//��ȡ��������Ϣ
	nStrIndex += snprintf(pStrInfo + nStrIndex, 127-nStrIndex, ">>������Ϣ\r\nT%ld,P%ld,W%ld\r\n", CarQueue_Get_Count(), CarQueue_Get_CanPayCount(), Get_Static_Weight());
	//��ȡ����Ϣ
	nStrIndex += snprintf(pStrInfo + nStrIndex, 127-nStrIndex, "A0%d,AU%d,AD%d\r\n", (int)gWetPar.PreAxle2ZeroAD, gWetPar.PreAxleUpWet, gWetPar.PreAxleDownWet);
	pStrInfo[nStrIndex] = 0;
	debug(Debug_Business, pStrInfo);
	
	i = 1;
	pIter = CarQueue_Get_Head();
	while(pIter != NULL)
	{		
		debug(Debug_Business, "C%d,%s,AX%2d,%5dkg,(%2d),%s\r\n", i, 
			Get_Car_StatusName(pIter->nStatus), pIter->nAxleNum, pIter->nBestKg, pIter->bWetVaild, Get_Car_LevelName(pIter->WetLevel));

		pIter = pIter->pNext;
		i++;
	}
	debug(Debug_Business, "\r\n");
}

static void RDelay(int n)
{

}

void CarComfirm_Print(void)
{
	debug(Debug_Business, "\r\n>>����ȷ��,%.3fs\r\n",  ((float)Get_Sys_Tick())/1000);
}

void MsgInfo_Print(TaskMsg *pMsg)
{
	uint8 id = 0;

	if(pMsg == NULL)
		return;
		
	if(pMsg->msg_id == Msg_Tick) return;
	if(pMsg->msg_id == Msg_Refresh) return;

	if(Get_FollowCarMode() == DevideAtOnce)
	{
		if((pMsg->msg_id == Msg_BackAlex_Come)	||
			(pMsg->msg_id == Msg_BackAlex_Add) ||
			(pMsg->msg_id == Msg_BackAlex_Remove)||
			(pMsg->msg_id == Msg_At_Turn))
		{
			return;
		}
	}
		
	id = pMsg->msg_id - Msg_Business_Base;
	if(id >= sizeof(strMsgArr)/sizeof(strMsgArr[0]))		 //��ֹ����Խ��
		return;

	//�¼���Ϣ
	debug(Debug_Business, "\r\n>>%s,%.3fs\r\n", strMsgArr[id], ((float)Get_Sys_Tick())/1000);
	RDelay(10);
}

void TrafficSignal_PowerOn(void)
{
	
}

void TrafficSignal_PowerOff(void)
{
	
}

CSysTime curTime;
CSysTime* Get_System_Time(void)
{
	//SYSTEMTIME st;
	//GetLocalTime(&st);
	return (CSysTime*)&curTime;
}

void Set_CarSpeed(int32 speed)
{
	CarSpeed = speed;
}

BOOL Get_Tire_ErrStatus(void)
{
	return TRUE;
}

void Build_NetData_Pack(char *buf, uint16 len, uint8 type)
{

}

void BarrierGateAction_After_LC2DownOff(void)
{

}


void BarrierGate_Reset_Init(void)
{

}

int Build_Rcmp_HeadPack(CmdRMCP msgType, unsigned char* buf, int context_size)
{
	return 0;
}


void BarrierGate_Service(void)
{

}

void SoftDog_Feed(uint32 id)
{

}

int Register_SoftDog(unsigned short time, int state, const char *name)
{
	return 1;
}

char fPeripheral_Control(unsigned char type , unsigned char status)
{
	return 0;
}

FollowCarMod Get_FollowCarMode(void)
{
	//return DevideAtOnce;
	return DevideAtDown;
}

FollowCarMod Get_LittleFollowCarMode(void)
{
	return DevideAtOnce;
	//return DevideAtDown;
}

//��ȡ����ģʽ
LongCarMod Get_LongCar_Mode(void)
{
	return AutoLongCarMode;

	//return HandleLongCarMode;
}


void barriergate_open(void)
{
	return;
}
