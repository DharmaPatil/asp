#include "includes.h"
#include "car_queue.h"
#include "sys_config.h"
#include "weight_param.h"
#include "axle_business.h"
#include "car_business.h"
#include "device_info.h"
#include "wet_algorim.h"
#include "debug_info.h"

#define SCALER_EMPTY_MAX		200		//�ճ����ֵ��������ڸ�ֵ����Ϊ���г��ڳ���
#define SCALER_BIG_CAR_MIN		2000	//��(�������ϵĳ�)����С����
#define  ALEX_ARR_MAX			20
#define AXLE_ERR_TIME			2500	//������ʱ��

#define SCALER_EMPTY_WET		500		//�ճ���ֵ100kg
#define SCALER_CLEAR_TIME		120000	//�ճ�ʱ����ֵ20s

static volatile int32 m_nAlexCount = 0;
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

//������¼
void Clear_Axle_Recoder(void)
{
	m_nAlexCount = 0;				//�����
}

//����AlexRecoderArr������쳣���
static void Modify_AxleRecoder_Overflow(void)
{
	int i = 0;
	
	for(i = 1; i < m_nAlexCount; i++)
	{
		if(AlexRecoderArr[i].direct != AlexRecoderArr[i-1].direct)
		{
			//�涨ʱ���ڲ��ܷ���
			if(AlexRecoderArr[i-1].nTimeInterval < AXLE_ERR_TIME)
			{
				AlexRecoderArr[i].direct = AlexRecoderArr[i-1].direct;
			}
			else
			{
				//����һ�ԼӼ���,��������[i+1]�������Ԫ�ص�[i-1]��ַ��
				if(m_nAlexCount >= 2) m_nAlexCount -= 2;	      
				if((i > 0) && (m_nAlexCount + 1 > i))
				{
					memmove(&AlexRecoderArr[i-1], &AlexRecoderArr[i+1], (m_nAlexCount-i+1)*sizeof(AlexRecoder));
					i--;
				}					
			}			
		}
	}
}

//�������¼
static void AddAlexRecoder(AlexRecoder* pAlexRecoder)
{
	if(m_nAlexCount >= ALEX_ARR_MAX)
	{
		Modify_AxleRecoder_Overflow();
		if(m_nAlexCount >= ALEX_ARR_MAX) return;
	}

	AlexRecoderArr[m_nAlexCount].direct = pAlexRecoder->direct;
	AlexRecoderArr[m_nAlexCount].AlexKg = pAlexRecoder->AlexKg;
	AlexRecoderArr[m_nAlexCount].AlexKgRef = pAlexRecoder->AlexKgRef;
	AlexRecoderArr[m_nAlexCount].nTimePulse = pAlexRecoder->nTimePulse;
	AlexRecoderArr[m_nAlexCount].bDouble = pAlexRecoder->bDouble;
	AlexRecoderArr[m_nAlexCount].nTimeInterval = 0;

	m_nAlexCount++;
}

//��������Ϣ
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
		if(Get_Static_Weight() - pCar->nScalerKgComing > 600)
		{		
			for(i = 0; i < 2; i++)
			{
				pCar->AxleInfo[i].axle_wet= (Get_Static_Weight() - pCar->nScalerKgComing) / 2;
				pCar->AxleInfo[i].axle_type = 0;
				pCar->AxleInfo[i].axle_len = 100;
			}
			debug(Debug_Business, "UpDownWet>600,maybe a little car!\r\n");
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

	//�������ж�
	if(AlexRecoderArr[0].direct > 0)
	{
		return 2;
	}
	else
	{
		if(Get_Static_Weight() - pCar->nScalerKgComing < -200)
		{
			return -2;
		}
		else
		{
			return 2;
		}
	}
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
        //С������,ͬʱ����ʱ��������
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
	int nAlexRefMax;

	//������ʶ����������������ź�
	nAlexRefMax = 0;
	for(i = 0; i < m_nAlexCount; i++)
	{
		//Ѱ�����ֵ
		if(AlexRecoderArr[i].AlexKgRef > nAlexRefMax)
		{
			nAlexRefMax = AlexRecoderArr[i].AlexKgRef;
		}
	}
	for(i = 0, j = 0; i < m_nAlexCount; i++)
	{
		//������Ч�����ź�
		//if(AlexRecoderArr[i].AlexKgRef * 10 >= nAlexRefMax)
		if(AlexRecoderArr[i].AlexKgRef * 28 >= nAlexRefMax)		//28��Ҫ��ʶ����ᣬ���ϵ����������Ϊ�ɸ�
		{
			AlexRecoderArr[j] = AlexRecoderArr[i];
			j++;
		}
	}
	m_nAlexCount = j;


	//���������ۺϲ���
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
		
		 //��������Ϊ1�����������������ʵ�ʲ����ܷ�����������ڣ��������ʶ�����
		 if(SumNormalCount == 1)
		 {
			SumNormalCount = 0;
			if(IncCount > DecCount)
			{
				for(i = 0, j = 0; i < m_nAlexCount; i++)
				{				
					if(AlexRecoderArr[i].direct > 0)
					{
						AlexRecoderArrNormal[j].AlexKg = AlexRecoderArr[i].AlexKg;
						AlexRecoderArrNormal[j].bDouble = AlexRecoderArr[i].bDouble;
						AlexRecoderArrNormal[j].nTimeInterval = AlexRecoderArr[i].nTimeInterval;
						j++;
						SumNormalCount++;
					}
				}
			}
			else
			{
				for(i = 0, j = 0; i < m_nAlexCount; i++)
				{				
					if(AlexRecoderArr[i].direct > 0)
					{
						AlexRecoderArrNormal[j].AlexKg = AlexRecoderArr[i].AlexKg;
						AlexRecoderArrNormal[j].bDouble = AlexRecoderArr[i].bDouble;
						AlexRecoderArrNormal[j].nTimeInterval = AlexRecoderArr[i].nTimeInterval;
						j++;
						SumNormalCount++;
					}
				}
			}
		 }

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
			else
			{
 				if(i > 0) i--;		//�����ѱ���ļ�������
			}
		}

		return SumNormalCount;
	}
	//����������������һ����Ϊ����	
	else
	{
		if((DecCount == 1) && (IncCount == 2))
		{
			for(i = 1; i < m_nAlexCount; i++)
			{
				pCar->AxleInfo[i].axle_wet = AlexRecoderArr[i-1].AlexKg;
				pCar->AxleInfo[i].axle_type = AlexRecoderArr[i-1].bDouble;
				pCar->AxleInfo[i].axle_len = AlexRecoderArr[i-1].nTimeInterval;
			}
			return IncCount;
		}


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
			else
			{
 				if(i > 0) i--;		//�����ѱ���ļ�������
			}
		}

		return SumNormalCount;
	}
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
	//��Ҫ������λ���ٲ���Ч
	Scaler_Set_Zero(TRUE);

	//�Գ������н��д���
	pCar = CarQueue_Get_Head();
	while(pCar != NULL) 
	{
		if(pCar->nStatus < stCarWaitPay) 
		{
			//��û������շѵĿ϶�����
			CarQueue_Car_Kill(pCar);			
		}
		
		pCar = pCar->pNext;
	}
	CarQueue_Remove_Dead();
	Clear_Axle_Recoder();
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
				Empty_Scaler_Business();	//�ճӴ���
			}

			m_nScalerEmpetyCounter = 0;
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
	int nAlexRefMax;

	//������ʶ����������������ź�
	nAlexRefMax = 0;
	for(i = 0; i < m_nAlexCount; i++)
	{
		//Ѱ�����ֵ
		if(AlexRecoderArr[i].AlexKgRef > nAlexRefMax)
		{
			nAlexRefMax = AlexRecoderArr[i].AlexKgRef;
		}
	}
	for(i = 0, j = 0; i < m_nAlexCount; i++)
	{
		//������Ч�����ź�
		if(AlexRecoderArr[i].AlexKgRef * 10 >= nAlexRefMax)
		{
			AlexRecoderArr[j] = AlexRecoderArr[i];
			j++;
		}
	}
	m_nAlexCount = j;

	//���������ۺϲ���
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
		else
		{
 			if(i > 0) i--;		//�����ѱ���ļ�������
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
	memcpy(&m_AlexRecoder, pAxle, sizeof(AlexRecoder));
	m_AlexRecoder.nTimeInterval = 0;

	AddAlexRecoder(&m_AlexRecoder);
}

//������֮���ۺϴ���������
int32 Apply_Car_Axle(CarInfo* pCar)
{
	if(pCar == NULL) return 0;

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






