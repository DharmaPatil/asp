#ifndef __DYNCPROC_H_
#define __DYNCPROC_H_

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif

typedef struct {
    /*
     *  ����
     */
    int    iPowerOnDelay;
    float  fWetUp;
    float  fWetDown;
    
    /*
     *  ���
     */
	unsigned char ucAxelValid;  //  ��̨������  0: ��̨��       1:��̨����
	unsigned int  uiWaitCnt;    //  �ȴ�����(ʱ��)
	unsigned char ucUpDir;      //  ���ϳ�λ��    0: δ֪         1:AD1      2:AD2
	unsigned char ucDownDir;    //  ���³�λ��    0: δ֪         1:AD1      2:AD2
	unsigned int  uiRunCnt;     //  ����ʱ��(ʱ��)
	float fWet;                 //  ����
	float fWetReliab;	        //  �����ɿ��� wet1/(wet1 + wet2)
    
    /*
     *  ˽��
     */
    int     iPowerOnIndex;      //  ������ʱ
    int     iTimetick;          //  �������ڼ�ʱ
    int     iScaleTime_On;      //  �ϳ�ʱ��
    int     iScaleTime_Down;    //  �³�ʱ��
    char    cScaleTimeLock_On;  //  �ϳ�ʱ��������־
    char    cScaleTimeLock_Down;//  �³�ʱ��������־
    float   fMaxWet_AD1;        //  ��һ·�������
    float   fMaxWet_AD2;        //  �ڶ�·�������
    int     iMaxWetTime_AD1;    //  ��һ·�������ʱ��
    int     iMaxWetTime_AD2;    //  �ڶ�·�������ʱ��
    float   fWet_AD1Top;        //  ��һ·��ֵʱ����
    float   fWet_AD2Top;        //  �ڶ�·��ֵʱ����
    float   fWetBuf[800];       //  ���ػ���
    int     iSteadyWetIndex;
    float   fSteadyWetBuf[600]; //  �ȶ��ĳ������ݻ���
    int     iSteadyCount;
    float   fSteadyWet;         //  �ȶ�������
    
    float   fAD1Buf[20];        //  ��һ·AD�ϳ�ʱ��
    float   fAD2Buf[20];        //  ��һ·AD�ϳ�ʱ��
}sDyncProc;

//ȡ������
#define SDYNC(x)	(*((sDyncProc*)x))

/*
 * ����:    ��̬����
 */
char  DyncProcInit(void* pDecb);
float DyncProc(void* pDecb, float * pInArr, int nInCount);


#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
}
#endif

#endif
