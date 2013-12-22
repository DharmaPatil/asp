#include "includes.h"
#include "car_queue.h"
#include "sys_config.h"
#include "weight_param.h"
#include "axle_business.h"
#include "car_business.h"
#include "device_info.h"
#include "wet_algorim.h"

#define SCALER_EMPTY_MAX		200		//�ճ����ֵ��������ڸ�ֵ����Ϊ���г��ڳ���
#define SCALER_BIG_CAR_MIN		2000	//��(�������ϵĳ�)����С����
#define  ALEX_ARR_MAX			30
#define AXLE_ERR_TIME			2500	//������ʱ��

#define SCALER_EMPTY_WET		100		//�ճ���ֵ100kg
#define SCALER_CLEAR_TIME		120000	//�ճ�ʱ����ֵ20s

static int32 m_nAlexCount = 0;
static int32 m_nLongCarFrontAlexCount = 0;
static BOOL  m_bIsLongAlexCount = 0;
static AlexRecoder AlexRecoderArr[ALEX_ARR_MAX];
static AlexRecoder AlexRecoderArrNormal[ALEX_ARR_MAX];
static AlexRecoder m_AlexRecoder;

//ʱ������
static int32 MsgTimeAdd(int nBase, int nAdd)
{
	//��ֹ���
	if(nBase > 3600000) return nBase;		//��msΪ��λ�����������Ѿ�1Сʱ��

	return nBase + nAdd;
}


//�������¼
static void AddAlexRecoder(AlexRecoder* pAlexRecoder)
{
	if(m_nAlexCount >= ALEX_ARR_MAX) return;

	AlexRecoderArr[m_nAlexCount].direct = pAlexRecoder->direct;
	AlexRecoderArr[m_nAlexCount].AlexKg = pAlexRecoder->AlexKg;
	AlexRecoderArr[m_nAlexCount].nTimePulse = pAlexRecoder->nTimePulse;
	AlexRecoderArr[m_nAlexCount].bDouble = pAlexRecoder->bDouble;
	AlexRecoderArr[m_nAlexCount].nTimeInterval = 0;

	m_nAlexCount++;
}

static void Copy_AxleInfo(CarInfo* pCar, int nAlexCount)
{
	int i;

	if(pCar == NULL) return;

	for(i = 0; i < nAlexCount; i++)
	{
		pCar->AxleInfo[i].axle_wet = AlexRecoderArr[i].AlexKg;
		pCar->AxleInfo[i].axle_type = AlexRecoderArr[i].bDouble;
		pCar->AxleInfo[i].axle_len = AlexRecoderArr[i].nTimeInterval;
	}
}


//����û�����źŵĳ�
static int32 PraseNoneAlexCar(CarInfo* pCar)
{
	int i = 0;
	
	if(pCar == NULL) return 0;

	//����ϳ�ǰ����������SCALER_EMPTY_MAX�����ÿ����ˣ����ϳ�Ī��
	if(pCar->nScalerKgComing < 500)
	{
		if(Get_Static_Weight() - pCar->nScalerKgComing > 350)
		{		
			for(i = 0; i < 2; i++)
			{
				pCar->AxleInfo[i].axle_wet= (Get_Static_Weight() - pCar->nScalerKgComing) / 2;
				pCar->AxleInfo[i].axle_type = 0;
				pCar->AxleInfo[i].axle_len = 100;
			}
			debug(Debug_Business, "UpDownWet>350kg,maybe a little car!\r\n");
			return 2;
		}
	}

	return 0;
}

//����ֻ��һ������ź�
static int32 PraseSingleAlexCar(CarInfo* pCar)
{
	int i = 0;
	
	if(pCar == NULL) 
		return 0;

	for(i = 0; i < 2; i++)
	{
		pCar->AxleInfo[i].axle_wet = AlexRecoderArr[0].AlexKg;
		pCar->AxleInfo[i].axle_type = AlexRecoderArr[0].bDouble;
		pCar->AxleInfo[i].axle_len = AlexRecoderArr[0].nTimeInterval;
	}

	//�ֱ����жϣ�ֻһ�����źţ�����Ϊ���ᣬһ����Ϊ����
	return 2;
}

static int32 PraseLittleCarAlex(CarInfo* pCar)
{
	if(pCar == NULL) return 0;

	//�����������ö�ٷ���

	//��һ��Ϊ���ᣬ�ڶ���ҲΪ���ᣬ�϶����ϳ���-------------------------------------------
	if((AlexRecoderArr[0].direct > 0) && (AlexRecoderArr[1].direct > 0)) 
	{
		Copy_AxleInfo(pCar, 2);
		return 2;
	}

	//�����һ����Ϊ���ᣬ�ڶ���Ϊ����-----------------------------------------------------
	if(AlexRecoderArr[0].direct > 0)
	{
		//����ڶ�������ʱ��С��3S���������ǵ���
		if(AlexRecoderArr[0].nTimeInterval < AXLE_ERR_TIME)
		{
			Copy_AxleInfo(pCar, 2);
			return 2;
		}

		//����������ӿ��⣬����������
		//����ϳ�ǰ����������SCALER_EMPTY_MAX�����ÿ����ˣ����ϳ�Ī��
		if(Get_Static_Weight() - pCar->nScalerKgComing > SCALER_EMPTY_MAX)
		{		
			Copy_AxleInfo(pCar, 2);
			return 2;
		}

		//�����ڼ��٣����Ǽ�������������С�����ֵ2.5�֣�������ǰ�����³�����Ϊǰ��
		if(Get_Static_Weight() - pCar->nScalerKgComing < -2500)
		{	
			Copy_AxleInfo(pCar, 2);
			return 2;
		}

		return 0;
	}
	
	//�����һ�����Ǽ��ᣬ�ڶ�����Ҳ�Ǽ��ᣬ��Ϊ����---------------------------------------
	if((AlexRecoderArr[0].direct < 0) && (AlexRecoderArr[1].direct < 0)) 
	{
		//�����̨��������200kg����Ϊ����
		if(Get_Static_Weight() <= 200)
		{
			return -2;
		}
	
		//��������������С��20ms���������ٿ죬��Ϊ�ϳ�
		if((AlexRecoderArr[0].nTimePulse < 20) && (AlexRecoderArr[0].nTimePulse < 20))
		{
			return 2;
		}
	
		//����������ʱ��С��1.5s����Ϊ�ϳ�
		if(AlexRecoderArr[0].nTimeInterval < 500)
		{
			return 2;
		}	
	
		return -2;
	}

	//�����һ�����Ǽ��ᣬ�ڶ������Ǽ���---------------------------------------------------
	//����ڶ�������ʱ��С��8S���������ǵ���
	if(AlexRecoderArr[0].nTimeInterval < 8000)
	{
		//����һ�����ϳӵ�ʱ��ǰ�������³ӣ�������������
		Copy_AxleInfo(pCar, 2);
		return 2;
	}

	//�����̨���������ˣ�����ǰһ���Შ��ʶ�����
	//����ϳ�ǰ����������SCALER_EMPTY_MAX�����ÿ����ˣ����ϳ�Ī��
	if(Get_Static_Weight() - pCar->nScalerKgComing > SCALER_EMPTY_MAX)
	{		
		Copy_AxleInfo(pCar, 2);
		return 2;
	}

	//����һ�������ǰ���ϵ�ʱ��һֱѹ����ʶ�����ϣ�����ǰ�泵�³��ˣ������ٸ���
	//�������ʶ��Ϊ��С
	//��һ����ᱻʶ��Ϊ�����ź�
	//PS: ��������Ѿ�����ʶ��������ʱ������ź��Ѿ��Ǽ��ᣬ���ﲻ������


	//�п�������һ���ᣬ���ϳ�
	return 0;
}

//�����ۼ�����
int AlexCountQueue(void)
{
	int i;
	int SumCount = 0;

	//���������кͲ���
	for(i = 0; i < m_nAlexCount; i++)
	{
		SumCount += AlexRecoderArr[i].direct;
	}

	return SumCount;
}

//�����ۼ�����
static int AlexNormalCountQueue()
{
	int i;
	int SumCount = 0;

	//���������кͲ���
	for(i = 0; i < m_nAlexCount; i++)
	{
		SumCount += AlexRecoderArrNormal[i].direct;
	}

	return SumCount;
}

//�ڸ�������£�����������������
static void AlexCountByNormalizeHiSpeed(int nMinTime)
{
	int i;

	//���õ�һ��
	AlexRecoderArrNormal[0].direct = AlexRecoderArr[0].direct;
	AlexRecoderArrNormal[0].AlexKg = AlexRecoderArr[0].AlexKg;
	AlexRecoderArrNormal[0].nTimePulse = AlexRecoderArr[0].nTimePulse;
	AlexRecoderArrNormal[0].nTimeInterval = AlexRecoderArr[0].nTimeInterval;
	AlexRecoderArrNormal[0].bDouble = AlexRecoderArr[0].bDouble;

	//���������
	for(i = 1; i < m_nAlexCount; i++)
	{
		if(AlexRecoderArr[i].direct != AlexRecoderArrNormal[i-1].direct)
		{
			//�涨ʱ���ڲ��ܷ���
			if(AlexRecoderArrNormal[i-1].nTimeInterval < nMinTime)
			{
				AlexRecoderArrNormal[i].direct = AlexRecoderArrNormal[i-1].direct;
			}
			else
			{
				AlexRecoderArrNormal[i].direct = AlexRecoderArr[i].direct;
			}			
		}
		else
		{
			AlexRecoderArrNormal[i].direct = AlexRecoderArr[i].direct;
		}

		AlexRecoderArrNormal[i].AlexKg = AlexRecoderArr[i].AlexKg;
		AlexRecoderArrNormal[i].nTimePulse = AlexRecoderArr[i].nTimePulse;
		AlexRecoderArrNormal[i].nTimeInterval = AlexRecoderArr[i].nTimeInterval;
		AlexRecoderArrNormal[i].bDouble = AlexRecoderArr[i].bDouble;

	}
}

static int32 PraseCarAlexBigger3(CarInfo* pCar)
{
	int i, j;
	int SumCount = 0;
	int IncCount = 0;
	int SumNormalCount = 0;	
	int DecCount = 0;

	//���������кͲ���
	for(i = 0; i < m_nAlexCount; i++)
	{
		SumCount += AlexRecoderArr[i].direct;
		if(AlexRecoderArr[i].direct > 0) IncCount++;
		if(AlexRecoderArr[i].direct < 0) DecCount++;
	}

	//����ȫ��Ϊ��������-----------------------------------
	if(DecCount == 0) 
	{
		for(i = 0; i < m_nAlexCount; i++)
		{
			pCar->AxleInfo[i].axle_wet = AlexRecoderArr[i].AlexKg;
			pCar->AxleInfo[i].axle_type = AlexRecoderArr[i].bDouble;
			pCar->AxleInfo[i].axle_len = AlexRecoderArr[i].nTimeInterval;
		}
		return SumCount;
	}

	//����ȫ��Ϊ��������
	if(IncCount == 0) return SumCount;

	//����������������һ����Ϊ����
	if(AlexRecoderArr[0].direct > 0)
	{
		//���������ڳ��ַ�ת������
		 AlexCountByNormalizeHiSpeed(AXLE_ERR_TIME);	//3����

		 SumNormalCount = AlexNormalCountQueue();

		if(SumNormalCount > 6) SumNormalCount = 6;
		for(i = 0; i < SumNormalCount; i++)
		{
			pCar->AxleInfo[i].axle_wet = AlexRecoderArrNormal[i].AlexKg;
			pCar->AxleInfo[i].axle_type = AlexRecoderArrNormal[i].bDouble;
			pCar->AxleInfo[i].axle_len = AlexRecoderArrNormal[i].nTimeInterval;
		}

		return SumNormalCount;
	}
	//����������������һ����Ϊ����	
	else
	{
		//����ϳ�ǰ�����������ˣ����ÿ��ǣ���Ϊ�����ϳ�����һ��������Ϊ����
// 		if(Get_Static_Weight() - pCar->nScalerKgComing > SCALER_BIG_CAR_MIN)
// 		{
// 			//������������������
// 			AlexRecoderArr[0].direct = 1;
// 		}

		//�����̨��ֻ������һ��������ʱ��̨��������ֵ����SCALER_BIG_CAR_MIN����Ϊ�����ϳ�����һ��������Ϊ����
		if(CarQueue_Get_OnScaler_Count() == 1)
		{
			if(Get_Static_Weight() > SCALER_BIG_CAR_MIN)
			{
				AlexRecoderArr[0].direct = 1;
			}
		}

		//���������ڳ��ַ�ת������
		 AlexCountByNormalizeHiSpeed(AXLE_ERR_TIME);	//3����

		 SumNormalCount = AlexNormalCountQueue();

		if(SumNormalCount > 6) SumNormalCount = 6;
		for(i = 0, j = 0; (i < SumNormalCount) && (j < m_nAlexCount); j++)
		{
			if(AlexRecoderArrNormal[j].direct > 0)
			{
				pCar->AxleInfo[i].axle_wet = AlexRecoderArrNormal[j].AlexKg;
				pCar->AxleInfo[i].axle_type = AlexRecoderArrNormal[j].bDouble;
				pCar->AxleInfo[i].axle_len = AlexRecoderArrNormal[j].nTimeInterval;
				i++;
			}
		}

		//m_nAlexCount = SumNormalCount;
		return SumNormalCount;
	}


	//�����ۺϸ���
	//return SumCount;
}

static void PreAxle_Add_Tick(int nTime)
{
	if(m_nAlexCount >= ALEX_ARR_MAX) 
		return;

	if(m_nAlexCount > 0)
	{
		AlexRecoderArr[m_nAlexCount - 1].nTimeInterval = MsgTimeAdd(AlexRecoderArr[m_nAlexCount - 1].nTimeInterval, nTime);
	}
}

static void Empty_Scaler_Business(void)
{
	CarInfo* pCar = NULL;

	//��̨�����з��գ����������
	//�����ǿ��ܸպ����ϴ���ADֵ��������λ����
	Scaler_Set_Zero();

	//�Գ������н��д���
	pCar = CarQueue_Get_Head();
	while(pCar != NULL) 
	{
		if(pCar->nStatus < stCarWaitPay) 
		{
			//��û������շѵĿ϶�����
			CarQueue_Car_Kill(pCar);
			
		}
		
		//��Ԫ�շ�ͤ�ڳ�
		if(pCar->nStatus == stCarFarPay)
		{
			if((pCar->nAxleNum > 2) || (pCar->nAxleNum == 0))
				CarQueue_Car_Kill(pCar);
		}

//		//���ڴ��շ�(�������շ�)����ӽɷ�״̬֮��ĳ�Ӧ�ûᴥ���ظУ������ɾ��
//		else if(pCar->nStatus < stCarFarPay) {
//			//Ӧ����֤�ظ�û��
//			if(IsOutCoilHold() == FALSE)	//����ظ�û���źţ��������д��ڴ��շѺ����뿪״̬�ĳ�
//			{
//				CarKill(pCar);
//				bResetValid = TRUE;
//			}
//		}
		pCar = pCar->pNext;
	}
	CarQueue_Remove_Dead();
	Clear_Axle_Recoder();
#if 0
	//���ʱ�������Ļ�Ǵ����ģ������г���׼������
	if(IsInRaysHold() != FALSE) {
		m_pCar = CreateCar();
		if(m_pCar != NULL) {
			CarBeginComming(m_pCar);
			CarAddTail(m_pCar);
		}
	} else {
		//��ʶ��������
		//if(SGetAlexZero() < 10)
		{
			SetAlexZero();
		}
	}
#endif
}

//ϵͳ��ʱ����
static uint32 m_nScalerEmpetyCounter = 0;

void Clear_ScalerEmpty_Count(void)
{
	m_nScalerEmpetyCounter = 0;
}

void SysTimer_Business(void)
{
	//ϵͳ���м�ʱ
	if(Get_Static_Weight() < SCALER_EMPTY_WET) 
	{
		//����ʱ��
		m_nScalerEmpetyCounter = MsgTimeAdd(m_nScalerEmpetyCounter, CAR_TICK);

		//�ж��Ƿ���Ҫ��λ, �ճ��ҹ�Ļû�б���ס
		if(m_nScalerEmpetyCounter >= SCALER_CLEAR_TIME) 
		{
			if(!Get_LC_Status(2)) 
			{
				//debug(Debug_Notify, "Empty Scaler Reset! tick = %ldms\r\n", Get_Sys_Tick());
				m_nScalerEmpetyCounter = 0;
				Empty_Scaler_Business();	//�ճӴ���
			}
			else 
			{
				m_nScalerEmpetyCounter = 0;
			}
		}
	}
	else 
	{
		m_nScalerEmpetyCounter = 0;
	}

	//�����ᶨʱ��
	PreAxle_Add_Tick(CAR_TICK);
}

int32 Apply_LongCar_Axle(CarInfo *pCar)
{
	int i, j;
	int SumCount = 0;
	int IncCount = 0;
	int SumNormalCount = 0;	
	int DecCount = 0;

	//���������кͲ���
	for(i = 0; i < m_nAlexCount; i++)
	{
		SumCount += AlexRecoderArr[i].direct;
		if(AlexRecoderArr[i].direct > 0) IncCount++;
		if(AlexRecoderArr[i].direct < 0) DecCount++;
	}

	//����ȫ��Ϊ��������-----------------------------------
	if(DecCount == 0)
	{
		for(i = 0; i < m_nAlexCount; i++)
		{
			pCar->AxleInfo[i].axle_wet = AlexRecoderArr[i].AlexKg;
			pCar->AxleInfo[i].axle_type = AlexRecoderArr[i].bDouble;
			pCar->AxleInfo[i].axle_len = AlexRecoderArr[i].nTimeInterval;
		}
		return SumCount;
	}

	//����ȫ��Ϊ��������
	if(IncCount == 0) return SumCount;

	//���������ڳ��ַ�ת������
	 AlexCountByNormalizeHiSpeed(AXLE_ERR_TIME);	//3����

	 SumNormalCount = AlexNormalCountQueue();

	if(SumNormalCount > 6) SumNormalCount = 6;
	for(i = 0, j = 0; (i < SumNormalCount) && (j < m_nAlexCount); j++)
	{
		if(AlexRecoderArrNormal[j].direct > 0)
		{
			pCar->AxleInfo[i].axle_wet = AlexRecoderArrNormal[j].AlexKg;
			pCar->AxleInfo[i].axle_type = AlexRecoderArrNormal[j].bDouble;
			pCar->AxleInfo[i].axle_len = AlexRecoderArrNormal[j].nTimeInterval;
			i++;
		}
	}

	return SumNormalCount;
}

int32 Get_Axle_Weight(uint8 axle_num)
{
	return 0;
}

void Save_Car_AxleWet(CarInfo *pCar)
{
	return;
}

void Save_Axle_Info(AlexRecoder* pAxle)
{
	//��¼����Ϣ
	m_AlexRecoder.direct = pAxle->direct;
	m_AlexRecoder.AlexKg = pAxle->AlexKg; 
	m_AlexRecoder.nTimePulse = pAxle->nTimePulse;
	m_AlexRecoder.bDouble = pAxle->bDouble;

	AddAlexRecoder(&m_AlexRecoder);
}

//������¼
void Clear_Axle_Recoder(void)
{
	m_nAlexCount = 0;					//�����
	m_nLongCarFrontAlexCount = 0;	//������ǰ�벿���������������ظ�ʱ������������������Ǽ������ĸ�����
	m_bIsLongAlexCount = FALSE;		//��������־����
}


//������֮���ۺϴ���������
int32 Apply_Car_Axle(CarInfo* pCar)
{
	if(pCar == NULL) return 0;

    debug(Debug_Business, ">>��������,������=%d\r\n", m_nAlexCount);

	//û�յ����źŴ���
	if(m_nAlexCount == 0)
	{
		return PraseNoneAlexCar(pCar);
	}

	//��1����������
	if(m_nAlexCount == 1)
	{
		return PraseSingleAlexCar(pCar);
	}

	//����2��С�����⣬��Ҫ��С���������У��ص��ע����
	if(m_nAlexCount == 2)
	{
		return PraseLittleCarAlex(pCar);
	}

	//����3�������ϵĳ�
	return PraseCarAlexBigger3(pCar);
}

