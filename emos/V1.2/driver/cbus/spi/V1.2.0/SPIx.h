#ifndef _SPIX_H_	
#define _SPIX_H_

#ifdef __cplusplus		   		
extern "C" {
#endif

#include "includes.h"			
#include "driver.h"	

// ӳ���		SPIx						ӳ��1		ӳ��2
// 					SPI1_SCK				PA5			PB3
// 					SPI1_MISO				PA6			PB4
// 					SPI1_MOSI				PA7			PB5
// 					SPI2_SCK				PB13		PB10
// 					SPI2_MISO				PB14		PC2
// 					SPI2_MOSI				PB15		PC3
// 					SPI3_SCK				PC10		PB3
// 					SPI3_MISO				PC11		PB4
// 					SPI3_MOSI				PC12		PB5
	
#define SPIS		(SPI_TypeDef*)0				//���ģ��SPI
	
typedef struct
{
	SPI_TypeDef*		SPIx;								//SPIѡ��0-IO��ģ��SPI��1-SPI1��2-SPI2��3-SPI3��������Ч
	
	u16 						SPIx_SCK_Pin;				//SCK���ź�				GPIO_Pin_0 - GPIO_Pin_15
	GPIO_TypeDef* 	SPIx_SCK_Por;				//SCK�˿ں�				GPIOA - GPIOI
	u16 						SPIx_MISO_Pin;			//MISO���ź�			GPIO_Pin_0 - GPIO_Pin_15
	GPIO_TypeDef* 	SPIx_MISO_Por;			//MISO�˿ں�			GPIOA - GPIOI
	u16 						SPIx_MOSI_Pin;			//MOSI���ź�			GPIO_Pin_0 - GPIO_Pin_15	
	GPIO_TypeDef* 	SPIx_MOSI_Por;			//MOSI�˿ں�			GPIOA - GPIOI
	uint16_t				SPIx_BaudRate;			//������					SPI_BaudRatePrescaler_2	4	8	16	32	64	128	256
}SPI_Cfg;															//SPI����	

void SPIx_Init(void);			//��ʼ��SPI�����أ�HVL_NO_ERR��ʾ��ʼ���ɹ������أ�HVL_PARAM_ERR��ʾ������������ʼ��ʧ��

u8 SPIbyteSendGet(u8 byte);								//ģ��SPIӲ��SPI���շ���8λ����
void SPIWordSend(SPI_Cfg* Sx,u32 Word)	;							//ģ��SPIӲ��SPI Word����
u32 SPIWordGet(SPI_Cfg* Sx);													//ģ��SPIӲ��SPI Word����

void SPISSendcmd8(SPI_Cfg* Sx,u8 cmd);								//ģ��SPI����8λ����
u8 SPISReciveData8(SPI_Cfg* Sx);											//ģ��SPI����8λ����

#ifdef __cplusplus		   		
}
#endif

#endif

/*
���ģ��SPI
#include "MX25L3208DM2I_12G_init.h"
#include "SPIx.h"

u8 ID[3]={0};

SPI_Cfg SPIUser =
{
	SPIS,
	GPIO_Pin_10,
	GPIOC,
	GPIO_Pin_11,
	GPIOC,
	GPIO_Pin_12,
	GPIOC
};

	MX25L3208_Init();
	SPIx_Init(&SPIUser);
	GetMX25L3208ID(SPIS,ID);
*/

/*
Ӳ��SPI
#include "MX25L3208DM2I_12G_init.h"
#include "SPIx.h"

u8 ID[3]={0};

SPI_Cfg SPIUser =
{
	SPI3,
	GPIO_Pin_10,
	GPIOC,
	GPIO_Pin_11,
	GPIOC,
	GPIO_Pin_12,
	GPIOC
};

	MX25L3208_Init();
	SPIx_Init(&SPIUser);
	GetMX25L3208ID(SPI3,ID);
*/
