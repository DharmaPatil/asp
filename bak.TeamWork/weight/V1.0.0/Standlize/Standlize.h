#ifndef __ADCNORMALIZE_H_
#define __ADCNORMALIZE_H_

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif

typedef struct {
    float fParam;
}sStandlize;

//ȡ������
#define SSTAND(x)	(*((sStandlize*)x))

/*
 * ����:    ��׼��
 */
float  Standlize(void *pSCB,float fIn);

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
}
#endif

#endif
