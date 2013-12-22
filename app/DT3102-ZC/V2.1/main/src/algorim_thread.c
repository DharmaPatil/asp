#include "includes.h"
#include "task_def.h"
#include "task_timer.h"
#include "adjob.h"
#include "weight_param.h"
#include "device_info.h"
#include "wet_algorim.h"
#include "app_beep.h"
#include "io.h"
#include "sys_config.h"
#include "data_store_module.h"
#include "softdog.h"
#include "axletype_business.h"
#include "car_queue.h"
#include "scszc_debug_info.h"

static HANDLE ad_handle;

static int A_Handle;

void Algorim_Thread(void *arg)
{
	uint8 err;
	long adbuf[4] = {0};
//	static uint8 nFreqDiv = 0;
	sAxisType1_State_TypeDef axle1_status={0};//̥��ʶ����-��˫�֡����ϡ��ź�

	System_Config_Init();			//ϵͳ������FRAM��ʼ��
	Weight_Param_Init();			//���ز�����ʼ��
	init_flash();					//dataflash��ʼ��	
	Beep_Initial();					//ƽ̨��������������
	Beep_Voice(2, 10);		  		//��������2�� 100ms	
	CarQueue_Init();
	SCSZC_Debug_Cmd_Init();
	A_Handle = Register_SoftDog(300, SOFTDOG_RUN, "�㷨�߳�");

	Weight_Init();

	if(ADJOB_init(ADJobPrio) == 0) {
		debug(Debug_None,"ERROR:cs5532 init failed\r\n");
		SysTaskSuspend(SYS_PRIO_SELF);
	}
	
	ad_handle = DeviceOpen(CHAR_AD,0,&err);			//��AD��������
	if(err != HVL_NO_ERR) {
		debug(Debug_Error, "Error: open cs5532 failed!\r\n");
		while(1);
	}
	
	while(1) 
	{
		if(CharDeviceRead(ad_handle,(u8*)adbuf,16) != 16)
				continue;

		UpDate_ADChip_Status(DeviceIoctl(ad_handle, AD_CMDCheck, 0));

		if(A_Handle >= 0) SoftDog_Feed(A_Handle);
		
		Weight_Business(adbuf);

		//��̥ʶ��������
		axle1_status = fRead_AxisType1State();
		AxleType_Business(&axle1_status);
		

#if 0				 
		//���ϳӹ��������ش���
		nFreqDiv++;
		if(nFreqDiv >= 16) {
			nFreqDiv = 0;
			//��ĻҪ���ڴ���״̬�����㶯̬�³�����
			if(Get_LC_Status(2))	
				NewCarKgCal_Srv(Get_Dynamic_Weight());	
		}
#endif
	}
}
