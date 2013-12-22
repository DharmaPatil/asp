#include <string.h>
#include "car_queue.h"
#include "car_business.h"
#include "scszc_debug_info.h"

#define MAX_CAR_NUM 			10
static  CarInfo     carArray[MAX_CAR_NUM];
static  CarInfo   *pCarHead=NULL;
static  CarInfo   *pCarTail = NULL;
static  int32 	m_nCarCount = 0;

//������Ϣ��λ
static void CarInfo_Reset(CarInfo* pCar)
{
	if(pCar == NULL) 
		return;

	memset(pCar, 0, sizeof(CarInfo));
}

//���г�ʼ��
void CarQueue_Init(void)
{
	int i = 0;
	for(i = 0;  i  < MAX_CAR_NUM; i++)
	{
		carArray[i].bUse = FALSE;
		carArray[i].pNext = NULL;
		carArray[i].pPrev = NULL;		
	}

	m_nCarCount = 0;
	pCarHead = NULL;
	pCarTail = NULL;

}

//��������
CarInfo* CarQueue_Create_Car(void)
{
	int i = 0;
	for(i = 0;  i  < MAX_CAR_NUM; i++)
	{
		if(carArray[i].bUse ==  FALSE)
		{
			CarInfo_Reset(&carArray[i]);
			return &(carArray[i]);
		}		
	}

	debug(Debug_Business, "carqueue full\r\n");
	//for(i = 0;  i < MAX_CAR_NUM; i++)
	//{
	//	if(carArray[i].bResponse)
	//	{
	//		CarInfo_Reset(&carArray[i]);
	//		debug(Debug_Business, "carqueue circle\r\n");
	//		return &(carArray[i]);
	//	}		
	//}
	
	return NULL;
}

//�Ӷ���β��ӳ�
void CarQueue_Add_Tail(CarInfo* pCar)
{
	if(pCar == NULL) return;

	if(pCarTail == NULL)
	{
		pCarTail    = pCar;
		pCarHead = pCar;
		pCar->bUse   = TRUE;
		pCar->pNext = NULL;
		pCar->pPrev = NULL;
	}
	else
	{
		pCar->bUse   = TRUE;
		pCar->pPrev = pCarTail;
		pCarTail->pNext = pCar;
		pCarTail = pCar;
		pCarTail->pNext = NULL;
	}
	m_nCarCount++;
}

//�Ӷ���ͷ��ӳ�
void CarQueue_Add_Head(CarInfo* pCar)
{
	if(pCar == NULL) return;
	
	if(pCarHead == NULL)
	{
		pCarTail    = pCar;
		pCarHead = pCar;
		pCar->bUse   = TRUE;
		pCar->pNext = NULL;
		pCar->pPrev = NULL;
	}
	else
	{
		pCar->bUse   = TRUE;
		pCar->pNext = pCarHead;
		pCarHead->pPrev= pCar;
		pCarHead = pCar;
		pCarHead->pPrev = NULL;
	}
	
	m_nCarCount++;
}

//���ó���ɾ��
void CarQueue_Car_Kill(CarInfo* pCar)
{
	if(pCar == NULL) return;

	pCar->bCarIsDead = TRUE;
	debug(Debug_Business, "Kill car! CarState: %s\r\n", Get_Car_StatusName(pCar->nStatus));
}

//�жϳ��Ƿ���ɾ��
BOOL CarQueue_Car_Is_Dead(CarInfo* pCar)
{
	if(pCar == NULL) return FALSE;

	return pCar->bCarIsDead;
}

//�Ƴ�Ҫɾ���ĳ���
void CarQueue_Remove_Dead(void)
{
	CarInfo* pIter = NULL;
	CarInfo* pCar = NULL;
	pIter = CarQueue_Get_Head();
	while(pIter != NULL)
	{
		pCar = pIter;
		pIter = pIter->pNext;

		if(pCar->bCarIsDead == TRUE)
		{			
			pCar->bUse = FALSE;

			if(m_nCarCount > 0) m_nCarCount--;
			
			if(pCar->pPrev != NULL)
			{
				pCar->pPrev->pNext = pCar->pNext;
			}
			else
			{
				pCarHead = pCar->pNext;
			}
			
			if(pCar->pNext!= NULL)
			{
				pCar->pNext->pPrev = pCar->pPrev;
			}
			else
			{
				pCarTail = pCar->pPrev;
			}
			
			pCar->pPrev =  NULL;
			pCar->pNext = NULL;
		}		
	}	
}

//�Ƴ���ͷ
CarInfo*  CarQueue_Remove_Head(void)
{
	CarInfo* pTempCar = NULL;
	if(pCarHead == NULL) return NULL;

	pTempCar  = pCarHead;

	pCarHead = pCarHead->pNext;
	if(pCarHead == NULL) 
	{
		pCarTail = NULL;
	}
	else
	{
		pCarHead->pPrev = NULL;
	}

	if(m_nCarCount > 0)
		m_nCarCount--;

	pTempCar->bUse = FALSE;
	pTempCar->pNext = NULL;
	pTempCar->pPrev = NULL; 

	return pCarHead;	
}

//�Ƴ���β
CarInfo*  CarQueue_Remove_Tail(void)
{
	CarInfo* pTempCar = NULL;
	if(pCarTail == NULL) return NULL;

	pTempCar  = pCarTail;

	pCarTail = pCarTail->pPrev;	
	if(pCarTail == NULL) 
	{
		pCarHead = NULL;
	}
	else
	{
		pCarTail->pNext = NULL;
	}

	if(m_nCarCount > 0) m_nCarCount--;

	pTempCar->bUse = FALSE;
	pTempCar->pNext = NULL;
	pTempCar->pPrev = NULL;

	return pCarTail;
}

//�Ƿ�Ϊβ��
BOOL  CarQueue_Car_Is_Tail(CarInfo* pCar)
{
	if(pCar == NULL) return FALSE;
	if(pCarTail == NULL) return FALSE;
	if(pCar == pCarTail) return TRUE;

	return FALSE;
}


//�ж��Ƿ�Ϊͷ��
BOOL CarQueue_Car_Is_Head(CarInfo* pCar)
{
	if(pCar == NULL) return FALSE;
	if(pCarHead == NULL) return FALSE;

	if(pCar == pCarHead) return TRUE;

	return FALSE;
}

//�ж϶����Ƿ�Ϊ��
BOOL CarQueue_Is_Empty(void)
{
	if(pCarHead == NULL) return TRUE;

	return FALSE;
}

//��ȡ���г�ͷ
CarInfo* CarQueue_Get_Head(void)
{
	return pCarHead;
}

//��ȡ���г�β
CarInfo* CarQueue_Get_Tail(void)
{
	return pCarTail;
}

//��ȡ�����г�������
int32 CarQueue_Get_Count(void)
{
	int32  i = 0;
	CarInfo* pCar = NULL;
	pCar = CarQueue_Get_Head();
	while(pCar != NULL)
	{
		i++;
		pCar = pCar->pNext;
	}
	
	return i;
}

//��ȡ�ڳ��ϵĳ��ĸ���
int32 CarQueue_Get_OnScaler_Count(void)
{
	int32  i = 0;
	CarInfo* pCar = NULL;
	pCar = CarQueue_Get_Head();
	while(pCar != NULL)
	{
		if((pCar->nStatus >= stCarComing) && (pCar->nStatus <= stCarLeaving) && (!pCar->bCarIsDead)) 
		{
			i++;
		}

		pCar = pCar->pNext;
	}

	return i;
}

CarInfo *CarQueue_Get_OnScaler_FirstCar(void)
{
	CarInfo* pCar = NULL;
	
	pCar = CarQueue_Get_Head();
	while(pCar != NULL)
	{
		if((pCar->nStatus >= stCarComing) && (pCar->nStatus <= stCarLeaving)) 
		{
			break;
		}

		pCar = pCar->pNext;
	}

	return pCar;
}


//�жϳ���״̬�Ƿ�
BOOL CarQueue_CarState_Is_Change(CarInfo* pCar)
{
	if(pCar == NULL) return FALSE;

	return pCar->bStateChanged;
}

#if 0
//�Ƴ�����
BOOL CarQueue_Remove(CarInfo* pCar)
{
	if(pCar == NULL) return FALSE;
	if(pCarTail == NULL) return FALSE;
	if(pCarHead== NULL) return FALSE;
	if(pCar->bUse == FALSE) return FALSE;

	if(m_nCarCount > 0) m_nCarCount--;

	pCar->bUse = FALSE;

	if(pCar->pPrev != NULL)
	{
		pCar->pPrev->pNext = pCar->pNext;
	}
	else
	{
		//����pCar��Head
		pCarHead = pCar->pNext;
	}
	
	if(pCar->pNext!= NULL)
	{
		pCar->pNext->pPrev = pCar->pPrev;
	}
	else
	{
		//����pCar��Tail
		pCarTail = pCar->pPrev;
	}
	
	pCar->pPrev =  NULL;
	pCar->pNext = NULL;

	return TRUE;	
}


//��ȡϵͳ�д��ڵĳ�������
int32 CarQueue_Get_LiveCount(void)
{
	int32  i = 0;
	CarInfo* pCar = NULL;
	pCar = CarQueue_Get_Head();
	while(pCar != NULL)
	{
		if(pCar->bCarIsDead == FALSE)
		{
			i++;
		}

		pCar = pCar->pNext;
	}
	
	return i;
}
#endif
//��ȡ���Խɷѣ����һ�δ�ɷѵĳ�������
int32 CarQueue_Get_CanPayCount(void)
{
	int32  i = 0;
	CarInfo* pCar = NULL;
	pCar = CarQueue_Get_Head();
	while(pCar != NULL)
	{
		//����û���뿪�����߳���������ӽɷ�״̬
		if((pCar->nStatus < stCarLeaving) || (pCar->nStatus == stCarFarPay)) 
		{
			if(pCar->bWetVaild == TRUE)
			{
				i++;
			}
		}

		pCar = pCar->pNext;
	}
	return i;
}




