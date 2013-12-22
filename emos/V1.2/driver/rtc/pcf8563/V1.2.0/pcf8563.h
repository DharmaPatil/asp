#ifndef _PCF8563T_INIT_H_
#define _PCF8563T_INIT_H_

#ifdef __cplusplus		   		//�����CPP����C���� //��ʼ����
extern "C" {
#endif

#include "includes.h"

#define 		OUTRTCCMD_SetTime					0				//����ʱ��
#define 		OUTRTCCMD_GetTime					1				//��ȡʱ��
#define 		OUTRTCCMD_ShowTime				2				//��ʾʱ��	��Ҫ�������ú�printf ���ڲ�����
	
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
} OutTime_Body;

/****************************************************************************
* ��	�ƣ�u8 OutRTC_init(OutTime_Body* Time_t)
* ��	�ܣ��ⲿRTC��ʼ��
* ��ڲ�����OutTime_Body* Time_t			���õ�ʱ��	0��ʾ����Ҫʱ������
* ���ڲ�����u8												�Ƿ�ɹ�		1�ɹ�	0ʧ��
* ˵	������
typedef struct
{
	u16 Year;	   //��
	u8 Month;	   //��
	u8 Day;		   //��
	u8 HH;		   //ʱ
	u8 MM;		   //��
	u8 SS;		   //��
	u8 Week;	   //��
} OutTime_Body;
****************************************************************************/
u8 OutRTC_init(OutTime_Body* Time_t);

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
						u8* Buffer		ʱ��ֵ								OutTime_Body�ṹ��	
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
} OutTime_Body;
****************************************************************************/	

/****************************************************************************
* ��	�ƣ�s32 CharDeviceWrite(HANDLE IDH,u8* Buffer,u32 len,u32* Reallen,u32 lParam)
* ��	�ܣ��豸д����
* ��ڲ�����HANDLE IDH		�豸���							����
						u8* Buffer		ʱ��ֵ								OutTime_Body�ṹ��	
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
} OutTime_Body;
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
OUTRTCCMD_SetTime					OutTime_Body			�Ƿ�ɹ�		1�ɹ� 0ʧ��
OUTRTCCMD_GetTime					OutTime_Body			�Ƿ�ɹ�		1�ɹ� 0ʧ��
OUTRTCCMD_ShowTime				����							��ȡ��ʾʱ���Ƿ�ɹ�		0ʧ��	1�ɹ�	(����ʹ�ñ�����ʵ��Printf)
****************************************************************************/	

#ifdef __cplusplus		   		//�����CPP����C���� //��������
}
#endif

#endif

/*
#include "pcf8563t_init.h"

	u8 err = 0;
	u8 i = 0;
	
	HANDLE RTC_Dev = 0;												//�豸���
	
	USART_STRUCT usart1cfg = 
	{
		57600,																	//������
		USART_WordLength_8b,										//����λ
		USART_StopBits_1,												//ֹͣλ
		USART_Parity_No													//Ч��λ
	};

	OutTime_Body UserSetOutTime = 						//������ʱ��
	{
		2012,12,18,11,42,5,1
	};
	
	OutTime_Body Timex = {0};									//������ʱ��
	
	USART1_Config(&usart1cfg);								//���ڳ�ʼ��
	Set_PrintfPort(1);												//���ô�ӡ����
	printf("���Կ�ʼ\r\n");
	

	if(OutRTC_init(&UserSetOutTime) == 0)		//�ⲿRTC��ʼ��
		while(1);
	
	RTC_Dev = DeviceOpen(CHAR_RTC,0,&err);	//�豸��
	if(err != HVL_NO_ERR)
		while(1);
	
	while(1)
	{
		if(i == 20)			//20��֮����������ʱ��
		{
			UserSetOutTime.Year = 2013;
			UserSetOutTime.Month = 11;
			UserSetOutTime.Day = 11;
			UserSetOutTime.HH = 23;
			UserSetOutTime.MM = 14;
			UserSetOutTime.SS = 5;
			UserSetOutTime.Week = 3;
			if(DeviceIoctl(RTC_Dev,OUTRTCCMD_SetTime,(u32)&UserSetOutTime) == 0)									//��ʾʱ��
				while(1);
		}
		
		if(i == 40)			//40��֮����������ʱ��
		{
			UserSetOutTime.Year = 2045;
			UserSetOutTime.Month = 12;
			UserSetOutTime.Day = 15;
			UserSetOutTime.HH = 12;
			UserSetOutTime.MM = 4;
			UserSetOutTime.SS = 2;
			UserSetOutTime.Week = 6;
			if(CharDeviceWrite(RTC_Dev,0,0,0,(u32)&UserSetOutTime) == 0)			//�޸�ʱ��
				while(1);
		}
		
		if(i < 40)			//40��֮ǰ��Ioctl�������ʱ����ʾ
		{
			if(DeviceIoctl(RTC_Dev,OUTRTCCMD_ShowTime,0) == 0)							//��ʾʱ��
				while(1);
		}
		else						//40��֮ǰ��Read�������ʱ����ʾ
		{
			if(CharDeviceRead(RTC_Dev,0,0,0,(u32)&Timex) == 0)							//��ȡʱ��
				while(1);
			printf("%d+%d+%d %d:%d:%d %d\r\n",Timex.Year,		//��						//��ʾʱ��
																	Timex.Month,				//��
																	Timex.Day,					//��
																	Timex.HH,						//ʱ
																	Timex.MM,						//��
																	Timex.SS,						//��
																	Timex.Week);				//�� 0-6
		}
		SysTimeDly(100);			//1S
		i++;
	}
*/
