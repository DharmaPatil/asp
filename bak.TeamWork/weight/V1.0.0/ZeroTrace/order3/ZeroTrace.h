#ifndef __ZERO_TRACE_H_
#define __ZERO_TRACE_H_

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif


#ifndef NULL
#define NULL ((void *)0)
#endif

typedef struct _sZeroTrace
{
	int  		nTraceTime;					//��λ����ʱ��
	int	 		nTraceIndex;				//�������
	float		fPowerOnZero;				//���
	float		fTraceZero;					//���
	float		fTraceRange;  			    //��λ���ٷ�Χ
	float		fTraceStep;					//��λ�����ٶ�
	float		fHandRange;				    //�ֶ����㷶Χ
	signed char		cHandZerosFlag;			    //�ֶ������־
	float		fPowerRange;				//�������㷶Χ
	int			nPowerTime;					//��������ʱ��
	int			nPowerIndex;				//��������ʱ��
	float PointFirst;			//�����һ����
	float PointMid;				//�����м��
	float PointLast;				//�������һ����
	float fDrv;						//�ֶ�ֵ
}sZeroTrace;

//ȡ������
#define SZEROTRACE(x)	(*((sZeroTrace*)x))


/*
 * ����:    ������������
 * ����:
 *          pZtcb : ע��ʱ����
 *					bSet : 1�����㣻0���������
 * ���     float��������
 */
void SetZero(void *pZtcb, signed char bSet);


/*
 * ����:    ��λ����
 * ����:
 *          in : ����
 * ���     float��������
 */
float ZeroTrace(void *pZtcb, float in);


#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
}
#endif

#endif
