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

static OvertimeData t_carinfo = {{0}};

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



//����������������
static void Weight_Dynamic_Modify(CarInfo *pCar)
{	
	if(pCar == NULL) return;
	
	if(pCar->speed <= 50)
		pCar->nBestKg = (float)pCar->nBestKg * (float)gWetPar.Speed_0k  / 1000.0f;
	else if(pCar->speed > 50 && pCar->speed <= 100) 
		pCar->nBestKg = (float)pCar->nBestKg * (float)gWetPar.Speed_5k  / 1000.0f;
	else if(pCar->speed > 100 && pCar->speed <= 150) 
		pCar->nBestKg = (float)pCar->nBestKg * (float)(gWetPar.Speed_10k) / 1000.0f;
	else if(pCar->speed > 150 && pCar->speed <= 200) 
		pCar->nBestKg = (float)pCar->nBestKg * (float)(gWetPar.Speed_15k) / 1000.0f;
	else if(pCar->speed > 200 && pCar->speed <= 250) 
		pCar->nBestKg = (float)pCar->nBestKg * (float)(gWetPar.Speed_20k) / 1000.0f;
	else if(pCar->speed > 250 && pCar->speed <= 300) 
		pCar->nBestKg = (float)pCar->nBestKg * (float)(gWetPar.Speed_25k) / 1000.0f;
	else 
		pCar->nBestKg = (float)pCar->nBestKg * (float)(gWetPar.Speed_30k) / 1000.0f;
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
		pCar->AxleInfo[i].axle_wet = tmp;
	
	pCar->AxleInfo[pCar->nAxleNum-1].axle_wet = pCar->nBestKg - i*tmp;
}

//��������,�����쳣����
static void Modify_Abnormal_CarInfo(CarInfo *pCar)
{
	static uint8 cnt = 1;

	if(pCar == NULL) return;
	
	cnt++;
	if(cnt > 6) cnt = 1;

	if(pCar->nBestKg < 850) 		//��������С��850kg, ȡ�������1050~1800
		pCar->nBestKg = 900 + cnt * 150;
	
#if 0
	if(pCar->nBestKg < 7000) { 	//�����ж�, 3�����ϵĳ�������С��7�ֵ����
		if(pCar->nAxleNum > 2) {
			pCar->nAxleNum = 2;
			i = 2;
		}
	}
#endif

	if(pCar->nAxleNum < 2) {		//��������С��2���������������ȡĬ������
		if(pCar->nBestKg < 8000) 
			pCar->nAxleNum = 2;
		else
			pCar->nAxleNum = 6;
	}	

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

	for(i = 0; i < pCar->nAxleNum; i++)
		sum += pCar->AxleInfo[i].axle_wet;
	
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

//�����ͳ�����Ϣ���
static void Send_CarInfo(CarInfo *pCar)
{
	uint8 i = 0;
	uint8 TireOK = 0;
	TaskMsg msg = {0};
	GuiMsgInfo gui_msg = {0};
	char datebuf[30] = {0};
	int8 orig_axle = pCar->nAxleNum;
	int32 orig_wet = pCar->nBestKg;
	CSysTime *time = Get_System_Time();

	if(pCar == NULL) return;
	if(pCar->bResponse == 1) return;

	pCar->speed = Get_CarSpeed();
	debug(Debug_Business, "speed: %d.%dkm/h, Axlelength: %d\r\n", pCar->speed/10, pCar->speed%10, gWetPar.AxleLength);

	memset(&t_carinfo, 0, sizeof(OvertimeData));

	//�쳣������Ϣ����
	if((pCar->cmd != LongCarCmd) || (pCar->cmd != BackCarCmd))
		Modify_Abnormal_CarInfo(pCar);

	//�ж��Ƿ���
	if(orig_wet > (int)gWetPar.MaxWet)	t_carinfo.ScalerOverWetFlag = 1;
	
	t_carinfo.cmd = pCar->cmd;

	//�ж��Ƿ���
	Charge_Car_OverWeight(pCar, &t_carinfo);

	debug(Debug_Business, "\r\n>>���ͳ�����Ϣ, %ldms\r\n",Get_Sys_Tick());
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

		//�����ٶȲ�����������
		Weight_Dynamic_Modify(pCar);

		//���������ֶ�ֵ����
		pCar->nBestKg = WetDrvProc(pCar->nBestKg, gWetPar.StaticDivValue);
		
		if(pCar->cmd == LongCarCmd) 
		{
			t_carinfo.LongCarSegs  = pCar->nLongCarSegs;
			t_carinfo.LongCarReady = pCar->bLongCarReady;

			//���������쳣����
			if(pCar->nAxleNum == 0) pCar->nAxleNum = 2;
		}
		
		//��������
		Calc_Car_AxleWet(pCar);

		t_carinfo.TotalWet = pCar->nBestKg;			//����
		t_carinfo.AxleNum  = pCar->nAxleNum;		//����
		t_carinfo.AxleGroupNum = pCar->nAxleNum;	//������
		t_carinfo.CarStatus = pCar->CutOffState;	//����״̬

		debug(Debug_Business, "*Post CarInfo{wet=%d,axle=%d,speed=%d,%s,%s}\r\n", 	    	
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
		
		//���������
		//������ʶ����������ʱ�򣬰�����ʶ�����������˫��
		//if(Get_Tire_ErrStatus() && (TireOK == 1))
		//{
		//	//��ȫ��ȷ�����ʹ��ʶ����ĵ�˫���ź�
		//	for(i = 1; i < pCar->nAxleNum; i++)
		//	{
		//		t_carinfo.AxleType[i] = pCar->AxleInfo[i].axle_type + 1;
		//	}
		//}

		//
		//else
		{	
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
				t_carinfo.AxleType[2] = 2; 
				t_carinfo.AxleType[3] = 2; 
				t_carinfo.AxleType[4] = 2; 
				t_carinfo.AxleType[5] = 2; 
			}
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
			
			if(t_carinfo.AxleLen[i] <= 0)
			{
				t_carinfo.AxleLen[i] = 20;
			}
		}
		
		//���ʹ�������
		t_carinfo.SendNum = 0;
		
		//���ٶ�ȡĬ��ֵ
		t_carinfo.speeda = 0;
		t_carinfo.speed = pCar->speed;
		
		//ҵ���߳���Ϣ��
		msg.msg_id = Msg_CarInfo_Ready;
		
		//GUI�߳���Ϣ��
		gui_msg.ID = WM_CARIN;
		
		debug(Debug_Business, "*CarQueueLen=%d; seri=%d, orig_wet=%d, orig_axle=%d!\r\n",	
			CarQueue_Get_Count(), OverQueue_GetSeriID(), orig_wet, orig_axle);
		//��ӡ����
		for(i = 0; i < pCar->nAxleNum; i++)
		{
			debug(Debug_Business, "*AxleWet%d: %d; ", i, pCar->AxleInfo[i].axle_wet);
		}
		debug(Debug_Business, "\r\n");
		
		//��ӡʵ��̥��
		debug(Debug_Business, "*OrigTyretype: \r\n");
		for(i = 0; i < pCar->nAxleNum; i++)
		{
			debug(Debug_Business, "%d, %d\r\n", pCar->AxleInfo[i].axle_type + 1, pCar->AxleInfo[i].axle_len);
		}
		debug(Debug_Business, "\r\n");

		//��ӡ̥��
		debug(Debug_Business, "*SendTyretype: \r\n");
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
		debug(Debug_Business, "*Send BackCarInfo!\r\n\r\n");
	}	

	t_carinfo.SendMod = SendActiveMod;

	//������Ϣ��Ӵ��������ݶ���
	if(OverQueue_Enqueue(&t_carinfo) == 0)
	{
		//��ӳɹ�,�ó����ѷ��ͱ�־(��Ӻ���OverQueue����,CarQueue��Ϊ�ó�������)
		pCar->bResponse = 1;

		//��ҵ�����ݹ����̷߳�����Ϣ
		Task_QPost(&DataManagerMsgQ,  &msg);

		//������Ϣ�ѷ���,��ӡ����������Ϣ
		if(pCar->cmd != BackCarCmd)
		{
			debug(Debug_Business, "\r\n>>�����β\r\n");
			CarQueue_Print_Info();
		}
	
		//������ǰ���ڷ��ͼ�����Ϣ
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

//������������
static void Calc_FollowCar_Wet(CarInfo *pCar)
{
	int32 wet = 0, axlewet = 0;	
	uint8 i = 0;
	
	if(pCar == NULL) return;
	
	if(pCar->nAxleNum == 6) 
	{
		if((!pCar->bWetVaild) || (pCar->nBestKg < 8000)) 
		{
			pCar->nBestKg = Get_Static_Weight();
			pCar->WetLevel = LevelSingleCarStable;
			debug(Debug_Business, ">FollowCar, Axles=6, Get ScalerWet=%d\r\n", pCar->nBestKg);
		}
		return;
	}

	//�Ѿ���������,���Ƿ��л����������
	if(pCar->bWetVaild) 
	{
		if(CarQueue_Get_OnScaler_Count() == 1) 
		{
			if(Get_Static_Weight() > 850) 
			{
				pCar->nBestKg = Get_Static_Weight();
				pCar->WetLevel = LevelSingleCarBeginDown;
				debug(Debug_Business, ">FollowCar Get ScalerWet:%d,tick=%ld", pCar->nBestKg,Get_Sys_Tick());
			}
		}
		return;
	}			

	//��������
	for(i = 0; i < pCar->nAxleNum; i++)
		axlewet += pCar->AxleInfo[i].axle_wet;

	wet = pCar->nScalerKgDown - pCar->nScalerKgComing;	//��Ļ�����ͷ�ǰ���̨��̬������ֵ
//	if(wet + 500 > axlewet) 
	if(wet > ((float)axlewet * 0.9f)) 
	{
		pCar->nBestKg = wet;
		pCar->WetLevel = LevelMultiCar;
	} 
	else 
	{
		pCar->nBestKg = axlewet;
		pCar->WetLevel = LevelAxleWet;
	}
	debug(Debug_Business, ">��������,AxleWet=%d,UpDownWet=%d,tick=%ld", axlewet, wet, Get_Sys_Tick());
}

//��������
//�󳵴�����Ļ������������ظУ����泵����Ϣ
static void Save_FollowCar_Info(CarInfo *pCar)
{	
	if(pCar == NULL) return;

	if(pCar->bResponse == 1) return;
	
	debug(Debug_Business, "save follocar info!\r\n");
	if(pCar->nScalerKgDown == 0)
		pCar->nScalerKgDown = Get_Static_Weight();

	Calc_FollowCar_Wet(pCar);

	//�����ظб���������ʱ������Ϣ
	if(!pCar->bWetVaild) 
	{
		pCar->bWetVaild = TRUE;
		pCar->bWetTimeFlag = FALSE;
		pCar->nWetKeepTime = 0;
		//pCar->WetFlag = NoneWetFlag;	
	}
	
	Send_CarInfo(pCar);
}

static void LongCar_Segs_Business(CarInfo *pCar)
{
	if(pCar == NULL) return;

	pCar->nAxleNum = Apply_LongCar_Axle(pCar);
	if(pCar->nAxleNum <= 0) 
	{
		if(!pCar->bLongCarReady) 
		{
			CarQueue_Car_Kill(pCar);
			return;
		} 
	} 
	else 
	{
		pCar->nBestKg = Get_Static_Weight();
		pCar->cmd = LongCarCmd;
		pCar->WetLevel = LevelLongCar; 
		pCar->bWetVaild = TRUE;
	}
	Send_CarInfo(pCar);
	
	if(!pCar->bLongCarReady) 
	{
		pCar->nStatus = stCarLeaving;
		CarQueue_Car_Kill(pCar);			//ǰ��ֶ���Ϣ���ͺ���Ϊ���ѽɷ��뿪������ɾ��
	}
}

//����ȡ������ʱ
static void Car_GetWet_TimerBusiness(CarInfo *pCar)
{
    BOOL flag = FALSE;
    static int preMaxWet = 0;
    
	if(pCar == NULL) return;
	
	if(pCar->bWetTimeFlag) 
	{
	    pCar->nWetKeepTime += CAR_TICK;
	    switch(pCar->WetFlag) 
	    {  
    		case FollowWetFlag:                   //������ʱ����     
    			if(pCar->nWetKeepTime >= CAR_WET_TICK) 
    			{
    				pCar->nScalerKgDown = Get_Static_Weight();
    				Calc_FollowCar_Wet(pCar); 
    				pCar->WetLevel = LevelMultiCar;
    				pCar->bWetVaild = TRUE;
					//Send_CarInfo(pCar);
    				flag = TRUE;
    			}
    			break;
    		 
    		case SingleWetFlag:                //�����ȶ�����
			case SingleUnstableFlag:			//����δ�ȶ�����															 
				preMaxWet = Get_Static_MaxWeight();
    			
    			if(pCar->nWetKeepTime >= CAR_WET_TICK) 
    			{
    				if(Get_Static_Weight() < preMaxWet - 500) 
    				{
    					pCar->nBestKg = preMaxWet;
						debug(Debug_Business, "Get max ScalerWet:%d!\r\n",preMaxWet);
    				} 
    				else 
    				{
    					pCar->nBestKg= Get_Static_Weight();
						debug(Debug_Business, "Get current ScalerWet:%d! MaxWet:%d\r\n",
							pCar->nBestKg,preMaxWet);
					}
    				pCar->WetLevel = LevelSingleCarStable;
    				pCar->bWetVaild = TRUE;
    				preMaxWet = 0;
					Clear_ScalerMaxWeight();
    				Send_CarInfo(pCar);
    				flag = TRUE;
    			}
    			break;
    		default:
    		    break;
    	}
    	
    	if(flag) 
    	{
    	    pCar->bWetTimeFlag = FALSE;
    		pCar->nWetKeepTime = 0;
    		//pCar->WetFlag = NoneWetFlag;
    	}
	}//end if(pCar->bWetTimeFlag)
}

//������β����ʱ�����������ж�
static int Car_CutOffState_Fun(CarInfo *pCar)
{
	int32 naxle = 0;
	int ret = 0;
	OvertimeData *pLastCar = NULL;

	if(pCar == NULL) return 2;

	pCar->bAxleValid = TRUE;
	naxle = Apply_Car_Axle(pCar);
	if(naxle > 0) 
	{ 							
		pCar->nAxleNum = naxle;	//�����ϳ�
	} 
	else if(naxle < 0 ) 
	{
		debug(Debug_Business, "BackCar, TotalCarNumber=%d\r\n", CarQueue_Get_Count());
		CarQueue_Car_Kill(pCar);		//����
		pCar->cmd = BackCarCmd;
	
		if(pCar->pPrev != NULL) 		//ɾ��ǰ��ĳ�Ӧ������ƥ������
		{
			if(naxle + pCar->pPrev->nAxleNum <= 2)
			{
				pLastCar = OverQueue_Get_Tail();
				if(pLastCar != NULL)
				{
					//�������ݻ�δ���ͣ��Ҷ�β���ǵ�����Ϣɾ��
					if((pLastCar->cmd != BackCarCmd) && (pLastCar->bSend == 0))
					{		
						OverQueue_Remove_Tail();
						debug(Debug_Business, "OverQueue����,�����͵�����Ϣ!\r\n");
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
		ret = 1;
	} 
	else	
	{		
		debug(Debug_Business, "�ⲿ����!\r\n");
		CarQueue_Car_Kill(pCar);		//�ⲿ����	
		ret = 2;
	}			

	return ret;
}

//��������״̬��Ӧ�ⲿ�¼�������
static void Car_Business_CarComState(CarInfo *pCar, TaskMsg *pMsg)
{	
	AlexRecoder *tmpAxle = NULL;
	
	if((pCar == NULL) || (pMsg == NULL)) return;
	
	switch(pMsg->msg_id) 
	{
		case Msg_Cmd_PlusSave:
			pCar->nLongCarSegs = pMsg->msg_ctx;
			FSM_Change_CarState(pCar, stLongCar);
			LongCar_Segs_Business(pCar);
			break;
			
		case Msg_LC2_Leave:
			pCar->bWetTimeFlag = TRUE;
			pCar->CutOffState = 1;
//			if(pCar->WetFlag != SingleWetFlag)	   //??
//				pCar->WetFlag = FollowWetFlag;		
			FSM_Change_CarState(pCar, stCarInScaler);
			break;

		case Msg_Tick:
			pCar->nStatusKeepTime += CAR_TICK;
			pCar->nLiveTime += CAR_TICK;
			Car_GetWet_TimerBusiness(pCar);
			break;

		case Msg_PreAlex_Come:
			if(pCar->pNext == NULL)
			{
				if(pCar->nAxleNum == 0)
				{
					//��¼��ʼ����
					pCar->nScalerKgComing = Get_Static_Weight();
					if(pCar->nScalerKgComing < 500) 
					{		
						pCar->WetFlag = SingleWetFlag;		//�³�δ��ӣ��õ�����־
						Clear_ScalerMaxWeight();		
					}						
				}
			}
			break;

		case Msg_PreAlex_Add:
			tmpAxle = (AlexRecoder *)pMsg->msg_ctx;
			if(pCar->nAxleNum < MAX_AXLE_NUM)
			{
				if(tmpAxle != NULL)
				{
					if((pCar->nAxleNum >= 0) && (pCar->nAxleNum <= 6))
					{
						pCar->AxleInfo[pCar->nAxleNum].axle_wet = tmpAxle->AlexKg;
						pCar->AxleInfo[pCar->nAxleNum].axle_type = tmpAxle->bDouble;
					}
				}

				pCar->nAxleNum++;
			}
			break;

		case Msg_PreAlex_Remove:
			if(pCar->nAxleNum < MAX_AXLE_NUM)
			{
				pCar->nAxleNum--;
			}
			break;

		case Msg_GC2_Enter:
		case Msg_BackAlex:	
			if(Get_LongCar_Mode() == AutoLongCarMode) 
			{		
				if(CarQueue_Get_OnScaler_Count() == 1 &&
				    pCar->nStatus == stCarComing) 
				{
					if(pMsg->msg_id == Msg_BackAlex)
						pCar->nBackAxleNum++;
					
					pCar->nLongCarHalfWet = Get_Static_Weight();
					pCar->nLongCarHalfAxle = Apply_LongCar_Axle(pCar);
					
//					Clear_Axle_Recoder();   //�����ظ�������쳣
					FSM_Change_CarState(pCar, stLongCar);
				}
			}
			break;

		default:
			break;
	}
}

//������ȫ�ϳ�״̬��Ӧ�ⲿ�¼�������
static void Car_Business_CarInScalerState(CarInfo *pCar, TaskMsg *pMsg)
{
	AlexRecoder *tmpAxle = NULL;
	
	if((pCar == NULL) || (pMsg == NULL)) return;
	
	switch(pMsg->msg_id) 
	{
		case Msg_Tick:
			pCar->nStatusKeepTime += CAR_TICK;
			pCar->nLiveTime += CAR_TICK;
			if(!pCar->bAxleValid)
			{
				if(pCar->nStatusKeepTime >= CAR_WET_TICK - 10)
				{
					if(Car_CutOffState_Fun(pCar) != 0)
						break;
				}
			}
				
			Car_GetWet_TimerBusiness(pCar);
			break;

		case Msg_LC2_Enter:
			if(!pCar->bAxleValid)
			{
				if(Car_CutOffState_Fun(pCar) != 0)
					break;
			}
				
			if(pCar->WetFlag == SingleWetFlag) 
			{
				//pCar->WetFlag = SingleUnstableFlag;
				pCar->WetLevel = LevelSingleCarBy;
			} 
			else if((pCar->WetFlag == FollowWetFlag) ) //&& (pCar->nAxleNum > 0) && (pCar->cmd != BackCarCmd)) 
			{
				if(!pCar->bWetVaild)
					Save_FollowCar_Info(pCar);
				else if(pCar->bResponse == 0)
					Send_CarInfo(pCar);
			}
		    break;
		    
        case Msg_BackAlex:
		case Msg_GC2_Enter:
			//������β��60ms�ڲ������³�,��ֹ��ظ�����
			//if(pCar->nStatusKeepTime < 60) break;

			if(!pCar->bAxleValid)
			{
				if(Car_CutOffState_Fun(pCar) != 0)
					break;
			}

		    if(pCar->WetFlag == SingleWetFlag) 	
		    {
				//pCar->WetFlag = SingleUnstableFlag;
				pCar->WetLevel = LevelSingleCarBeginDown;
			} 
			else if((pCar->WetFlag == FollowWetFlag)) //&& (pCar->nAxleNum > 0) && (pCar->nBestKg!= BackCarCmd))
			{
				if(!pCar->bWetVaild)
					Save_FollowCar_Info(pCar);
				else if(pCar->bResponse == 0)
					Send_CarInfo(pCar);
			}

			if(pMsg->msg_id == Msg_BackAlex)
				pCar->nBackAxleNum++;
#ifdef _SIMULATION_NOPAY_
			FSM_Change_CarState(pCar, stCarLeaving);
#else
			FSM_Change_CarState(pCar, stCarWaitPay);
#endif
			break;

		case Msg_Cmd_PayNotify:
		    FSM_Change_CarState(pCar, stCarLeaving);
			break;

		case Msg_PreAlex_Add:
			if(pCar->bWetVaild) break;

			tmpAxle = (AlexRecoder *)pMsg->msg_ctx;
			if(pCar->nAxleNum < MAX_AXLE_NUM)
			{
				if(tmpAxle != NULL)
				{
					if((pCar->nAxleNum >= 0) && (pCar->nAxleNum <= 6))
					{
						pCar->AxleInfo[pCar->nAxleNum].axle_wet = tmpAxle->AlexKg;
						pCar->AxleInfo[pCar->nAxleNum].axle_type = tmpAxle->bDouble;
					}
				}
				pCar->nAxleNum++;
			}
			break;

		case Msg_PreAlex_Remove:
			if(pCar->bWetVaild) break;
			
			if(pCar->nAxleNum < MAX_AXLE_NUM)
			{
				pCar->nAxleNum--;
			}
			break;

		default:
			break;
	}		
}

//����״̬��Ӧ�ⲿ�¼�������
static void Car_Business_LongCarState(CarInfo *pCar, TaskMsg *pMsg)
{
	int32 sum = 0, i = 0;
	AlexRecoder *tmpAxle = NULL; 
	
	if((pCar == NULL) || (pMsg == NULL)) return;
	
	switch(pMsg->msg_id) 
	{
		case Msg_Tick:
			pCar->nStatusKeepTime += CAR_TICK;
			pCar->nLiveTime += CAR_TICK;
			break;

		case Msg_Cmd_PlusSave:
			pCar->nLongCarSegs = pMsg->msg_ctx;
			LongCar_Segs_Business(pCar);
			break;
 
		case Msg_GC2_Leave: 
			//��ظ��ͷ�,��Ļ�ڵ�,��������״̬
			if(Get_LC_Status(2))
			{
				if(pCar->pNext == NULL)
				{
					//����û������Ϊ�ǵظ����ź�
					FSM_Change_CarState(pCar, stCarComing);
				}
				else
				{
					FSM_Change_CarState(pCar, stCarFarPay);
				}
			}
			else
			{
				FSM_Change_CarState(pCar, stCarFarPay);
			}

#ifdef _SIMULATION_NOPAY_
			CarQueue_Car_Kill(pCar);
#endif
			break;

		//�����ɷ�֪ͨ�¼�
		case Msg_Cmd_PayNotify:
		    FSM_Change_CarState(pCar, stCarLeaving);
			break;

		case Msg_LC2_Leave:
			if(Get_LongCar_Mode() == AutoLongCarMode) 
			{	
				//�����Զ�����ģʽ
				pCar->nAxleNum = Apply_LongCar_Axle(pCar);
				if((pCar->nAxleNum - pCar->nLongCarHalfAxle) <= 0) 
				{
					pCar->nBestKg = pCar->nLongCarHalfWet;
				}
				else
				{
					sum = 0;
					for(i = 0; i < pCar->nAxleNum; i++)
					{
						sum += pCar->AxleInfo[i].axle_wet;
					}

					pCar->nBestKg = sum;
				}
				
				pCar->CutOffState = 1;
				pCar->bWetVaild = TRUE;
				pCar->WetLevel = LevelAutoLongCar;
				debug(Debug_Business, "AutoLongCar CutOff!\r\n");
				Send_CarInfo(pCar);
			} 
			else 
			{
				pCar->nAxleNum = Apply_LongCar_Axle(pCar);
				if(pCar->nAxleNum <= 0) 
				{
					if(!pCar->bLongCarReady) 
					{
						CarQueue_Car_Kill(pCar);
						break;
					} 
				} 
				pCar->bLongCarReady = TRUE;
				pCar->CutOffState = 1;
				debug(Debug_Business, "HandleLongCar CutOff!\r\n");
			}
			break;
			
		case Msg_PreAlex_Add:
			if(pCar->bWetVaild) break;

			//����¼����Ϣ�ᵼ�³���������ȡ�ر���
			tmpAxle = (AlexRecoder *)pMsg->msg_ctx;
			if(pCar->nAxleNum < MAX_AXLE_NUM)
			{
				if(tmpAxle != NULL)
				{
					if((pCar->nAxleNum >= 0) && (pCar->nAxleNum <= 6))
					{
						pCar->AxleInfo[pCar->nAxleNum].axle_wet = tmpAxle->AlexKg;
						pCar->AxleInfo[pCar->nAxleNum].axle_type = tmpAxle->bDouble;
					}
				}
				pCar->nAxleNum++;
			}
			break;
		
		case Msg_PreAlex_Remove:
			if(pCar->nAxleNum < MAX_AXLE_NUM)
			{
				pCar->nAxleNum--;
			}
			break;

		case Msg_BackAlex:
			pCar->nBackAxleNum++;
			if(pCar->nBackAxleNum >= pCar->nAxleNum)
                FSM_Change_CarState(pCar, stCarFarPay);
            break;
            
		default:
			break;
	}	
}

static void Car_DealDoubleNextCar(CarInfo *pCar)
{
	if(pCar == NULL) return;
	if(pCar->pNext == NULL) return;

				
	if((pCar->pNext->WetFlag == FollowWetFlag) ) //&& (pCar->pNext->nAxleNum > 0)&& (pCar->pNext->cmd != BackCarCmd))
	{
		if(pCar->pNext->nStatus == stCarInScaler)
		{
			if(pCar->pNext->bWetVaild == 0)
			{
				pCar->pNext->WetFlag = SingleWetFlag;
				pCar->pNext->nScalerKgComing = 0;
			}
			else
			{
				if(pCar->pNext->bResponse == 0)
				{
					Save_FollowCar_Info(pCar->pNext);		//�������ڱ궨
				}
			}
		}
		else if(pCar->pNext->nStatus == stCarComing)
		{
			pCar->pNext->WetFlag = SingleWetFlag;
			pCar->pNext->nScalerKgComing = 0;
		}	
	}

}

//�����ȴ��շ�״̬��Ӧ�ⲿ�¼�������
static void Car_Business_CarWaitPayState(CarInfo *pCar, TaskMsg *pMsg)
{
	if((pCar == NULL) || (pMsg == NULL)) return;
	
	switch(pMsg->msg_id) 
	{
		case Msg_Tick:
			pCar->nStatusKeepTime += CAR_TICK;
			pCar->nLiveTime += CAR_TICK;
			Car_GetWet_TimerBusiness(pCar);
			break;

		case Msg_Cmd_PayNotify:
		    FSM_Change_CarState(pCar, stCarLeaving);
			break;

		case Msg_BackAlex:
			pCar->nBackAxleNum++;
			if(pCar->nBackAxleNum >= pCar->nAxleNum)
                FSM_Change_CarState(pCar, stCarFarPay);
            break;

       	case Msg_GC2_Leave:
			//����2��ĳ���������ӽɷ�
			//if(pCar->nAxleNum > 2) break;
			
       		FSM_Change_CarState(pCar, stCarFarPay);
       	
       		if(pCar->pNext != NULL)
       		{
				Car_DealDoubleNextCar(pCar);
			}
			
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
		case Msg_Tick:
			pCar->nStatusKeepTime += CAR_TICK;
			pCar->nLiveTime += CAR_TICK;
			Car_GetWet_TimerBusiness(pCar);
			break;

		case Msg_Cmd_PayNotify:
			CarQueue_Car_Kill(pCar);
			break;

		default:
			break;
	}
}


//�����ɷ��뿪״̬��Ӧ�ⲿ�¼�������
static void Car_Business_CarLeave(CarInfo *pCar, TaskMsg *pMsg)
{
	if((pCar == NULL) || (pMsg == NULL)) return;
	
	switch(pMsg->msg_id) 
	{
		case Msg_Tick:
			pCar->nStatusKeepTime += CAR_TICK;
			pCar->nLiveTime += CAR_TICK;	
#ifdef _SIMULATION_NOPAY_
			Car_GetWet_TimerBusiness(pCar);
#endif
			break;

		case Msg_GC2_Leave: 
       		if(pCar->pNext != NULL)
       		{
				Car_DealDoubleNextCar(pCar);
			}

			CarQueue_Car_Kill(pCar);
			break;

		case Msg_BackAlex:
			pCar->nBackAxleNum++;
			if(pCar->nBackAxleNum >= pCar->nAxleNum)
                CarQueue_Car_Kill(pCar);
			break;					//fatal err;

        case Msg_Cmd_PayNotify:
			CarQueue_Car_Kill(pCar);
			if(pCar->pNext != NULL) 
			{
				FSM_Change_CarState(pCar->pNext, stCarLeaving);
				debug(Debug_Business, "First car is already Leave, maybe the second car pay!\r\n");
			}
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

struct _BusinessArray {
	uint8 state;
	CarBusinessFun fun;
};

static const struct _BusinessArray BusinessArray[] = {
	{stCarNone, 	Car_Business_None},
	{stCarComing, 	Car_Business_CarComState},
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

	if(pCar->nStatus == state) 
		return;

	if(state > stCarFarPay)
	   	return;

	debug(Debug_Business, "CarState change : %s -> %s\r\n", Get_Car_StatusName(pCar->nStatus), Get_Car_StatusName(state));
	pCar->nPreStatus = pCar->nStatus;
	pCar->nStatus = state;
	pCar->nStatusKeepTime = 0;
	pCar->bStateChanged = TRUE;
	pCar->CarFSMFun = BusinessArray[state].fun;
}

void Car_Business_RepeatCmd(CarInfo *pCar, TaskMsg *msg) 
{
	if(pCar != NULL) 
	{
		pCar->nBestKg = Get_Static_Weight();
		Send_CarInfo(pCar);
	} 
	else 
	{
		if(Get_Static_Weight() > 500) 
		{
			pCar = CarQueue_Create_Car();
			if(pCar != NULL) 
			{
				pCar->nBestKg = Get_Static_Weight();
				pCar->cmd = RepeatCarCmd;
				Send_CarInfo(pCar);
			}
		}
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
	}
}

