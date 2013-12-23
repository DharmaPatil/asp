#ifndef _YMODEM_UPDATE_H_
#define _YMODEM_UPDATE_H_

#ifdef __cplusplus		   		//�����CPP����C���� //��ʼ����
extern "C" {
#endif

#include "debug_info.h"
	
int Do_Serial_Update(char *arg, DebugSend send);

u16 Ymodem_CRC16(u8* data, u32 len);

void Init_YmodemUpdate_Buffer(void);


#ifdef __cplusplus		   		//�����CPP����C���� //��������
}
#endif

#endif
