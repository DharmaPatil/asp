#include "rtc_init.h"
#include "printf_init.h"		//DebugPf

//δʵ��WakeUP���ѹ��ܡ�Tamper���ܡ�Time stamp event����

typedef void(*Alarm_fun)(void); //����ָ��
static Alarm_fun A_Alarm_fun = 0;			//����A��Ӧִ�к���ָ��			����static
static Alarm_fun B_Alarm_fun = 0;			//����B��Ӧִ�к���ָ��			����static

static u8 RTC_pDisc[] = "LuoHuaiXiang_Rtc\r\n";				//�豸�����ַ�		����static

/****************************************************************************
* ��	�ƣ�void RTC_Config(void)
* ��	�ܣ�����RTCʱ��
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void RTC_Config(void)
{
  RCC_BackupResetCmd(ENABLE);					//����RTC�󱸼Ĵ���
	RCC_BackupResetCmd(DISABLE);				//����RTC�󱸼Ĵ���
	
  RCC_LSEConfig(RCC_LSE_ON);					//ʹ���ⲿ����
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);	//�ȴ��ⲿ����׼����

	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);							//ѡ��ʱ����Դ
	RCC_RTCCLKCmd(ENABLE);															//RTC����	��������RCC_RTCCLKConfig֮��

  RTC_WaitForSynchro();																//�ȴ�RTC�Ĵ�����APBʱ��ͬ��
}

/****************************************************************************
* ��	�ƣ�ErrorStatus SetRTC_Init(void)
* ��	�ܣ����������ṹ
* ��ڲ�������
* ���ڲ�����ErrorStatus		ERROR or SUCCESS
* ˵	������
****************************************************************************/
ErrorStatus SetRTC_Init(void)
{
	RTC_InitTypeDef RTC_InitTemp;			//RTC�����ṹ

	RTC_InitTemp.RTC_AsynchPrediv = 0x7F;		//RTC 7λ�첽�Ĵ��� <0x7F
	RTC_InitTemp.RTC_SynchPrediv = 0xFF;		//RTC 15λͬ���Ĵ��� <0x7FFF
	RTC_InitTemp.RTC_HourFormat = RTC_HourFormat_24;			//12Сʱ��
	
	return RTC_Init(&RTC_InitTemp);
}

/****************************************************************************
* ��	�ƣ�ErrorStatus SetRTC_Time(u8 hour,u8 min,u8 sec)
* ��	�ܣ�����ʱ��
* ��ڲ�����u8 hour		ʱ
						u8 min		��
						u8 sec		��
* ���ڲ�����ErrorStatus		ERROR or SUCCESS
* ˵	���������24Сʱ�ƹ�������RTC_H12����
					SetRTC_Time(23,59,58);		//23:59:58
****************************************************************************/
ErrorStatus SetRTC_Time(u8 hour,u8 min,u8 sec)
{
	ErrorStatus err = ERROR;
	RTC_TimeTypeDef RTC_TimeTemp;			//ʱ����
	
	SysIntEnter();
  RTC_TimeTemp.RTC_Hours = hour;					//ʱ
  RTC_TimeTemp.RTC_Minutes = min;					//��
  RTC_TimeTemp.RTC_Seconds = sec;					//��
  err = RTC_SetTime(RTC_Format_BIN, &RTC_TimeTemp);		//BIN ���õ�ʱ��Ϊʮ����	BCD ���õ�ʱ��Ϊʮ������
	SysIntExit();
	
  return err;
}

/****************************************************************************
* ��	�ƣ�ErrorStatus SetRTC_Date(u16 year,u8 month,u8 day,u8 week)
* ��	�ܣ���������
* ��ڲ�����u16 year	��
						u8 month	��
						u8 day		��
						u8 week		��
* ���ڲ�����ErrorStatus		ERROR or SUCCESS
* ˵	����SetRTC_Date(2012,10,4,4);			//2012-10-4 ����
****************************************************************************/
ErrorStatus SetRTC_Date(u16 year,u8 month,u8 day,u8 week)
{
	RTC_DateTypeDef RTC_DateTemp;			//��������
	u8 YearChar = 0;
	ErrorStatus err = ERROR;
	
	SysIntEnter();
	if(year >= 2000)	
		year -= 2000;
	YearChar = (u8)year;								//16λ��ת��Ϊ8λ��
	
	RTC_DateTemp.RTC_Year  = YearChar;	//��
	RTC_DateTemp.RTC_Month = month;			//��
	RTC_DateTemp.RTC_Date  = day;				//��
	RTC_DateTemp.RTC_WeekDay  = week;		//��
	
	err = RTC_SetDate(RTC_Format_BIN, &RTC_DateTemp);		//BIN ���õ�ʱ��Ϊʮ����	BCD ���õ�ʱ��Ϊʮ������
	SysIntExit();
	return err;
}

/**************************************************
**����ԭ��: void RTC_AlarmExtiNivc(void)
**����:		RTC�����ж�����
**�������:	��
**���ز���:	��
**˵    ������
**************************************************/
void RTC_AlarmExtiNivc(void)
{
	EXTI_InitTypeDef E;		//�ж���
	NVIC_InitTypeDef N;		//�ж�
	
	E.EXTI_Line = EXTI_Line17;									//�ж�������
	E.EXTI_Mode = EXTI_Mode_Interrupt;					//�ж�ģʽ
	E.EXTI_Trigger = EXTI_Trigger_Rising;				//�������ж�
	E.EXTI_LineCmd = ENABLE;										//�洢���õ��Ĵ���
	EXTI_Init(&E);
	
	N.NVIC_IRQChannel = RTC_Alarm_IRQn;					//�����ж�
	N.NVIC_IRQChannelPreemptionPriority = 2;		//���ȼ�
	N.NVIC_IRQChannelSubPriority = 0;						//��Ӧ��
	N.NVIC_IRQChannelCmd = ENABLE;							//�����жϼ���
	NVIC_Init(&N);															//�����������ó�ʼ���жϿ�����	
}

/**************************************************
**����ԭ��: void RTC_AlarmA(u8 A_Day,u8 A_Unit,u8 A_Hours,u8 A_Minutes,u8 A_Seconds,u8 A_Alarm_CE)
**����:		RTC����A����
**�������:	u8 A_Day				���ںŻ��ܺ�								// > 31����
						u8 A_Unit				���� or ��			0���� 1��
						u8 A_Hours			Сʱ												// > 23����
						u8 A_Minutes		����												// > 59����
						u8 A_Seconds		��													// > 59����
						u8 A_Alarm_CE		��������				0�رձ���	1��������
**���ز���:	��
**˵    ����
	RTC_AlarmMask_DateWeekDay		//���ε��ܺ���		��Ҫʱ����ƥ����������					ÿ�촥��һ��
	RTC_AlarmMask_None					//�����κ�����		��Ҫ���ڡ�ʱ����ƥ�����������	�ܵ��촥��һ�� or �������촥��һ��
	RTC_AlarmMask_Hours					//���ε�Сʱ			��Ҫ���ڡ�����ƥ�����������		�ܵ���ÿСʱ����һ�� or ��������ÿСʱ����һ��
	RTC_AlarmMask_Minutes				//���ε�����			��Ҫ���ڡ�ʱ��ƥ�����������		�ܵ���Сʱ��ʱÿ���Ӵ���һ�� or ��������Сʱ��ʱÿ���Ӵ���һ��
	RTC_AlarmMask_Seconds				//���ε���				��Ҫ���ڡ�ʱ��ƥ�����������		�ܵ���Сʱ���ӵ�ʱÿ�봥��һ�� or ��������Сʱ���ӵ�ʱÿ�봥��һ��
	RTC_AlarmMask_All						//�����κ�				������������ʱ���з����������������		ÿ�뱨��
**************************************************/
void RTC_AlarmA(u8 A_Day,u8 A_Unit,u8 A_Hours,u8 A_Minutes,u8 A_Seconds,u8 A_Alarm_CE)
{
	u8 i = 0;							//2��������������Ϊ�����ر�
	
	RTC_AlarmTypeDef A;		//����A
	
	RTC_AlarmCmd(RTC_Alarm_A,DISABLE);			//����A�ر�

	//�Ƿ�رձ���
	if(A_Alarm_CE == 0)			
	{
		RTC_ITConfig(RTC_IT_ALRA,DISABLE);			//����A�жϹر�
		RTC_AlarmCmd(RTC_Alarm_A,DISABLE);			//����A�ر�;
		return;
	}
	
	//���ñ���A
	A.RTC_AlarmDateWeekDay = A_Day;					//��������
	if(A_Unit == 0)													//����Ϊ���ڻ�������
		A.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;				//ѡ������
	else
		A.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_WeekDay;		//ѡ����
	
	//������������������Ϊ������������
	if(A_Day > 31)			i++;
	if(A_Hours > 23)		i++;
	if(A_Minutes > 59)	i++;
	if(A_Seconds > 59)	i++;
	if(i > 1)
		A.RTC_AlarmMask = RTC_AlarmMask_All;			//�����κ����� 				ÿ����������
	else
	{
		A.RTC_AlarmMask = RTC_AlarmMask_None;			//���ò����κ�����		һ�»�һ����������
		
		if(A_Day > 31)					//���ε��� or ����
			A.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;						
		else if(A_Hours > 23)		//���ε�Сʱ
			A.RTC_AlarmMask = RTC_AlarmMask_Hours;									
		else if(A_Minutes > 59)	//���ε���
			A.RTC_AlarmMask = RTC_AlarmMask_Minutes;									
		else if(A_Seconds > 59)	//���ε���
			A.RTC_AlarmMask = RTC_AlarmMask_Seconds;	
	}
	
	//A.RTC_AlarmTime.RTC_H12 = RTC_H12_PM;								//����	24Сʱ�Ʋ���Ҫ��
	A.RTC_AlarmTime.RTC_Hours = A_Hours;									//ʱ
	A.RTC_AlarmTime.RTC_Minutes = A_Minutes;							//��
	A.RTC_AlarmTime.RTC_Seconds = A_Seconds;							//��
	RTC_SetAlarm(RTC_Format_BIN,RTC_Alarm_A,&A);					//���ñ���
	
	//���������־
	RTC_ClearFlag(RTC_FLAG_ALRAF);				//�������A��־
	EXTI_ClearITPendingBit(EXTI_Line17);	//����ж�17�߱�־
	
	RTC_ITConfig(RTC_IT_ALRA,ENABLE);			//����A�жϿ���
	RTC_AlarmCmd(RTC_Alarm_A,ENABLE);			//����A����
}

/**************************************************
**����ԭ��: void RTC_AlarmB(u8 B_Day,u8 B_Unit,u8 B_Hours,u8 B_Minutes,u8 B_Seconds,u8 B_Alarm_CE)
**����:		RTC����A����
**�������:	u8 B_Day				���ںŻ��ܺ�								// > 31����
						u8 B_Unit				���� or ��			0���� 1��
						u8 B_Hours			Сʱ												// > 23����
						u8 B_Minutes		����												// > 59����
						u8 B_Seconds		��													// > 59����
						u8 B_Alarm_CE			��������				0�رձ���	1��������
**���ز���:	��
**˵    ����
	RTC_AlarmMask_DateWeekDay		//���ε��ܺ���		��Ҫʱ����ƥ����������					ÿ�촥��һ��
	RTC_AlarmMask_None					//�����κ�����		��Ҫ���ڡ�ʱ����ƥ�����������	�ܵ��촥��һ�� or �������촥��һ��
	RTC_AlarmMask_Hours					//���ε�Сʱ			��Ҫ���ڡ�����ƥ�����������		�ܵ���ÿСʱ����һ�� or ��������ÿСʱ����һ��
	RTC_AlarmMask_Minutes				//���ε�����			��Ҫ���ڡ�ʱ��ƥ�����������		�ܵ���Сʱ��ʱÿ���Ӵ���һ�� or ��������Сʱ��ʱÿ���Ӵ���һ��
	RTC_AlarmMask_Seconds				//���ε���				��Ҫ���ڡ�ʱ��ƥ�����������		�ܵ���Сʱ���ӵ�ʱÿ�봥��һ�� or ��������Сʱ���ӵ�ʱÿ�봥��һ��
	RTC_AlarmMask_All						//�����κ�				������������ʱ���з����������������		ÿ�뱨��
**************************************************/
void RTC_AlarmB(u8 B_Day,u8 B_Unit,u8 B_Hours,u8 B_Minutes,u8 B_Seconds,u8 B_Alarm_CE)
{
	u8 i = 0;							//2��������������Ϊ�����ر�
	
	RTC_AlarmTypeDef B;		//����B
	
	RTC_AlarmCmd(RTC_Alarm_B,DISABLE);			//����B�ر�

	//�Ƿ�رձ���
	if(B_Alarm_CE == 0)			
	{
		RTC_ITConfig(RTC_IT_ALRB,DISABLE);			//����B�жϹر�
		RTC_AlarmCmd(RTC_Alarm_B,DISABLE);			//����B�ر�;
		return;
	}
	
	//���ñ���B
	B.RTC_AlarmDateWeekDay = B_Day;					//��������
	if(B_Unit == 0)													//����Ϊ���ڻ�������
		B.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;				//ѡ������
	else
		B.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_WeekDay;		//ѡ����
	
	//������������������Ϊ������������
	if(B_Day > 31)			i++;
	if(B_Hours > 23)		i++;
	if(B_Minutes > 59)	i++;
	if(B_Seconds > 59)	i++;
	if(i > 1)
		B.RTC_AlarmMask = RTC_AlarmMask_All;			//�����κ����� 				ÿ����������
	else
	{
		B.RTC_AlarmMask = RTC_AlarmMask_None;			//���ò����κ�����		һ�»�һ����������
		
		if(B_Day > 31)					//���ε��� or ����
			B.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;						
		else if(B_Hours > 23)		//���ε�Сʱ
			B.RTC_AlarmMask = RTC_AlarmMask_Hours;									
		else if(B_Minutes > 59)	//���ε���
			B.RTC_AlarmMask = RTC_AlarmMask_Minutes;									
		else if(B_Seconds > 59)	//���ε���
			B.RTC_AlarmMask = RTC_AlarmMask_Seconds;	
	}
	
	//B.RTC_AlarmTime.RTC_H12 = RTC_H12_PM;								//����	24Сʱ�Ʋ���Ҫ��
	B.RTC_AlarmTime.RTC_Hours = B_Hours;									//ʱ
	B.RTC_AlarmTime.RTC_Minutes = B_Minutes;							//��
	B.RTC_AlarmTime.RTC_Seconds = B_Seconds;							//��
	RTC_SetAlarm(RTC_Format_BIN,RTC_Alarm_B,&B);					//���ñ���
	
	//���������־
	RTC_ClearFlag(RTC_FLAG_ALRAF);				//�������A��־
	EXTI_ClearITPendingBit(EXTI_Line17);	//����ж�17�߱�־
	
	RTC_ITConfig(RTC_IT_ALRB,ENABLE);			//����B�жϿ���
	RTC_AlarmCmd(RTC_Alarm_B,ENABLE);			//����B����
}

/**************************************************
**����ԭ��: void RTC_TamperExtiNivc(void)
**����:		RTC��Ԥ�ж�����
**�������:	��
**���ز���:	��
**˵    ������δʵ��
**************************************************/
void RTC_TamperExtiNivc(void)
{
	EXTI_InitTypeDef E;		//�ж���
	NVIC_InitTypeDef N;		//�ж�
	
	E.EXTI_Line = EXTI_Line21;									//�ж���
	E.EXTI_Mode = EXTI_Mode_Interrupt;					//�ж�
	E.EXTI_Trigger = EXTI_Trigger_Rising;				//�������ж�
	E.EXTI_LineCmd = ENABLE;										//�洢���õ��Ĵ���
	EXTI_Init(&E);
	
	N.NVIC_IRQChannel = TAMP_STAMP_IRQn;				//��Ԥ�ж�
	N.NVIC_IRQChannelPreemptionPriority = 2;		//���ȼ�
	N.NVIC_IRQChannelSubPriority = 0;						//��Ӧ��
	N.NVIC_IRQChannelCmd = ENABLE;							//�����жϼ���
	NVIC_Init(&N);															//�����������ó�ʼ���жϿ�����	
}

/****************************************************************************
* ��	�ƣ�void GetTime(Time_Body* pTime)
* ��	�ܣ���ȡʱ��
* ��ڲ�����Time_Body* pTime		ʱ��
* ���ڲ�������
* ˵	����24Сʱ�������ж�������
****************************************************************************/
void GetTime(Time_Body* pTime)
{
	RTC_TimeTypeDef RTC_Time;			//ʱ����
	RTC_DateTypeDef RTC_Date;			//��������
	
	SysIntEnter();
	RTC_GetTime(RTC_Format_BIN, &RTC_Time);		//��ȡʱ��		//BIN ���õ�ʱ��Ϊʮ����	BCD ���õ�ʱ��Ϊʮ������
  RTC_GetDate(RTC_Format_BIN, &RTC_Date);		//��ȡ����		//BIN ���õ�ʱ��Ϊʮ����	BCD ���õ�ʱ��Ϊʮ������
	
	pTime->HH = RTC_Time.RTC_Hours;					//Сʱ
	pTime->MM = RTC_Time.RTC_Minutes;				//��
	pTime->SS = RTC_Time.RTC_Seconds;				//��
	
// 	if(RTC_Time.RTC_H12 == RTC_H12_AM)			//���绹������	24Сʱ�������ж�
// 		pTime->AMorPM = 0;
// 	else
// 		pTime->AMorPM = 1;	
	
	pTime->Year = RTC_Date.RTC_Year + 2000;	//��
	pTime->Month = RTC_Date.RTC_Month;			//��
	pTime->Day = RTC_Date.RTC_Date;					//��
	pTime->Week = RTC_Date.RTC_WeekDay;			//��	
	SysIntExit();
}

/****************************************************************************
* ��	�ƣ�void ShowTime(void)
* ��	�ܣ���ʾʱ��
* ��ڲ�������
* ���ڲ�������
* ˵	��������Ϊ7
****************************************************************************/
void ShowTime(void)
{
	Time_Body thistime = {0,0,0,0,0,0,0};		//ʱ��
	
	SysIntEnter();
	GetTime(&thistime);	//��ȡϵͳʱ��		
	printf("%d-%d-%d   %d:%d:%d   ����%d\r\n",thistime.Year,		//��
	thistime.Month,			//��
	thistime.Day,				//��
	thistime.HH,				//ʱ
	thistime.MM,				//��
	thistime.SS,				//��
	thistime.Week);			//��
	SysIntExit();
}

/****************************************************************************
* ��	�ƣ�void Alarm_A_Caller(void(*Alarm_fun)(void))
* ��	�ܣ�����A��Ӧ�ص�����
* ��ڲ�������Ӧ����
* ���ڲ�������
* ˵	������ִ�к���ָ��ָ��ִ�к���
****************************************************************************/
void Alarm_A_Caller(void(*Alarm_fun)(void))
{
	A_Alarm_fun = Alarm_fun;	//ִ�к���ָ��ָ��ִ�к���
}

/****************************************************************************
* ��	�ƣ�void Alarm_B_Caller(void(*Alarm_fun)(void))
* ��	�ܣ�����B��Ӧ�ص�����
* ��ڲ�������Ӧ����
* ���ڲ�������
* ˵	������ִ�к���ָ��ָ��ִ�к���
****************************************************************************/
void Alarm_B_Caller(void(*Alarm_fun)(void))
{
	B_Alarm_fun = Alarm_fun;	//ִ�к���ָ��ָ��ִ�к���
}

/****************************************************************************
* ��	�ƣ�void RTC_Alarm_IRQHandler(void)
* ��	�ܣ������ж�
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void RTC_Alarm_IRQHandler(void)
{
	SysIntEnter();
	
	//����A����
	if(RTC_GetITStatus(RTC_IT_ALRA) != RESET)		
	{
		if(A_Alarm_fun == 0)								//���ִ�к���û��ָ��
			printf("RTX ALRA!!\r\n");					//��ӡ��ʾ��Ϣ
		else
			A_Alarm_fun();										//���ִ�к�����ָ��  ��ִ��ִ�к���
		
		RTC_ClearITPendingBit(RTC_IT_ALRA);		//���������־
		EXTI_ClearITPendingBit(EXTI_Line17);	//����ж��߱�־
	}
	
	//����B����
	if(RTC_GetITStatus(RTC_IT_ALRB) != RESET)		
	{
		if(B_Alarm_fun == 0)								//���ִ�к���û��ָ��
			printf("RTX ALRB!!\r\n");					//��ӡ��ʾ��Ϣ
		else
			B_Alarm_fun();										//���ִ�к�����ָ��  ��ִ��ִ�к���
		
		RTC_ClearITPendingBit(RTC_IT_ALRB);		//���������־
		EXTI_ClearITPendingBit(EXTI_Line17);	//����ж��߱�־
	}
	
	SysIntExit();
}

/****************************************************************************
* ��	�ƣ�static s8 RTC_Open(u32 lParam)
* ��	�ܣ�ͳһ�ӿ�
* ��ڲ�����u32 lParam		��������
* ���ڲ�����u8						�򿪲�������ֵ	��HVL_ERR_CODE��
* ˵	������
****************************************************************************/
static s8 RTC_Open(u32 lParam)
{
	return 0;					
}

/****************************************************************************
* ��	�ƣ�static s8 RTC_Close(void)
* ��	�ܣ�ͳһ�ӿ�
* ��ڲ�������
* ���ڲ�����s8						�رղ�������ֵ	��HVL_ERR_CODE��
* ˵	������
****************************************************************************/
static s8 RTC_Close(void)
{
	return 0;		
}

/****************************************************************************
* ��	�ƣ�static s32 RTC_Read(u8* buffer,u32 len,u32* Reallen,u32 lParam) 
* ��	�ܣ��豸��
* ��ڲ�����u8* buffer			ʱ��ֵ		Time_Body�ṹ��
						u32 len				ϣ�����յĳ���
						u32* Reallen	ʵ�ʽ��յĳ���
						u32 lParam		����
* ���ڲ�����s32						�����Ƿ�ɹ�		0�ɹ�	1ʧ��
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
	u8 AMorPM;		//����or����	0����	1����			//24Сʱ�Ʋ�ʹ��
} Time_Body;
****************************************************************************/
static s32 RTC_Read(u8* buffer,u32 len,u32* Reallen,u32 lParam)
{
	SysSchedLock();															//������
	GetTime((Time_Body*)buffer);	//��ȡʱ��
	SysSchedUnlock();														//��������	
	return 1;
}

/****************************************************************************
* ��	�ƣ�static s32 RTC_Write(u8* buffer,u32 len,u32* Reallen,u32 lParam) 
* ��	�ܣ����ڶ�
* ��ڲ�����u8* buffer			ʱ��ֵ		Time_Body�ṹ��
						u32 len				ϣ��д��ĳ���
						u32* Reallen	ʵ��д��ĳ���
						u32 lParam		����
* ���ڲ�����s32						�����Ƿ�ɹ�		0�ɹ�	1ʧ��
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
	u8 AMorPM;		//����or����	0����	1����			//24Сʱ�Ʋ�ʹ��
} Time_Body;
****************************************************************************/
static s32 RTC_Write(u8* buffer,u32 len,u32* Reallen,u32 lParam) 
{
	SysSchedLock();															//������
	
	if(SetRTC_Time(((Time_Body*)buffer)->HH,		//Сʱ
		((Time_Body*)buffer)->MM,									//����
		((Time_Body*)buffer)->SS) == ERROR)				//��
	{
		SysSchedUnlock();													//��������
		return 0;
	}
	
	if(SetRTC_Date(((Time_Body*)buffer)->Year,	//��
		((Time_Body*)buffer)->Month,							//��
		((Time_Body*)buffer)->Day,								//��
		((Time_Body*)buffer)->Week) == ERROR)			//��
	{
		SysSchedUnlock();													//��������
		return 0;
	}	
	
	SysSchedUnlock();														//��������
	return 1;	
}

/****************************************************************************
* ��	�ƣ�static s32 RTC_Ioctl(u32 cmd,u32 lParam)
* ��	�ܣ����ڿ���
* ��ڲ�����u32 cmd				����
						u32 lParam		����
* ���ڲ�����s32						��������Ĳ�ͬ���ص�ֵ���岻ͬ
* ˵	����	��
****************************************************************************/
static s32 RTC_Ioctl(u32 cmd,u32 lParam)
{
	switch(cmd)
	{
		case (u32)RTCCMD_SetTime:											//����ʱ��
		{
			SysSchedLock();															//������
			if(SetRTC_Time(((Time_Body*)lParam)->HH,		//Сʱ
				((Time_Body*)lParam)->MM,									//����
				((Time_Body*)lParam)->SS) == ERROR)				//��
			{
				SysSchedUnlock();														//��������
				return 0;
			}
			
			SysSchedUnlock();														//��������
			return 1;
		}
		case (u32)RTCCMD_SetDate:											//��������
		{
			SysSchedLock();															//������
			if(SetRTC_Date(((Time_Body*)lParam)->Year,	//��
				((Time_Body*)lParam)->Month,							//��
				((Time_Body*)lParam)->Day,								//��
				((Time_Body*)lParam)->Week) == ERROR)			//��
			{
				SysSchedUnlock();													//��������
				return 0;
			}
			
			SysSchedUnlock();														//��������
			return 1;
		}
		case (u32)RTCCMD_LookTimeDate:								//��ѯʱ������
		{
			SysSchedLock();															//������
			GetTime((Time_Body*)lParam);	
			SysSchedUnlock();														//��������
			return 1;
		}
		case (u32)RTCCMD_ShowTimeDate:								//��ʾ����	���ڴ�ӡ��ʾ
		{
			SysSchedLock();															//������
			ShowTime();																	//��ȡ����ʾʱ��
			SysSchedUnlock();														//��������
			return 1;
		}
		case (u32)RTCCMD_SetAlarm_A:							//��������A
		{
			SysSchedLock();																		//������
			RTC_AlarmA(((ALARM_Cfg*)lParam)->ALARM_Cfg_Day,		//��or��
			((ALARM_Cfg*)lParam)->ALARM_Cfg_Unit,							//��λ 0�� 1��
			((ALARM_Cfg*)lParam)->ALARM_Cfg_Hours,						//Сʱ
			((ALARM_Cfg*)lParam)->ALARM_Cfg_Minutes,					//����
			((ALARM_Cfg*)lParam)->ALARM_Cfg_Seconds,					//��
			((ALARM_Cfg*)lParam)->ALARM_Cfg_Alarm_CE);				//���� 1��0��
			SysSchedUnlock();																	//��������
			return 1;
		}
		case (u32)RTCCMD_SetAlarm_B:							//��������B
		{
			SysSchedLock();																		//������
			RTC_AlarmB(((ALARM_Cfg*)lParam)->ALARM_Cfg_Day,		//��or��
			((ALARM_Cfg*)lParam)->ALARM_Cfg_Unit,							//��λ 0�� 1��
			((ALARM_Cfg*)lParam)->ALARM_Cfg_Hours,						//Сʱ
			((ALARM_Cfg*)lParam)->ALARM_Cfg_Minutes,					//����
			((ALARM_Cfg*)lParam)->ALARM_Cfg_Seconds,					//��
			((ALARM_Cfg*)lParam)->ALARM_Cfg_Alarm_CE);				//���� 1��0��
			SysSchedUnlock();																	//��������
			return 1;
		}
		case (u32)RTCCMD_SetAlarm_A_FUN:					//��������Aִ�к���
		{
			Alarm_A_Caller((Alarm_fun)lParam);			//ִ�к���
			return 1;
		}
		case (u32)RTCCMD_SetAlarm_B_FUN:					//��������Bִ�к���
		{
			Alarm_B_Caller((Alarm_fun)lParam);			//ִ�к���
			return 1;
		}
	}
	return 0;		//�������
}

/**************************************************
**����ԭ��: u8 RTC_init(void)
**����:		RTC��ʼ��
**�������:	��
**���ز���:	u8	��ʼ���Ƿ�ɹ�	1�ɹ� 0ʧ��
**˵    ������
**************************************************/
u8 RTCInit(void)
{
	//ע���ñ���
	DEV_REG rtc = 			//�豸ע����Ϣ��						����static		
	{
		CHAR_UNDEF1,						//�豸ID��
		0,  								//�Ƿ���			0:���ɹ���ʹ��, 1:�ɹ���ʹ��
		0, 									//���ڹ����豸���򿪴���
		1,									//��������
		1,									//���д����
		RTC_pDisc,					//��������			�Զ���
		20120001,						//�����汾			�Զ���
		(u32*)RTC_Open,			//�豸�򿪺���ָ��
		(u32*)RTC_Close, 		//�豸�رպ���ָ��
		(u32*)RTC_Read,			//�ַ�������
		(u32*)RTC_Write,		//�ַ�д����
		(u32*)RTC_Ioctl			//���ƺ���
	};

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);		//��PWRʱ��	
  PWR_BackupAccessCmd(ENABLE);													//��RTC�󱸼Ĵ���ͨ��

  if(RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x1004)			//�󱸼Ĵ������
	{  
    RTC_Config();
		if(SetRTC_Init() == ERROR)							//��ʼ��RTC
			return 0;
    if(SetRTC_Time(15,45,30) == ERROR)			//����ʱ��
			return 0; 			
    if(SetRTC_Date(2012,10,4,4) == ERROR) 	//��������
			return 0;
		
		RTC_WriteBackupRegister(RTC_BKP_DR0, 0x1004);				//д�󱸼Ĵ����������
  }
  else
  {
    if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)			//�ϴ�Ϊ ��Դ�رո�λ
			;//  DebugPf("POWER Is Close\r\n");
		
    else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)	//�ϴ�Ϊ RESET��λ
			;//  DebugPf("RESET System\r\n");

    RCC_ClearFlag();								//���RCC�ĸ�λ��־���Ա���һ�ζ�RCC_FLAG_PORRST��Դ��λRCC_FLAG_PINRST���Ÿ�λ���ж�
		
		RCC_LSEConfig(RCC_LSE_ON);					//����LSE��������
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);	//�ȴ�ʼ���ȶ�
		
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);							//ѡ��ʱ����Դ
		RCC_RTCCLKCmd(ENABLE);					//RTC����	��������RCC_RTCCLKConfig֮��
		
    RTC_WaitForSynchro();						//�ȴ�APB1����ʱ����RTC����ʱ��ͬ�������ܶ�д�Ĵ���
  }
	
	RTC_AlarmExtiNivc();		//RTC�����ж�����
	//RTC_TamperExtiNivc();		//RTC��Ԥ�ж�����		��δʵ��
	
	if(DeviceInstall(&rtc) != HVL_NO_ERR)			//�豸ע��
		return 0;
	
	return 1;
}
