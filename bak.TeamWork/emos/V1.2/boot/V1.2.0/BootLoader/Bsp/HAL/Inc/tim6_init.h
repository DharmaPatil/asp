#ifndef _TIM6_INIT_H_	
#define _TIM6_INIT_H_

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif

#include "stm32f4xx.h"

#define Mode_1US				0							//1΢��
#define Mode_10US				1							//10΢��
#define Mode_100US			2							//100΢��
#define Mode_500US			3							//500΢��

typedef void(*TIM6_fun)(void); 			//����ָ��
	
u8 TIM6_Config(const TIM6_fun Cfun,const u16 TimeUS,const u8 mode);	 	//��ʱ����ʼ��
void TIM6_TimeISR_Open(void);												//������ʱ�����ж�
void TIM6_TimeISR_Close(void);											//�رն�ʱ�����ж�
void TIM6_ResetCount(void);													//��λ����ֵ
u32 TIM6_GetCount(void);														//��ü���ֵ
void TIM6_CntFlagOnOff(const u8 OnOff);							//�Ƿ����ü�������

#ifdef __cplusplus		   //�����CPP����C���� //��������
}
#endif

#endif
