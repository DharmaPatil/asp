#include "DyncProc.h"
#include <string.h>
#include <math.h>


#define  DIRSAFETIME    100    //  ��ȫʱ����ڷ����ж�

/*
 * ����:    ��̬����
 */
char  DyncProcInit(void* pDecb)
{
	if (pDecb == NULL) return (char)-1;

    SDYNC(pDecb).ucAxelValid = 0;
    SDYNC(pDecb).uiWaitCnt = 0;
    SDYNC(pDecb).ucUpDir = 0;
    SDYNC(pDecb).ucDownDir = 0;
    SDYNC(pDecb).uiRunCnt = 0;
    SDYNC(pDecb).fWet = 0;
    SDYNC(pDecb).fWetReliab = 0;
    
    SDYNC(pDecb).iPowerOnIndex = 0;
    SDYNC(pDecb).iTimetick = 0;
    SDYNC(pDecb).iScaleTime_On = 0;      //  �ϳ�ʱ��
    SDYNC(pDecb).iScaleTime_Down = 0;    //  �³�ʱ��
    SDYNC(pDecb).cScaleTimeLock_On = 0;  //  �ϳ�ʱ��������־
    SDYNC(pDecb).cScaleTimeLock_Down = 0;//  �³�ʱ��������־
    SDYNC(pDecb).fMaxWet_AD1 = 0;        //  ��һ·�������
    SDYNC(pDecb).fMaxWet_AD2 = 0;        //  �ڶ�·�������
    SDYNC(pDecb).iMaxWetTime_AD1 = 0;    //  ��һ·�������ʱ��
    SDYNC(pDecb).iMaxWetTime_AD2 = 0;    //  �ڶ�·�������ʱ��
    SDYNC(pDecb).fWet_AD1Top = 0;        //  ��һ·��ֵʱ����
    SDYNC(pDecb).fWet_AD2Top = 0;        //  �ڶ�·��ֵʱ����
    memset((char *)SDYNC(pDecb).fAD1Buf,0,sizeof(SDYNC(pDecb).fAD1Buf));
    memset((char *)SDYNC(pDecb).fAD2Buf,0,sizeof(SDYNC(pDecb).fAD2Buf));
    return 0;
}


/*
 * ����:    ����AD����
 */
 void  SaveADData(void* pDecb,float * pInArr)
{
    sDyncProc *pDync = (sDyncProc *)pDecb;
    int i;

    i=sizeof(pDync->fAD1Buf)/sizeof(float);
    for(;i>1;i--) {
        pDync->fAD1Buf[i-1] = pDync->fAD1Buf[i-2];
    }
    
    i=sizeof(pDync->fAD2Buf)/sizeof(float);
    for(;i>1;i--) {
        pDync->fAD2Buf[i-1] = pDync->fAD2Buf[i-2];
    }
    pDync->fAD1Buf[0] = pInArr[0];
    pDync->fAD2Buf[0] = pInArr[1];
}

/*
 * ����:    �ж��ϳƷ���
 */
char  Dir_ScaleUp(void* pDecb)
{
    const char cmpnum = 20;
    sDyncProc *pDync = (sDyncProc *)pDecb;
    int i;
    char num=0;
    signed char dir=0;

    num = sizeof(pDync->fAD1Buf)/sizeof(float);
    if(sizeof(pDync->fAD2Buf)/sizeof(float) < num) {
        num= sizeof(pDync->fAD2Buf);
    }
    if(num<cmpnum) {
        return dir;
    }
    if(num>cmpnum) {
        num=cmpnum;
    }
    for(i=0;i<num;i++) {
        if(pDync->fAD1Buf[i] && pDync->fAD2Buf[i]) {
            if(pDync->fAD1Buf[i] > 1.4f*pDync->fAD2Buf[i]) {
                dir++;
            } else if(pDync->fAD2Buf[i] > 1.4f*pDync->fAD1Buf[i]){
                dir--;
            }
        }
    }
    if(dir>(cmpnum-2)) {
        dir = 1;
    } else if(dir < -(cmpnum-2)) {
        dir = 2;
    } else {
        dir = 0;
    }
    
    return dir;
}

/*
 * ����:    �ж��³Ʒ���
 */
char  Dir_ScaleDown(void* pDecb)
{
    const char cmpnum = 10;
    sDyncProc *pDync = (sDyncProc *)pDecb;
    int i;
    char num=0;
    signed char dir=0;

    num = sizeof(pDync->fAD1Buf)/sizeof(float);
    if(sizeof(pDync->fAD2Buf)/sizeof(float) < num) {
        num= sizeof(pDync->fAD2Buf);
    }
    if(num<cmpnum) {
        return dir;
    }
    if(num>cmpnum) {
        num=cmpnum;
    }
    for(i=0;i<num;i++) {
        if(pDync->fAD1Buf[i] && pDync->fAD2Buf[i]) {
            if(pDync->fAD1Buf[i] > 1.3f*pDync->fAD2Buf[i]) {
                dir++;
            } else if(pDync->fAD2Buf[i] > 1.3f*pDync->fAD1Buf[i]){
                dir--;
            }
        }
    }
    if(dir>(cmpnum-2)) {
        dir = 1;
    } else if(dir < -(cmpnum-2)) {
        dir = 2;
    } else {
        dir = 0;
    }
    
    return dir;
}


/*
 * ����:    ���Ӵ�С˳������������
 */
 void  InsertWetData(void* pDecb,float * pInArr)
{
    int count = 0;
    int MinTime=0;
    int i;
    int buflen;
    float wet = pInArr[0] + pInArr[2];
    double dWetAll=0;

    /*
     *  �жϴ��ڳ�̨���ĵ�ʱ���
     */
    if(pInArr[0] > SDYNC(pDecb).fMaxWet_AD1) {
        SDYNC(pDecb).fMaxWet_AD1     = pInArr[0];
        SDYNC(pDecb).iMaxWetTime_AD1 = SDYNC(pDecb).iTimetick;
        SDYNC(pDecb).fWet_AD1Top     = wet;
    }
    
    if(pInArr[1] > SDYNC(pDecb).fMaxWet_AD2) {
        SDYNC(pDecb).fMaxWet_AD2     = pInArr[2];
        SDYNC(pDecb).iMaxWetTime_AD2 = SDYNC(pDecb).iTimetick;
        SDYNC(pDecb).fWet_AD2Top     = wet;
    }
    if(SDYNC(pDecb).iMaxWetTime_AD2 < SDYNC(pDecb).iMaxWetTime_AD1) {
        MinTime = SDYNC(pDecb).iMaxWetTime_AD2;
    } else {
        MinTime = SDYNC(pDecb).iMaxWetTime_AD1;
    }
    count = SDYNC(pDecb).iTimetick - MinTime;
    if(count >= (sizeof(SDYNC(pDecb).fWetBuf)/sizeof(float))) {
        count = sizeof(SDYNC(pDecb).fWetBuf)/sizeof(float)-1;
    }
    
    buflen = sizeof(SDYNC(pDecb).fSteadyWetBuf)/sizeof(float);
    if(0==count) {
        SDYNC(pDecb).iSteadyWetIndex = 0;
        SDYNC(pDecb).fSteadyWetBuf[0]=wet;
    } else if(fabs(wet - SDYNC(pDecb).fSteadyWetBuf[0]) < 2) {
        i = SDYNC(pDecb).iSteadyWetIndex;
        if(i > (buflen - 2)) {
            i = (buflen - 2);
        }
        for(;i>=0;i--) {
            SDYNC(pDecb).fSteadyWetBuf[i+1] = SDYNC(pDecb).fSteadyWetBuf[i];
        }
        SDYNC(pDecb).fSteadyWetBuf[0] = wet;
        SDYNC(pDecb).iSteadyWetIndex++;
    } else {
        if (SDYNC(pDecb).iSteadyWetIndex > buflen){
            for(i=0;i< (buflen-200);i++) {
                dWetAll += SDYNC(pDecb).fSteadyWetBuf[i];
            }
            dWetAll /= (buflen-200);
            if(dWetAll > SDYNC(pDecb).fSteadyWet) {
                SDYNC(pDecb).fSteadyWet = dWetAll;
                SDYNC(pDecb).iSteadyCount = SDYNC(pDecb).iSteadyWetIndex;
            }
        }
        SDYNC(pDecb).iSteadyWetIndex = 0;
        SDYNC(pDecb).fSteadyWetBuf[0]=wet;
    }
        
    SDYNC(pDecb).fWetBuf[count] = 0;
    for(i=0;i<=count;i++) {
        if(wet > SDYNC(pDecb).fWetBuf[i]) {
            for(;count > i; count--) {
                SDYNC(pDecb).fWetBuf[count] = SDYNC(pDecb).fWetBuf[count-1];
            }
            SDYNC(pDecb).fWetBuf[i] = wet;
            break;
        }
    }
}

/*
 * ����:    ��������
 */
 void  CalcWet(void* pDecb)
{
    int count = 0;
    int buflen = 0;
    int i;
    int numcnt = 0;
    double dWetAll=0;

    count = SDYNC(pDecb).iMaxWetTime_AD2 - SDYNC(pDecb).iMaxWetTime_AD1;
    if(count < 0) {
        count = -count;
    }
    buflen = sizeof(SDYNC(pDecb).fSteadyWetBuf)/sizeof(float);
    if ((SDYNC(pDecb).iSteadyCount < buflen)||(count < SDYNC(pDecb).iSteadyCount)){
        buflen = sizeof(SDYNC(pDecb).fWetBuf)/sizeof(float)-1;
        if(count > buflen) {
            count = buflen;
        }
        if(count >= 10) {
            for(i=count*3/10;i<count*7/10;i++) {
                dWetAll += SDYNC(pDecb).fWetBuf[i];
                numcnt++;
            }
            dWetAll /= (double)numcnt;
        } else {
            for(i=0;i<count;i++) {
                dWetAll += SDYNC(pDecb).fWetBuf[i];
                numcnt++;
            }
            dWetAll /= (double)numcnt;
        }
        SDYNC(pDecb).fWetReliab = SDYNC(pDecb).fWet_AD1Top/(SDYNC(pDecb).fWet_AD1Top + SDYNC(pDecb).fWet_AD2Top);
    } else {
        dWetAll  = SDYNC(pDecb).fSteadyWet;
        SDYNC(pDecb).fWetReliab = 0.5;
    }

    SDYNC(pDecb).fWet = (float)dWetAll;
}

/*
 * ����:    ��̬�����ʼ��
 */
float DyncProc(void* pDecb, float * pInArr, int nInCount)
{
    sDyncProc *pDync = (sDyncProc *)pDecb;
    float ScaleWet = 0;
	//ֱ��������̬����

	return pInArr[0];


	if(pDecb == 0) return 0;
    if(pDync->iPowerOnIndex < pDync->iPowerOnDelay) {
        pDync->iPowerOnIndex++;
        return 0;
    }
    if(pDync->fWetUp < 50) return 0;
    if(pDync->fWetDown < 40) return 0;
    if(pDync->fWetDown > pDync->fWetUp) return 0;
    
    pDync->iTimetick++;
    if(pDync->iTimetick> 3000000) {
        pDync->iTimetick -= 1000000;
        // û�г�ʱ���㣬�г�ʱʱ�䶼Ҫ����
    }
    
    ScaleWet = pInArr[0]+pInArr[1];
    SaveADData(pDecb,pInArr);
    

    /*
     *  ���ش���
     */
    if(0==pDync->ucAxelValid) {

        if(ScaleWet > pDync->fWetUp) {
            if(0==pDync->cScaleTimeLock_On) {
                pDync->iScaleTime_On = pDync->iTimetick;
            }
            if(pDync->cScaleTimeLock_On++>18) {
                pDync->ucUpDir = Dir_ScaleUp(pDecb);
                pDync->uiWaitCnt   = pDync->iScaleTime_On;
                pDync->ucAxelValid = 1;
                
                pDync->iScaleTime_Down = 0;    //  �³�ʱ��
                pDync->cScaleTimeLock_Down = 0;//  �³�ʱ��������־
            }
            
            InsertWetData(pDecb,pInArr);
        } else {
            pDync->cScaleTimeLock_On=0;
        }
    } else {
        InsertWetData(pDecb,pInArr);
        if(ScaleWet < pDync->fWetDown) {
            if(0==pDync->cScaleTimeLock_Down) {
                pDync->cScaleTimeLock_Down=1;
                pDync->iScaleTime_Down = pDync->iTimetick;
            }
            if(ScaleWet < 0.8f*pDync->fWetDown) {
                pDync->ucDownDir = Dir_ScaleDown(pDecb);
                CalcWet(pDecb);
                pDync->uiRunCnt       = pDync->iScaleTime_Down - pDync->iScaleTime_On;
                pDync->ucAxelValid    = 0;
                /*
                 *  ״̬��ʼ��
                 */
                pDync->iTimetick = 0;
                pDync->iScaleTime_On = 0;      //  �ϳ�ʱ��
                pDync->iScaleTime_Down = 0;    //  �³�ʱ��
                pDync->cScaleTimeLock_On = 0;  //  �ϳ�ʱ��������־
                pDync->cScaleTimeLock_Down = 0;//  �³�ʱ��������־
                pDync->fMaxWet_AD1 = 0;        //  ��һ·�������
                pDync->fMaxWet_AD2 = 0;        //  �ڶ�·�������
                pDync->iMaxWetTime_AD1 = 0;    //  ��һ·�������ʱ��
                pDync->iMaxWetTime_AD2 = 0;    //  �ڶ�·�������ʱ��
                pDync->fWet_AD1Top = 0;        //  ��һ·��ֵʱ����
                pDync->fWet_AD2Top = 0;        //  �ڶ�·��ֵʱ����
                pDync->fSteadyWet = 0;
                pDync->iSteadyCount = 0;
            }
        } else {
            pDync->cScaleTimeLock_Down=0;
        }
    }
        
    return ScaleWet;
}




