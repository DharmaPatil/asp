#ifndef _ETH_INIT_H_	
#define _ETH_INIT_H_

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif

#include "includes.h"
	
u8 ETH_Config(void);		//��̫����ʼ��

void ETH_NVIC_Enable(void);

void ETH_NVIC_Disable(void);

	
#ifdef __cplusplus		   //�����CPP����C���� //��������
}
#endif

#endif
