#ifndef _DRVPROC_H    
#define _DRVPROC_H

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif

#define SDIGSAMPLE(x)	(*((sDigSample*)x))



/*
 * ����:   �ֶ�ֵ���� 
 */
float WetDrvProc(float Wet, float Drv);


#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
}
#endif

#endif

