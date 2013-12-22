#include "sdk_serial.h"
#include "task_def.h"

static HANDLE USART6_Dev = 0;							//����6�豸���

//����6д������
SYS_EVENT *Usart6Lock = NULL;

s32 Usart6_ReadData(char *buf,u32 len)
{	
	if(buf == NULL) return -1;
	
	return CharDeviceRead(USART6_Dev, (u8 *)buf, len);
}

s32 Usart6_WriteData(char *buf, u32 len)
{
	uint8 err;
	int tmp = 0;

	if(buf == NULL) return -1;
	
	SysMutexPend(Usart6Lock, 0, &err);
	if(err != SYS_ERR_NONE) return -1;

	tmp = CharDeviceWrite(USART6_Dev, (u8 *)buf, len);
	SysMutexPost(Usart6Lock);

	return tmp;
}

s32 Usart6_Get_BufferLen(u32 param)
{
	return DeviceIoctl(USART6_Dev,CMD_GET_INPUT_BUF_SIZE, param);
}

//����6��ʼ����򿪺���
void Com_Port6_Init(void)
{
	HVL_ERR_CODE err = HVL_NO_ERR;						//����״̬
	
	USART_InitTypeDef USART6_User = 		
	{
		9600,											//������
		USART_WordLength_8b,
		USART_StopBits_1,
		USART_Parity_No,
		USART_Mode_Rx | USART_Mode_Tx,
		USART_HardwareFlowControl_None
	};
	
	usart6_init();					
	
	USART6_Dev = DeviceOpen(CHAR_USART6,(u32)&USART6_User,&err);			//�豸��
	if(err != HVL_NO_ERR)
	{
		debug(Debug_None, "�򿪴���6ʧ��!\r\n");
		SysTaskSuspend(SYS_PRIO_SELF);
	}	

	//���ô���6��������ģʽ
	DeviceIoctl(USART6_Dev, CMD_SET_RX_MODE, RX_LENGTH_MODE);

	//��������6д������
	Usart6Lock = SysMutexCreate(Com6MutexPrio, &err);
}


