#ifndef __DYNCZEROLIZE_H_
#define __DYNCZEROLIZE_H_

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif

typedef struct {
    float fZeroValue;   // ��̬��λ
}sDyncZero;

//ȡ������
#define SDYNCZERO(x)	(*((sDyncZero*)x))

/*
 * ����:    ��̬��λ����
 */
float  DyncZeroLize(void *pZCB, float fIn, int bStaticZeroState);


#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
}
#endif

#endif
