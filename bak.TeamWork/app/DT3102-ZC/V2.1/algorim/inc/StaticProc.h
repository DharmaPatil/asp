#ifndef __STATICPROC_H_
#define __STATICPROC_H_

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif

typedef struct {
    char nInCount;
}sStaticProc;

//ȡ������
#define SSTATIC(x)	(*((sStaticProc*)x))

/*
 * ����:    ��̬����
 */
float  StaticProc(void* pSecb, float * pInArr, int nInCount);


#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
}
#endif

#endif
