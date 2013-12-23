#ifndef SENSOR_ALEX_PARSER_H
#define SENSOR_ALEX_PARSER_H

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif

typedef int (*AlexEventProc)(void* pADP);

#define	MAX_ALEX_NUM	10
typedef struct __TAlexArr 
{
	int num; 	//����
	float fSpeed; //�ٶ�
	int width[MAX_ALEX_NUM];  //ƽ̨�ĳ���
	int avg[MAX_ALEX_NUM];  //ƽ̨�����ڵ�ƽ��ֵ
	
}TAlexArr;

//3280
typedef struct {

	int		mFixAlexMode ; //����ģʽѡ��;
	int  		m_LimitK;		//ɾ�����ʱ�򣬲��õ�0.05��������ϵ��3
	int		m_rate;			//ÿ��ƽ̨�Ĳ���ֵ��0.5��ϵ��  2
	int		m_SumLen;		//��ʾ�����ٶȵ������ӵ�0.1��   8

	float  fWetUp;
	float  fWetDown;

	int 	 m_CmpSpeddPT;
	int    iPowerOnIndex;      //  ������ʱ
	int    iPowerOnDelay;

	///������
	int	nAlexNum;			//��ʾ����
	int   nAlexINorRemove;	// 1 ---�����0 --����
	int	nAlexWet;		//����
	int	nAlexPulseWith; //����Чʱ��
	int	nAlexMaxWet;   //���źŴ����ڼ䣬�ɼ����������ֵ��
	float CarSpeed ;	//�ٶ�
	
	AlexEventProc pDecb;
	  
}sSoftAlexDyncProc;

//ȡ������
#define SSOFTALEXDYNC(x)	(*((sSoftAlexDyncProc*)x))


/*
��ȡ���źŴ�����
*/
void   SoftAlexDyncProcResult(TAlexArr* alexarr);

/*
��Ҫ�ڹ�Ļ��β����á�
�������źŴ���
*/
void  SoftAlexDyncProcStop(void);


/*
 * ����:    ��̬����
 */
char  SoftAlexDyncProcInit(void* pDecb);

/*
������ķ��򣬽��ỹ�ǵ���
axisAd : �ᴫ����AD
bigScalerKg: ���ʵʱ����
*/
float SoftAlexDyncProc(void* pDecb, float * pInArr, int nInCount);


#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
}
#endif

#endif

