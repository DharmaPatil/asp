#include "includes.h"
#include "task_msg.h"

#include "usart1.h"
#include "printf_init.h"

#include "app_beep.h"

#include "Keyboard.h"
#include "dt3102_io.h"
#include "guiwindows.h"
#include "debug_info.h"
#include "sys_param.h"

static HANDLE KEY_Dev=HANDLENULL;		//key device handle

static void fDT3102_IO_Init(void)
{
	uint16 tmp = 0xffff;
	
	DT3102_OutputInit();
	DT3102_InputInit();

	Param_Read(GET_U16_OFFSET(ParamDeviceSignal), &tmp, sizeof(unsigned short));
	if((tmp & 0xf800) != 0)		//��ƽ����ʹ�������ֽڵĵ�11λ����λ��ʼ��Ϊ0������λ������0ʱ�����¼���Ĭ�ϵ�ƽ
	{
		//����Ĭ�ϵ�ƽ
		DT3102_DeviceSignal_DefaultInit();
	}
	else
	{
		Assign_3102_DeviceSignal(tmp);
	}
}

void Key_Enable(BOOL status)
{
	if(KEY_Dev!=HANDLENULL)
	{
		if(status)
			DeviceIoctl(KEY_Dev,KEYCMD_SETONOFF,1);	//open
		else	
			DeviceIoctl(KEY_Dev,KEYCMD_SETONOFF,0);	//close
	}
}

void Key_Thread(void *arg)
{
	int KeyLookVal = 0;			//��ѯ�ñ���	
	HVL_ERR_CODE err = HVL_NO_ERR;	//�豸��������											
	GuiMsgInfo guiMsgInfo;
	guiMsgInfo.ID = WM_KEYDOWN;
    
	//���̳�ʼ��
	if(Key_Config() == 0) 
	{										
		debug(Debug_None, "Error: Emos key init failed!\r\n");
		SysTaskSuspend(SYS_PRIO_SELF);
	}
	
	KEY_Dev = DeviceOpen(CHAR_KEY,0,&err);		//�򿪼���
	if(err != HVL_NO_ERR)
		debug(Debug_Warning, "Warning: Emos key open failed!\r\n");
	
	Key_Enable(TRUE);											//������������ 
	//�����ʼ���ŵ����,��Ϊ��Ҫ��ȡFRAM
	fDT3102_IO_Init();

	while(1)			//������̲���
	{	
		KeyLookVal = CharDeviceRead(KEY_Dev,0,0);			//��ȡ����ֵ	
		if(KeyLookVal < 0) continue;

		Beep_Voice(1, 5);
       		
		guiMsgInfo.wParam = KeyLookVal;
		GuiMsgQueuePost(&guiMsgInfo);
	}

 }
