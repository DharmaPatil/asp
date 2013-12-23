#include "Mode7AlexParser.h"
#include <math.h>  
#include <stdlib.h>
#include "device_info.h"
#include "axletype_business.h"
#if 0
//2S�����ݻ���
#define MAX_DELAY  10						//���峤��Ϊ������
static int nBigDelayArr[MAX_DELAY];			//
static int nBigDelayIndex;					//

static int				nPulseWidth = 0;	//��ʶ�����˲�
static int				nAlexStopKg = 0;	//��ͣ����ʶ������һ���ȡ������
static int				nLeaveStopKg = 0;	//��ͣ����ʶ������һ���ȡ������

//��̬����50Hz��5���ֵ�˲���ʼ��
static int nAlexRaiseBefore200msKg = 0;
static int nAlexRaiseBeforeTmp = 0;
static int nAlexFallAfter200msKg	 = 0;
static int nAlexFallAfterAvgKg	 = 0;
static int nAlexFallAfterAvgIndex = 0;
static int   nAlexMaxKg = 0; 


static int nLeaveRaiseBefore200msKg = 0;
static int nLeaveRaiseBeforeTmp = 0;
static int nLeaveFallAfter200msKg	 = 0;
static int nLeaveFallAfterAvgKg	 = 0;
static int nLeaveFallAfterAvgIndex = 0;
static int nLeaveMaxKg = 0; 

static void TotalDelayQueue(int nBigKg)			//
{
	if(nBigDelayIndex >= MAX_DELAY) nBigDelayIndex= 0;

	//ÿ200msһ������
	nBigDelayArr[nBigDelayIndex++] = nBigKg;
}

//��ȡ������ǰ������
static int GetBigWeightBefore2S()
{
	int n = nBigDelayIndex;

	if(n >= MAX_DELAY) n= 0;

	return nBigDelayArr[n];
}

static void MsgPostAlexAdd(void *pDecb,int event,int alexwet,int alexvalidtime,int alexmax)
{
	sMode7AlexDyncProc *pDync = (sMode7AlexDyncProc *)pDecb;
	pDync->nAlexINorRemove 		= event;
	pDync->nAlexWet				= alexwet;
	pDync->nAlexPulseWith			= alexvalidtime;
	pDync->nAlexMaxWet			= alexmax;
	pDync->nAlexNo				= 2;
	if(alexvalidtime != 0)
	{
		pDync->fSpeed				= 2.0f * (float)(pDync->mSpeedLen * 0.8f * 3.6f) / alexvalidtime;
	}

#if 1
	if(pDync->nAlexINorRemove == 1)
	{
		if(pDync->bDouble != 2)
			pDync->pADP(pDecb);
		else
			debug(Debug_Business, "Warning:TyreQueue empty, ingore this AxleAdd event!\r\n");
	}
	else
	{
		pDync->pADP(pDecb);
	}	
#else
	pDync->pADP(pDecb);
#endif
}

//�������ź�
static void AlexParser_SendAlexSignal(void *pDecb,int BfRaiseKg, int AftFallKg, int AftFallAvg, int nAlexMax)
{
	//����ǰ��ֵ
	if((AftFallKg - BfRaiseKg) >= -200)
	{
		//�������ֵ������ǰ��ֵ�����Ȼ��Щ
		MsgPostAlexAdd(pDecb,1, AftFallAvg - BfRaiseKg, nPulseWidth, nAlexMax);
	}

	//������峬������ͣ�����ϣ����ж�������ǰ��ֵ
	else if(nPulseWidth > 1600)	//800Hz�����ʣ�1600��������
	{
		if((GetBigWeightBefore2S() - AftFallKg)  <= 200)
		{
			MsgPostAlexAdd(pDecb,1, nAlexStopKg - BfRaiseKg, nPulseWidth, nAlexMax);
		}
		else
		{
			MsgPostAlexAdd(pDecb,0, nAlexStopKg - BfRaiseKg, nPulseWidth, nAlexMax);
		}
	}
	else 
	{
		int diff_kg = AftFallKg - BfRaiseKg;

		//ȡ���������ز�����Ҫʹ�ø�������
		//diff_kg = (diff_kg < 0) ? -diff_kg : diff_kg;

		MsgPostAlexAdd(pDecb,0, diff_kg, nPulseWidth, nAlexMax);
	}
}

//ˢ�´��̨����
static void AlexParser_UpdateWet(int bigScalerKg)
{
	static int cnt_200ms = 0;	
	if(++cnt_200ms >= 16)
	{
		//�ӳٶ���
		TotalDelayQueue(bigScalerKg);
		cnt_200ms = 0;
	}
}
#endif
 /*
 * ����:    ��̬�ᴦ���ʼ��
 */
char  Mode7AlexDyncProcInit(void* pDecb)
{
	if (pDecb == 0) return (char)-1;

	SMODE7ALEXDYNC(pDecb).nAlexValid = 0;
	SMODE7ALEXDYNC(pDecb).nPulseWidth = 0;
	SMODE7ALEXDYNC(pDecb).nUpFlag = 0;
	SMODE7ALEXDYNC(pDecb).nUpCnt = 0;
	SMODE7ALEXDYNC(pDecb).nUPkeepCnt = 0;
	SMODE7ALEXDYNC(pDecb).nDownkeepCnt = 0;
	SMODE7ALEXDYNC(pDecb).nDownCnt = 0;
	//SALEXDYNC(pDecb).fWetUp = 0;
	//SALEXDYNC(pDecb).fWetDown = 0;
	if(SMODE7ALEXDYNC(pDecb).mSpeedLen == 0)
		SMODE7ALEXDYNC(pDecb).mSpeedLen = 80;
	
	SMODE7ALEXDYNC(pDecb).fWet = 0;      
	SMODE7ALEXDYNC(pDecb).fWetReliab = 0;     
	SMODE7ALEXDYNC(pDecb).iPowerOnIndex = 0;  
	//SALEXDYNC(pDecb).iPowerOnDelay = 0; 

	SMODE7ALEXDYNC(pDecb).nAlexNo = 0; 
	SMODE7ALEXDYNC(pDecb).nAlexINorRemove = 0; 
	SMODE7ALEXDYNC(pDecb).nAlexWet = 0; 
	SMODE7ALEXDYNC(pDecb).nAlexPulseWith = 0; 
	SMODE7ALEXDYNC(pDecb).nAlexMaxWet = 0; 
	
	//memset((char *)SALEXDYNC(pDecb).fAD1Buf,0,sizeof(SALEXDYNC(pDecb).fAD1Buf));
	//memset((char *)SALEXDYNC(pDecb).fAD2Buf,0,sizeof(SALEXDYNC(pDecb).fAD2Buf));

    return 0;
}

//===================================================================
//��̥ʶ����
/* �������� */
#if 0
static void Judge_Axle_Business(sMode7AlexDyncProc *pDync)
{
	TyreDevice tyreinfo = {0};
	
	//��̥����Ϊ��
	if(TyreQueue_len() == 0) 
	{	
		pDync->bDouble = 2;
	}
	else
	{
		if(TyreQueue_Get_Rear(&tyreinfo) == 0)
		{
			pDync->bDouble = tyreinfo.bDouble;
			debug(Debug_Business, "Tryetype=%d\r\n", pDync->bDouble);
		}
		else
		{
			pDync->bDouble = 2;
		}
		//���������̥����
		TyreQueue_Init();
	}
}
#endif
/*
 * ����:    ��̬�ᴦ��
 */
float Mode7AlexDyncProc(void* pDecb, float * pInArr, int nInCount)
{
	return pInArr[0];
#if 0
	sMode7AlexDyncProc *pDync = (sMode7AlexDyncProc *)pDecb;
	float mScaleWet = pInArr[0];
	float mAlexWet 	= pInArr[3];
	float mLeaveWet = pInArr[1];

	//������������
	if(pDecb == 0) return mScaleWet;
	if(pDync->iPowerOnIndex < pDync->iPowerOnDelay) 
	{
	    pDync->iPowerOnIndex++;
	    return 0;
	}

	//��50HzƵ�ʶ�����5���˲�
	AlexParser_UpdateWet((int)mScaleWet);
		
	//================================================================================================
	//================================================================================================
	// ǰ��ʶ����
	if(mAlexWet > pDync->fWetUp)
	{
		pDync->nUpCnt++;
		pDync->nDownCnt = 0;

		if(pDync->nUpCnt > 2) //������2�α������趨��ADֵ���ϣ���ʾ����ϵ�����
		{
			
			if(pDync->nUpFlag == 0)
			{
				if(pDync->nAlexValid == 1)
				{
					nAlexRaiseBeforeTmp = (int)mScaleWet;
				}
				else
				{
					nAlexRaiseBefore200msKg = (int)mScaleWet;
				}
				pDync->nUpFlag = 1;				//��¼�Ѿ��Ϸ�ת
				
				//�����ᴥ����Ϣ
				//MsgPost(msgInAlexCome);
			}	
		}		
	}
	else if( mAlexWet < pDync->fWetDown )
	{
		pDync->nDownCnt++;
		if (pDync->nDownCnt >= 2)
		{
			if(pDync->nUpFlag  == 1) //�ڵ����趨ֵ�Ժ�������Ϲ�һ�εĶ�������ʾ���ˡ�
			{
				pDync->nAlexValid = 1;				//��¼����Ч
				pDync->nDownkeepCnt = 0;
				
				//����̥��, ������̥�����Ƿ�Ϊ�վ����¼���Ӧ
				pDync->bDouble = 1;
				if(Get_Tire_ErrStatus())
				{
					Judge_Axle_Business(pDync);
				} 
			}				
			pDync->nUpCnt= 0;
			pDync->nDownCnt = 0;
			pDync->nUpFlag = 0;
			
		}
	}
	//������Ч��ʱ����м�������¼�����ʱ��
	if(pDync->nUpFlag == 1)
	{
		if(pDync->nPulseWidth < 100000000)		//��ֹ���
		{
			pDync->nPulseWidth++;
			nPulseWidth = pDync->nPulseWidth;
			if(pDync->nPulseWidth == 850)	//��ͣ������1S��ʱȡһ�����̨����,AlexParser_Get200MsAvgWet�е���ʱ����850�β���
			{
				nAlexStopKg = (int)mScaleWet;
			}
		}
	}

	//ʶ������Ч
	if(pDync->nAlexValid ) //����������һ����ʱ�ж�
	{
		//��������ľ�ֵ
		nAlexFallAfter200msKg = (int)mScaleWet;
		nAlexFallAfterAvgKg += nAlexFallAfter200msKg;
		nAlexFallAfterAvgIndex++;

		if((pDync->nDownkeepCnt++ >= 16*10) || (pDync->nUpFlag == 1))		//10�����Ժ��˲�ȡ�Ӵ����½��ؿ�200ms�� ������
		{
			AlexParser_SendAlexSignal(pDecb,nAlexRaiseBefore200msKg, nAlexFallAfter200msKg, nAlexFallAfterAvgKg/nAlexFallAfterAvgIndex,nAlexMaxKg);

			pDync->nPulseWidth = 0;			//��������
			pDync->nAlexValid = 0;
			pDync->nDownkeepCnt = 0;
			
			nAlexFallAfter200msKg 		=  0;
			nAlexRaiseBefore200msKg	 = 0;
			
			nAlexFallAfterAvgKg = 0;
			nAlexFallAfterAvgIndex = 0;
			nAlexMaxKg = 0;

			
			if(pDync->nUpFlag == 1)
			{
				nAlexRaiseBefore200msKg = nAlexRaiseBeforeTmp;
			}
		}
	}

	//��¼��ķ�ֵ
	if((pDync->nUpFlag == 1) && (mAlexWet > nAlexMaxKg))
	{
		nAlexMaxKg = (int)mAlexWet;
	}
	


		
	//================================================================================================
	//================================================================================================
	// ����ʶ����
	if(mLeaveWet > pDync->fLeaveWetUp)
	{
		pDync->nLeaveUpCnt++;
		pDync->nLeaveDownCnt = 0;

		if(pDync->nLeaveUpCnt > 2) //������2�α������趨��ADֵ���ϣ���ʾ����ϵ�����
		{
			
			if(pDync->nLeaveUpFlag == 0)
			{
				pDync->nLeaveUpFlag = 1;				//��¼�Ѿ��Ϸ�ת
				
				//�����ᴥ����Ϣ
				//MsgPost(msgInLeaveCome);
			}	
		}		
	}
	else if( mLeaveWet < pDync->fLeaveWetDown )
	{
		pDync->nLeaveDownCnt++;
		if (pDync->nLeaveDownCnt >= 2)
		{
			if(pDync->nLeaveUpFlag  == 1) //�ڵ����趨ֵ�Ժ�������Ϲ�һ�εĶ�������ʾ���ˡ�
			{
				pDync->nLeaveValid = 1;				//��¼����Ч
				pDync->nDownkeepCnt = 0;
			}				
			pDync->nLeaveUpCnt= 0;
			pDync->nLeaveDownCnt = 0;
			pDync->nLeaveUpFlag = 0;
			
		}
	}
#if 0
	//������Ϣ
	if(pDync->nLeaveValid ) //����������һ����ʱ�ж�
	{
		if((pDync->nLeaveDownkeepCnt++ >= 16*10) || (pDync->nLeaveUpFlag == 1))		//10�����Ժ��˲�ȡ�Ӵ����½��ؿ�200ms�� ������
		{	
			pDync->nLeaveValid = 0;
			pDync->nLeaveDownkeepCnt = 0;
			
			pDync->nAlexINorRemove		= 0;
			pDync->nAlexNo				= 3;
			pDync->pADP(pDecb);
		}
	}
#endif
	return mScaleWet;
#endif	
}

