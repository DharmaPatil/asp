#ifndef _YMODEM_INIT_H_
#define _YMODEM_INIT_H_

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif

#include "stm32f4xx.h"

u8 Ymodem_START(const u8 Option);			//ymodem��ʼ						 	extern				flash_bootloader_init.c
u32 GetFlieSize(void);						//��ȡ�ļ���С						extern				flash_bootloader_init.c
u8 GetFlieNameLen(void);					//��ȡ�ļ�������					extern				flash_bootloader_init.c
u8* GetFlieNameDataAddr(void);		//��ȡ�ļ�����ŵ�ַ			extern				flash_bootloader_init.c

#ifdef __cplusplus		   //�����CPP����C���� //��������
}						   
#endif

#endif
