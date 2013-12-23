#ifndef _POWER_INIT_H_	
#define _POWER_INIT_H_

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif

#include "stm32f4xx.h"
	
#define PowerLine_Read 		GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4)	//��Դ�߼��
#define Power_Close 		GPIO_SetBits(GPIOE, GPIO_Pin_3)				//��Դ��
#define Power_Open 			GPIO_ResetBits(GPIOE, GPIO_Pin_3)			//��Դ��

void Power_init(void);

#ifdef __cplusplus		   //�����CPP����C���� //��������
}
#endif

#endif
