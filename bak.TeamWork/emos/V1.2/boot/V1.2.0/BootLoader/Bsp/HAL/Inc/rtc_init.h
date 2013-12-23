#ifndef _RTC_INIT_H_
#define _RTC_INIT_H_

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif

#include "stm32f4xx.h"

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

u8 RTC_Config(const u16 year,const u8 month,const u8 day,const u8 week,const u8 hour,const u8 min,const u8 sec);	 	//RTC����
ErrorStatus SetRTC_Time(const u8 hour,const u8 min,const u8 sec);										//����ʱ��
ErrorStatus SetRTC_Date(const u16 year,const u8 month,const u8 day,const u8 week);	//��������
void GetTime(Time_Body* pTime);															//��ȡʱ��
void ShowTime(void);																				//��ȡ����ʾʱ��

void RTC_AlarmA(const u8 A_Day,const u8 A_Unit,const u8 A_Hours,const u8 A_Minutes,const u8 A_Seconds,const u8 A_Alarm_CE,void (*Alarm_fun)(void));		//��������A		D>31 H>23 M>59 S>59�����θ��� ������������Ϊ��һ���� ��ÿ�뱨��
void RTC_AlarmB(const u8 B_Day,const u8 B_Unit,const u8 B_Hours,const u8 B_Minutes,const u8 B_Seconds,const u8 B_Alarm_CE,void (*Alarm_fun)(void));		//��������B		D>31 H>23 M>59 S>59�����θ��� ������������Ϊ��һ���� ��ÿ�뱨��

void Alarm_A_Caller(void(*Alarm_fun)(void));								//����A��Ӧִ�к���
void Alarm_B_Caller(void(*Alarm_fun)(void));								//����B��Ӧִ�к���

#ifdef __cplusplus		   //�����CPP����C���� //��������
}
#endif

#endif

/*
	#include "printf_init.h"		//DebugPf

	void Alarm_A_Respond(void)						//���屨��Aִ�к���
	{
		DebugPf("AAAAAA!!\r\n");						//����
	}
	void Alarm_B_Respond(void)						//���屨��Bִ�к���
	{
		DebugPf("BBBBBBB!!\r\n");						//����
	}

	if(RTC_Config(2012,10,4,4,15,45,30))		//ʵʱʱ�ӳ�ʼ��		��������ʱ����
	{
		RTC_AlarmA(4,0,15,45,35,1,Alarm_A_Respond);		//��������A		ÿ��4��15:45:30��������A
		RTC_AlarmB(4,1,15,45,40,1,Alarm_B_Respond);		//��������B		ÿ��4  15:45:30��������B
	}

	ShowTime();										//��ȡ����ʾʱ��
*/
