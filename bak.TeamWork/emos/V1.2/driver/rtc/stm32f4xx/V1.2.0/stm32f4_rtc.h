#ifndef _RTC_INIT_H_
#define _RTC_INIT_H_

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif
	
#include "includes.h"
	
#define 		RTCCMD_SetTime					0				//����ʱ��	
#define 		RTCCMD_SetDate					1				//��������
#define 		RTCCMD_LookTimeDate			2				//��ѯ����
#define 		RTCCMD_ShowTimeDate			3				//��ʾ����	���ڴ�ӡ��ʾ
#define 		RTCCMD_SetAlarm_A				4				//��������Aʱ��
#define 		RTCCMD_SetAlarm_B				5				//��������Bʱ��
#define 		RTCCMD_SetAlarm_A_FUN		6				//��������Aִ�к���
#define 		RTCCMD_SetAlarm_B_FUN		7				//��������Bִ�к���

//ʱ��ṹ��
typedef struct
{
	u16 Year;	   //��
	u8 Month;	   //��
	u8 Day;		   //��
	u8 HH;		   //ʱ
	u8 MM;		   //��
	u8 SS;		   //��
	u8 Week;	   //��
	u8 AMorPM;		//����or����	0����	1����			//24Сʱ�Ʋ�ʹ��
} Time_Body;

typedef struct
{
	u8 	ALARM_Cfg_Day;						//����	or ��
	u8 	ALARM_Cfg_Unit;						//��λ	 0���� 1��
	u8 	ALARM_Cfg_Hours;					//Сʱ
	u8 	ALARM_Cfg_Minutes;				//����
	u8 	ALARM_Cfg_Seconds;				//��
	u8 	ALARM_Cfg_Alarm_CE;				//����		1��0��
} ALARM_Cfg;										//��������

/****************************************************************************
* ��	�ƣ�u8 RTCInit(void);
* ��	�ܣ�Ӳ��RTC��ʼ��
* ��ڲ�������
* ���ڲ�����u8	�����Ƿ�ɹ�		1�ɹ� 0ʧ��
* ˵	������
****************************************************************************/
u8 RTCInit(void);	 //RTC��ʼ��

/****************************************************************************
* ��	�ƣ�HANDLE DeviceOpen(DEV_ID ID,u32 lParam,HVL_ERR_CODE* err)
* ��	�ܣ��豸�򿪻���豸���
* ��ڲ�����DEV_ID ID						�豸ID��
						u32 lParam					����				����
						HVL_ERR_CODE* err		����״̬
* ���ڲ�����HANDLE 							�豸���
* ˵	������
****************************************************************************/	

/****************************************************************************
* ��	�ƣ�HVL_ERR_CODE DeviceClose(HANDLE IDH)
* ��	�ܣ��豸�ر�
* ��ڲ�����HANDLE IDH					�豸���
* ���ڲ�����HVL_ERR_CODE* err		����״̬
* ˵	������
****************************************************************************/	

/****************************************************************************
* ��	�ƣ�s32 CharDeviceRead(HANDLE IDH,u8* Buffer,u32 len,u32* Reallen,u32 lParam)
* ��	�ܣ��豸������
* ��ڲ�����HANDLE IDH		�豸���
						u8* Buffer		ʱ��ֵ								Time_Body�ṹ��	
						u32 len				ϣ����ȡ�����ݸ���		����
						u32* Reallen	ʵ�ʶ�ȡ�����ݸ���		����
						u32 lParam		����									����
* ���ڲ�����s32						��ȡ�����Ƿ�ɹ�			����
* ˵	����
typedef struct
{
	u16 Year;	   //��
	u8 Month;	   //��
	u8 Day;		   //��
	u8 HH;		   //ʱ
	u8 MM;		   //��
	u8 SS;		   //��
	u8 Week;	   //��
	u8 AMorPM;	//����or����	0����	1����			//24Сʱ�Ʋ�ʹ��
} Time_Body;
****************************************************************************/	

/****************************************************************************
* ��	�ƣ�s32 CharDeviceWrite(HANDLE IDH,u8* Buffer,u32 len,u32* Reallen,u32 lParam)
* ��	�ܣ��豸д����
* ��ڲ�����HANDLE IDH		�豸���							����
						u8* Buffer		ʱ��ֵ								Time_Body�ṹ��	
						u32 len				ϣ��д������ݸ���		����
						u32* Reallen	ʵ��д������ݸ���		����
						u32 lParam		����									����
* ���ڲ�����s32						д������Ƿ�ɹ�			����
* ˵	����
typedef struct
{
	u16 Year;	   //��
	u8 Month;	   //��
	u8 Day;		   //��
	u8 HH;		   //ʱ
	u8 MM;		   //��
	u8 SS;		   //��
	u8 Week;	   //��
	u8 AMorPM;	//����or����	0����	1����			//24Сʱ�Ʋ�ʹ��
} Time_Body;
****************************************************************************/	

/****************************************************************************
* ��	�ƣ�s32 DeviceIoctl(HANDLE IDH,u32 cmd,u32 lParam)
* ��	�ܣ��豸����
* ��ڲ�����HANDLE IDH		�豸���
						u32 cmd				�豸����
						u32 lParam		���������˵��
* ���ڲ�����s32 					�����ֵ��˵��
* ˵	����
����											����							����ֵ
RTCCMD_SetTime						Time_Body					�Ƿ�ɹ�		1�ɹ� 0ʧ��
RTCCMD_SetDate						Time_Body					�Ƿ�ɹ�		1�ɹ� 0ʧ��
RTCCMD_LookTimeDate				Time_Body					����
RTCCMD_ShowTimeDate				����							����
RTCCMD_SetAlarm_A					ALARM_Cfg					����
RTCCMD_SetAlarm_B					ALARM_Cfg					����
RTCCMD_SetAlarm_A_FUN			Alarm_fun					����
RTCCMD_SetAlarm_B_FUN			Alarm_fun					����
****************************************************************************/	

#ifdef __cplusplus		   //�����CPP����C���� //��������
}
#endif

#endif

/*
	void ALarmA(void)					//����A
	{
		printf("\r\nALarmA\r\n");
	}		
	void ALarmB(void)					//����B
	{
		printf("\r\nBLarmB\r\n");
	}

	u32 Reallen = 0;
	HVL_ERR_CODE err = HVL_NO_ERR;	//�豸��������
	
	HANDLE RTC_Dev = 0;												//�豸���
	
	Time_Body SetTime = 
	{
		2012,11,11,			//������
		23,59,58,				//ʱ����
		7,0							//�� AM/PM
	};
	
	ALARM_Cfg RTCA = 		//����ʱ��				//ÿ��
	{
		99,				//����	or ��
		0,				//��λ	 	0���� 1��
		99,				//Сʱ
		99,				//����
		99,				//��
		1,				//����		1��0��
	};
	
	ALARM_Cfg RTCB = 		//����ʱ��				//4��16:xx:10���ʱ�򴥷�����	ÿ����
	{
		4,				//����	or ��
		0,				//��λ	 	0���� 1��
		16,				//Сʱ
		99,				//����
		10,				//��
		1,				//����		1��0��
	};

	USART_STRUCT usart1cfg = 
	{
		57600,																	//������
		USART_WordLength_8b,										//����λ
		USART_StopBits_1,												//ֹͣλ
		USART_Parity_No													//Ч��λ
	};

	USART1_Config(&usart1cfg);								//���ڳ�ʼ��
	Set_PrintfPort(1);												//���ô�ӡ����
	printf("���Կ�ʼ\r\n");
	
	RTCInit();
	
	RTC_Dev = DeviceOpen(CHAR_RTC,0,&err);		//�豸��
	if(err != HVL_NO_ERR)
		while(1);
	
	if(CharDeviceWrite(RTC_Dev,(u8*)&SetTime,0,&Reallen,0) == 0)					//����ʱ��
		while(1);
	if(DeviceIoctl(RTC_Dev,RTCCMD_SetAlarm_A,(u32)&RTCA) == 0)						//��������A
		while(1);
	if(DeviceIoctl(RTC_Dev,RTCCMD_SetAlarm_B,(u32)&RTCB) == 0)						//��������B
		while(1);
	if(DeviceIoctl(RTC_Dev,RTCCMD_SetAlarm_A_FUN,(u32)ALarmA) == 0)				//��������Aִ�к���
		while(1);
	if(DeviceIoctl(RTC_Dev,RTCCMD_SetAlarm_B_FUN,(u32)ALarmB) == 0)				//��������Bִ�к���
		while(1);
	
	while(1)
	{
		if(DeviceIoctl(RTC_Dev,RTCCMD_ShowTimeDate,0) == 0)									//��ʾʱ��
			while(1);
		SysTimeDly(100);			//1S
	}
*/
