#include "Mode8AlexParser.h"
#include <math.h>  
#include <stdlib.h>

#include "axletype_business.h"
#include "device_info.h"
#include "FirFilter.h"
#include "axle_business.h"
#include "scszc_debug_info.h"
#include "Weight_Param.h"
#include "debug_info.h"

#define ALEXUPDOWN	150


/* 80 Tap Low Pass Root Raised Cosine                                         */
/* Finite Impulse Response                                                    */
/* Sample Frequency = 800.0 Hz                                                */
/* Standard Form                                                              */
/* Arithmetic Precision = 7 Digits                                            */
/*                                                                            */
/* Pass Band Frequency = 10.00 Hz                                             */
/* Alpha = 1.0000                                                             */
static float const znumF800L10R1[80] = {
        -2.157603e-03,-2.223231e-03,-2.235263e-03,-2.186788e-03,-2.071255e-03,-1.8826e-03,-1.615354e-03,-1.264759e-03,-8.268721e-04,-2.986579e-04,
        3.219224e-04,1.035839e-03,1.842935e-03,2.741877e-03,3.73013e-03,4.803945e-03,5.958361e-03,7.187231e-03,8.48326e-03,9.838064e-03,
        1.124225e-02,1.268549e-02,1.415667e-02,1.564394e-02,1.713493e-02,1.861682e-02,2.007654e-02,2.150091e-02,2.287679e-02,2.419129e-02,
        2.543187e-02,2.658654e-02,2.764402e-02,2.859386e-02,2.942658e-02,3.013384e-02,3.070848e-02,3.114467e-02,3.143796e-02,3.158536e-02,
        3.158536e-02,3.143796e-02,3.114467e-02,3.070848e-02,3.013384e-02,2.942658e-02,2.859386e-02,2.764402e-02,2.658654e-02,2.543187e-02,
        2.419129e-02,2.287679e-02,2.150091e-02,2.007654e-02,1.861682e-02,1.713493e-02,1.564394e-02,1.415667e-02,1.268549e-02,1.124225e-02,
        9.838064e-03,8.48326e-03,7.187231e-03,5.958361e-03,4.803945e-03,3.73013e-03,2.741877e-03,1.842935e-03,1.035839e-03,3.219224e-04,
        -2.986579e-04,-8.268721e-04,-1.264759e-03,-1.615354e-03,-1.8826e-03,-2.071255e-03,-2.186788e-03,-2.235263e-03,-2.223231e-03,-2.157603e-03
    };
static float FirBuffMainAlex[80] = {0};
static sFirFilter FirMainAlex;
float ProcessMainAlexWeight(float in)
{
	static unsigned short nDivIndex = 0;
	static unsigned short nDivMax = 6;
	static float fDivSum = 0.0f;
	static float fResult = 0.0f;

	fDivSum += in;
	nDivIndex++;
	if(nDivIndex < nDivMax) return fResult;

	fResult = FirFilterStatic(&FirMainAlex, fDivSum / nDivIndex);
	fDivSum = 0.0f;
	nDivIndex = 0;

	//��̬�������һ��˥�����ã�����ָ�
	fResult /= 0.955f;

	return fResult;
}

/*
 * ����:    ��̬�ᴦ���ʼ��
 */
char  Mode8AlexDyncProcInit(void* pDecb)
{
	if (pDecb == 0) return (char)-1;

	SMODE8ALEXDYNC(pDecb).fAlexMaxWet = 0;
	SMODE8ALEXDYNC(pDecb).fAlexWet = 0;
	SMODE8ALEXDYNC(pDecb).fAlexUpWet = 0;
	SMODE8ALEXDYNC(pDecb).fAlexDownWet 	= 0;
	SMODE8ALEXDYNC(pDecb).nAlexDealOver	= 0;
		
	SMODE8ALEXDYNC(pDecb).nAlexValid = 0;
	SMODE8ALEXDYNC(pDecb).nPulseWidth = 0;
	SMODE8ALEXDYNC(pDecb).nUpFlag = 0;
	SMODE8ALEXDYNC(pDecb).nUpCnt = 0;
	SMODE8ALEXDYNC(pDecb).nDownkeepCnt = 0;
	SMODE8ALEXDYNC(pDecb).nDownCnt = 0;

	if(SMODE8ALEXDYNC(pDecb).mSpeedLen == 0)
		SMODE8ALEXDYNC(pDecb).mSpeedLen = 80;
	
	SMODE8ALEXDYNC(pDecb).iPowerOnIndex = 0;  
	SMODE8ALEXDYNC(pDecb).nAlexNo = 0; 
	SMODE8ALEXDYNC(pDecb).nAlexINorRemove = 0; 
	SMODE8ALEXDYNC(pDecb).nAlexWet = 0; 
	SMODE8ALEXDYNC(pDecb).nAlexPulseWith = 0; 
	SMODE8ALEXDYNC(pDecb).nAlexMaxWet = 0; 

	SMODE8ALEXDYNC(pDecb).nAlexConvIndex = 0;

	FirMainAlex.nLen = 80;
	FirMainAlex.pDelay = FirBuffMainAlex;
	FirMainAlex.pZNum = znumF800L10R1;
	FirMainAlex.bInit = 0;

    return 0;
}

#define SPEED_POINT_COUNT		10
static const float arrSpeedPoint[SPEED_POINT_COUNT] = {0.0f, 2.5f, 5.0f, 7.5f, 10.0f, 12.5f, 15.0f, 20.0f, 25.0f, 30.0f};
static float arrK[SPEED_POINT_COUNT];
extern gWetParam gWetPar;
float AlexSpeedCorrect(float fWet, float fSpeed)
{
	int i;
	float k, wet = 0.0f;

	arrK[0] = (float)gWetPar.Follow_0k / 1000.0f;
	arrK[1] = (float)gWetPar.Follow_2P5k / 1000.0f;
	arrK[2] = (float)gWetPar.Follow_5k / 1000.0f;
	arrK[3] = (float)gWetPar.Follow_7P5k / 1000.0f;
	arrK[4] = (float)gWetPar.Follow_10k / 1000.0f;
	arrK[5] = (float)gWetPar.Follow_12P5k / 1000.0f;
	arrK[6] = (float)gWetPar.Follow_15k / 1000.0f;
	arrK[7] = (float)gWetPar.Follow_20k / 1000.0f;
	arrK[8] = (float)gWetPar.Follow_25k / 1000.0f;
	arrK[9] = (float)gWetPar.Follow_30k / 1000.0f;

	do
	{
		//��������ٶ�
		if(fSpeed <= arrSpeedPoint[0])
		{
			wet = fWet * arrK[0];
			break;
		}

		//��������ٶ�
		if(fSpeed >= arrSpeedPoint[SPEED_POINT_COUNT-1])
		{
			wet = fWet * arrK[9];
			break;
		}

		//Ѱ����������ڵĶ�
		for(i = 1; i < SPEED_POINT_COUNT; i++)
		{
			if(fSpeed < arrSpeedPoint[i])
			{
				k = arrK[i-1] +  (arrK[i] - arrK[i-1]) * (fSpeed-arrSpeedPoint[i-1]) / (arrSpeedPoint[i]-arrSpeedPoint[i-1]);
				wet = fWet * k;
				break;
			}
		}
	}while(0);

	//�ٶ�����ϵ���쳣����
	if((wet < 10.0f) || (wet > 5.0f*fWet)) return fWet;

	return wet;
}

//��������Ϣ
static void MsgPostAlexEvent(void *pDecb, int event, int alexwet, int alexmax, int mAxleDValue)
{
	sMode8AlexDyncProc *pDync = (sMode8AlexDyncProc *)pDecb;
	int tmptick = 0, tick = pDync->nPulseWidth;
	
	pDync->nAlexINorRemove 		= event;
	pDync->nAlexWet				= alexwet;
	pDync->nAlexPulseWith		= pDync->nPulseWidth;
	pDync->nAlexMaxWet			= alexmax;
	pDync->nAlexDValue 			= mAxleDValue;
	pDync->nAlexNo				= 2;
	
	if(tick != 0)
	{
		//�����������������ʱ��
		if(((alexmax - pDync->fWetUp) > 0) && ((alexmax - pDync->fWetDown) > 0) )
		{
			//����ǰ��ʱ��
			tmptick = (int)((pDync->nHalfPulseWidth + 2) * pDync->fWetUp / (alexmax - pDync->fWetUp));
			//�ͷź��ʱ��
			tmptick += (int)((pDync->nPulseWidth - pDync->nHalfPulseWidth + 2) * pDync->fWetDown / (alexmax - pDync->fWetDown));
			tmptick += pDync->nPulseWidth;
		}
		
		if(tmptick > 0) tick = tmptick;

		pDync->fSpeed = 2.0f * (float)(pDync->mSpeedLen * 0.8f * 3.6f) / tick;
		pDync->nAlexWet = (int)AlexSpeedCorrect((float)alexwet, pDync->fSpeed);
	}

	pDync->pADP(pDecb);
}

//Ԥ�������źţ��������ź�
static void AlexParserSignal(void *pDecb,int BfRaiseKg, int AftFallKg, int nAlexMax, int BfScaleRaiseKg,int AftScaleFallKg, int mScalerDValue)
{
	int nAlexUpDownGate;
	nAlexUpDownGate = (int)((float)nAlexMax * 0.1f);
	if(nAlexUpDownGate < ALEXUPDOWN) nAlexUpDownGate = ALEXUPDOWN;

	//����ǰ��ֵ�仯�����ڳ�����ı仯�� ��ʾ����
	if(AftFallKg  >= BfRaiseKg  + nAlexUpDownGate)
	{
		MsgPostAlexEvent(pDecb,1, AftScaleFallKg - BfScaleRaiseKg + BfScaleRaiseKg * SMODE8ALEXDYNC(pDecb).fDyncK, nAlexMax, mScalerDValue);
		SMODE8ALEXDYNC(pDecb).nAlexConvIndex = 0;	//�������������
	}
	else if(AftFallKg <=  BfRaiseKg - nAlexUpDownGate)
	{
		MsgPostAlexEvent(pDecb,0, AftScaleFallKg - BfScaleRaiseKg + BfScaleRaiseKg * SMODE8ALEXDYNC(pDecb).fDyncK, nAlexMax, mScalerDValue);
	}
	else if(SMODE8ALEXDYNC(pDecb).nPulseWidth < 200)
	{
		MsgPostAlexEvent(pDecb,1, AftScaleFallKg - BfScaleRaiseKg + BfScaleRaiseKg * SMODE8ALEXDYNC(pDecb).fDyncK, nAlexMax, mScalerDValue);
		SMODE8ALEXDYNC(pDecb).nAlexConvIndex = 0;	//�������������
	}
}

//��������Ϣ
static void MsgPostLeaveEvent(void *pDecb, int event, int alexwet, int alexvalidtime, int alexmax)
{
	sMode8AlexDyncProc *pDync = (sMode8AlexDyncProc *)pDecb;
	pDync->nAlexINorRemove 		= event;
	pDync->nAlexWet				= alexwet;
	pDync->nAlexPulseWith		= alexvalidtime;
	pDync->nAlexMaxWet			= alexmax;
	pDync->nAlexNo				= 3;
	
	if(alexvalidtime != 0)
	{
		pDync->fSpeed = 2.0f * (float)(pDync->mSpeedLen * 0.8f * 3.6f) / alexvalidtime;
	}

	pDync->pADP(pDecb);
}


//Ԥ�����³��ᣬ�����³����ź�
static void LeaveParserSignal(void *pDecb,int BfRaiseKg, int AftFallKg, int nAlexMax, int BfScaleRaiseKg,int AftScaleFallKg, char nPreAlexEvent)
{
	int nAlexUpDownGate;
	nAlexUpDownGate = (int)((float)nAlexMax * 0.1f);
	if(nAlexUpDownGate < ALEXUPDOWN) nAlexUpDownGate = 100;

	if(AftFallKg  >= BfRaiseKg  + nAlexUpDownGate)
	{
		if(nPreAlexEvent == 1)
		{
			MsgPostLeaveEvent(pDecb, 1, AftScaleFallKg - BfScaleRaiseKg + BfScaleRaiseKg * 0.15, SMODE8ALEXDYNC(pDecb).nLeavePulseWidth, (float)nAlexMax);
			debug(Debug_Business, "**����ǿ������Ϊ����\r\n");
		}
		else
		{
			MsgPostLeaveEvent(pDecb, 0, AftScaleFallKg - BfScaleRaiseKg + BfScaleRaiseKg * 0.15, SMODE8ALEXDYNC(pDecb).nLeavePulseWidth, (float)nAlexMax);
		}	
	}
	else if(AftFallKg <=  BfRaiseKg - nAlexUpDownGate)
	{
		MsgPostLeaveEvent(pDecb, 1, AftScaleFallKg - BfScaleRaiseKg + BfScaleRaiseKg * 0.15, SMODE8ALEXDYNC(pDecb).nLeavePulseWidth, (float)nAlexMax);
	}
	else if(SMODE8ALEXDYNC(pDecb).nLeavePulseWidth < 200)
	{
		MsgPostLeaveEvent(pDecb, 1, AftScaleFallKg - BfScaleRaiseKg + BfScaleRaiseKg * 0.15, SMODE8ALEXDYNC(pDecb).nLeavePulseWidth, (float)nAlexMax);
	}
}


//===================================================================
//��̥ʶ����
/* �������� */
static void Judge_Axle_Business(sMode8AlexDyncProc *pDync)
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
			debug(Debug_Business, "̥ʶ��=%s\r\n", Get_Car_TryeType(pDync->bDouble));
		}
		else
		{
			pDync->bDouble = 2;
		}

		//���������̥����
		TyreQueue_Init();
	}
}


int g_bAlexHold = 0;
int IsAlexHold(void)
{
	return g_bAlexHold;
}

static float mMainAlexSteadyWet;

float GetSmallWet(void)
{
	return mMainAlexSteadyWet;

}

//============================================
//��̨���ز����
static int nAlexFallAfter200msKg	 = 0;		//�ᴥ��ʱ�ĳ�̨����
static int nAlexFallAfterAvgKg	 = 0;		//����Ч�ͷź�200ms�ĳ�̨������
static int nAlexFallAfter200msKgTmp = 0;
static int nAlexDValue  = 0;
static BOOL bDvalueFlag = FALSE;

//��ʱʱ�� 160*1.25ms = 200ms
#define D_VALUE_TICK	160

static void Calc_DValue(int tick)
{
	if(tick <= 0) return;

	nAlexDValue = nAlexFallAfterAvgKg/tick - nAlexFallAfter200msKgTmp;
	nAlexFallAfterAvgKg = 0;
	nAlexFallAfter200msKgTmp = 0;
}

/*
 * ����:    ��̬�ᴦ��
 */
float Mode8AlexDyncProc(void* pDecb, float * pInArr, int nInCount)
{
	//��̬����
	static int fAlexUpWetTmp = 0;
	static float mMainAlexSteadyMin;
	static float mMainAlexSteadyMax;
	static float mMainAlexSteadyMinTmp;
	static float mMainAlexSteadyMaxTmp;

	static int nLeaveFallSumKg = 0;
	static int nLeaveFallIndex = 0;
	static int fLeaveUpWetTmp = 0;
	static char bPreAlexEvent = 0;

	//��ʱ����
	TaskMsg msg = {0};
	sMode8AlexDyncProc *pDync = (sMode8AlexDyncProc *)pDecb;
	float mScaleWet = pInArr[0];
	float mLeaveWet = pInArr[1];
	float mMainAlexWet = pInArr[2];
	float mRegAlexWet = pInArr[3];
	
	
	//������������
	if(pDecb == 0) return mScaleWet;
	if(pDync->iPowerOnIndex++ < pDync->iPowerOnDelay) return 0;

	//���������˲�����
	mMainAlexSteadyWet = ProcessMainAlexWeight(mMainAlexWet);

	//����������������б����ϳӶ��м����ź�10�����³Ӷ˲��÷�������
	if(SMODE8ALEXDYNC(pDecb).nAlexConvIndex < 100000)
	{
		SMODE8ALEXDYNC(pDecb).nAlexConvIndex++;	//�ۼ�
	}

	//================================================================================================
	//================================================================================================
	// ǰ��ʶ����
	if(mRegAlexWet > pDync->fWetUp)
	{
		pDync->nUpCnt++;
		pDync->nDownCnt = 0;

		if(pDync->nUpCnt > 2) //������2�α������趨��ADֵ���ϣ���ʾ����ϵ�����
		{			
			if(pDync->nUpFlag == 0)
			{
				if(pDync->nAlexValid == 1)
				{
					fAlexUpWetTmp = mMainAlexWet;				//����һ���ᵽ��ǰ�ỹûȷ��ʱ�������ᴥ������
					mMainAlexSteadyMinTmp = mMainAlexSteadyWet;
					mMainAlexSteadyMaxTmp = mMainAlexSteadyWet;
				}
				else
				{
					pDync->fAlexUpWet = mMainAlexWet;			//��¼����ᴥ������
					mMainAlexSteadyMin = mMainAlexSteadyWet;
					mMainAlexSteadyMax = mMainAlexSteadyWet;

					pDync->fAlexMaxWet = mRegAlexWet;	//����һ���������pDync->nAlexValidΪ1�ǣ���Ȼ���������ֵ
				}

				nAlexFallAfter200msKg = (int)mScaleWet;    //��¼�ᴥ��ʱ�ĳ�̨����

				//��¼�Ѿ��Ϸ�ת
				pDync->nUpFlag = 1;		
				SMODE8ALEXDYNC(pDecb).nAlexConvIndex = 0;

				//�����ᴥ����Ϣ
				msg.msg_id = Msg_PreAlex_Come;
				Task_QPost(&BusinessMsgQ, &msg);
			}	
		}		
	}
	else if(mRegAlexWet < pDync->fWetDown)
	{
		pDync->nDownCnt++;
		pDync->nUpCnt= 0;
		if (pDync->nDownCnt >= 2)
		{
			if(pDync->nUpFlag == 1)						//�ڵ����趨ֵ�Ժ�������Ϲ�һ�εĶ�������ʾ���ˡ�
			{
				pDync->fAlexDownWet = mMainAlexWet;		//��¼��С��������
				pDync->nAlexValid = 1;					//��¼����Ч
				pDync->nDownkeepCnt = 0;

				pDync->bDouble = 1;
				if(Get_Tire_ErrStatus())
				{
					Judge_Axle_Business(pDync);
				}
			}				
			
			pDync->nUpFlag = 0;			
		}
	}

	//������Ч��ʱ����м�������¼�����ʱ��
	if(pDync->nUpFlag == 1)
	{
		if(pDync->nPulseWidth < 100000000)		//��ֹ���
		{
			pDync->nPulseWidth++;
		}		
	}

	//��¼��ʶ�������ֵ
	if(pDync->nUpFlag == 1)
	{
		if(mRegAlexWet > pDync->fAlexMaxWet)
		{
			pDync->fAlexMaxWet = mRegAlexWet;					//��¼��ʶ�������ֵ
			pDync->nHalfPulseWidth = pDync->nPulseWidth;
		}
	}

	//������Ч��ʱ�򣬴Ӵ�����ط�ֵ��ʼ���������ص�ƽ��ֵ
	if((pDync->nUpFlag == 1) || (pDync->nAlexValid == 1))
	{
		if(mMainAlexSteadyWet < mMainAlexSteadyMin)
		{
			mMainAlexSteadyMin = mMainAlexSteadyWet;
		}

		if(mMainAlexSteadyWet > mMainAlexSteadyMax)
		{
			mMainAlexSteadyMax = mMainAlexSteadyWet;
		}
	}
	
	//��ʶ������Ч�ź�
	if((pDync->nUpFlag == 1) || (pDync->nAlexValid == 1))
	{
		g_bAlexHold = 1;
	}
	else
	{
		g_bAlexHold = 0;
	}

	//���źŷ�����ɺ���ʱһ��ʱ��ȡ����
	if(pDync->nAlexValid == 1)
	{
		//���津��ʱ��̨����
		if(pDync->nDownkeepCnt == 0)
		{
			nAlexFallAfter200msKgTmp = nAlexFallAfter200msKg;
			nAlexFallAfter200msKg = 0;
		}
		
		//��¼���ͷź�200ms�ڵĳ�̨������
		if(pDync->nDownkeepCnt < D_VALUE_TICK)
		{
			nAlexFallAfterAvgKg += (int)mScaleWet;
		}
		//200ms��ʱ��,�����̨������ֵ
		if(pDync->nDownkeepCnt == D_VALUE_TICK)
		{
			Calc_DValue(D_VALUE_TICK);
			bDvalueFlag = TRUE;
		}
		
		if((pDync->nDownkeepCnt++ >= 80*5) || (pDync->nUpFlag == 1))		//10�����Ժ��˲�ȡ�Ӵ����½��ؿ�200ms�� ������
		{
			//��ʱδ��200ms,�Ե�ǰ��ʱ����
			if(!bDvalueFlag) Calc_DValue(pDync->nDownkeepCnt);

			AlexParserSignal(pDecb, pDync->fAlexUpWet, pDync->fAlexDownWet,  pDync->fAlexMaxWet, 
				mMainAlexSteadyMin, mMainAlexSteadyMax, nAlexDValue);
			
			bDvalueFlag  = FALSE;

			pDync->fAlexMaxWet = 0;
			pDync->fAlexWet = 0;
			pDync->fAlexUpWet = 0;
			pDync->fAlexDownWet = 0;
			pDync->nAlexDealOver = 0;
				
			pDync->nPulseWidth = 0;
			pDync->nAlexValid = 0;
			pDync->nDownkeepCnt = 0;

			if(pDync->nUpFlag == 1)
			{
				pDync->fAlexUpWet = fAlexUpWetTmp;
				mMainAlexSteadyMin = mMainAlexSteadyMinTmp;
				mMainAlexSteadyMax = mMainAlexSteadyMaxTmp;
			}
		}
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
				if(pDync->nLeaveValid == 1)
				{
					fLeaveUpWetTmp = mScaleWet;
				}
				else
				{
					pDync->fLeaveUpWet = mScaleWet;
					pDync->fLeaveMaxWet = mLeaveWet;
				}
				
				pDync->nLeaveUpFlag = 1;				//��¼�Ѿ��Ϸ�ת
				bPreAlexEvent = 0;

				//�����³��ᴥ����Ϣ
				msg.msg_id = Msg_BackAlex_Come;
				Task_QPost(&BusinessMsgQ, &msg);
			}	
		}		
	}
	else if(mLeaveWet < pDync->fLeaveWetDown)
	{
		pDync->nLeaveDownCnt++;
		pDync->nLeaveUpCnt= 0;
		if (pDync->nLeaveDownCnt >= 2)
		{
			if(pDync->nLeaveUpFlag == 1) //�ڵ����趨ֵ�Ժ�������Ϲ�һ�εĶ�������ʾ���ˡ�
			{
				pDync->fLeaveDownWet = mScaleWet;
				pDync->nLeaveValid = 1;				//��¼����Ч
				pDync->nLeaveDownkeepCnt = 0;
			}
			
			pDync->nLeaveUpFlag = 0;			
		}
	}
	
	if(pDync->nLeaveUpFlag == 1)
	{
		//��¼�³���ʶ���������ֵ
		if(mLeaveWet > pDync->fLeaveMaxWet)
		{
			pDync->fLeaveMaxWet = mLeaveWet;
		}

		//��¼��������
		if(pDync->nLeavePulseWidth < 100000000)		//��ֹ���
		{
			pDync->nLeavePulseWidth++;
		}

		//�ж��ϳӶ���û�г�ͬʱ�ϳ�
		//if(pDync->nUpFlag == 1)
		//{
		//	bPreAlexEvent = 1;
		//}

		//���ʮ�������ڣ��ϳӶ˽��������Ϊ�³Ӷ˽���
		if(SMODE8ALEXDYNC(pDecb).nAlexConvIndex < 8000)
		{
			bPreAlexEvent = 1;
		}
	}

	//������Ϣ
	if(pDync->nLeaveValid ) //����������һ����ʱ�ж�
	{
		nLeaveFallSumKg += (int)mScaleWet;
		nLeaveFallIndex++;

		if((pDync->nLeaveDownkeepCnt++ >= 80*2) || (pDync->nLeaveUpFlag == 1))		//10�����Ժ��˲�ȡ�Ӵ����½��ؿ�200ms�� ������
		{	
			LeaveParserSignal(pDecb, pDync->fLeaveUpWet, pDync->fLeaveDownWet, pDync->fLeaveMaxWet, pDync->fLeaveUpWet, nLeaveFallSumKg / nLeaveFallIndex, bPreAlexEvent);

			pDync->nLeaveValid = 0;
			pDync->nLeaveDownkeepCnt = 0;
			pDync->fLeaveMaxWet = 0;
			pDync->nLeavePulseWidth = 0;

			nLeaveFallSumKg = 0;
			nLeaveFallIndex = 0;
			bPreAlexEvent = 0;

			if(pDync->nLeaveUpFlag == 1)
			{
				pDync->fLeaveUpWet = fLeaveUpWetTmp;
			}
		}
	}

	return mScaleWet;
}

