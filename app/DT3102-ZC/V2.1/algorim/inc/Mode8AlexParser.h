#ifndef MODE8_ALEX_PARSER_H
#define MODE8_ALEX_PARSER_H

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif

typedef int (*AlexEventProc)(void* pADP);

typedef struct 
{
/////////////////////////////////////	
	int		fAlexUpWet;
	int  	fAlexDownWet;
	int		fAlexMaxWet;	
	int		fAlexWet;
	int		nAlexDealOver;
/////////////////////////////////////	

/////////////////////////////////////
	
	float  fWetUp;
	float  fWetDown;
	float  fLeaveWetUp;
	float  fLeaveWetDown;
	int	   mSpeedLen;
	int    iPowerOnIndex;      //  ������ʱ
	int    iPowerOnDelay;
	
/////////////////////////////////////	
	int		nAlexValid;
	int 	nPulseWidth;
	int 	nUpFlag;			//����Ч��ʼ��־
	int 	nUpCnt;
	int		nUPkeepCnt;
	int		nDownkeepCnt;
	int		nDownCnt;
	char    bDouble;			//����̥�� 0, 1, 2
///	float	fWet;                 //  ����
//	float	fWetReliab;	        //  �����ɿ��� wet1/(wet1 + wet2)
////////////////////

	int		nLeaveValid;
	int 	nLeavePulseWidth;
	int 	nLeaveUpFlag;			//����Ч��ʼ��־
	int 	nLeaveUpCnt;
	int		nLeaveDownCnt;
	int		nLeaveUPkeepCnt;
	int		nLeaveDownkeepCnt;
	float	nLeaveMaxWet;
////////////////////	
	float mScaleWet ;
	float mUpScaleAlex1Wet ;
	float mUpScaleAlex2Wet ;
	float mDownScaleAlex1Wet ;
	
/////////////////������//////////
	int	nAlexNo;			//��ʾ���ĸ���ʶ���� 1---ǰ�����ϵ��� 2 ----�ϳƶ˵��� 3 ----�³ƶ˵���
	int nAlexINorRemove;	// 1 ---�����0 --����
	int	nAlexWet;		//����
	int	nAlexPulseWith; //����Чʱ��
	int	nAlexMaxWet;   //���źŴ����ڼ䣬�ɼ����������ֵ��
	float fSpeed;			//���ٶ�
	
	AlexEventProc pADP;	  
	  
}sMode8AlexDyncProc;


//ȡ������
#define SMODE8ALEXDYNC(x)	(*((sMode8AlexDyncProc*)x))


/*
���źŴ���ģ���ʼ��
*/
/*
 * ����:    ��̬����
 */
char  Mode8AlexDyncProcInit(void* pDecb);

/*
������ķ��򣬽��ỹ�ǵ���
axisAd : �ᴫ����AD
bigScalerKg: ���ʵʱ����
*/
float Mode8AlexDyncProc(void* pDecb, float * pInArr, int nInCount);


#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
}
#endif

#endif
