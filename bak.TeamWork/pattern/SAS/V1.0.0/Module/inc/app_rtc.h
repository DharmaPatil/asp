#ifndef _APP_RTC_H	
#define _APP_RTC_H
	#ifdef __cplusplus		   		//�����CPP����C���� //��ʼ����
	extern "C" {
	#endif

#include "includes.h"
		
//ʱ��ṹ��
typedef  struct CSysTime { 
	u16 	year;		//��  2000+�����ɵ�65535
	u8  	mon;	//��  1-12
	u8 		day;		//��  1-31
	u8  	week;	//��  0-6��0Ϊ����
	u8		hour;	//ʱ  0-13
	u8		min;		//��  0-59
	u8		sec;		//��  0-59
}CSysTime;

CSysTime* Get_System_Time(void);
// ����ֵ������curTimeʱ��ṹ���ַ
// ˵����ͨ�����ص�ָ�룬�ɲ�ѯ����ǰ��ʱ��

u8 Set_System_Time(const CSysTime *time);
// ������timeΪҪ���õ�ʱ��ṹ��ָ��
// ����ֵ��Ϊ0ʱ���óɹ���Ϊ1ʱ���ó�������������Χ
// ˵��������timeָ��Ľṹ����������ϵͳʱ��


void CSysTime_Init(void);//˵������10ms��ʱ�����ã���ʼ��rtcģ��

void Update_System_Time(void);
// ˵������10ms��ʱ�����ã�����RTCʱ�䵽curTime��

		
	#ifdef __cplusplus		   		//�����CPP����C���� //��������
	}
	#endif
#endif	//_APP_RTC_H
