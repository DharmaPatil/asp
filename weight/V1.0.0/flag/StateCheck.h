#ifndef __STATE_CHECK_H_
#define __STATE_CHECK_H_

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif

typedef struct _sStateCheck
{
    int         nTimeIndex;
    int         nTimeRange;
    int         nPreState;
	float 	    fPrev;
	float		fSteadyRange;					//�ȶ��жϷ�Χ
	float		fZeroRange;					    //��λ�жϷ�Χ
}sStateCheck;
   
//ȡ������
#define SSTATECHECK(x)	(*((sStateCheck*)x))


/*
 * ����:    �������
 * ����:
 *          in : ����
 * ���     float���˲����
 */
int StateCheck(void *pStcb, float in);

char GetSteadySta(int State);
char GetZeroSta(int State);


#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
}
#endif

#endif
