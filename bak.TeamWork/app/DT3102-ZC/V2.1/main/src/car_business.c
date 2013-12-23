#include "includes.h"
#include "car_queue.h"
#include "app_msg.h"
#include "car_business.h"
#include "overtime_queue.h"
#include "axle_business.h"
#include "sys_config.h"
#include "weight_param.h"
#include "wet_algorim.h"
#include "app_rtc.h"
#include "guiwindows.h"
#include "task_timer.h"
#include "speed_check.h"
#include "device_info.h"
#include "task_timer.h"
#include "scszc_debug_info.h"
#include "Form_Main.h"
#include "debug_info.h"

extern void ResetTurnTick(void);
extern void ResetPayTimeOutTick(void);
extern void SetPayTimeOutThreshold(int tick);
extern void ClosePayTimeOut(void);
extern void FSM_CarFunction(CarInfo *pCar, TaskMsg *pMsg);

int nCarWetMaxTick = 2000;
static OvertimeData t_carinfo = {{0}};

//���������Ŷӽɷѳ�ʱʱ��
static void Set_Car_PayOutTick(CarInfo *pCar)
{
	if(pCar == NULL) return;

	//����Ĭ�ϳ�ʱʱ��10��
	SetPayTimeOutThreshold(10000);

	//������������������
	if(pCar->nAxleNum <= 2)
	{
		if(pCar->nBestKg < 8000) SetPayTimeOutThreshold(8000);
		if(pCar->nBestKg < 3000) SetPayTimeOutThreshold(3000);
	}
}

//�ж��Ƿ񵥳�
BOOL IsSingleCarComming(CarInfo *pCar)
{
	if(pCar == NULL) return TRUE;

	if(pCar->nScalerKgComing < SCALER_MIN_KG) return TRUE;
	if(pCar->nScalerKgDyncComing < SCALER_MIN_KG) return TRUE;

	return FALSE;
}

//���ó����������
void CarSetBestKg(CarInfo *pCar, int nKg, unsigned char nLevel)
{
	if(pCar == NULL) return;

	//��ֹ�³ӷֳ�����
	if(pCar->WetLevel == LevelMultiCar)
	{
		if(nKg > ((float)pCar->nBestKg * 1.25f))
		{
			debug(Debug_Business, "�������ӳ���:%d, %d\r\n", nKg, pCar->nBestKg);
			return;
		}
	}

	//ֻ�е������Ŷȵȼ������ϵȼ�ʱ�Ÿ�������
	if(nLevel > pCar->WetLevel)
	{
		pCar->nBestKg = nKg;
		pCar->WetLevel = nLevel;

		if(pCar->nBestKg < 850) pCar->nBestKg = 850;
	}
}

//��ǰ���뿪�󳵻�δ��βʱ,�ж�4�����ϴ��Ƿ����㵥������,���ÿ�������
static BOOL Charge_FollowToSingle_Business(CarInfo *pCar, CarKgLevel level)
{
	if(pCar == NULL) return FALSE;

	if(pCar->WetLevel == LevelMultiCar)
	{
		if((pCar->nAxleNum >= 4) && (pCar->bFollowToSingle == 1))
		{
			//4�����ϵĳ�, ���������ǰ���뿪ʱ����δ��β��
			//ǰ���뿪�����ϳӵ��������ڵ���2
			//����Ϊ�����㵥������,ȡ��������
			if((pCar->nAxleNum - pCar->nFollowAxleNum) >= 2)
			{
				pCar->nBestKg = Get_Static_MaxWeight();
				if(pCar->nBestKg < 850) pCar->nBestKg = 850;

				pCar->WetLevel = level;
				debug(Debug_Business, "�����л�Ϊ����\r\n");
				return TRUE;
			}
		}
	}

	return FALSE;
}

//���������жϳ�������
static void Charge_Car_OverWeight(CarInfo *pCar, OvertimeData *tCar)
{
	if(pCar == NULL) return;
	if(tCar == NULL) return;
	
	switch(pCar->nAxleNum) 
	{
		case 2:
			if(pCar->nBestKg > (int)gWetPar.Max2AxleWet)
				tCar->OverWetFlag = 1;
			break;

		case 3:
			if(pCar->nBestKg > (int)gWetPar.Max3AxleWet)
				tCar->OverWetFlag = 1;
			break;

		case 4:
			if(pCar->nBestKg > (int)gWetPar.Max4AxleWet)
				tCar->OverWetFlag = 1;
			break;

		case 5:
			if(pCar->nBestKg > (int)gWetPar.Max5AxleWet)
				tCar->OverWetFlag = 1;
			break;

		case 6:
			if(pCar->nBestKg > (int)gWetPar.Max6AxleWet)
				tCar->OverWetFlag = 1;
			break;

		default: 
			break;
	}
}

static int32 WetDrvProc(int32 Wet, u8 Drv)
{
	long nStep;

	if(Drv == 0) Drv = 1;
	
	//�ֶ�ֵ����
	if(Wet >= 0)
	{
		nStep = (long)((2 * Wet + Drv) / (2 * Drv));
	}
	else
	{
		nStep = (long)((2 * Wet - Drv) / (2 * Drv));
	}
	
	return (int32)(Drv * nStep);
}

#define CARSPEED_POINT_COUNT 7

//����������������
static void Weight_Dynamic_Modify(CarInfo *pCar)
{	
	static const int CarSPoint[CARSPEED_POINT_COUNT] = {0, 50, 100, 150, 200, 250, 300};
	static float CarK[CARSPEED_POINT_COUNT];
	float wet = 0.0f, k = 0.0f;
	int i = 0;
	
	if(pCar == NULL) return;

	pCar->speed = Get_CarSpeed();

	CarK[0] = (float)gWetPar.Speed_0k  / 1000.0f;
	CarK[1] = (float)gWetPar.Speed_5k  / 1000.0f;
	CarK[2] = (float)gWetPar.Speed_10k  / 1000.0f;
	CarK[3] = (float)gWetPar.Speed_15k  / 1000.0f;
	CarK[4] = (float)gWetPar.Speed_20k  / 1000.0f;
	CarK[5] = (float)gWetPar.Speed_25k  / 1000.0f;
	CarK[6] = (float)gWetPar.Speed_30k  / 1000.0f;

	do 
	{
		if(pCar->speed <= CarSPoint[0])
		{
			wet = (float)pCar->nBestKg * CarK[0];
			break;
		}

		if(pCar->speed > CarSPoint[6])
		{
			wet = (float)pCar->nBestKg * CarK[6];
			break;
		}
		
		for(i = 1; i < CARSPEED_POINT_COUNT; i++)
		{
			if(pCar->speed < CarSPoint[i])
			{
				k = CarK[i-1] + (CarK[i]-CarK[i-1])*(pCar->speed-CarSPoint[i-1])/(float)(CarSPoint[i]-CarSPoint[i-1]);
				wet = k * (float)pCar->nBestKg;
				break;
			}
		}
	}while(0);

	//�ٶ�����ϵ���쳣����
	if((wet < 10.0f) || ((int)wet > (5*pCar->nBestKg))) return;

	pCar->nBestKg = (int)wet;
}  

//���������쳣�����,ȡƽ������
static void Modify_Abnormal_AxleWeight(CarInfo *pCar)
{
	int i = 0;
	float tmp = 0.0f;

	if(pCar == NULL) return;
	if(pCar->nAxleNum < 1) return;

	tmp = pCar->nBestKg / pCar->nAxleNum;

	//�ֶ�ֵ����
	tmp = WetDrvProc(tmp, gWetPar.StaticDivValue);
	if(tmp > gWetPar.StaticDivValue) tmp -= gWetPar.StaticDivValue;
		
	for(i = 0; i < pCar->nAxleNum - 1; i++) 
	{
		pCar->AxleInfo[i].axle_wet = tmp;
	}

	pCar->AxleInfo[pCar->nAxleNum-1].axle_wet = pCar->nBestKg - i*tmp;
}

//��������,�����쳣����
static void Modify_Abnormal_CarInfo(CarInfo *pCar)
{
	static uint8 cnt = 1;

	if(pCar == NULL) return;
	if((pCar->bAckErr == 1) 		||
		(pCar->cmd == LongCarCmd)	|| 
		(pCar->cmd == BackCarCmd)) return;
	
	cnt++;
	if(cnt > 6) cnt = 1;

	if(pCar->nBestKg < 850)
	{
		//��������С��850kg, ȡ�������1050~1800
		pCar->nBestKg = 900 + cnt * 150;
	}
	
	if(pCar->nAxleNum < 2)  pCar->nAxleNum = 2;
	if(pCar->nAxleNum > 6) pCar->nAxleNum = 6;
}

//���ݱ�����������, ������Ϣ����ʱȡ
static void Calc_Car_AxleWet(CarInfo *pCar)
{
	int32 sum = 0;
	float div = 0.0f;
	uint8 i = 0;

	if(pCar == NULL) return;
	if(pCar->nAxleNum < 1) return;

	sum = 0;
	for(i = 0; i < pCar->nAxleNum; i++)
	{
		sum += pCar->AxleInfo[i].axle_wet;
	}
	debug(Debug_Business, "ԭʼ����:%d, tick=%ld\r\n", sum, Get_Sys_Tick());
	
	if(sum <= 0) 
	{
		//����ƽ������
		Modify_Abnormal_AxleWeight(pCar);
		return;
	}
		
	div = (float)pCar->nBestKg / (float)sum;
	sum = 0;
	for(i = 0; i < pCar->nAxleNum - 1; i++) 
	{
		pCar->AxleInfo[i].axle_wet = div * pCar->AxleInfo[i].axle_wet;

		//�ֶ�ֵ����
		pCar->AxleInfo[i].axle_wet = WetDrvProc(pCar->AxleInfo[i].axle_wet, gWetPar.StaticDivValue);
		//�ֶ�ֵ����
		if(pCar->AxleInfo[i].axle_wet > gWetPar.StaticDivValue) 
			pCar->AxleInfo[i].axle_wet -= gWetPar.StaticDivValue;
			
		sum += pCar->AxleInfo[i].axle_wet;
	}
	
	if(sum > pCar->nBestKg) 
	{
		Modify_Abnormal_AxleWeight(pCar);
		return;
	}
	
	pCar->AxleInfo[pCar->nAxleNum-1].axle_wet = pCar->nBestKg - sum;
}

//�����ͳ�����Ϣ���(�������������)
static void Send_CarInfo(CarInfo *pCar)
{
	uint8 i = 0;
	uint8 TireOK = 0;
	TaskMsg msg = {0};
	GuiMsgInfo gui_msg = {0};
	char datebuf[30] = {0};
	int32 orig_wet = pCar->nBestKg;
	CSysTime *time = Get_System_Time();

	if(pCar == NULL) return;
	if(pCar->bResponse == 1) return;

	pCar->speed = Get_CarSpeed();
	debug(Debug_Business, "����: %d.%dkm/h\r\n", pCar->speed/10, pCar->speed%10);

	memset(&t_carinfo, 0, sizeof(OvertimeData));

	//�쳣������Ϣ����
	Modify_Abnormal_CarInfo(pCar);

	//�ж��Ƿ���
	if(orig_wet > (int)gWetPar.MaxWet)	t_carinfo.ScalerOverWetFlag = 1;
	
	t_carinfo.cmd = pCar->cmd;

	//�ж��Ƿ���
	Charge_Car_OverWeight(pCar, &t_carinfo);

	debug(Debug_Business, "\r\n���ͳ�����Ϣ, %ldms\r\n", Get_Sys_Tick());
	if(pCar->cmd != BackCarCmd) 
	{
		t_carinfo.year = pCar->year = time->year;
		t_carinfo.mon  = pCar->mon  = time->mon;
		t_carinfo.day  = pCar->day  = time->day;
		t_carinfo.hour = pCar->hour = time->hour;
		t_carinfo.min	 = pCar->min  = time->min;
		t_carinfo.sec  = pCar->sec  = time->sec;

		snprintf(datebuf, 29, "*%d-%02d-%02d %02d:%02d:%02d", pCar->year, pCar->mon, pCar->day, 
			pCar->hour, pCar->min, pCar->sec);
		debug(Debug_Business, "%s\r\n", datebuf);

		//���������ֶ�ֵ����
		pCar->nBestKg = WetDrvProc(pCar->nBestKg, gWetPar.StaticDivValue);

		//ҵ���߳���Ϣ��
		msg.msg_id = Msg_CarInfo_Ready;
		//GUI�߳���Ϣ��
		gui_msg.ID = WM_CARIN;
		
		//�������ֶ�
		if(pCar->cmd == LongCarCmd) 
		{
			t_carinfo.LongCarSegs  = pCar->nLongCarSegs;
		}
		//������β״̬
		t_carinfo.CarStatus = pCar->CutOffState;	

		//sdk���������쳣����
		if((pCar->bAckErr == 1) || (pCar->nAxleNum <= 0)) goto send_end;
		
		//��������
		Calc_Car_AxleWet(pCar);

		t_carinfo.TotalWet = pCar->nBestKg;			//����
		t_carinfo.AxleNum  = pCar->nAxleNum;		//����
		t_carinfo.AxleGroupNum = pCar->nAxleNum;	//������

		debug(Debug_Business, "�ϴ�����{wet=%d,axle=%d,speed=%d,%s,%s}\r\n", 	    	
				pCar->nBestKg, pCar->nAxleNum, pCar->speed, 
				Get_Car_LevelName(pCar->WetLevel), Get_Car_StatusName(pCar->nStatus));

		//=====================����=====================
		for(i = 0; i < t_carinfo.AxleNum; i++) 
		{
			t_carinfo.AxleWet[i] = pCar->AxleInfo[i].axle_wet;
		}

		//=====================����=====================
		//����ͳ������ʶ���������Ƿ�����
		//ֻ����������ͨ��ʱ������ʶ�����������Ч�źţ�����Ϊ�乤����������
		TireOK = 1;
		for(i = 0; i < pCar->nAxleNum; i++)
		{
			if((pCar->AxleInfo[i].axle_type != 0) && (pCar->AxleInfo[i].axle_type != 1))
			{
				TireOK = 0;
				break;
			}
		}

		//��һ̥��Ȼ�ǵ�̥
		t_carinfo.AxleType[0] = 1;
		
		//��̥ʶ��������δʶ���������Ϊ���źŲ��ɿ�
		//����ÿ�ֳ��ͷֱ���
		//�������ᳵ********************************************************
		if(pCar->nAxleNum <= 2)
		{
			if(t_carinfo.TotalWet > 5000)
			{
				t_carinfo.AxleType[1] = 2;
			}
			else
			{
				t_carinfo.AxleType[1] = 1; 
			}
		}

		//����3�ᳵ*********************************************************
		else if(pCar->nAxleNum == 3)
		{
			//�ڶ�����������ʶ��
			if(pCar->AxleInfo[0].axle_len < pCar->AxleInfo[1].axle_len)
			{
				t_carinfo.AxleType[1] = 1;
			}
			else
			{
				t_carinfo.AxleType[1] = 2; 
			}

			//��������������ж�
			if(t_carinfo.TotalWet < 4500)
			{
				//������Ĭ��Ϊ˫��
				t_carinfo.AxleType[2] = 1; 
			}
			else
			{
				//������Ĭ��Ϊ˫��
				t_carinfo.AxleType[2] = 2; 
			}
		}

		//�������ᳵ*********************************************************
		else if(pCar->nAxleNum == 4)
		{
			//Ĭ��Ϊ1122
			t_carinfo.AxleType[1] = 1; 
			t_carinfo.AxleType[2] = 2; 
			t_carinfo.AxleType[3] = 2; 
		}

		//����5�ᳵ*********************************************************
		else if(pCar->nAxleNum == 5)
		{
			//�ڶ�����������ʶ��
			if(pCar->AxleInfo[0].axle_len < pCar->AxleInfo[1].axle_len)
			{
				t_carinfo.AxleType[1] = 1;
			}
			else
			{
				t_carinfo.AxleType[1] = 2; 
			}

			//������Ĭ��Ϊ˫��
			t_carinfo.AxleType[2] = 2; 
			t_carinfo.AxleType[3] = 2; 
			t_carinfo.AxleType[4] = 2; 
		}

		//����6�ᳵ*********************************************************
		else if(pCar->nAxleNum >= 6)
		{
			//�ڶ�����������ʶ��
			if(pCar->AxleInfo[0].axle_len < pCar->AxleInfo[1].axle_len)
			{
				t_carinfo.AxleType[1] = 1;
			}
			else
			{
				t_carinfo.AxleType[1] = 2; 
			}

			//������Ĭ��Ϊ˫��
			for(i = 2; i < pCar->nAxleNum; i++) t_carinfo.AxleType[i] = 2;
		}



		//��󲹳�Ӳʶ��
		if(Get_Tire_ErrStatus() && (TireOK == 1))
		{
			//�ڶ�����������Ӳ��ʶ��
			//��ȫ��ȷ�����ʹ�õڶ���Ӳʶ�����
			t_carinfo.AxleType[1] = pCar->AxleInfo[1].axle_type + 1;

			//�����4�ᳵ�����������ʶ��
			if(pCar->nAxleNum == 4)
			{
				t_carinfo.AxleType[2] = pCar->AxleInfo[2].axle_type + 1;
			}
		}

		//===============================================
		//��������������һ��
		for(i = 0; i < t_carinfo.AxleNum; i++)
		{
			t_carinfo.AxleGroupType[i] = t_carinfo.AxleType[i];
		}

		//=====================����=====================
		for(i = 0;i < t_carinfo.AxleNum - 1;i++) 	
		{
			if((pCar->AxleInfo[i].axle_len / 5) > 30000)
			{
				t_carinfo.AxleLen[i] = 30000;
			}
			else
			{
				t_carinfo.AxleLen[i] = pCar->AxleInfo[i].axle_len/5;
			}
			
			if(t_carinfo.AxleLen[i] < 100)
			{
				t_carinfo.AxleLen[i] = 100;		//Ȩֵ0.01
			}
		}
		
		//���ʹ�������
		t_carinfo.SendNum = 0;
		
		//���ٶ�ȡĬ��ֵ
		t_carinfo.speeda = 0;
		t_carinfo.speed = pCar->speed;
		
		//��ӡ����
		for(i = 0; i < pCar->nAxleNum; i++)
		{
			debug(Debug_Business, "����%d: %d\r\n", i, pCar->AxleInfo[i].axle_wet);
		}
		debug(Debug_Business, "\r\n");
		
		//��ӡʵ��̥��
		debug(Debug_Business, "ԭ̥�����: \r\n");
		for(i = 0; i < pCar->nAxleNum; i++)
		{
			debug(Debug_Business, "%d, %d\r\n", pCar->AxleInfo[i].axle_type + 1, pCar->AxleInfo[i].axle_len);
		}
		debug(Debug_Business, "\r\n");

		//��ӡ̥��
		debug(Debug_Business, "��̥�����: \r\n");
		for(i = 0; i < pCar->nAxleNum; i++)
		{
			debug(Debug_Business, "%d, %d\r\n", t_carinfo.AxleType[i], t_carinfo.AxleLen[i]);
		}
		debug(Debug_Business, "\r\n");
	}
	else 
	{
		gui_msg.ID = WM_CARBACK;
		msg.msg_id = Msg_BackCarInfo;
		debug(Debug_Business, "���͵�����Ϣ!\r\n\r\n");
	}	

send_end:
	t_carinfo.SendMod = SendActiveMod;

	//������Ϣ��Ӵ��������ݶ���
#ifdef _SIMULATION_NOPAY_
	if(1)
#else
	if(OverQueue_Enqueue(&t_carinfo) == 0)
#endif
	{
		//��ӳɹ�,�ó����ѷ��ͱ�־(��Ӻ���OverQueue����,CarQueue��Ϊ�ó�������)
		pCar->bResponse = 1;

		//��ҵ�����ݹ����̷߳�����Ϣ
		Task_QPost(&DataManagerMsgQ,  &msg);

		//������ǰ���ڷ��ͼ�����Ϣ,����Ϊ0�Ĳ���ʾ
		if(t_carinfo.AxleNum > 0)
		{
			if(g_pCurWindow != &gWD_Main)
			{
				memcpy(&gMainCarInfo, &t_carinfo, sizeof(OvertimeData));
			}
			else
			{
				gui_msg.pWindow = &gWD_Main;
				gui_msg.wParam = (u32)&t_carinfo;
				GuiMsgQueuePost(&gui_msg);
			}
		}
	}
}

//������������
static void Calc_FollowCar_Wet(CarInfo *pCar)
{
	int32 wet = 0, axlewet = 0;	
	uint8 i = 0;
	
	if(pCar == NULL) return;

	//��������
	axlewet = 0;
	for(i = 0; i < pCar->nAxleNum; i++)
	{
		axlewet += pCar->AxleInfo[i].axle_wet;
	}

	//�����̨������
	wet = pCar->nScalerKgIn - pCar->nScalerKgComing;	//��Ļ�����ͷ�ǰ���̨��̬������ֵ

	//�Ƚ�Ȼ��ȡ�ϴ�ֵ
	if((wet > ((float)axlewet / 1.03f)) || ((wet>3000) && ((wet+500) > axlewet)))
	{
		CarSetBestKg(pCar, wet, LevelMultiCar);
	}
	else 
	{
		CarSetBestKg(pCar, axlewet, LevelMultiCar);
	}

//	debug(Debug_Business, ">��������, ���غ�=%d, ���Ӳ�=%d, tick=%ld\r\n", axlewet, wet, Get_Sys_Tick());

	//�������ᳵ�����ܳ�̨��ֻ��ͣ��һ�����ᳵ
	if((pCar->nAxleNum >= 6) && ((pCar->pPrev == NULL) || ((pCar->pPrev != NULL) && (pCar->pPrev->nAxleNum > 2)))) 
	{
		pCar->nBestKg = Get_Static_MaxWeight();
		if(pCar->nBestKg < 850) pCar->nBestKg = 850;
		pCar->WetLevel = LevelSingleCarStable;

		debug(Debug_Business, ">6�����, ���غ�=%d, ����=%d, tick=%ld\r\n", axlewet, pCar->nBestKg, Get_Sys_Tick());
	}
	else
	{
		debug(Debug_Business, ">��������, ���غ�=%d, ���Ӳ�=%d, tick=%ld\r\n", axlewet, wet, Get_Sys_Tick());
	}
}

//��������������
static void Calc_LongCar_Wet(CarInfo *pCar)
{
	int32 wet = 0, axlewet = 0;	
	uint8 i = 0;
	
	if(pCar == NULL) return;

	//��������
	axlewet = 0;
	for(i = 0; i < pCar->nAxleNum; i++)
	{
		axlewet += pCar->AxleInfo[i].axle_wet;
	}

	//�ж��Ƿ��ܹ�����ֶ�ȡ������������û�����³ӵظ�ʱ�޷��ֶβɼ�
	if((Get_FollowCarMode() != DevideAtOnce) && (pCar->nLongCarHalfAxle != 0) && (pCar->nBackAxleNum == pCar->nLongCarHalfAxle))
	{
		//ǰ�������������
		wet = pCar->nLongCarHalfWet + Get_Static_Weight();
		
		//�Ƚ�Ȼ��ȡ�ϴ�ֵ
		if((wet > ((float)axlewet / 1.12f)) && (wet < ((float)axlewet * 1.25f))) 
		{
			CarSetBestKg(pCar, wet, LevelAutoLongCar);
		} 
		else 
		{
			CarSetBestKg(pCar, axlewet, LevelAutoLongCar);
		}
		//�ٳ���������
		if((TRUE == IsSingleCarComming(pCar))&&(wet > axlewet))
		{
			pCar->nBestKg = wet;
		}
		debug(Debug_Business, ">����������, ���غ�=%d, �ֶκ�=%d, tick=%ld\r\n", axlewet, wet, Get_Sys_Tick());
	}
	else
	{
		//�����̨������
		wet = Get_Static_MaxWeight();	//������ȥ��̨���ֵ��Ϊ��������ֵ

		//�Ƚ�Ȼ��ȡ�ϴ�ֵ
		if((wet > ((float)axlewet / 1.03f)) && (wet < ((float)axlewet * 1.25f))) 
		{
			CarSetBestKg(pCar, wet, LevelAutoLongCar);
		} 
		else 
		{
			CarSetBestKg(pCar, axlewet, LevelAutoLongCar);
		}
		
		//�ٳ���������
		if((TRUE == IsSingleCarComming(pCar))&&(wet > axlewet))
		{
			pCar->nBestKg = wet;
		}

		if((pCar->nAxleNum - pCar->nLongCarHalfAxle) == 0)
		{			
			pCar->nBestKg = (pCar->nLongCarHalfWet > wet) ? pCar->nLongCarHalfWet : wet;
		}

		debug(Debug_Business, ">����������, ���غ�=%d, ���ֵ=%d, tick=%ld\r\n", axlewet, wet, Get_Sys_Tick());
	}
}


static void LongCar_Segs_Business(CarInfo *pCar)
{	
	int num = 0;
	
	if(pCar == NULL) return;

	num = Apply_LongCar_Axle(pCar);
	if(num <= pCar->nLongCarSegAxle)
	{	
		//�����쳣����
		AbnormalAckInfo_To_SDK(LongCarCmd, pCar->CutOffState);
		if(pCar->CutOffState == 1)
		{
			//��Ļ��β,����,ɾ��
			if(pCar->nAxleNum <= 0) CarQueue_Car_Kill(pCar);
		}
		return;
	}
	//��������������
	pCar->nAxleNum = num - pCar->nLongCarSegAxle;
	debug(Debug_Business, "��%d�ֶ�,�ֶ�����%d\r\n", pCar->nLongCarSegs, pCar->nAxleNum);
	
	pCar->bResponse = 0;
	pCar->nBestKg = Get_Static_Weight();
	pCar->cmd = LongCarCmd;
	pCar->WetLevel = LevelLongCar; 
	pCar->bWetVaild = TRUE;

	//���ͷֶ���Ϣ
	Send_CarInfo(pCar);
	//���泵��ʵ�ʵ�����, ��ԭ����
	pCar->nLongCarSegAxle = num;
	pCar->nAxleNum = num;
}

//������β����ʱ�����������жϣ��������ظ�ȷ��������
int Car_Confirm(CarInfo *pCar)
{
	int32 nAxleCount = 0;	
	OvertimeData *pLastCar = NULL;

	if(pCar == NULL) return 2;

	if(pCar->bCarIsDead == TRUE) return 2;

	//��ֹ�ظ�ȷ��
	if(pCar->bConfirm == TRUE) return 0;

	//�жϳ����Ƿ��յ�������Ϣ
	if(pCar->nAlexMsgCount == 0)
	{
		//�����Ǹ����ź�
		debug(Debug_Business, "�ⲿ����!\r\n");
		CarQueue_Car_Kill(pCar);
		return 2;
	}
	
	//���㳵������
	nAxleCount = Apply_Car_Axle(pCar);

	//��ӡ������Ϣ
	debug(Debug_Business, "**��������,������=%d\r\n", nAxleCount);
	
	if(nAxleCount > 0) 
	{
		//�����ϳ�
		pCar->nAxleNum = nAxleCount;
		pCar->bConfirm = TRUE;
		return 0;
	}
	else if(nAxleCount < 0 ) 
	{
		//����
		debug(Debug_Business, "����! \r\n");
		CarQueue_Car_Kill(pCar);		
		pCar->cmd = BackCarCmd;
	
		//ɾ��ǰ��ĳ�Ӧ������ƥ������
		if(pCar->pPrev != NULL)
		{
			if(nAxleCount + pCar->pPrev->nAxleNum <= 2)
			{
				pLastCar = OverQueue_Get_Tail();
				if(pLastCar != NULL)
				{
					//�������ݻ�δ���ͣ��Ҷ�β���ǵ�����Ϣɾ��
					if((pLastCar->cmd != BackCarCmd) && (pLastCar->bSend == 0))
					{		
						OverQueue_Remove_Tail();
						debug(Debug_Business, "ͨ�Ŷ�������,�����͵�����Ϣ!\r\n");
					} 
					else
					{
						pCar->cmd = BackCarCmd;
						pCar->nAxleNum = 2;
						Send_CarInfo(pCar);				//���͵�����Ϣ		
					}
				}
				else
				{
					pCar->cmd = BackCarCmd;
					pCar->nAxleNum = 2;
					Send_CarInfo(pCar);				//���͵�����Ϣ	
				}
				CarQueue_Car_Kill(pCar->pPrev);
			}	
		}
		else
		{
			//�ڹ����������У��շѵ���һ����ȡ�����ݣ��ͻᷢ��ɾ������
			//�����ڵ���ʱ��ǰ��ĳ��Ѿ�ɾ����
			pCar->cmd = BackCarCmd;
			pCar->nAxleNum = 2;
			Send_CarInfo(pCar);				//���͵�����Ϣ		
		}
		return 1;
	} 
	else	
	{		
		debug(Debug_Business, "�ⲿ����!\r\n");
		CarQueue_Car_Kill(pCar);		//�ⲿ����	
		return 2;
	}			
}

//��ʱȡ����
void Car_Confirm_Wet(CarInfo *pCar)
{
	if(pCar == NULL) return;

	if((pCar->bWetVaild == FALSE) && (pCar->WetLevel >= LevelMultiCar))
	{
		debug(Debug_Business, "��ʱȡ����:%d\r\n", pCar->nBestKg);

		pCar->bWetVaild = TRUE;

		Send_CarInfo(pCar);
	}
}


//==========================================������״̬��Ӧ����=====================================

//��������״̬��Ӧ�ⲿ�¼�������
static void Car_Business_CarComState(CarInfo *pCar, TaskMsg *pMsg)
{	
	AlexRecoder *tmpAxle = NULL;
	
	if((pCar == NULL) || (pMsg == NULL)) return;
	
	switch(pMsg->msg_id) 
	{
		//����
		case Msg_Tick:
			pCar->nStatusKeepTime += CAR_TICK;
			pCar->nLiveTime += CAR_TICK;
			break;

		//�ᵽ����Ϣ
		case Msg_PreAlex_Come:
			if(pCar->nAlexMsgCount == 0)
			{
				//��¼��ʼ����
				pCar->nScalerKgComing = Get_Static_Weight();
				pCar->nScalerKgDyncComing = Get_Dynamic_Weight();
				if((pCar->nScalerKgComing < SCALER_MIN_KG) || (pCar->nScalerKgDyncComing < SCALER_MIN_KG))
				{
					debug(Debug_Business, "��������, ��̨����=%d\r\n", Get_Dynamic_Weight());
				}

				//�������жϸ�λ
				LongCarReset();
			}
			pCar->nAlexMsgCount++;
			break;

		//����
		case Msg_PreAlex_Add:
			tmpAxle = (AlexRecoder *)pMsg->msg_ctx;
			if(pCar->nAxleNum < MAX_AXLE_NUM)
			{
				if(tmpAxle != NULL)
				{
					if((pCar->nAxleNum >= 0) && (pCar->nAxleNum <= gWetPar.nMaxAxleNum))
					{
						pCar->AxleInfo[pCar->nAxleNum].axle_wet = tmpAxle->AlexKg;
						pCar->AxleInfo[pCar->nAxleNum].axle_peak = tmpAxle->AlexKgRef;
						pCar->AxleInfo[pCar->nAxleNum].axle_type = tmpAxle->bDouble;
					}
				}


				//�������жϸ�λ
				if((pCar->nAxleNum == 0) && (pCar->nAlexMsgCount == 1))
				{
					LongCarReset();
				}

				//��¼����
				pCar->nAxleNum++;
			}
	
			Clear_ScalerMaxWeight();
			break;

		//����
		case Msg_PreAlex_Remove:
			pCar->nAxleNum--;
			Clear_ScalerMaxWeight();
			break;
	
		//��Ļ�ͷ�
		case Msg_LC2_Leave:
			pCar->CutOffState = 1;

			//��stCarEnter״̬ǰ�����̨���ֵ��¼�����¼���
			//Clear_ScalerMaxWeight();
		

			//�л���stCarEnter״̬
			FSM_Change_CarState(pCar, stCarEnter);
			break;

		//������ظ�
		case Msg_GC2_Enter:
			//����������
			if(Get_LongCar_Mode() == AutoLongCarMode) 
			{
				//����Ҫ��һ��������ǳ�������
				if(pCar->nAlexMsgCount <= 0) break;

				if(CarQueue_Get_OnScaler_Count() == 1)
				{
					//��¼��ǰ�����������������
					pCar->nLongCarHalfWet = Get_Static_Weight();
					pCar->nLongCarHalfAxle = Apply_LongCar_Axle(pCar);
					
					//��������״̬ǰ�����̨���ֵ��¼�����¼���
					Clear_ScalerMaxWeight();

					//�л���������״̬
					FSM_Change_CarState(pCar, stLongCar);
				}
			}
			break;

		//�����ź�
		case Msg_BackAlex_Come:	
			if(Get_LongCar_Mode() == AutoLongCarMode) 
			{
				//����Ҫ��һ��������ǳ�������
				if(pCar->nAlexMsgCount <= 0) break;

				if(CarQueue_Get_OnScaler_Count() == 1) 
				{			
					//��¼��ǰ�����������������
					pCar->nLongCarHalfWet = Get_Static_Weight();
					pCar->nLongCarHalfAxle = Apply_LongCar_Axle(pCar);
					
					//�л���������״̬
					FSM_Change_CarState(pCar, stLongCar);
				}
			}
			break;

		//�ֶβɼ�
		case Msg_Cmd_PlusSave:
			pCar->nLongCarSegs = pMsg->msg_ctx;
			LongCar_Segs_Business(pCar);
			break;

		case Msg_At_Turn:
			//��¼��������ʱ������
			pCar->nFollowAxleNum = pCar->nAxleNum;
			pCar->bFollowToSingle = 1;
			break;

		default:
			break;
	}
}

int GetDyncAvgValue(CarInfo *pCar)
{
	//Ĭ��1200kg��С��
	if(pCar == NULL) return 1200;

	if(pCar->bDyncMinOk == FALSE)
	{
		return pCar->fDyncMaxKg;
	}

	if((pCar->fDyncMaxKg - pCar->fDyncMinKg)*1/8 > (pCar->fDyncMaxLastKg - pCar->fDyncMinKg))
	{
		return pCar->fDyncMaxKg;
	}

	return pCar->fDyncAvgKg;
}

void Car_Dync_Kg_Update(CarInfo *pCar)
{
	if(pCar == NULL) return;

	if(pCar->nStatusKeepTime <= 850)
	{
		//��ȡ��һ��������ֵ
		if(pCar->fDyncMaxKg <= Get_Static_Weight())
		{
			pCar->fDyncMaxKg = Get_Static_Weight();
		}		
		
		pCar->fDyncMinKg = pCar->fDyncMaxKg;
		pCar->fDyncAvgKg = 0.0f;
		pCar->fDyncSumKg = 0.0f;
		pCar->fDyncMaxLastKg = 0.0f;
		pCar->nDyncKgIndex = FALSE;
		pCar->nDyncKgCount = FALSE;
		pCar->bDyncMaxOk = FALSE;
		pCar->bDyncMinOk = FALSE;
		pCar->bDyncValid = FALSE;
		pCar->bDyncRaise = FALSE;
		return;
	}

	if((pCar->fDyncMaxKg <= Get_Static_Weight()) && (pCar->bDyncMaxOk == FALSE))
	{
		if(pCar->fDyncMaxKg < Get_Static_Weight())
		{
			pCar->fDyncMaxKg = Get_Static_Weight();
			pCar->fDyncMinKg = pCar->fDyncMaxKg;
			pCar->fDyncSumKg = 0.0f;
			pCar->fDyncAvgKg = pCar->fDyncMaxKg * 0.999f;
			pCar->nDyncKgIndex = 0;
			pCar->nDyncKgCount = 0;
		}
		else
		{
			pCar->fDyncSumKg += Get_Static_Weight();
			pCar->nDyncKgIndex++;
		}
	}
	else
	{		
		if(pCar->bDyncMinOk == FALSE)
		{
			pCar->bDyncMaxOk = TRUE;

			if(pCar->fDyncMinKg >= Get_Static_Weight())
			{
				pCar->fDyncMinKg = Get_Static_Weight();
				pCar->fDyncSumKg += Get_Static_Weight();
				pCar->nDyncKgIndex++;				
			}
			else
			{
				//pCar->fDyncAvgKg = (float)(pCar->fDyncSumKg) / pCar->nDyncKgIndex;	
				pCar->fDyncAvgKg = (pCar->fDyncMaxKg + pCar->fDyncMinKg) / 2;
				pCar->fDyncMaxLastKg = pCar->fDyncMinKg;
				pCar->bDyncMinOk = TRUE;
			}	
		}
	}

	//��¼������Сֵ֮�󣬺�������ֵ���жϲ���
	if(pCar->bDyncMinOk == TRUE)
	{
		if(pCar->fDyncMaxLastKg <= Get_Static_Weight())
		{
			pCar->fDyncMaxLastKg = Get_Static_Weight();
		}	
	}
}

//��������״̬
extern int IsAlexHold(void);
extern BOOL IsAtTurn(void);
static void Car_Business_CarEnterState(CarInfo *pCar, TaskMsg *pMsg)
{
	//int CarWet = 0;
	int nCarMaxWet = 0;
	//int poise = 0;
	AlexRecoder *tmpAxle = NULL;
	TaskMsg msg = {0};
	
	if((pCar == NULL) || (pMsg == NULL)) return;
	
	switch(pMsg->msg_id)
	{
		case Msg_Tick:
			pCar->nStatusKeepTime += CAR_TICK;
			pCar->nLiveTime += CAR_TICK;

			//�ڵ����趨ʱ��֮ǰ��ֵ
			if(pCar->nStatusKeepTime <= nCarWetMaxTick)
			{
				Car_Dync_Kg_Update(pCar);
			}

			if(pCar->nStatusKeepTime >= nCarWetMaxTick)
			{
				//�ж���ʶ�����Ƿ��Ѿ��ͷ�
				if(IsAlexHold() > 0)
				{
					nCarWetMaxTick += 50;
					break;
				}

				//��λ��Ĭ��ֵ��������ͬʱ��Enter״̬ʱ����������
				nCarWetMaxTick = 2000;

				//��ӡ����ȷ����Ϣ
				CarComfirm_Print();

				//�ֳ������ж��Ƿ�Ϊ�����ϳ������Ż򵹳������Ϊ0��ʾ�����ϳ�
				if(Car_Confirm(pCar) != 0) 
				{
					//ˢ�¶�����Ϣ
					msg.msg_id = Msg_Refresh;
					Task_QPost(&BusinessMsgQ, &msg);
					break;
				}

				if(IsSingleCarComming(pCar))
				{
					//����������
					if((IsLongCar() == TRUE) && (Get_LongCar_Mode() == AutoLongCarMode))
					{
    					pCar->nScalerKgIn = Get_Static_Weight();
    					Calc_LongCar_Wet(pCar);
					}

					//��������
					else
					{
						//�����ϳ�ȡֵ
						pCar->nScalerKgIn = Get_Static_Weight();
						nCarMaxWet = Get_Static_MaxWeight();
						//������������
						if(nCarMaxWet < 400)
						{
							debug(Debug_Business, "������ֵ:%d,̫Сɾ��!\r\n", nCarMaxWet);
							CarQueue_Car_Kill(pCar);
							break;
						}	
#if 1
						//CarSetBestKg(pCar, nCarMaxWet, LevelSingleCarStable);
						//debug(Debug_Business, "ȡ������ֵ:%d\r\n", nCarMaxWet);
		
						
						CarSetBestKg(pCar, GetDyncAvgValue(pCar), LevelSingleCarStable);
						debug(Debug_Business, "ȡ����ƽ��ֵ:%d\r\n", (int)(GetDyncAvgValue(pCar)));
#else						
						//���ݳ���������������
						poise = 500;
						if(nCarMaxWet > 30000) poise = 1000;
						
						if(Get_Static_Weight() < nCarMaxWet - poise) 
						{
						CarSetBestKg(pCar, nCarMaxWet, LevelSingleCarStable);

						debug(Debug_Business, "ȡ������ֵ:%d\r\n", nCarMaxWet);
						} 
						else 
						{
						CarSetBestKg(pCar, Get_Static_Weight(), LevelSingleCarStable);

						debug(Debug_Business, "ȡ������ǰ:%d  ��ֵ:%d\r\n", pCar->nBestKg, nCarMaxWet);
						}
#endif
						//�����ٶȲ�����������
						Weight_Dynamic_Modify(pCar);
					}

					pCar->bWetVaild = TRUE;
				}
				else
				{
					//��ȡ�����ĳ�̨����
					pCar->nScalerKgIn = Get_Static_Weight();
					
					//��ֹ��Ļ��β��ǰ���Ѿ���ʼ�³�
					nCarMaxWet = Get_Static_MaxWeight();
					if(nCarMaxWet > pCar->nScalerKgIn + 50)
					{
						pCar->nScalerKgIn = nCarMaxWet;
					}

					//�����������
    				Calc_FollowCar_Wet(pCar);

					//���³ӷֳ�ģʽ���ж��Ƿ��е�������
					if(Get_FollowCarMode() != DevideAtOnce)
					{
						//�ж��Ƿ��л���ȡ��������
						if((pCar->pPrev == NULL) || ((pCar->pPrev != NULL) && (pCar->pPrev->nStatus == stCarFarPay)))
						{
							//�����ڳ��ϣ�Ҫ��ǰ�������뿪��һ��������
							if(IsAtTurn() == TRUE)
							{	
								if(Charge_FollowToSingle_Business(pCar, LevelSingleCarBeginDown) == FALSE)
								{
									CarSetBestKg(pCar, Get_Static_Weight(), LevelSingleCarBeginDown);
								}
								pCar->bWetVaild = TRUE;

								debug(Debug_Business, "ȡ��������:%d\r\n", pCar->nBestKg);
							}
						}
					}
    				
					//������ȡֵ��ȡ����ȡֵ��ʽʱ�������ϴ�����
					if(Get_FollowCarMode() == DevideAtOnce)
					{
						pCar->bWetVaild = TRUE;
					}
					else
					{
						//���С��ȡֵΪ�����ϴ�
						if(Get_LittleFollowCarMode() == DevideAtOnce)
						{
							if(pCar->nBestKg < 3000)
							{
								if(pCar->pPrev != NULL)
								{
									//��Ҫ�ж�ǰ���Ѿ��ϴ�����
									if(pCar->pPrev->bResponse == 1) pCar->bWetVaild = TRUE;
								}
								else
								{
									pCar->bWetVaild = TRUE;
								}
							}
						}
					}
				}

				//�л�����һ��״̬
				if(pCar->nNextStatus == stCarWaitPay)
				{
					//��������������ڻ�û�е���1200ms��ظоͱ�����

					//�ڷ��ϳӷֳ�ģʽ�£��ж�ǰ��ĳ��Ƿ��Ѿ��뿪��
					if((pCar->bWetVaild != TRUE) && (Get_FollowCarMode() != DevideAtOnce))
					{
						if(CarQueue_Car_Is_Head(pCar))
						{
							pCar->nScalerKgIn = Get_Static_Weight();
							nCarMaxWet = Get_Static_MaxWeight();
							if(nCarMaxWet > pCar->nScalerKgIn + 50)
							{
								pCar->nScalerKgIn = nCarMaxWet;
							}
						
							if(Charge_FollowToSingle_Business(pCar, LevelSingleCarBeginDown) == FALSE)
							{
								CarSetBestKg(pCar, Get_Static_Weight(), LevelSingleCarBeginDown);
							}
							debug(Debug_Business, "ȡ��������:%d\r\n", nCarMaxWet);
						}
					}

					//ת����ɷ�ģʽǰһ��Ҫ�ϴ�������
					pCar->bWetVaild = TRUE;

					//�л���ָ��״̬
					pCar->nNextStatus = stCarNone;

#ifdef _SIMULATION_NOPAY_
					FSM_Change_CarState(pCar, stCarLeaving);
#else
					FSM_Change_CarState(pCar, stCarWaitPay);
#endif
				}
				else
				{
					FSM_Change_CarState(pCar, stCarInScaler);
				}

				//��ӡ������Ϣ
				CarQueue_Print_Info();

				//�ϴ�������Ϣ
				if(pCar->bWetVaild == TRUE)
				{
    				Send_CarInfo(pCar);
				}
			}
			break;

		//�ᵽ����Ϣ
		case Msg_PreAlex_Come:
			if(pCar->nAlexMsgCount == 0)
			{
				//��¼��ʼ����
				pCar->nScalerKgComing = Get_Static_Weight();
				pCar->nScalerKgDyncComing = Get_Dynamic_Weight();
				if((pCar->nScalerKgComing < SCALER_MIN_KG) || (pCar->nScalerKgDyncComing < SCALER_MIN_KG))
				{
					debug(Debug_Business, "��������, ��̨����=%d\r\n", Get_Dynamic_Weight());
				}					
			}
			pCar->nAlexMsgCount++;
			break;

		//����
		case Msg_PreAlex_Add:
			
			//����Ҫ��������Ϣ�Ž��к���������ֹ��ǰһ����©�������ź�
			if(pCar->nAlexMsgCount == 0) break;

			tmpAxle = (AlexRecoder *)pMsg->msg_ctx;
			if(pCar->nAxleNum < MAX_AXLE_NUM)
			{
				if(tmpAxle != NULL)
				{
					if((pCar->nAxleNum >= 0) && (pCar->nAxleNum <= gWetPar.nMaxAxleNum))
					{
						pCar->AxleInfo[pCar->nAxleNum].axle_wet = tmpAxle->AlexKg;
						pCar->AxleInfo[pCar->nAxleNum].axle_peak = tmpAxle->AlexKgRef;
						pCar->AxleInfo[pCar->nAxleNum].axle_type = tmpAxle->bDouble;
					}
				}

				pCar->nAxleNum++;
			}
			
			Clear_ScalerMaxWeight();
			break;

		//����
		case Msg_PreAlex_Remove:

			//����Ҫ��������Ϣ�Ž��к���������ֹ��ǰһ����©�������ź�
			if(pCar->nAlexMsgCount == 0) break;

			pCar->nAxleNum--;
			Clear_ScalerMaxWeight();
			break;

		//�󳵴�����Ļ
		case Msg_LC2_Enter:

			//ȷ�ϳ�������
			//���ﲻ��ȷ�ϣ��п������һ�����źŻ�û�ͷ�
			//ϵͳ���¼���ʱ�̸ĵ��˺�һ������һ������ʱ
			//Car_Confirm(pCar);
			
			//��ʱ���ı�״̬���ȵ�1200ms����Ȼת��
		    break;
		
		//�����ź�
        case Msg_BackAlex_Come:
			//ָʾ1200ms��ʱ��ת����ɷ�״̬
			pCar->nNextStatus = stCarWaitPay;
			break;

		//�³ӵظд���
		case Msg_GC2_Enter:
			//ָʾ1200ms��ʱ��ת����ɷ�״̬
			pCar->nNextStatus = stCarWaitPay;
			break;

		//�����ź�
        case Msg_BackAlex_Add:
			tmpAxle = (AlexRecoder *)pMsg->msg_ctx;
			pCar->nDownAlexMaxKg = tmpAxle->AlexKgRef;

			pCar->nBackAxleNum++;
			if(pCar->nBackAxleNum >= pCar->nAxleNum)
			{
				//������ڽ���״̬�º���������ˣ����ǽ�û����
                //FSM_Change_CarState(pCar, stCarFarPay);
			}
			break;

		//�³ӵظ��ͷţ��쳣�źţ�
		case Msg_GC2_Leave:
			//��ظд����ź��п����Ǹ����ź�
			pCar->nNextStatus = stCarNone;
			break;

		//�ֶβɼ�
		case Msg_Cmd_PlusSave:
			pCar->nLongCarSegs = pMsg->msg_ctx;
			LongCar_Segs_Business(pCar);
			break;

		default:
			break;
	}		
}

//������ȫ�ϳ�״̬��Ӧ�ⲿ�¼�������
static void Car_Business_CarInScalerState(CarInfo *pCar, TaskMsg *pMsg)
{
	if((pCar == NULL) || (pMsg == NULL)) return;
	
	switch(pMsg->msg_id) 
	{
		//����
		case Msg_Tick:
			pCar->nStatusKeepTime += CAR_TICK;
			pCar->nLiveTime += CAR_TICK;
			break;
  
		//�³��ᴥ�����ظд���
		case Msg_BackAlex_Come:
		case Msg_GC2_Enter:
			//����ó������һ��������������ûȷ�ϣ������ȡ����������
			if(pCar->bWetVaild == FALSE)
			{
				if((pCar->pNext != NULL) && (pCar->pNext->nAlexMsgCount > 0))
				{
					debug(Debug_Business, "ȡ��������:%d\r\n", pCar->nBestKg);
				}
				else
				{
					//�����ڳ���
					if(Charge_FollowToSingle_Business(pCar, LevelSingleCarBeginDown) == FALSE)
					{
						CarSetBestKg(pCar, Get_Static_Weight(), LevelSingleCarBeginDown);
					}
					debug(Debug_Business, "ȡ�����³�����:%d\r\n", pCar->nBestKg);
				}

				//����ȡ����������ʽ����������Ϊ��Ч���ϴ�
				pCar->bWetVaild = TRUE;
				Send_CarInfo(pCar);
			}

#ifdef _SIMULATION_NOPAY_
			FSM_Change_CarState(pCar, stCarLeaving);
#else
			FSM_Change_CarState(pCar, stCarWaitPay);
#endif
			break;

		//�Ŷ�ʱ�䵽
		case Msg_At_Turn:
			//����ó������һ��������������ûȷ�ϣ������ȡ����������
			if(pCar->bWetVaild == FALSE)
			{
				if((pCar->pNext != NULL) && (pCar->pNext->nAlexMsgCount > 0))
				{
					debug(Debug_Business, "ȡ��������:%d\r\n", pCar->nBestKg);
				}
				else
				{
					//�����ڳ���
					if(Charge_FollowToSingle_Business(pCar, LevelSingleCarBeginDown) == FALSE)
					{
						CarSetBestKg(pCar, Get_Static_Weight(), LevelSingleCarBeginDown);
					}
					debug(Debug_Business, "ȡ�����Ŷ�����:%d\r\n", pCar->nBestKg);
				}

				//����ȡ����������ʽ����������Ϊ��Ч���ϴ�
				pCar->bWetVaild = TRUE;
				Send_CarInfo(pCar);
			}			
			
			break;

		//�󳵴�����Ļ
		case Msg_LC2_Enter:
			if(pCar->bWetVaild == FALSE)
			{
				//����ó��ǵ�һ����������ȡ��̨������Ϊ��������
				if(CarQueue_Car_Is_Head(pCar) || ((pCar->pPrev != NULL) && (pCar->pPrev->nStatus == stCarFarPay)))
				{
					if(Charge_FollowToSingle_Business(pCar, LevelSingleCarBy) == FALSE)
					{
						CarSetBestKg(pCar, Get_Static_Weight(), LevelSingleCarBy);
					}
					debug(Debug_Business, "ȡ����δ������:%d!\r\n", pCar->nBestKg);
					pCar->bWetVaild = TRUE;
					Send_CarInfo(pCar);					
				}
				else
				{
					debug(Debug_Business, "ȡ��������:%d!\r\n",  pCar->nBestKg);
					pCar->bWetVaild = TRUE;
					Send_CarInfo(pCar);
				}
			}
		  break;

		//�ɷ�֪ͨ
		case Msg_Cmd_PayNotify:
			if(Get_FollowCarMode() == DevideAtOnce)
			{
				CarQueue_Car_Kill(pCar);
			}
			else
			{
				Set_Car_PayOutTick(pCar);
				ResetPayTimeOutTick();

				FSM_Change_CarState(pCar, stCarLeaving);
			}
			break;

		//�ֶβɼ�
		case Msg_Cmd_PlusSave:
			pCar->nLongCarSegs = pMsg->msg_ctx;
			LongCar_Segs_Business(pCar);
			break;

		default:
			break;
	}
}

//�����ȴ��շ�״̬��Ӧ�ⲿ�¼�������
static void Car_Business_CarWaitPayState(CarInfo *pCar, TaskMsg *pMsg)
{
	AlexRecoder *tmpAxle = NULL;

	if((pCar == NULL) || (pMsg == NULL)) return;
	
	switch(pMsg->msg_id) 
	{
		//����
		case Msg_Tick:
			pCar->nStatusKeepTime += CAR_TICK;
			pCar->nLiveTime += CAR_TICK;
			break;

		//�ɷ�֪ͨ
		case Msg_Cmd_PayNotify:
			if(Get_FollowCarMode() == DevideAtOnce)
			{
				CarQueue_Car_Kill(pCar);
			}
			else
			{
				Set_Car_PayOutTick(pCar);
				ResetPayTimeOutTick();

				FSM_Change_CarState(pCar, stCarLeaving);
			}
			break;

		//��ظ��ͷ�
       	case Msg_GC2_Leave:
			//if(pCar->nBackAxleNum > 1)	//�п�����ǰһ�����뿪��ظе��ź�
			if(pCar->nBackAxleNum > (int8)(pCar->nAxleNum/2))
			{
				ResetTurnTick();
       			FSM_Change_CarState(pCar, stCarFarPay); 
			}
			break;
		
		//�³ӽ���
		case Msg_BackAlex_Add:
			tmpAxle = (AlexRecoder *)pMsg->msg_ctx;
			pCar->nDownAlexMaxKg = tmpAxle->AlexKgRef;

			pCar->nBackAxleNum++;
			if(pCar->nBackAxleNum >= pCar->nAxleNum)
			{
				ResetTurnTick();
                FSM_Change_CarState(pCar, stCarFarPay);
			}
            break;

		//�³�����
		case Msg_BackAlex_Remove:
			if(pCar->nBackAxleNum > 0) pCar->nBackAxleNum--;

			if(pCar->nBackAxleNum == 0)
			{
                FSM_Change_CarState(pCar, stCarInScaler);
			}
            break;   

        //�ֶβɼ�
		case Msg_Cmd_PlusSave:
			pCar->nLongCarSegs = pMsg->msg_ctx;
			LongCar_Segs_Business(pCar);
			break;

		default:
			break;
	}
}

//�����ɷ��뿪״̬��Ӧ�ⲿ�¼�������
static void Car_Business_CarLeave(CarInfo *pCar, TaskMsg *pMsg)
{
	AlexRecoder *tmpAxle = NULL;

	if((pCar == NULL) || (pMsg == NULL)) return;
	
	switch(pMsg->msg_id) 
	{
		//����
		case Msg_Tick:
			pCar->nStatusKeepTime += CAR_TICK;
			pCar->nLiveTime += CAR_TICK;	
			break;

		//��ظ�
		case Msg_GC2_Leave: 
			
			//���ʱ���п����Ǻ�ظ���˸���п������³���������˭�����Ŷȸߣ�

#if(1)//���˺�ظ�

			//�����ظ����Ŷȸ�pCar->nBackAxleNum�������࣬��ǰɾ����
			if(pCar->nBackAxleNum > (int8)(pCar->nAxleNum/2))
			{
				ResetTurnTick();
				ClosePayTimeOut();
				CarQueue_Car_Kill(pCar);
			}
			else
			{
				//��������ǰһ��������û����
				pCar->nBackAxleNum = 0;
			}

#else
			//�����ʶ�������Ŷȸ�
			if(pCar->nBackAxleNum >= pCar->nAxleNum)
			{
				ResetTurnTick();
				ClosePayTimeOut();
				CarQueue_Car_Kill(pCar);
			}
#endif
			break;

		//�³ӽ���
		case Msg_BackAlex_Add:
			tmpAxle = (AlexRecoder *)pMsg->msg_ctx;
			pCar->nDownAlexMaxKg = tmpAxle->AlexKgRef;

			pCar->nBackAxleNum++;
			ResetPayTimeOutTick();
			if(pCar->nBackAxleNum >= pCar->nAxleNum)
			{
			   ResetTurnTick();
			   ClosePayTimeOut();
               CarQueue_Car_Kill(pCar);
			}
            break;    

		//�³�����
		case Msg_BackAlex_Remove:
#ifndef _SIMULATION_NOPAY_			
			//�뿪״̬�ĳ��ѽɷѣ������ź���Ϊ�ǽ��ᱻ����
			pCar->nBackAxleNum++;
			ResetPayTimeOutTick();
			if(pCar->nBackAxleNum >= pCar->nAxleNum)
			{
				ResetTurnTick();
				ClosePayTimeOut();
				CarQueue_Car_Kill(pCar);
			}
#else
			if(pCar->nBackAxleNum > 0) pCar->nBackAxleNum--;
			if(pCar->nBackAxleNum == 0)
			{
		       FSM_Change_CarState(pCar, stCarInScaler);
			}
#endif
		  break;  

		//�ɷ�֪ͨ
        case Msg_Cmd_PayNotify:
			//��ظй��ϵ��쳣��Ӧ
			ResetTurnTick();
			CarQueue_Car_Kill(pCar);
			
			//�ú�������
			if(pCar->pNext != NULL) 
			{
				debug(Debug_Business, "��ظ��쳣, �������뿪, �ɷ�֪ͨ����!\r\n");

				//�ú���һ������Ӧ�ɷ���Ϣ��һ����˵����ʱ����һ�������ڳ���״̬
				FSM_CarFunction(pCar->pNext, pMsg);
			}
			break;

		//�ֶβɼ�
		case Msg_Cmd_PlusSave:
			AbnormalAckInfo_To_SDK(LongCarCmd, pCar->CutOffState);
			break;

		default:
			break;
	}
}

//������ӽɷ�״̬��Ӧ�ⲿ�¼�������
static void Car_Business_CarFarPay(CarInfo *pCar, TaskMsg *pMsg)
{
	if((pCar == NULL) || (pMsg == NULL)) return;
	
	switch(pMsg->msg_id) 
	{
		//����
		case Msg_Tick:
			pCar->nStatusKeepTime += CAR_TICK;
			pCar->nLiveTime += CAR_TICK;
			break;

		//�ɷ�֪ͨ
		case Msg_Cmd_PayNotify:
			CarQueue_Car_Kill(pCar);
			break;

		//�ֶβɼ�
		case Msg_Cmd_PlusSave:
			pCar->nLongCarSegs = pMsg->msg_ctx;
			LongCar_Segs_Business(pCar);
			break;

		default:
			break;
	}
}


//����״̬��Ӧ�ⲿ�¼�������
static void Car_Business_LongCarState(CarInfo *pCar, TaskMsg *pMsg)
{
	AlexRecoder *tmpAxle = NULL; 
	
	if((pCar == NULL) || (pMsg == NULL)) return;
	
	switch(pMsg->msg_id) 
	{
		//����
		case Msg_Tick:
			pCar->nStatusKeepTime += CAR_TICK;
			pCar->nLiveTime += CAR_TICK;
			break;

		//��Ļ�ͷ�
		case Msg_LC2_Leave:
			if(Get_LongCar_Mode() == AutoLongCarMode) 
			{
				//�����Զ�����ģʽ
				pCar->nScalerKgIn = Get_Static_Weight();
				
				//������
				pCar->nAxleNum = Apply_LongCar_Axle(pCar);
				if(pCar->nAxleNum <= 0)
				{
					debug(Debug_Business, "��������������0, ��Ϊ����!\r\n");
					CarQueue_Car_Kill(pCar);
					break;
				}
				pCar->CutOffState = 1;

				//�㳬��������
    			Calc_LongCar_Wet(pCar);
				pCar->bWetVaild = TRUE;
    				
				//�����ϴ�����
				Send_CarInfo(pCar);

				//�л������ɷ�״̬
#ifdef _SIMULATION_NOPAY_
				FSM_Change_CarState(pCar, stCarLeaving);
#else
				FSM_Change_CarState(pCar, stCarWaitPay);
#endif
			} 
			else
			{
				debug(Debug_Business, "�ֶγ�������β!\r\n");
				pCar->nAxleNum = Apply_LongCar_Axle(pCar);
				if(pCar->nAxleNum <= 0) 
				{
					CarQueue_Car_Kill(pCar);
					break;
				}

				pCar->CutOffState = 1;
			}
			break;
		
		//����
		case Msg_PreAlex_Add:
			tmpAxle = (AlexRecoder *)pMsg->msg_ctx;
			if(pCar->nAxleNum < MAX_AXLE_NUM)
			{
				if(tmpAxle != NULL)
				{
					if((pCar->nAxleNum >= 0) && (pCar->nAxleNum <= gWetPar.nMaxAxleNum))
					{
						pCar->AxleInfo[pCar->nAxleNum].axle_wet = tmpAxle->AlexKg;
						pCar->AxleInfo[pCar->nAxleNum].axle_peak = tmpAxle->AlexKgRef;
						pCar->AxleInfo[pCar->nAxleNum].axle_type = tmpAxle->bDouble;
					}
				}

				pCar->nAxleNum++;
			}
			break;

		//����
		case Msg_PreAlex_Remove:
			pCar->nAxleNum--;
			break;

		//����ʶ����
		case Msg_BackAlex_Add:
			tmpAxle = (AlexRecoder *)pMsg->msg_ctx;
			pCar->nDownAlexMaxKg = tmpAxle->AlexKgRef;

			pCar->nBackAxleNum++;
			//if(pCar->nBackAxleNum >= pCar->nAxleNum)
			//{
			//	ResetTurnTick();
            //    FSM_Change_CarState(pCar, stCarFarPay);
			//}
            break;

		//�³�����
		case Msg_BackAlex_Remove:
			if(pCar->nBackAxleNum > 0) pCar->nBackAxleNum--;

			if(Get_LongCar_Mode() == AutoLongCarMode)
			{
				if(pCar->nBackAxleNum == 0)
				{
					FSM_Change_CarState(pCar, stCarComing);
				}
			}
            break;    

		//�����ɷ�֪ͨ�¼��������Զ�ģʽ���쳣, �ֶ�ģʽ��������
		case Msg_Cmd_PayNotify:
		    FSM_Change_CarState(pCar, stCarLeaving);
			break;

		//�ظ��ͷ���Ϣ���쳣ʱ�����յ���
		case Msg_GC2_Leave: 
			//��ظ��ͷ�,��Ļһ�������ڵ���
			if(Get_LC_Status(2))
			{
				//������״̬�¹�Ļ��Ȼ���ڵ���
				if(pCar->pNext == NULL)
				{
					//����û������Ϊ�ǵظ����ź�
					FSM_Change_CarState(pCar, stCarComing);

					debug(Debug_Business, "!!�쳣������, ��ظ��ͷ�\r\n");
				}
				else
				{
					//δ֪�쳣�������г�
					FSM_Change_CarState(pCar, stCarComing);

					debug(Debug_Business, "!!�쳣������, �����ܺ����г�\r\n");
				}
			}
			else
			{
				//δ֪�쳣����Ļû�ڵ�
				FSM_Change_CarState(pCar, stCarComing);

				debug(Debug_Business, "!!�쳣������, �����ܹ�Ļδ�ڵ�\r\n");
			}

#ifdef _SIMULATION_NOPAY_
			CarQueue_Car_Kill(pCar);
#endif
			break;

		//�ֶβɼ�(δ���)
		case Msg_Cmd_PlusSave:
			pCar->nLongCarSegs = pMsg->msg_ctx;
			LongCar_Segs_Business(pCar);
			break;
            
		default:
			break;
	}	
}





//empty function
static void Car_Business_None(CarInfo *pCar, TaskMsg *pMsg)
{
	return;
}

struct _BusinessArray 
{
	uint8 state;
	CarBusinessFun fun;
};

static const struct _BusinessArray BusinessArray[] = 
{
	{stCarNone, 	Car_Business_None},
	{stCarComing, 	Car_Business_CarComState},
	{stCarEnter, 	Car_Business_CarEnterState},	
	{stCarInScaler, Car_Business_CarInScalerState},
	{stCarWaitPay,  Car_Business_CarWaitPayState},
	{stLongCar,		Car_Business_LongCarState},
	{stCarLeaving,	Car_Business_CarLeave},
	{stCarFarPay,	Car_Business_CarFarPay}
};

void FSM_Change_CarState(CarInfo *pCar, int32 state)
{
	if(pCar == NULL)
		return;

	if(pCar->bCarIsDead == TRUE)
		return;

	if(pCar->nStatus == state) 
		return;

	if(state > stCarFarPay)
	   	return;

	pCar->nPreStatus = pCar->nStatus;
	pCar->nStatus = state;
	pCar->nStatusKeepTime = 0;
	pCar->CarFSMFun = BusinessArray[state].fun;
}

//�ظ��ɼ�
void Car_Business_RepeatCmd(CarInfo *pCar, TaskMsg *msg) 
{
	CarInfo *tCar = pCar;
	
	if(tCar != NULL) 
	{
		if(tCar->nStatus == stCarLeaving)
		{
			if(tCar->pNext == NULL)
			{
				//����ֻ��һ�������Ѿ�����
				AbnormalAckInfo_To_SDK(RepeatCarCmd, 0);
				return;
			}
			tCar = tCar->pNext;
		}
		
		if(tCar->nStatus <= stCarEnter)
		{
			AbnormalAckInfo_To_SDK(RepeatCarCmd, 0);
			return;
		}
		tCar->nBestKg = Get_Static_Weight();
		tCar->bResponse = 0;
		tCar->cmd = RepeatCarCmd;
		//�����ص���ԣ�Send_CarInfo�Ƿ��ظ���
		Send_CarInfo(tCar);
	} 
	else 
	{
		AbnormalAckInfo_To_SDK(RepeatCarCmd, 0);
	}
}

//����һ��ģ��ĳ�����Ϣ
void Send_Simulate_CarInfo(uint8 axle, int32 wet, int cmd)
{
	CarInfo *pCar = NULL;
	
	pCar = CarQueue_Create_Car();
	if(pCar != NULL) 
	{
		pCar->nBestKg = wet;
		pCar->nAxleNum = axle;
		pCar->cmd = cmd;
		pCar->speed = 33;
		
		Send_CarInfo(pCar);
		pCar->bUse = FALSE;		//ɾ������
	}
}

//SDK�����ɼ������¶����޳����ǳ����Ѿ���β
void AbnormalAckInfo_To_SDK(uint8 cmd, uint8 CutOffState)
{
	CarInfo *pCar = NULL;

	debug(Debug_Business, "AckCmd:%d�ɼ��쳣,CutOffState:%d!\r\n", cmd, CutOffState);
	pCar = CarQueue_Create_Car();
	if(pCar != NULL) 
	{
		pCar->nAxleNum = 0;
		pCar->cmd = cmd;
		pCar->bAckErr = 1;
		pCar->CutOffState = CutOffState;
		
		Send_CarInfo(pCar);
		pCar->bUse = FALSE;		//ɾ������
	}
}


//�������ж�
#define LONG_CAR_MAX_COUNT		20
static int32 nLongCarWetArr[LONG_CAR_MAX_COUNT];
static int32 nLongCarWetInit = 0;
static BOOL bIsLongCar = 0;
static int32 nLongCarMax;
static int32 nLongCarMin;
static int32 nLongCarMaxPos;
static int32 nLongCarMinPos;
static int32 nLongCarSum;
static int32 nLongCarAvg;
static int32 nLongCarCounter;
void LongCarRecognize(int32 nWet)	//����Ƶ�ʣ�100Hz
{
	int i;
	
	nLongCarSum += nWet;
	nLongCarCounter++;
	if(nLongCarCounter < (100 / LONG_CAR_MAX_COUNT)) return;

	nLongCarAvg = nLongCarSum / nLongCarCounter;
	nLongCarSum = 0;
	nLongCarCounter = 0;

	//��ʼ��
	if(nLongCarWetInit == 0)
	{
		for(i = 0; i < LONG_CAR_MAX_COUNT; i++)
		{
			nLongCarWetArr[i] = nLongCarAvg;
		}
		
		bIsLongCar = 0;
		nLongCarWetInit = 1;
	}
	
	//��λ
	else
	{
		for(i = 0; i < LONG_CAR_MAX_COUNT-1; i++)
		{
			nLongCarWetArr[i] = nLongCarWetArr[i+1];
		}	
		nLongCarWetArr[LONG_CAR_MAX_COUNT-1] = nLongCarAvg;
	}
	
	//��ȡ���ֵ��Сֵλ��
	nLongCarMax = -100000;
	nLongCarMin = 150000;
	for(i = 0; i < LONG_CAR_MAX_COUNT; i++)
	{
		//���ֵ
		if(nLongCarWetArr[i] > nLongCarMax)
		{
			nLongCarMax = nLongCarWetArr[i];
			nLongCarMaxPos = i;
		}
		
		//��Сֵ
		if(nLongCarWetArr[i] < nLongCarMin)
		{
			nLongCarMin = nLongCarWetArr[i];
			nLongCarMinPos = i;
		}	
	}
	
	//�ж��½�����
	if(nLongCarMaxPos < nLongCarMinPos)
	{
		//�ж��½����ȳ���2.6��
		if((nLongCarMax - nLongCarMin) > 2600)
		{
			bIsLongCar = 1;
		}
	}
}

BOOL IsLongCar(void)
{
	return bIsLongCar;	
	//return FALSE;
}

void LongCarReset(void)
{
	nLongCarSum = 0;
	nLongCarCounter = 0;
	bIsLongCar = 0;
	nLongCarWetInit = 0;
}


