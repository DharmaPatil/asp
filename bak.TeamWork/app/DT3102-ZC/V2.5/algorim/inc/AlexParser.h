#ifndef ALEX_PARSER_H
#define ALEX_PARSER_H

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif

typedef int (*AlexEventProc)(void* pADP);

typedef struct {

/////////////////////////////////////
	
	float  fWetUp;
	float  fWetDown;
	int	 mSpeedLen;
	int    iPowerOnIndex;      //  ������ʱ
	int    iPowerOnDelay;
	
/////////////////////////////////////	
	int	nAlexValid;
	int 	nPulseWidth;
	int 	nUpFlag;			//����Ч��ʼ��־
	int 	nUpCnt;
	int	nUPkeepCnt;
	int	nDownkeepCnt;
	int	nDownCnt;
	float fWet;                 //  ����
	float fWetReliab;	        //  �����ɿ��� wet1/(wet1 + wet2)
	
	float mScaleWet ;
	float mUpScaleAlex1Wet ;
	float mUpScaleAlex2Wet ;
	float mDownScaleAlex1Wet ;
	
/////////////////������//////////
	int	nAlexNo;			//��ʾ���ĸ���ʶ���� 1---ǰ�����ϵ��� 2 ----�ϳƶ˵��� 3 ----�³ƶ˵���
	int   nAlexINorRemove;	// 1 ---�����0 --����
	int	nAlexWet;		//����
	int	nAlexPulseWith; //����Чʱ��
	int	nAlexMaxWet;   //���źŴ����ڼ䣬�ɼ����������ֵ��
	float fSpeed;			//���ٶ�
	
	AlexEventProc pADP;
	  
	  
}sAlexDyncProc;


//ȡ������
#define SALEXDYNC(x)	(*((sAlexDyncProc*)x))


/*
���źŴ���ģ���ʼ��
*/
/*
 * ����:    ��̬����
 */
char  AlexDyncProcInit(void* pDecb);

/*
������ķ��򣬽��ỹ�ǵ���
axisAd : �ᴫ����AD
bigScalerKg: ���ʵʱ����
*/
float AlexDyncProc(void* pDecb, float * pInArr, int nInCount);


#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
}
#endif

#endif


