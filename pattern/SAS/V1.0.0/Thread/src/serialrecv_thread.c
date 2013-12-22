#include "includes.h"
#include "serial_protocol.h"
#include "sdk_serial.h"
#include "stm32f4_crc.h"
#include "debug_info.h"
#include "task_msg.h"
#include "app_msg.h"
#include "dt3102_io.h"
#include "crc.h"

#define COMRECVLEN	128

static char ComRecvBuf[COMRECVLEN] = {0};
static int ComKeepLen = 0;

void SerialRecv_Thread(void *arg)
{
	uint32 Reallen = 0, len = 0;
	
	Com_Port6_Init();

	while(1) 
	{
		//��ȡ���ڻ��������ݳ���
		Usart6_Get_BufferLen((u32)&len);
		if(len > COMRECVLEN) len = COMRECVLEN;
		
		//����û�н��յ�����, ��ʱ120ms
		if(len == 0) 
		{
			SysTimeDly(12);
			continue;
		}
		
		//����Խ�籣��
		if(ComKeepLen < 0) ComKeepLen = 0;
		
		//������
		if((len + ComKeepLen) > COMRECVLEN) 
		{
			Reallen = Usart6_ReadData(&ComRecvBuf[0],len);
			ComKeepLen = Reallen;
		}
		else
		{
			Reallen = Usart6_ReadData(&ComRecvBuf[ComKeepLen],len);
			ComKeepLen += Reallen;
		}

		SDK_Protocol_Parse(ComRecvBuf, &ComKeepLen);	
	}
}
