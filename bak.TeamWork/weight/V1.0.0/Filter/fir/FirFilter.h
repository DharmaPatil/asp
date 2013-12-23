#ifndef __FIR_FILTER_H_
#define __FIR_FILTER_H_

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif

typedef struct _sFirFilter
{
	const float*  pZNum;		//���
	float* 	pDelay;		//������
	int		nLen;		//����
	int		bInit;		//�Ƿ��Ѿ���ʼ��
}sFirFilter;

//ȡ������
#define SFIR(x)	(*((sFirFilter*)x))



/*
 * ����:    FIR�˲���ʼ��
 * ����:
 *          ad : ADֵ
 * ���     0: ����   -1: �쳣
 */
char  FirFilterInit(void *pFCB);

/*
 * ����:    FIR�˲�
 * ����:
 *          ad : ADֵ
 * ���     float���˲����
 */
float FirFilterDynamic(void *pFCB, float in);
float FirFilterStatic(void *pFCB, float in);


#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
}
#endif

#endif
