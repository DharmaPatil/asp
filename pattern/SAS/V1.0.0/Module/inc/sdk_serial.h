#ifndef SDK_SERIAL_H
#define SDK_SERIAL_H

#include "includes.h"
#include "usart_cfg.h"			//�������ú���
#include "usart6.h"				//����6����SDKͨ���߳�

/**********************************************************************/
//����6��ʼ������
void Com_Port6_Init(void);

//����6����д����
s32 Usart6_ReadData(char *buf,u32 len);

s32 Usart6_WriteData(char *buf, u32 len);

s32 Usart6_Get_BufferLen(u32 param);

#endif //__SERIAL_PROTOCOL_H__

