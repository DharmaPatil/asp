#ifndef __ADCVALUECHECK_H_
#define __ADCVALUECHECK_H_

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif

#define  MAXADCVALUE    (long)5000000
#define  MinADCVALUE    (long)-5000000

/*
 * ����:    ���������߼��
 * ����:
 *          ad : ADֵ
 * ���     0: ����   -1: �쳣
 */
int  ADCValueCheck(long ad);


#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
}
#endif

#endif
