#include "Mode8AlexParser.h"
#include <math.h>  
#include <stdlib.h>

#include "axletype_business.h"
#include "device_info.h"
#include "FirFilter.h"
#include "axle_business.h"
#include "scszc_debug_info.h"
#include "Weight_Param.h"

#define ALEXUPDOWN	150

#define ALEX_DELAY_NUM		24
static float BuffAlexDelay[ALEX_DELAY_NUM] = {0};
void PushAlexWeight(float in)
{
	int i;
	for(i = 0; i < ALEX_DELAY_NUM - 1; i++)
	{
		BuffAlexDelay[i+1] = BuffAlexDelay[i];
	}

	//���µ�һ������ڶ���
	BuffAlexDelay[0] = in;
}

float GetAlexBuffSum(int nCount)
{
	float sum;
	int i;

	if(nCount > ALEX_DELAY_NUM) nCount = ALEX_DELAY_NUM;

	sum = 0;
	for(i = 0; i < nCount; i++)
	{
		sum += BuffAlexDelay[i];
	}

	return sum;
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
#if 1
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
#else
	arrK[0] = 1.0f;
	arrK[1] = 1.0f;
	arrK[2] = 1.0f;
	arrK[3] = 1.0f;
	arrK[4] = 1.0f;
	arrK[5] = 1.0f;
	arrK[6] = 1.0f;
	arrK[7] = 1.0f;
	arrK[8] = 1.0f;
	arrK[9] = 1.0f;
#endif

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
	if((wet < 0.00001f) || (wet > 5.0f*fWet)) return fWet;

	return wet;
}

//��������Ϣ
static void MsgPostAlexEvent(void *pDecb, int event, int alexwet, int alexvalidtime, int alexmax)
{
	sMode8AlexDyncProc *pDync = (sMode8AlexDyncProc *)pDecb;
	pDync->nAlexINorRemove 		= event;
	pDync->nAlexWet				= alexwet;
	pDync->nAlexPulseWith		= alexvalidtime;
	pDync->nAlexMaxWet			= alexmax;
	pDync->nAlexNo				= 2;
	
	if(alexvalidtime != 0)
	{
		pDync->fSpeed = (float)(pDync->mSpeedLen * 0.8f * 3.6f) / alexvalidtime;

		//�����ٶ�����
		pDync->nAlexWet = (int)AlexSpeedCorrect((float)alexwet, pDync->fSpeed);
	}

	pDync->pADP(pDecb);
}

//Ԥ�������źţ��������ź�
static void AlexParserSignal(void *pDecb, int nDerect, int nAlexWet, int nAlexMax)
{
	int nAlexUpDownGate;
	nAlexUpDownGate = (int)((float)nAlexMax * 0.1f);
	if(nAlexUpDownGate < ALEXUPDOWN) nAlexUpDownGate = ALEXUPDOWN;

	//����ǰ��ֵ�仯�����ڳ�����ı仯�� ��ʾ����
	if(nDerect  > 0)
	{
		MsgPostAlexEvent(pDecb,1, nAlexWet, SMODE8ALEXDYNC(pDecb).nPulseWidth, (float)nAlexMax);
		SMODE8ALEXDYNC(pDecb).nAlexConvIndex = 0;	//�������������
	}
	else if(nDerect < 0)
	{
		MsgPostAlexEvent(pDecb,0, nAlexWet, SMODE8ALEXDYNC(pDecb).nPulseWidth, (float)nAlexMax);
	}

	//���淴�����ܶ��ᣬ����ǳ���ѹ����ʶ������ԵѸ������
	//else if((nDerect == 0) && (SMODE8ALEXDYNC(pDecb).nPulseWidth < 500))
	//{
	//	MsgPostAlexEvent(pDecb,1, nAlexWet, SMODE8ALEXDYNC(pDecb).nPulseWidth, (float)nAlexMax);
	//	SMODE8ALEXDYNC(pDecb).nAlexConvIndex = 0;	//�������������
	//}
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

//============================
//���ضԱ�
static int mAlexIndex = 0;

static float mRightTopWet = 0.0f;
static float mRightHalfAlexSum = 0.0f;
static int mRightHalfCnt = 0;

static float mLeftTopWet = 0.0f;
static float mLeftHalfAlexSum = 0.0f;
static int mLeftHalfCnt = 0;

static float Calc_Ratio_AlexWet(void)
{
	float mAlexLeftAvg = 0.0f;
	float mAlexRightAvg = 0.0f;
	float val = 0.0f;
	int nExceptCount = 0;

	nExceptCount = (int)((float)mAlexIndex * 0.3f);
	if(nExceptCount > ALEX_DELAY_NUM) nExceptCount = ALEX_DELAY_NUM;
	
	//�����ұ�����
	if(mRightHalfCnt > nExceptCount)
	{
		mAlexRightAvg = (mRightHalfAlexSum - GetAlexBuffSum(nExceptCount)) / (mRightHalfCnt - nExceptCount);
	}
	else
	{
		mAlexRightAvg = mRightTopWet;
	}
	//�����������
	if(mLeftHalfCnt > 0)
	{
		mAlexLeftAvg = mLeftHalfAlexSum / mLeftHalfCnt;
	}
	else
	{
		mAlexLeftAvg = mLeftTopWet;
	}
	debug(Debug_Business, "$�������:%d,�ұ�����:%d\r\n", (int)mAlexLeftAvg, (int)mAlexRightAvg);

	//����������
	if((mAlexRightAvg<0.000001f) || (mAlexLeftAvg<0.000001f)) return mRightTopWet;

	//�����������ر���
	if(mAlexLeftAvg > mAlexRightAvg)
	{
		val = (mAlexLeftAvg - mAlexRightAvg) / mAlexRightAvg;
	}
	else
	{
		val = (mAlexRightAvg - mAlexLeftAvg) / mAlexLeftAvg;
	}

	//0.25����ȡƽ��ֵ
	if(val < 0.25f) return ((mAlexRightAvg + mAlexLeftAvg)/2);

	//0.25~1.5ȡ��Сֵ
	if(val < 1.5f)
	{
		return (((mAlexRightAvg - mAlexLeftAvg) > 0.000001f) ? mAlexLeftAvg : mAlexRightAvg);
	}	
	//1.5~ ��Ϊ�������ӣ�ȡ���ֵ
	return (((mAlexRightAvg - mAlexLeftAvg) > 0.000001f) ? mAlexRightAvg : mAlexLeftAvg);
}

static int Direction_Abnormal_Business(int direction, int lcnt, int rcnt)
{
	int tmp = direction;
	
	if(tmp == 0)
	{
		if((lcnt > 0) || (rcnt > 0))
		{
			if(lcnt > (rcnt+5))
			{
				tmp = 1;
			}
			else if((lcnt+5) < rcnt)
			{
				tmp = -1;
			}
		}
		debug(Debug_Business, "��ֵ�ֳ���������,����:%d LCnt=%d, RCnt=%d\r\n", tmp, lcnt, rcnt);
	}
	
	return tmp;
}
/*
 * ����:    ��̬�ᴦ��
 */
float Mode8AlexDyncProc(void* pDecb, float * pInArr, int nInCount)
{
	//��̬����
	static int bDerectionUp = 0;
	static int bDerectionDown = 0;
	static float mAlexLeftMax = 0.0f;
	static float mAlexRightMax = 0.0f;
	static int mAlexLeftValid = 0;
	static int mAlexRightValid = 0;
	static float mAlexSum = 0.0f;
	
	static float mFloatZero = 0;
	static float mAlexHalfMax = 0;
	
	//��ʱ����
	TaskMsg msg = {0};
	sMode8AlexDyncProc *pDync = (sMode8AlexDyncProc *)pDecb;
	float mScaleWet = pInArr[0] + pInArr[2] + pInArr[3];		//�������ض�ֵ̬
	float mAlexWet = pInArr[2] + pInArr[3];						//���ض�ֵ̬���������Һ�

	float mAlexLeftWet = pInArr[2];
	float mAlexRightWet = pInArr[3];
//	float mAlexAvg = 0.0f;

	static int mAlexLCnt = 0, mAlexRCnt = 0;	

	
	//������������
	if(pDecb == 0) return mScaleWet;
	if(pDync->iPowerOnIndex++ < pDync->iPowerOnDelay) return 0;

	//����������������б����ϳӶ��м����ź�10�����³Ӷ˲��÷�������
	if(SMODE8ALEXDYNC(pDecb).nAlexConvIndex < 100000)
	{
		SMODE8ALEXDYNC(pDecb).nAlexConvIndex++;	//�ۼ�
	}

	//================================================================================================
	//================================================================================================
	// ����ǰ��ʶ����
	if(mAlexWet > pDync->fWetUp)
	{
		pDync->nUpCnt++;
		pDync->nDownCnt = 0;
		if(pDync->nUpCnt > 2) //������2�α������趨��ADֵ���ϣ���ʾ����ϵ�����
		{			
			if(pDync->nUpFlag == 0)
			{
				//��¼�Ѿ��Ϸ�ת
				pDync->nUpFlag = 1;		

				//����ʶ��
				//���津������ȷ�����򣬵�Ҫ������ʱ�ϳ���ֵһ��Ҫ�����³���ֵ��Ĭ��200��150
				if(mAlexLeftWet > mAlexRightWet)
				{
					bDerectionUp = 1;
				}
				else
				{
					bDerectionUp = -1;
				}

				bDerectionDown = 0;
					
				mAlexLeftMax = 0.0f;
				mAlexRightMax = 0.0f;
				mAlexLeftValid = 0;
				mAlexRightValid = 0;
				mAlexSum = 0.0f;
				mAlexIndex = 0;

				mRightHalfAlexSum = 0.0f;
				mRightHalfCnt = 0;
				mLeftHalfAlexSum = 0.0f;
				mLeftHalfCnt = 0;
				mRightTopWet = 0.0f;
				mLeftTopWet  = 0.0f;

				pDync->fAlexMaxWet = 0;
				mAlexHalfMax = 0;
				mFloatZero = 0;
				pDync->nPulseWidth = 0;
				SMODE8ALEXDYNC(pDecb).nAlexConvIndex = 0;

				//�����ᴥ����Ϣ
				msg.msg_id = Msg_PreAlex_Come;
				Task_QPost(&BusinessMsgQ, &msg);
			}	
		}		
	}

	//�Ƿ���ʶ����
	else if(mAlexWet < pDync->fWetDown)
	{
		pDync->nDownCnt++;
		pDync->nUpCnt= 0;
		if (pDync->nDownCnt >= 2)
		{
			if(pDync->nUpFlag == 1)						//�ڵ����趨ֵ�Ժ�������Ϲ�һ�εĶ�������ʾ���ˡ�
			{
				pDync->nAlexValid = 1;					//��¼����Ч
				
				//����ʶ��
				//�³��п��ܴ��ڸ����ֵ���ٽ�ʶ�𣬲��ܵ����ıȴ�С
				if(bDerectionDown == 0)
				{
					if((mAlexLeftWet + pDync->fWetDown / 2) < mAlexRightWet)
					{
						bDerectionDown = 1;	//��ǰ
					}
					else if((mAlexRightWet + pDync->fWetDown / 2) < mAlexLeftWet)
					{
						bDerectionDown = -1;//���
					}
					else
					{
						//�ٽ�����������ʱ������
						bDerectionDown = bDerectionUp;
					}
				}

				//̥��ʶ��
				pDync->bDouble = 1;				
				if(Get_Tire_ErrStatus())
				{
					Judge_Axle_Business(pDync);
				}
			}				
			
			pDync->nUpFlag = 0;			
		}
	}

	//������Ч��ʱ��������ش���
	if(pDync->nUpFlag == 1)
	{
		//����������������������ƣ������ۼ�����
		if((mAlexLeftWet > mAlexLeftMax) && (mAlexRightWet > mAlexRightMax))
		{
			//��������
			mAlexSum = 0;
			mAlexIndex = 0;
			mAlexLeftValid = 0;
			mAlexRightValid = 0;

			mRightHalfAlexSum = 0.0f;
			mRightHalfCnt = 0;
			mLeftHalfAlexSum = 0.0f;
			mLeftHalfCnt = 0;
			mRightTopWet = 0.0f;
			mLeftTopWet  = 0.0f;

			//�������Ϊ�����жϷ���
			//�ں��ֿ�������������ֵ�ֳ�ʱ����ʱ����ȷ�������ڴ�ʱȷ��
			if((bDerectionUp == 0) || (mAlexWet <= 1000))
			{
				if(mAlexLeftWet > mAlexRightWet)
				{
					bDerectionUp = 1;
				}
				else
				{
					bDerectionUp = -1;
				}
			}
		}

		//ʶ�����ֵ
		if(mAlexLeftWet > mAlexLeftMax)
		{
			mAlexLeftMax = mAlexLeftWet;
			mLeftTopWet = mAlexWet;
		}
		else if((mAlexLeftWet + 80) < mAlexLeftMax)
		{
			mAlexLeftValid = 1;
		}

		//ʶ���ҷ�ֵ
		if(mAlexRightWet > mAlexRightMax)
		{
			mAlexRightMax = mAlexRightWet;
			mRightTopWet = mAlexWet;			//��¼�ҷ�ֵ����
		}
		else if((mAlexRightWet + 80) < mAlexRightMax)
		{
			mAlexRightValid = 1;
		}

		//�������ۼ�����
		if(mAlexLeftValid != mAlexRightValid)
		{
			mAlexSum += mAlexWet;
			mAlexIndex++;

			if(mAlexRightWet >= mAlexLeftWet)
			{
				mRightHalfAlexSum += mAlexWet;		//�ұ����غ�
				mRightHalfCnt++;
			}
			else
			{
				mLeftHalfAlexSum += mAlexWet;		//������غ�
				mLeftHalfCnt++;
			}

			//������󼸸���
			PushAlexWeight(mAlexWet);
		}

		//��¼���ص����ֵ
		if(mAlexWet > pDync->fAlexMaxWet)
		{
			pDync->fAlexMaxWet = mAlexWet;					//��¼��ʶ�������ֵ
			mFloatZero = pDync->fAlexMaxWet;				//Ŀ���ǽ�mFloatZero��ʼ��Ϊ���ֵ
		}

		//��������ʼ�½�ʱ����¼���ֵ��һ�룬��Ϊ����ֶεĲο�
		if((mAlexWet + 80) < pDync->fAlexMaxWet)
		{
			mAlexHalfMax = pDync->fAlexMaxWet / 3;			
		}

		//�ڴﵽ��ֵ֮��Ѱ�ҹ�ֵ����Ϊ��ֵ���ܳ�����ֵ��һ��
		if((mAlexWet < mAlexHalfMax) && (mAlexWet < mFloatZero))
		{
			//���¹�ֵ��
			mFloatZero = mAlexWet;

			//��δ�ﵽ��ֵʱһֱ�����³ӷ���
			//�³��п��ܴ��ڸ����ֵ���ٽ�ʶ�𣬲��ܵ����ıȴ�С
			if(mAlexWet >= pDync->fAlexMaxWet / 10)
			{
				if((mAlexLeftWet + pDync->fWetDown / 2) < mAlexRightWet)
				{
					bDerectionDown = 1;	//��ǰ
					mAlexLCnt++;
				}
				else if((mAlexRightWet + pDync->fWetDown / 2) < mAlexLeftWet)
				{
					bDerectionDown = -1;//���
					mAlexRCnt++;
				}
				else
				{
					//�ٽ�����������ʱ������
					bDerectionDown = bDerectionUp;
				}
			}
		}

		//��ʱ���Ͻ��������Ѿ��ﵽ��ֵ����ʼ�����������жϺ�һ�������
		if(mAlexWet > (mFloatZero + 100))
		{
			//����һ�����ź�--------------------------------------------------
			{
				if(mAlexIndex <= 0)
				{
					AlexParserSignal(pDecb, Direction_Abnormal_Business(bDerectionUp+bDerectionDown, mAlexLCnt, mAlexRCnt), pDync->fAlexMaxWet,  pDync->fAlexMaxWet);
				}
				else
				{
					//mAlexAvg = Calc_Ratio_AlexWet();
					AlexParserSignal(pDecb, Direction_Abnormal_Business(bDerectionUp+bDerectionDown, mAlexLCnt, mAlexRCnt), Calc_Ratio_AlexWet(),  pDync->fAlexMaxWet);
				}

				pDync->fAlexMaxWet = 0;
				pDync->nAlexValid = 0;
				mAlexLCnt = 0;
				mAlexRCnt = 0;
			}
			//-----------------------------------------------------------------

			//��ʼ����һ�����ź�-----------------------------------------------
			//����ʶ�𣬹��˹�ֵ�㣬����ֱ�ӱȴ�С
			if(mAlexLeftWet > mAlexRightWet)
			{
				bDerectionUp = 1;
			}
			else
			{
				bDerectionUp = -1;
			}

			bDerectionDown = 0;
					
			mAlexLeftMax = 0.0f;
			mAlexRightMax = 0.0f;
			mAlexLeftValid = 0;
			mAlexRightValid = 0;
			mAlexSum = 0.0f;
			mAlexIndex = 0;

			mRightHalfAlexSum = 0.0f;
			mRightHalfCnt = 0;
			mLeftHalfAlexSum = 0.0f;
			mLeftHalfCnt = 0;
			mRightTopWet = 0.0f;
			mLeftTopWet  = 0.0f;

			pDync->fAlexMaxWet = 0;
			mAlexHalfMax = 0;
			mFloatZero = 0;
			pDync->nPulseWidth = 0;

			//�����ᴥ����Ϣ
			msg.msg_id = Msg_PreAlex_Come;
			Task_QPost(&BusinessMsgQ, &msg);
		}		



		//��¼����ʱ�����ٶ�
		if(pDync->nPulseWidth < 100000000)		//��ֹ���
		{
			pDync->nPulseWidth++;
		}	
	}


	//���źŷ�����ɺ���ʱһ��ʱ��ȡ����
	if(pDync->nAlexValid == 1)
	{
		if(mAlexIndex <= 0)
		{
			AlexParserSignal(pDecb, Direction_Abnormal_Business(bDerectionUp+bDerectionDown, mAlexLCnt, mAlexRCnt), pDync->fAlexMaxWet,  pDync->fAlexMaxWet);
		}
		else
		{
			//mAlexAvg = Calc_Ratio_AlexWet();
			AlexParserSignal(pDecb, Direction_Abnormal_Business(bDerectionUp+bDerectionDown, mAlexLCnt, mAlexRCnt), Calc_Ratio_AlexWet(), pDync->fAlexMaxWet);
		}

		pDync->fAlexMaxWet = 0;
		pDync->nAlexValid = 0;
		mAlexLCnt = 0;
		mAlexRCnt = 0;
	}


	g_bAlexHold = pDync->nUpFlag;
	
	return mScaleWet;

	//================================================================================================
	//================================================================================================
	// ����ʶ����
	//if(mLeaveWet > pDync->fLeaveWetUp)
	//{
	//	pDync->nLeaveUpCnt++;
	//	pDync->nLeaveDownCnt = 0;

	//	if(pDync->nLeaveUpCnt > 2) //������2�α������趨��ADֵ���ϣ���ʾ����ϵ�����
	//	{			
	//		if(pDync->nLeaveUpFlag == 0)
	//		{
	//			if(pDync->nLeaveValid == 1)
	//			{
	//				fLeaveUpWetTmp = mScaleWet;
	//			}
	//			else
	//			{
	//				pDync->fLeaveUpWet = mScaleWet;
	//				pDync->fLeaveMaxWet = mLeaveWet;
	//			}
	//			
	//			pDync->nLeaveUpFlag = 1;				//��¼�Ѿ��Ϸ�ת
	//			bPreAlexEvent = 0;

	//			//�����³��ᴥ����Ϣ
	//			msg.msg_id = Msg_BackAlex_Come;
	//			Task_QPost(&BusinessMsgQ, &msg);
	//		}	
	//	}		
	//}
	//else if(mLeaveWet < pDync->fLeaveWetDown)
	//{
	//	pDync->nLeaveDownCnt++;
	//	pDync->nLeaveUpCnt= 0;
	//	if (pDync->nLeaveDownCnt >= 2)
	//	{
	//		if(pDync->nLeaveUpFlag == 1) //�ڵ����趨ֵ�Ժ�������Ϲ�һ�εĶ�������ʾ���ˡ�
	//		{
	//			pDync->fLeaveDownWet = mScaleWet;
	//			pDync->nLeaveValid = 1;				//��¼����Ч
	//			pDync->nLeaveDownkeepCnt = 0;
	//		}
	//		
	//		pDync->nLeaveUpFlag = 0;			
	//	}
	//}
	//
	//if(pDync->nLeaveUpFlag == 1)
	//{
	//	//��¼�³���ʶ���������ֵ
	//	if(mLeaveWet > pDync->fLeaveMaxWet)
	//	{
	//		pDync->fLeaveMaxWet = mLeaveWet;
	//	}

	//	//��¼��������
	//	if(pDync->nLeavePulseWidth < 100000000)		//��ֹ���
	//	{
	//		pDync->nLeavePulseWidth++;
	//	}

	//	//�ж��ϳӶ���û�г�ͬʱ�ϳ�
	//	//if(pDync->nUpFlag == 1)
	//	//{
	//	//	bPreAlexEvent = 1;
	//	//}

	//	//���ʮ�������ڣ��ϳӶ˽��������Ϊ�³Ӷ˽���
	//	if(SMODE8ALEXDYNC(pDecb).nAlexConvIndex < 8000)
	//	{
	//		bPreAlexEvent = 1;
	//	}
	//}

	////������Ϣ
	//if(pDync->nLeaveValid ) //����������һ����ʱ�ж�
	//{
	//	nLeaveFallSumKg += (int)mScaleWet;
	//	nLeaveFallIndex++;

	//	if((pDync->nLeaveDownkeepCnt++ >= 80*2) || (pDync->nLeaveUpFlag == 1))		//10�����Ժ��˲�ȡ�Ӵ����½��ؿ�200ms�� ������
	//	{	
	//		LeaveParserSignal(pDecb, pDync->fLeaveUpWet, pDync->fLeaveDownWet, pDync->fLeaveMaxWet, pDync->fLeaveUpWet, nLeaveFallSumKg / nLeaveFallIndex, bPreAlexEvent);

	//		pDync->nLeaveValid = 0;
	//		pDync->nLeaveDownkeepCnt = 0;
	//		pDync->fLeaveMaxWet = 0;
	//		pDync->nLeavePulseWidth = 0;

	//		nLeaveFallSumKg = 0;
	//		nLeaveFallIndex = 0;
	//		bPreAlexEvent = 0;

	//		if(pDync->nLeaveUpFlag == 1)
	//		{
	//			pDync->fLeaveUpWet = fLeaveUpWetTmp;
	//		}
	//	}
	//}

	//return mScaleWet;
}

