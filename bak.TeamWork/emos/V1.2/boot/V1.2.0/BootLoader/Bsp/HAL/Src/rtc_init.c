#include "rtc_init.h"
#include "printf_init.h"		//DebugPf

//δʵ��WakeUP���ѹ��ܡ�Tamper���ܡ�Time stamp event����

typedef void (*Alarm_fun)(void); 			//����ָ��
static Alarm_fun A_Alarm_fun = 0;			//����A��Ӧִ�к���ָ��
static Alarm_fun B_Alarm_fun = 0;			//����B��Ӧִ�к���ָ��

/****************************************************************************
* ��	�ƣ�void RTCInit(void)
* ��	�ܣ���ʼ��RTCʱ��
* ��ڲ�������
* ���ڲ�������
* ˵	������ 
****************************************************************************/
void RTCInit(void)
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
* ��	�ƣ�ErrorStatus SetRTC_Time(const u8 hour,const u8 min,const u8 sec)
* ��	�ܣ�����ʱ��
* ��ڲ�����	const u8 hour		ʱ
						const u8 min		��
						const u8 sec		��
* ���ڲ�����ErrorStatus		ERROR or SUCCESS
* ˵	���������24Сʱ�ƹ�������RTC_H12����
					SetRTC_Time(23,59,58);		//23:59:58
****************************************************************************/
ErrorStatus SetRTC_Time(const u8 hour,const u8 min,const u8 sec)
{
	RTC_TimeTypeDef RTC_TimeTemp;			//ʱ����
	
	RTC_TimeTemp.RTC_Hours = hour;					//ʱ
	RTC_TimeTemp.RTC_Minutes = min;					//��
	RTC_TimeTemp.RTC_Seconds = sec;					//��
    
	return RTC_SetTime(RTC_Format_BIN, &RTC_TimeTemp);		//BIN ���õ�ʱ��Ϊʮ����	BCD ���õ�ʱ��Ϊʮ������
}

/****************************************************************************
* ��	�ƣ�ErrorStatus SetRTC_Date(const u16 year,const u8 month,const u8 day,const u8 week)
* ��	�ܣ���������
* ��ڲ�����	const u16 year	��
						const u8 month	��
						const u8 day		��
						const u8 week		��
* ���ڲ�����ErrorStatus		ERROR or SUCCESS
* ˵	����SetRTC_Date(2012,10,4,4);			//2012-10-4 ����
****************************************************************************/
ErrorStatus SetRTC_Date(const u16 year,const u8 month,const u8 day,const u8 week)
{
	RTC_DateTypeDef RTC_DateTemp;			//��������
	
	RTC_DateTemp.RTC_Year  = year < 2000 ? (u8)year : (u8)(year % 100);	//��
	RTC_DateTemp.RTC_Month = month;			//��
	RTC_DateTemp.RTC_Date  = day;				//��
	RTC_DateTemp.RTC_WeekDay  = week;		//��
	
	return RTC_SetDate(RTC_Format_BIN, &RTC_DateTemp);		//BIN ���õ�ʱ��Ϊʮ����	BCD ���õ�ʱ��Ϊʮ������
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
**����ԭ��: void RTC_AlarmA(const u8 A_Day,const u8 A_Unit,const u8 A_Hours,const u8 A_Minutes,const u8 A_Seconds,const u8 A_Alarm_CE,void (*Alarm_fun)(void))
**����:		RTC����A����
**�������:	const u8 A_Day				���ںŻ��ܺ�								// > 31����
						const u8 A_Unit				���� or ��			0���� 1��
						const u8 A_Hours			Сʱ												// > 23����
						const u8 A_Minutes		����												// > 59����
						const u8 A_Seconds		��													// > 59����
						const u8 A_Alarm_CE		��������				0�رձ���	1��������
						void (*Alarm_fun)(void)					����ִ�г���
**���ز���:	��
**˵    ����
	RTC_AlarmMask_DateWeekDay		//���ε��ܺ���		��Ҫʱ����ƥ����������					ÿ�촥��һ��
	RTC_AlarmMask_None					//�����κ�����		��Ҫ���ڡ�ʱ����ƥ�����������	�ܵ��촥��һ�� or �������촥��һ��
	RTC_AlarmMask_Hours					//���ε�Сʱ			��Ҫ���ڡ�����ƥ�����������		�ܵ���ÿСʱ����һ�� or ��������ÿСʱ����һ��
	RTC_AlarmMask_Minutes				//���ε�����			��Ҫ���ڡ�ʱ��ƥ�����������		�ܵ���Сʱ��ʱÿ���Ӵ���һ�� or ��������Сʱ��ʱÿ���Ӵ���һ��
	RTC_AlarmMask_Seconds				//���ε���				��Ҫ���ڡ�ʱ��ƥ�����������		�ܵ���Сʱ���ӵ�ʱÿ�봥��һ�� or ��������Сʱ���ӵ�ʱÿ�봥��һ��
	RTC_AlarmMask_All						//�����κ�				������������ʱ���з����������������		ÿ�뱨��
**************************************************/
void RTC_AlarmA(const u8 A_Day,const u8 A_Unit,const u8 A_Hours,const u8 A_Minutes,const u8 A_Seconds,const u8 A_Alarm_CE,void (*Alarm_fun)(void))
{
	u8 i = 0;							//2��������������Ϊ�����ر�
	
	RTC_AlarmTypeDef A;		//����A
	
	RTC_AlarmCmd(RTC_Alarm_A,DISABLE);				//����A�ر�

	//�Ƿ�رձ���
	if(A_Alarm_CE == 0)			
	{
		RTC_ITConfig(RTC_IT_ALRA,DISABLE);			//����A�жϹر�
		RTC_AlarmCmd(RTC_Alarm_A,DISABLE);			//����A�ر�;
		return;
	}
	
	if(Alarm_fun != 0)
		A_Alarm_fun = Alarm_fun;								//ִ�к���ָ��ָ��ִ�к���
	
	//���ñ���A
	A.RTC_AlarmDateWeekDay = A_Day;					//��������
	
	A.RTC_AlarmDateWeekDaySel = A_Unit == 0 ? RTC_AlarmDateWeekDaySel_Date : RTC_AlarmDateWeekDaySel_WeekDay;
	
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
**����ԭ��: void RTC_AlarmB(const u8 B_Day,const u8 B_Unit,const u8 B_Hours,const u8 B_Minutes,const u8 B_Seconds,const u8 B_Alarm_CE,void (*Alarm_fun)(void))
**����:		RTC����A����
**�������:	const u8 B_Day				���ںŻ��ܺ�								// > 31����
						const u8 B_Unit				���� or ��			0���� 1��
						const u8 B_Hours			Сʱ												// > 23����
						const u8 B_Minutes		����												// > 59����
						const u8 B_Seconds		��													// > 59����
						const u8 B_Alarm_CE			��������				0�رձ���	1��������
						void (*Alarm_fun)(void)											//��������
**���ز���:	��
**˵    ����
	RTC_AlarmMask_DateWeekDay		//���ε��ܺ���		��Ҫʱ����ƥ����������					ÿ�촥��һ��
	RTC_AlarmMask_None					//�����κ�����		��Ҫ���ڡ�ʱ����ƥ�����������	�ܵ��촥��һ�� or �������촥��һ��
	RTC_AlarmMask_Hours					//���ε�Сʱ			��Ҫ���ڡ�����ƥ�����������		�ܵ���ÿСʱ����һ�� or ��������ÿСʱ����һ��
	RTC_AlarmMask_Minutes				//���ε�����			��Ҫ���ڡ�ʱ��ƥ�����������		�ܵ���Сʱ��ʱÿ���Ӵ���һ�� or ��������Сʱ��ʱÿ���Ӵ���һ��
	RTC_AlarmMask_Seconds				//���ε���				��Ҫ���ڡ�ʱ��ƥ�����������		�ܵ���Сʱ���ӵ�ʱÿ�봥��һ�� or ��������Сʱ���ӵ�ʱÿ�봥��һ��
	RTC_AlarmMask_All						//�����κ�				������������ʱ���з����������������		ÿ�뱨��
**************************************************/
void RTC_AlarmB(const u8 B_Day,const u8 B_Unit,const u8 B_Hours,const u8 B_Minutes,const u8 B_Seconds,const u8 B_Alarm_CE,void (*Alarm_fun)(void))
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
	
	if(Alarm_fun != 0)
		B_Alarm_fun = Alarm_fun;								//ִ�к���ָ��ָ��ִ�к���
	
	//���ñ���B
	B.RTC_AlarmDateWeekDay = B_Day;						//��������
	
	B.RTC_AlarmDateWeekDaySel = B_Unit == 0 ? RTC_AlarmDateWeekDaySel_Date : RTC_AlarmDateWeekDaySel_WeekDay;
	
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

/**************************************************
**����ԭ��: u8 RTC_Config(const u16 year,const u8 month,const u8 day,const u8 week,const u8 hour,const u8 min,const u8 sec)
**����:		RTC����
**�������:	const u16 year	��
						const u8 month	��
						const u8 day		��
						const u8 week		��
						const u8 hour		ʱ
						const u8 min		��
						const u8 sec		��
**���ز���:	u8 �Ƿ�ɹ�		1�ɹ� 0ʧ��
**˵    ������
**************************************************/
u8 RTC_Config(const u16 year,const u8 month,const u8 day,const u8 week,const u8 hour,const u8 min,const u8 sec)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);		//��PWRʱ��	
  PWR_BackupAccessCmd(ENABLE);													//��RTC�󱸼Ĵ���ͨ��

  if(RTC_ReadBackupRegister(RTC_BKP_DR0) != 0xA5A5)			//�󱸼Ĵ������
	{  
    RTCInit();
		if(SetRTC_Init() == ERROR)													//��ʼ��RTC
			return 0;
    if(SetRTC_Time(hour,min,sec) == ERROR)							//����ʱ��
			return 0; 			
    if(SetRTC_Date(year,month,day,week) == ERROR) 			//��������
			return 0;
		
		RTC_WriteBackupRegister(RTC_BKP_DR0, 0xA5A5);				//д�󱸼Ĵ����������
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
	
	return 1;																								//�ɹ�
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
}

/****************************************************************************
* ��	�ƣ�void ShowTime(void)
* ��	�ܣ���ʾʱ��
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void ShowTime(void)
{
	Time_Body thistime = {0,0,0,0,0,0,0};		//ʱ��
	
	GetTime(&thistime);	//��ȡϵͳʱ��		
	printf("%d-%d-%d   %d:%d:%d   ����%d\r\n",thistime.Year,		//��
	thistime.Month,			//��
	thistime.Day,				//��
	thistime.HH,				//ʱ
	thistime.MM,				//��
	thistime.SS,				//��
	thistime.Week);			//��
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
}
