#include "includes.h"
#include "app_msg.h"
#include "task_timer.h"
#include "sys_param.h"
#include "car_business.h"
#include "wet_algorim.h"
#include "weight_param.h"
#include "axle_business.h"
#include "overtime_queue.h"
#include "net_business_thread.h"
#include "scszc_debug_info.h"
#include "device_info.h"
#include "adjob.h"
#include "app.h"
#include "sys_config.h"

//��Ϣ˵��
static const char* strMsgArr[] = 
{
	"��Ч",				//msgNone = 0,
	"��λ",			//msgReset,
	"ˢ��",				//msgRefresh
	"�ظ�1����",		//msgInCoilEnter, //����ظд���
	"�ظ�1�ͷ�",		//msgInCoilLeave,
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
	"�ɷ�֪ͨ",			//msgCmdDelCar,
	"ǿ�Ʋɼ�",
	"����",			//msgTick,
	"����ź�"
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

__align(8) static char strInfo[128];
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
	RDelay(10);
	
	i = 1;
	pIter = CarQueue_Get_Head();
	while(pIter != NULL)
	{		
		debug(Debug_Business, "I%d,%11s,AX%2d,%5dkg,(%2d),%s\r\n", i, 
			Get_Car_StatusName(pIter->nStatus), pIter->nAxleNum, pIter->nBestKg, pIter->bWetVaild, Get_Car_LevelName(pIter->WetLevel));
		RDelay(5);
		pIter = pIter->pNext;
		i++;
	}
	debug(Debug_Business, "\r\n");
}

void CarComfirm_Print(void)
{
	debug(Debug_Business, "\r\n>>����ȷ��, %ldms\r\n",  Get_Sys_Tick());
}

void MsgInfo_Print(TaskMsg *pMsg)
{
	uint8 id = 0;

	if(pMsg == NULL) return;
		
 	if(pMsg->msg_id == Msg_Tick) return;
	if(pMsg->msg_id == Msg_Refresh) return;
	
	if(Get_FollowCarMode() == DevideAtOnce)
	{
		if((pMsg->msg_id == Msg_BackAlex_Come)	||
			(pMsg->msg_id == Msg_BackAlex_Add) ||
			(pMsg->msg_id == Msg_BackAlex_Remove) ||
			(pMsg->msg_id == Msg_At_Turn))
		{
			return;
		}
	}
	

	id = pMsg->msg_id - Msg_Business_Base;
	if(id >= sizeof(strMsgArr)/sizeof(strMsgArr[0]))		 //��ֹ����Խ��
		return;
		
	//�¼���Ϣ
	debug(Debug_Business, "\r\n>>%s, %ldms\r\n", strMsgArr[id], Get_Sys_Tick());
	RDelay(10);
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

//=========================================================================
//���ڵ�������

//ģ��һ����
static int Do_Simulate_Car(char *arg, DebugSend send)
{
	char *buf = arg;
	uint32 wet;
	uint8 axle;

	if(arg == NULL) return -1;

	//���Ȳ���
	if(strlen(buf) < 4) return -1;
	
	axle = *buf - 0x30;
	//��������
	if((axle > 6) || (axle < 2)) return -1;
	
	buf++;
	wet = atoi(buf);
	//����������
	if((wet < 500) || (wet > 200000)) return -1;
	
	Send_Simulate_CarInfo(axle, wet, NormalCarCmd);
	return 0;
}

//ģ�⵹��
static int Do_Simulate_BackCar(char *arg, DebugSend send)
{
	Send_Simulate_CarInfo(2, 1000, BackCarCmd);

	return 0;
}

//�ֶ�����
static int Do_Scaler_Set_Zero(char *arg, DebugSend send)
{
	Scaler_Set_Zero(FALSE);
	return 0;
}

//��ӡ������Ϣ
static int Do_Print_DeviceInfo(char *arg, DebugSend send)
{
	char *prompt = "�豸״̬:\r\n";
	int ADChipErr = 0;
	char buf[20] = {0};

	if(send == NULL) return 1;

	send(prompt, strlen(prompt));

	if(Get_LC_ErrStatus(2))   //�жϹ�Ļ״̬,˫��ֻ̨���ú��Ļ
	{
		snprintf(buf,19, "%s", "1.��Ļ����\r\n");
	}
	else 
	{
		snprintf(buf,19, "%s", "1.��Ļ����\r\n");
	}
	send(buf,strlen(buf));
	
	if(Get_GC_ErrStatus(1))//�жϵظ�״̬��˫��ֻ̨����ǰ�ظ�
	{
		snprintf(buf,19, "%s", "2.ǰ�ظ�����\r\n");
	}
	else 
	{
		snprintf(buf,19, "%s", "2.ǰ�ظй���\r\n");
	}
	send(buf,strlen(buf));

	if(Get_GC_ErrStatus(2))//�жϵظ�״̬��˫��ֻ̨����ǰ�ظ�
	{
		snprintf(buf,19, "%s", "3.��ظ�����\r\n");
	}
	else 
	{
		snprintf(buf,19, "%s", "3.��ظй���\r\n");
	}
	send(buf,strlen(buf));

	if(Get_AD_ErrStatus(1))
	{
		snprintf(buf,19, "%s", "4.������1�쳣\r\n");
	}
	else 
	{
		snprintf(buf,19, "%s", "4.������1����\r\n");
	}	
	send(buf,strlen(buf));

	if(Get_AD_ErrStatus(2))
	{
		snprintf(buf,19, "%s", "5.������2�쳣\r\n");
	}
	else 
	{
		snprintf(buf,19, "%s", "5.������2����\r\n");
	}	
	send(buf,strlen(buf));

	if(Get_AD_ErrStatus(3))
	{
		snprintf(buf,19, "%s", "6.������3�쳣\r\n");
	}
	else 
	{
		snprintf(buf,19, "%s", "6.������3����\r\n");
	}	
	send(buf,strlen(buf));

	if(Get_AD_ErrStatus(4))
	{
		snprintf(buf,19, "%s", "7.������4�쳣\r\n");
	}
	else 
	{
		snprintf(buf,19, "%s", "7.������4����\r\n");
	}
	send(buf,strlen(buf));

	if(Get_Tire_ErrStatus())//�ж���̥ʶ����״̬
	{
		snprintf(buf,19, "%s", "8.��̥Sensor����\r\n");
	}
	else 
	{
		snprintf(buf,19, "%s", "8.��̥Sensor�쳣\r\n");
	}	
	send(buf,strlen(buf));

	memset(buf, 0, 20);
	ADChipErr = Get_ADChip_ErrStatus();
	if(ADChipErr > 0)
	{
 		if(ADChipErr & ERROR_AD1) snprintf(buf, 15, "1 ");
 		if(ADChipErr & ERROR_AD2) snprintf(buf+2,15-2,"2 ");
 		if(ADChipErr & ERROR_AD3) snprintf(buf+2,15-4,"3 ");
 		if(ADChipErr & ERROR_AD4) snprintf(buf+2,15-6,"4 ");
 		snprintf(buf,19, "9.AD %s·�쳣\r\n", buf);
	}
	else if(ADChipErr == 0)
	{
		snprintf(buf,19, "%s", "9.AD����\r\n");
	}	
	else
	{
		;
	}
	send(buf,strlen(buf));

	return 0;
}

static int Do_Get_Version(char *arg, DebugSend send)
{
	static char buf[64] = {0};

	if(send == NULL) return 1;
	
	snprintf(buf, 63, "����汾:DT3101-V%d.%d.%d\r\n", AppVersion1, AppVersion2, AppVersion3);
	send( buf, strlen(buf));

	send("\r\n3102���ز���:\r\n", 15); 
	snprintf(buf, 63, "PowerZero:%d,ZeroTruckRang:%d,ZeroTick:%d\r\n", 
		gWetPar.PowerZero, gWetPar.ZeroTruckRang, gWetPar.ZeroTruckTick);
	send(buf, strlen(buf));

	snprintf(buf, 63, "Scaler_K:%f,AD3_K:%f,AD4_K:%f\r\n", 
		gWetPar.Scaler_K,gWetPar.PreAxle1_K,gWetPar.PreAxle2_K);
	send(buf, strlen(buf));

	snprintf(buf, 63, "Scaler_zero:%d,AD3_zero:%d,AD4_zero:%d\r\n", 
		(int)gWetPar.ScalerZeroAD, (int)gWetPar.PreAxle1ZeroAD, (int)gWetPar.PreAxle2ZeroAD);
	send(buf, strlen(buf));

	snprintf(buf, 63, "ǰ������ֵ:%d,ǰ������ֵ:%d\r\n", 
		gWetPar.PreAxleUpWet, gWetPar.PreAxleDownWet);
	send(buf, strlen(buf));

	snprintf(buf, 63, "��������ֵ:%d,��������ֵ:%d\r\n",
		gWetPar.BackAxleUpWet, gWetPar.BackAxleDownWet);
	send(buf, strlen(buf));
	
	return 0;
}

static int Do_Set_FollowMode(char *arg, DebugSend send)
{
	uint8 level = 0;
	char tmp;
	char buf[25] = {0};
	
	if(arg == NULL) return -1;
	if(send == NULL) return -1;

	tmp = Get_FollowCarMode();
	level = *arg - 0x30;

	snprintf(buf, 24, "OrigMod:%d,RevMod:%d\r\n", tmp, level);
	send(buf, strlen(buf));
	
	if(level > DevideAtDown) return -1;
	Set_FollowCarMode(level);

	return 0;
}

static int Do_Set_ScalerK(char *arg, DebugSend send)
{
	float k = 0.0f;

	k = atof(arg);

	return Set_ScalerStatic_K(k);
}

static int Do_Set_AD3K(char *arg, DebugSend send)
{
	float k = 0.0f;

	k = atof(arg);

	return Set_3Axle_Static_K(k);
}


//ע�ᴮ�ڵ�������
void SCSZC_Debug_Cmd_Init(void)
{
	Register_Debug_Cmd("car", Do_Simulate_Car, "ģ��һ����, e.g.car23000 2��3T", TRUE);
	Register_Debug_Cmd("backcar", Do_Simulate_BackCar, "ģ��һ������", FALSE);
	Register_Debug_Cmd("setzero", Do_Scaler_Set_Zero, "��̨�ֶ�����", FALSE);
	Register_Debug_Cmd("print", Do_Print_DeviceInfo, "��ӡ��̨��������Ϣ", FALSE);
	Register_Debug_Cmd("version", Do_Get_Version, "��ȡ����汾�ͳ��ز���", FALSE);
	Register_Debug_Cmd("setfmode", Do_Set_FollowMode, "���ø���ѡ��: 0�ϳӷֳ�1�³ӷֳ�", TRUE);
	Register_Debug_Cmd("setscalerk", Do_Set_ScalerK,	"��������Kϵ��", TRUE);
	Register_Debug_Cmd("setad3k", Do_Set_AD3K,	"����AD3_Kϵ��", TRUE);
}

