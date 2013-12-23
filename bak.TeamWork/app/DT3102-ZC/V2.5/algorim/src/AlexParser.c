#include "AlexParser.h"
#include <math.h>  
#include <stdlib.h>

//2S�����ݻ���
#define MAX_DELAY  2				//��ʹ�ã���Լ�ڴ棬�Ķ̵�2����ǰ��10		//���峤��Ϊ������
static int nBigDelayArr[MAX_DELAY];			//
static int nBigDelayIndex;					//

static int				nPulseWidth = 0;	//��ʶ�����˲�
static int				nAlexStopKg = 0;	//��ͣ����ʶ������һ���ȡ������

//��̬����50Hz��5���ֵ�˲���ʼ��
static int m_200msAvgWet = 0;
static int filter_cnt = 0;
static int filter_buf[5];
static int filter_valid_cnt = 0;

static int nAlexRaiseBefore200msKg = 0;
static int nAlexRaiseBeforeTmp = 0;
static int nAlexFallAfter200msKg	 = 0;
static int nAlexFallAfterAvgKg	 = 0;
static int nAlexFallAfterAvgIndex = 0;
static int   nAlexMaxKg = 0; 

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


static void AlexParser_ResetFilter(void)
{
	filter_cnt = 0;
	filter_valid_cnt = 0;
}

//50Hz��5���ֵ�˲������쳣��ȥ�����ܣ����뷶Χ��0--120000֮�䣩
static void AlexParser_UpdateAvgWet(int wet)
{
	int i = 0;
	int  sum  = 0;

	if((wet >= -10000) && (wet <= 120000))
	{
		if(filter_cnt >= 5) filter_cnt = 0;
	 
		filter_buf[filter_cnt++] = wet;
		
		if(filter_valid_cnt < 5) filter_valid_cnt++;
		
		for(i = 0; i < filter_valid_cnt; i++)
		{
			sum += filter_buf[i];
		}
		m_200msAvgWet = sum / filter_valid_cnt;		
	}
}

//��ȡ���̨50Hz��5���ֵ�˲������쳣��ȥ�����ܣ����뷶Χ��0--120000֮�䣩
static int AlexParser_Get200MsAvgWet(void)
{
	return m_200msAvgWet;
}
	
static void MsgPostAlexAdd(void *pDecb,int event,int alexwet,int alexvalidtime,int alexmax)
{
	sAlexDyncProc *pDync = (sAlexDyncProc *)pDecb;
	pDync->nAlexINorRemove 		= event;
	pDync->nAlexWet				= alexwet;
	pDync->nAlexPulseWith			= alexvalidtime;
	pDync->nAlexMaxWet			= alexmax;
	pDync->nAlexNo				= 2;
	if(alexvalidtime != 0)
		pDync->fSpeed				= (float)(pDync->mSpeedLen * 0.8f * 3.6f) / alexvalidtime;
	
	pDync->pADP(pDecb);
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
	
	if(cnt_200ms++ >= 16)
	{
		//��50Hz������������		
		//�����ؽ���5���˲�
		AlexParser_UpdateAvgWet(bigScalerKg);
		//�ӳٶ���
		TotalDelayQueue(bigScalerKg);
		cnt_200ms = 0;
	}
}

 /*
 * ����:    ��̬�ᴦ���ʼ��
 */
char  AlexDyncProcInit(void* pDecb)
{
	if (pDecb == 0) return (char)-1;

	SALEXDYNC(pDecb).nAlexValid = 0;
	SALEXDYNC(pDecb).nPulseWidth = 0;
	SALEXDYNC(pDecb).nUpFlag = 0;
	SALEXDYNC(pDecb).nUpCnt = 0;
	SALEXDYNC(pDecb).nUPkeepCnt = 0;
	SALEXDYNC(pDecb).nDownkeepCnt = 0;
	SALEXDYNC(pDecb).nDownCnt = 0;
	//SALEXDYNC(pDecb).fWetUp = 0;
	//SALEXDYNC(pDecb).fWetDown = 0;
	if(SALEXDYNC(pDecb).mSpeedLen == 0)
		SALEXDYNC(pDecb).mSpeedLen = 80;
	
	SALEXDYNC(pDecb).fWet = 0;      
	SALEXDYNC(pDecb).fWetReliab = 0;     
	SALEXDYNC(pDecb).iPowerOnIndex = 0;  
	//SALEXDYNC(pDecb).iPowerOnDelay = 0; 

	SALEXDYNC(pDecb).nAlexNo = 0; 
	SALEXDYNC(pDecb).nAlexINorRemove = 0; 
	SALEXDYNC(pDecb).nAlexWet = 0; 
	SALEXDYNC(pDecb).nAlexPulseWith = 0; 
	SALEXDYNC(pDecb).nAlexMaxWet = 0; 
	
	//memset((char *)SALEXDYNC(pDecb).fAD1Buf,0,sizeof(SALEXDYNC(pDecb).fAD1Buf));
	//memset((char *)SALEXDYNC(pDecb).fAD2Buf,0,sizeof(SALEXDYNC(pDecb).fAD2Buf));

	AlexParser_ResetFilter();
	
    return 0;
}
 
/*
 * ����:    ��̬�ᴦ��
 */
float AlexDyncProc(void* pDecb, float * pInArr, int nInCount)
{
	sAlexDyncProc *pDync = (sAlexDyncProc *)pDecb;
	float mScaleWet = pInArr[0];
	float mAlexWet 	= pInArr[2];

	//������������
	if(pDecb == 0) return mScaleWet;
	if(pDync->iPowerOnIndex < pDync->iPowerOnDelay) {
	    pDync->iPowerOnIndex++;
	    return 0;
	}
	if(pDync->fWetUp < 20) return mScaleWet;
	if(pDync->fWetDown < 20) return mScaleWet;

	//��50HzƵ�ʶ�����5���˲�
	AlexParser_UpdateWet(mScaleWet);
		
	// ��ʶ����
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
					nAlexRaiseBeforeTmp = AlexParser_Get200MsAvgWet();
				}
				else
				{
					nAlexRaiseBefore200msKg = AlexParser_Get200MsAvgWet();
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
				nAlexStopKg = AlexParser_Get200MsAvgWet();
			}
		}
	}

	//ʶ������Ч
	if(pDync->nAlexValid ) //����������һ����ʱ�ж�
	{
		//��������ľ�ֵ
		nAlexFallAfter200msKg = AlexParser_Get200MsAvgWet();
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
		nAlexMaxKg = mAlexWet;
	}
	
	return mScaleWet;
	
}

