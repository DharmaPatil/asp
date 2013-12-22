#ifndef __DIG_SAMPLE_H_
#define __DIG_SAMPLE_H_

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif

typedef struct _sDigSample
{
	int		nCycle;					//��������
	int		nIndex;					//��������
	float	fKeep;					//����ֵ
	float	fSum;						//�м����ֵ
}sDigSample;

//ȡ������
#define SDIGSAMPLE(x)	(*((sDigSample*)x))



/*
 * ����:    ����
 * ����:
 *          in : ����
 * ���     int�������Ƿ����
 *					pOut���������
 */
int DigSample(void *pDscb, float in, float* pOut);


#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
}
#endif

#endif
