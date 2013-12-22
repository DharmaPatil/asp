#ifndef _MX25L3208DM2I_12G_INIT_H_	
#define _MX25L3208DM2I_12G_INIT_H_

#ifdef __cplusplus		   	//�����CPP����C���� //��ʼ����
extern "C" {
#endif

#include "stm32f4xx.h"
#include "SPIx.h"
	
#define	MX25L3208_CEPin		GPIO_Pin_9
#define MX25L3208_CEPort	GPIOC

#define MX25L3208_SPI			0//SPI3
	
void MX25L3208_Init(void);											//��ʼ��FLASH�豸
void GetMX25L3208ID(SPI_Cfg* spix,u8* ID);			//��ȡ�豸ID��
	
#ifdef __cplusplus		   	//�����CPP����C���� //��������
}
#endif

#endif

/*
	u8 ID[3]={0};

	MX25L3208_Init();
	GetMX25L3208ID(ID);
*/
