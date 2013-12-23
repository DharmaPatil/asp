#ifndef _FLASH_BOOTLOADER_INIT_H_	
#define _FLASH_BOOTLOADER_INIT_H_

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif

#include "stm32f4xx.h"			//u32
	
void Jump_IAP(const u32 MainAppAddr);					//������ת
void BOOTLoader(const u32 BaudRate);					//�������		

#ifdef __cplusplus		   //�����CPP����C���� //��������
}						   
#endif

#endif
