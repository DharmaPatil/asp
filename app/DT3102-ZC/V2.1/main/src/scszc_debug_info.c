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

//��Ϣ˵��
static const char* strMsgArr[] = 
{
	"MsgNone",				//msgNone = 0,
	"MsgReset",			//msgReset,
	"MsgRefresh",				//msgRefresh
	"�ظ�1����",		//msgInCoilEnter, //����ظд���
	"�ظ�1�ͷ�",		//msgInCoilLeave,
	"�ظ�2����",		
	"�ظ�2�ͷ�",		
	"��Ļ1����",			
	"��Ļ1�ͷ�",		
	"��Ļ2����",		
	"��Ļ2�ͷ�",	
	"����",	
	"����",			//msgInAlexAdd,
	"����",			//msgInAlexRemove,
	"����",			//msgOutAlexEnter,
	"���²ɼ�",
	"�����ֶ�",
	"�ɷ�֪ͨ",			//msgCmdDelCar,
	"ǿ�Ʋɼ�",
	"MsgTick",			//msgTick,
	"����ź�"
};

//����״̬˵��
static char* strCarStateArr[] = 
{
	"NoCar",				//stCarNone
	"CarComing",			//stCarComing
	"CarInScaler",			//stCarInScaler
	"CarWaitPay",			//stCarWaitPay
	"LongCar",				//stLongCarInScaler, //�����ڳ�̨�ϣ�ͷ���Ѿ�������ظ�
	"CarLeaving",			//stCarLeaving
	"CarFarPay",			//stCarFarPay
	"CarDead"				//stDead,		
};

static char *strCarLevel[] = {
	"levelNone",
	"LevelAxleWet",
	"LevelMultiCar",
	"LevelAutoLongCar",
	"LevelSingleCarBy",
	"LevelSingleCarDown",
	"LevelLongCarSegs",
	"LevelSingleCar"
};

__align(8) static char strInfo[128];
static char* pStrInfo = (char*)strInfo;

void CarQueue_Print_Info(void)
{
	CarInfo* pIter = NULL;
	int nStrIndex = 0;
	int i = 0;
	
	//��ȡ��������Ϣ
	nStrIndex += snprintf(pStrInfo + nStrIndex, 127-nStrIndex, "T%ld,P%ld,W%ld\r\n", CarQueue_Get_Count(), CarQueue_Get_CanPayCount(), Get_Static_Weight());
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

void MsgInfo_Print(TaskMsg *pMsg)
{
	uint8 id = 0;

	if(pMsg == NULL) return;
		
 	if(pMsg->msg_id == Msg_Tick) return;
		
	id = pMsg->msg_id - Msg_Business_Base;
	if(id >= sizeof(strMsgArr)/sizeof(strMsgArr[0]))		 //��ֹ����Խ��
		return;
		
	//�¼���Ϣ
	debug(Debug_Business, "\r\n>>%s, %ldms\r\n", strMsgArr[id], Get_Sys_Tick());
	RDelay(10);

    //����ӡ������Ϣ���¼�
	if((pMsg->msg_id == Msg_GC1_Enter)	|| 
		(pMsg->msg_id == Msg_GC1_Leave) || 
		(pMsg->msg_id == Msg_None) ||
		(pMsg->msg_id == Msg_BackAlex) ||
		(pMsg->msg_id == Msg_PreAlex_Come) ||
		(pMsg->msg_id == Msg_WBAxle)) 
		return;
	
	CarQueue_Print_Info();
}

char *Get_Car_StatusName(int32 id)
{
	if(id >= sizeof(strCarStateArr)/sizeof(strCarStateArr[0]))	
		id = 0;
		
	return strCarStateArr[id];
}

char *Get_Car_LevelName(uint8 id)
{
	if(id > 0)
		id = id - LevelAxleWet + 1;

	if(id >= sizeof(strCarLevel)/sizeof(strCarLevel[0]))	
		id = 0;	

	return strCarLevel[id];
}


//=========================================================================
//���ڵ�������

//ģ��һ����
static int Do_Simulate_Car(char *arg)
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
static int Do_Simulate_BackCar(char *arg)
{
	Send_Simulate_CarInfo(2, 1000, BackCarCmd);

	return 0;
}

//�ֶ�����
static int Do_Scaler_Set_Zero(char *arg)
{
	Scaler_Set_Zero();
	return 0;
}

//��ӡ������Ϣ
static int Do_Print_DeviceInfo(char *arg)
{
	char *prompt = "�豸״̬:\r\n";
	int ADChipErr = 0;
	char buf[20] = {0};

	Usart1_WriteData(prompt, strlen(prompt));

	if(Get_LC_ErrStatus(2))   //�жϹ�Ļ״̬,˫��ֻ̨���ú��Ļ
	{
		snprintf(buf,19, "%s", "1.��Ļ����\r\n");
	}
	else 
	{
		snprintf(buf,19, "%s", "1.��Ļ����\r\n");
	}
	Usart1_WriteData(buf,strlen(buf));
	
	if(Get_GC_ErrStatus(1))//�жϵظ�״̬��˫��ֻ̨����ǰ�ظ�
	{
		snprintf(buf,19, "%s", "2.ǰ�ظ�����\r\n");
	}
	else 
	{
		snprintf(buf,19, "%s", "2.ǰ�ظй���\r\n");
	}
	Usart1_WriteData(buf,strlen(buf));

	if(Get_GC_ErrStatus(2))//�жϵظ�״̬��˫��ֻ̨����ǰ�ظ�
	{
		snprintf(buf,19, "%s", "3.��ظ�����\r\n");
	}
	else 
	{
		snprintf(buf,19, "%s", "3.��ظй���\r\n");
	}
	Usart1_WriteData(buf,strlen(buf));

	if(Get_AD_ErrStatus(1))
	{
		snprintf(buf,19, "%s", "4.������1�쳣\r\n");
	}
	else 
	{
		snprintf(buf,19, "%s", "4.������1����\r\n");
	}	
	Usart1_WriteData(buf,strlen(buf));

	if(Get_AD_ErrStatus(2))
	{
		snprintf(buf,19, "%s", "5.������2�쳣\r\n");
	}
	else 
	{
		snprintf(buf,19, "%s", "5.������2����\r\n");
	}	
	Usart1_WriteData(buf,strlen(buf));

	if(Get_AD_ErrStatus(3))
	{
		snprintf(buf,19, "%s", "6.������3�쳣\r\n");
	}
	else 
	{
		snprintf(buf,19, "%s", "6.������3����\r\n");
	}	
	Usart1_WriteData(buf,strlen(buf));

	if(Get_AD_ErrStatus(4))
	{
		snprintf(buf,19, "%s", "7.������4�쳣\r\n");
	}
	else 
	{
		snprintf(buf,19, "%s", "7.������4����\r\n");
	}
	Usart1_WriteData(buf,strlen(buf));

	if(Get_Tire_ErrStatus())//�ж���̥ʶ����״̬
	{
		snprintf(buf,19, "%s", "8.��̥Sensor����\r\n");
	}
	else 
	{
		snprintf(buf,19, "%s", "8.��̥Sensor�쳣\r\n");
	}	
	Usart1_WriteData(buf,strlen(buf));

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
	Usart1_WriteData(buf,strlen(buf));

	return 0;
}

static int Do_Get_Version(char *arg)
{
	char buf[20] = {0};
	
	snprintf(buf, 19, "DT3101-V%d.%d.%d\r\n", AppVersion1, AppVersion2, AppVersion3);
	Usart1_WriteData( buf, strlen(buf));

	return 0;
}

//ע�ᴮ�ڵ�������
void SCSZC_Debug_Cmd_Init(void)
{
	Register_Debug_Cmd("car", Do_Simulate_Car, "ģ��һ����, e.g.car23000 2��3T", TRUE);
	Register_Debug_Cmd("backcar", Do_Simulate_BackCar, "ģ��һ������", FALSE);
	Register_Debug_Cmd("setzero", Do_Scaler_Set_Zero, "��̨�ֶ�����", FALSE);
	Register_Debug_Cmd("print", Do_Print_DeviceInfo, "��ӡ��̨��������Ϣ", FALSE);
	Register_Debug_Cmd("version", Do_Get_Version, "��ȡ����汾", FALSE);
}

