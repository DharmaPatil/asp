#ifndef __LINE_FIT_H_
#define __LINE_FIT_H_

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif

typedef struct _FitPoint
{
	float x;
	float y;
	float k;
} FitPoint;

typedef struct _sLineFit
{
	FitPoint   *pFitPoint;		//�궨��
	int		nFitCount;					//�궨����
}sLineFit;

//ȡ������
#define SLINEFIT(x)	(*((sLineFit*)x))



/*
 * ����:    �������
 * ����:
 *          in : ����
 * ���     float���˲����
 */
float LineFit(void *pNcb, float in);


#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
}
#endif

#endif
