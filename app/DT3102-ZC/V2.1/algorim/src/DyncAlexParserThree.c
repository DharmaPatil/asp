#include "DyncAlexParserThree.h"
#include <math.h>  
#include <stdlib.h>

//2S�����ݻ���
#define MAX_DELAY_ALEX  10						//���峤��Ϊ������
static int nBigDelayArr[MAX_DELAY_ALEX];			//
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
static int TempCarDir = 0;

typedef struct{
	float  Bf_AD;
	int    Up_Counter;
	int    Find_HighPt_Flag; //find high pt flag
	int    Up_Flag;   //up flag
	int    Down_Flag; //down flag
	int    Ret;
	
}SENSOR_STATUS,*PTR_SENSOR_STATUS;

SENSOR_STATUS	mAlexSensor1;
SENSOR_STATUS	mAlexSensor2;

static void TotalDelayQueue(int nBigKg)			//
{
	if(nBigDelayIndex >= MAX_DELAY_ALEX) nBigDelayIndex= 0;

	//ÿ200msһ������
	nBigDelayArr[nBigDelayIndex++] = nBigKg;
}

//��ȡ������ǰ������
static int GetBigWeightBefore2S()
{
	int n = nBigDelayIndex;

	if(n >= MAX_DELAY_ALEX) n= 0;

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
	
static void MsgPostAlexAdd(void *pDecb,int event,int alexno,int alexwet,int alexvalidtime,int alexmax)
{
	sAlexThreeDyncProc *pDync = (sAlexThreeDyncProc *)pDecb;
	pDync->nAlexINorRemove 		= event;
	pDync->nAlexWet				= alexwet;
	pDync->nAlexPulseWith			= alexvalidtime;
	pDync->nAlexMaxWet			= alexmax;
	pDync->nAlexNo				= alexno;
	if(alexvalidtime != 0)
		pDync->fSpeed				= (float)(pDync->mSpeedLen * 0.8f * 3.6f) / alexvalidtime;
	pDync->pADP(pDecb);
}

//�������ź�
static void AlexParser_SendAlexSignal(void *pDecb,int BfRaiseKg, int AftFallKg, int AftFallAvg, int nAlexMax)
{
	if(TempCarDir == 1)
	{
		MsgPostAlexAdd(pDecb,1,2, AftFallAvg - BfRaiseKg, nPulseWidth, nAlexMax);
	}
	else if(TempCarDir == 2)
	{
		MsgPostAlexAdd(pDecb,0,2, AftFallAvg - BfRaiseKg, nPulseWidth, nAlexMax);
	}
	//MsgPostAlexAdd(pDecb,1,2, nAlexStopKg - BfRaiseKg, nPulseWidth, nAlexMax);
	//����ǰ��ֵ
	//������峬������ͣ�����ϣ����ж�������ǰ��ֵ
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
//////////////////////////////////////////////

 /*
 * ����:    ��̬�ᴦ���ʼ��
 */
char  AlexDyncProcThreeInit(void* pDecb)
{
	if (pDecb == 0) return (char)-1;

	SALEXTHREEDYNC(pDecb).nAlexValid = 0;
	SALEXTHREEDYNC(pDecb).nPulseWidth = 0;
	SALEXTHREEDYNC(pDecb).nUpFlag = 0;
	SALEXTHREEDYNC(pDecb).nUpCnt = 0;
	SALEXTHREEDYNC(pDecb).nUPkeepCnt = 0;
	SALEXTHREEDYNC(pDecb).nDownkeepCnt = 0;
	SALEXTHREEDYNC(pDecb).nDownCnt = 0;
	//SALEXTHREEDYNC(pDecb).fWetUp = 0;
	//SALEXTHREEDYNC(pDecb).fWetDown = 0;
	if(SALEXTHREEDYNC(pDecb).mSpeedLen == 0)
		SALEXTHREEDYNC(pDecb).mSpeedLen = 80;
	SALEXTHREEDYNC(pDecb).fWet = 0;      
	SALEXTHREEDYNC(pDecb).fWetReliab = 0;     
	SALEXTHREEDYNC(pDecb).iPowerOnIndex = 0;  
	//SALEXTHREEDYNC(pDecb).iPowerOnDelay = 0; 

	SALEXTHREEDYNC(pDecb).nAlexNo = 0; 
	SALEXTHREEDYNC(pDecb).nAlexINorRemove = 0; 
	SALEXTHREEDYNC(pDecb).nAlexWet = 0; 
	SALEXTHREEDYNC(pDecb).nAlexPulseWith = 0; 
	SALEXTHREEDYNC(pDecb).nAlexMaxWet = 0; 
	
	AlexParser_ResetFilter();
	
    return 0;
}

static  void AlexDealDownScaleAlex(void* pDecb,float mAlexWet) 
{
	sAlexThreeDyncProc *pDync = (sAlexThreeDyncProc *)pDecb;
	
	if(mAlexWet > pDync->fDownAlexWetUp)
	{
		pDync->nDownAlexUpCnt++;
		pDync->nDownAlexDownCnt = 0;

		if(pDync->nDownAlexUpCnt > 2) //������2�α������趨��ADֵ���ϣ���ʾ����ϵ�����
		{
			
			if(pDync->nDownAlexUpFlag == 0)
			{
				if(pDync->nDownAlexAlexValid == 1)
				{
					nAlexRaiseBeforeTmp = AlexParser_Get200MsAvgWet();
				}
				else
				{
					nAlexRaiseBefore200msKg = AlexParser_Get200MsAvgWet();
				}
				pDync->nDownAlexUpFlag = 1;				//��¼�Ѿ��Ϸ�ת				
				//�����ᴥ����Ϣ
				//MsgPost(msgInAlexCome);
			}
			
			
		}		
	}
	else if( mAlexWet < pDync->fDownAlexWetDown )
	{
		pDync->nDownAlexDownCnt++;
		if (pDync->nDownAlexDownCnt >= 2)
		{
			if(pDync->nDownAlexUpFlag  == 1) //�ڵ����趨ֵ�Ժ�������Ϲ�һ�εĶ�������ʾ���ˡ�
			{
				pDync->nDownAlexAlexValid = 1;				//��¼����Ч
				MsgPostAlexAdd(pDecb,1,3, pDync->nDownAlexMaxWet, pDync->nDownAlexPulseWidth, pDync->nDownAlexMaxWet);
			}				
			pDync->nDownAlexUpCnt= 0;
			pDync->nDownAlexDownCnt = 0;
			pDync->nDownAlexUpFlag = 0;
			pDync->nDownAlexDownkeepCnt = 0;
			pDync->nDownAlexPulseWidth = 0;
			pDync->nDownAlexMaxWet = 0;
		}
	}
	if(pDync->nDownAlexUpFlag )
	{
		if(pDync->nDownAlexPulseWidth < 100000000)		//��ֹ���
			pDync->nDownAlexPulseWidth++;
		
		if(mAlexWet > pDync->nDownAlexMaxWet)
			pDync->nDownAlexMaxWet = mAlexWet;
	}
	
}
static int   Axis_Analysis_Channel(float inAD,int wet_up,PTR_SENSOR_STATUS status)
{
	status->Down_Flag = 0;
	
	if ( inAD > wet_up ) 
	{	
		//�ϳ�̨��
		if (  inAD > status->Bf_AD ) 
		{
			status->Bf_AD = inAD;
			status->Up_Counter++;
			if ( status->Up_Counter > 5 )
            		{
				status->Up_Flag = 1;
				status->Ret     = 0;
				return 0;//�ϳ�̨(��·������ADֵ�����ϳ�̨��ֵ��,����5���㶼���ֵ���̬������Ϊ�ϳ�̨��).
			}
		} 
		else if ( ( status->Bf_AD  - inAD ) >= 5 ) 
		{
			//���ϳ�̨�����ٷ�����10���źŵ���������������ź�ǿ�ȴ�������ĳ����ֵ
			if ( ( status->Up_Counter >  10)  &&  ( ( status->Bf_AD - wet_up ) > 30 ) ) 
			{
				status->Up_Counter       	= 0;
				status->Find_HighPt_Flag 	= 1;
				status->Ret     		 	= 1;	
				return 1;///��·�ҵ���ߵ�.
			}
		}

	} 
	else 
	{
		status->Bf_AD                = 0;
		if ( status->Find_HighPt_Flag ) 
		{
			status->Down_Flag        = 1;
			status->Ret     		 = 2;			
			return 2;//һ·�ҵ�����ߵ�,���Ѿ��³�̨��
		}
	}
	
	status->Ret     		         = 3;
	
	return 3;//��״̬��������.
}

/**
 * @Function Name: f_Lsw_Axis_Reset_AxisInfo
 * @Description:
 *               �����󣬸�λ�������Ϣ
 * @author coff (2008-8-27)
 */
static void  f_Lsw_Axis_Reset_AxisInfo(void)
{
	mAlexSensor1.Bf_AD               = 0;
	mAlexSensor2.Bf_AD               = 0;
	mAlexSensor1.Up_Counter          = 0;
	mAlexSensor2.Up_Counter          = 0;
	mAlexSensor1.Up_Flag 			= 0;
	mAlexSensor2.Up_Flag 			= 0;
	mAlexSensor1.Down_Flag 			= 0;
	mAlexSensor2.Down_Flag 			= 0;
	mAlexSensor1.Find_HighPt_Flag 	= 0;
	mAlexSensor2.Find_HighPt_Flag 	= 0;
}

static void AlexDyncFindHigh(float wet1,float wet2, int wetup )
{	

	Axis_Analysis_Channel(wet1,wetup,&mAlexSensor1);
	Axis_Analysis_Channel(wet2,wetup,&mAlexSensor2);

	if((mAlexSensor1.Ret == 0) || (mAlexSensor2.Ret == 0))
	{
		
	}
	//�������ҵ�����ߵ�====================================================================================
	if(mAlexSensor1.Ret == 1) 
	{ //����һ��˲ֵ̬,��ʾ�ҵ�һ����ߵ㣬����һ������Ч����ߵ㣬�������������жϷ���
		if(!mAlexSensor2.Find_HighPt_Flag) 
		{
			TempCarDir = 1;
		}
	}
	if(mAlexSensor2.Ret == 1) 
	{
		if(!mAlexSensor1.Find_HighPt_Flag)
		{ 
			TempCarDir = 2;			
		}
	}

	if( mAlexSensor1.Ret == 2 ) 
	{//������1��⵽һ��˲̬�����³�̨(�����ҵ�����ߵ�)
		
       	 if ( mAlexSensor2.Up_Flag == 0 ) //��������2��û���ϳ�̨���������ж�Ϊ
		{
			//f_Lsw_Axis_Reset_AxisInfo();  
		}
	}

	if( mAlexSensor2.Ret == 2 ) //������2��⵽һ��˲̬�����³�̨(�����ҵ�����ߵ�)
	{

       	 if ( mAlexSensor1.Up_Flag == 0 ) //��������1��û���ϳ�̨���������ж�Ϊ
		{
			//f_Lsw_Axis_Reset_AxisInfo();
		}
	}
	//��·����������⵽�³�̨��======================================================================
	if(mAlexSensor1.Down_Flag && mAlexSensor2.Down_Flag) 
	{
		f_Lsw_Axis_Reset_AxisInfo();
	}
	
}
/*
 * ����:    ��̬�ᴦ��
 */
float AlexDyncThreeProc(void* pDecb, float * pInArr, int nInCount)
{
	sAlexThreeDyncProc *pDync = (sAlexThreeDyncProc *)pDecb;
	float mAlexWet = 0.0;
	
	pDync->mScaleWet = pInArr[0];

	pDync->mDownScaleAlex1Wet 	= pInArr[1];
	
	pDync->mUpScaleAlex1Wet 	= pInArr[2];
	pDync->mUpScaleAlex2Wet 	= pInArr[3];

	mAlexWet = pDync->mUpScaleAlex1Wet + pDync->mUpScaleAlex2Wet;
	
	//������������
	if(pDecb == 0) return pDync->mScaleWet;
	if(pDync->iPowerOnIndex < pDync->iPowerOnDelay) {
	    pDync->iPowerOnIndex++;
	    return pDync->mScaleWet;
	}
	if(pDync->fWetUp < 20) return pDync->mScaleWet;
	if(pDync->fWetDown < 20) return pDync->mScaleWet;

	//��50HzƵ�ʶ�����5���˲�
	AlexParser_UpdateWet(pDync->mScaleWet);

	//�³���ʶ��
	AlexDealDownScaleAlex(pDecb,pDync->mDownScaleAlex1Wet);
	AlexDyncFindHigh(pDync->mUpScaleAlex1Wet,pDync->mUpScaleAlex2Wet,pDync->fWetUp);
	
	// ����ʶ����
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
	
	return pDync->mScaleWet;
	
}

