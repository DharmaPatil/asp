#include "app_rtc.h"
#include "pcf8563.h"
#include "debug_info.h"

volatile static CSysTime curTime={0};
static HANDLE rtc_handle;


// ˵������10ms��ʱ�����ã�����RTCʱ�䵽curTime��
void Update_System_Time(void)
{
	OutTime_Body systime={0};

	DeviceIoctl(rtc_handle,OUTRTCCMD_GetTime, (u32)&systime);
	curTime.year= systime.Year;
	curTime.mon	= systime.Month;
	curTime.day	= systime.Day;
	curTime.hour= systime.HH;
	curTime.min	= systime.MM;
	curTime.sec	= systime.SS;
	curTime.week= systime.Week;
}

//˵������10ms��ʱ�����ã���ʼ��rtcģ��
void CSysTime_Init(void)
{
	HVL_ERR_CODE err;
	if(OutRTC_init(0)==0)
	{
		debug(Debug_None,"init outer rtc failed!!!\n");
		return;
	}
	
	rtc_handle=DeviceOpen(CHAR_RTC,0,&err);
	if(err != HVL_NO_ERR)
	{
		debug(Debug_Error,"open rtc device failed!!!\n");
		while(1);
	}
}


// ����ֵ������curTimeʱ��ṹ���ַ
// ˵����ͨ�����ص�ָ�룬�ɲ�ѯ����ǰ��ʱ��
CSysTime* Get_System_Time(void)
{
	return (CSysTime*)&curTime;
}

// ������timeΪҪ���õ�ʱ��ṹ��ָ��
// ����ֵ��Ϊ0ʱ���óɹ���Ϊ1ʱ���ó�������������Χ
// ˵��������timeָ��Ľṹ����������ϵͳʱ��
u8 Set_System_Time(const CSysTime *time)
{
	OutTime_Body systime;
	
	if(time==NULL)
		return 1;
	systime.Year=time->year;
	systime.Month=time->mon;
	systime.Day=time->day;
	systime.HH=time->hour;
	systime.MM=time->min;
	systime.SS=time->sec;
	systime.Week=time->week;
	
	//����rtcоƬ���ʱ��
	return DeviceIoctl(rtc_handle, OUTRTCCMD_SetTime, (u32)&systime);
}


