#ifndef _ConnectCfg_H_
#define _ConnectCfg_H_

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif

#include "stm32f4xx.h"
#include "tim6_init.h"					//TIM6_ResetCount
#include "usart1_init.h"				//����
#include "usart3_init.h"				//����
#include "usart6_init.h"				//����
	
/*
STM32F4xx 1M �ռ����
size			size				size				name				addr
1M				0x100000		1048576			Space				[0x8000000 - 0x8100000]		
Sector0 - Sector11	16K  16K  16K  16K  64K  128K  128K  128K  128K  128K  128K  128K
��������������


32K				0x008000		32768				BootLoader	[0x8000000 - 0x8007FFF]		Sector0 - Sector1		16K  16K
	
16K				0x004000		16384				AppFlag			[0x8008000 - 0x800BFFF]		Sector2							16K
4Byte			0x000004		4						AppUP				[0x8008000 - 0x8008003]
4Byte			0x000004		4						AppERR			[0x8008004 - 0x8008007]

16K				0x004000		16384				NULL				[0x800C000 - 0x800FFFF]		Sector3							16K

64K				0x010000		65536				NULL				[0x8010000 - 0x801FFFF]		Sector4							64K

256K			0x040000		262144			App					[0x8020000 - 0x805FFFF]		Sector5 - Sector6		128K  128K

256K			0x040000		262144			AppBak			[0x8060000 - 0x809FFFF]		Sector7 - Sector8		128K  128K
4Byte			0x000004		4						AppBakCRC		[0x809FFFC - 0x809FFFF]
4Byte			0x000004		4						AppBakSize	[0x809FFF8 - 0x809FFFB]
4Byte			0x000004		4						AppBakUPSrc	[0x809FFF4 - 0x809FFF7]

16K				0x004000		16384				ASCII				[0x800C000 - 0x800FFFF]		Sector3							16K
2K				0x000800		2048				ASCIIFont		[0x800C000 - 0x800C7FF]
4Byte			0x000004		4						ASCIICRC		[0x800FFFC - 0x800FFFF]

16K				0x020000		131072			ASCII				[0x80A0000 - 0x80BFFFF]		Sector9							128K
2K				0x000800		2048				ASCIIFont		[0x80A0000 - 0x80A07FF]
4Byte			0x000004		4						ASCIICRC		[0x80BFFFC - 0x80BFFFF]
4Byte			0x000004		4						ASCIISize		[0x80BFFF8 - 0x80BFFFB]
4Byte			0x000004		4						ASCIIUpSrc	[0x80BFFF4 - 0x80BFFF7]

256K			0x040000		262144			GB					[0x80C0000 - 0x80FFFFF]		Sector10 - Sector11	128K  128K
225K			0x038400		230400			GBFont			[0x80C0000 - 0x80D83FF]
4Byte			0x000004		4						GBCRC				[0x80FFFFC - 0x80FFFFF]
4Byte			0x000004		4						GBSize			[0x80FFFF8 - 0x80FFFFB]
4Byte			0x000004		4						GBUpSrc			[0x80FFFF4 - 0x80FFFF7]
*/

//system_stm32f4xx.c Line 150				stm32f4xx.h		Line 91									//CPUƵ��

//ComPort
#define BootLoaderUsart_BaudRate		57600																	//ͨѶ�ڲ�����
#define BootLoaderUsart_Config			USART1_Config													//ͨѶ������
#define BootLoaderUsart_Init				USART1_Init														//ͨѶ�ڳ�ʼ��
#define ComSendByte  								USART1_SendByte												//ͨѶ�ڵ��ַ�����
#define ComGetByte_Wait  						USART1_GetByte_WaitTime								//ͨѶ�ڽ��յȴ�
#define BootLoaderUsart_GetString 	USART1_GetString											//ͨѶ�ڻ�ȡ����ַ�
#define ComSendString 							USART1_SendString											//ͨѶ���ַ�������
#define BootLoaderUsart_RX_Clear 		USART1_RX_Buffer_Clear								//ͨѶ�ڽ������
#define ComSendFinishWait()					while(USART1CheckISRSendBusy());			//ͨѶ�ڵȴ��������
#define PrintfPort									1																			//��ӡ��ʾ�˿ں� �����ں�

//Message
#define BootVersion									"STM32F4_BootLoader_Version: 000001"	//�汾��
#define MenuWaitTime								15																		//�����˵��޲�����תʱ�� ��
#define ShowGetFileWait							60																		//��ʾ�ȴ������ļ�N��		����ȴ�ʱ��ΪYmodemGetFileWaits��������һ��
#define CopyEver										0																			//����Ӧ����������Ӧ��������ʱ���Ƿ�һֱִ�п�������	0��ִ��	1ִ��
#define GetTimeMSecCnt							TIM6_GetCount													//��ȡ����ĺ���
#define ResetTimeMSecCnt						TIM6_ResetCount												//��λ������ֵ

//#define RTCTimeFlag													//ʵʱʱ�ӿ�����־		���ιر�ʵʱʱ��
//#define AutoBaudRateCheckFlag								//�Ƿ��Զ�ʶ������		������		������
//#define BootKey															//�Ƿ�����������		������		������		���������������ֻ��ѡ��һ��������ѡ��ֻ������������Ч
#define CmdKey															//�Ƿ��������	������		������				������������������������Ч
		
#ifdef BootKey
#define BootKeyPort			GPIOE									//���������Ŷ˿ں�
#define BootKeyPin			GPIO_Pin_0						//���������ź�
#define BootKeyDownVal	Bit_RESET							//����������ʱ��ƽΪ�߻��ǵ�
#define BootKeyRCC			RCC_AHB1Periph_GPIOE	//������ʱ�ӵ�Դ
#endif

void BlockDataCheck(void);										//���ݿ���
u32 AutoBaudRateCheck(const u8 Val);					//�Զ�������ʶ����
void TimDelayMS(const u16 DelayMS);						//��ʱ�����뼶�ӳ�
void BootLoaderOver(void);										//BootLoader������ʼ��

#ifdef __cplusplus		   //�����CPP����C���� //��������
}						   
#endif

#endif
