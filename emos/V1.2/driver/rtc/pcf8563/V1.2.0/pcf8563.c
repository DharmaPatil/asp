#include "pcf8563.h"
#include "I2Cx.h"
#include "driver.h"
#include "system_init.h"
#include "stdio.h"

#define ColReg1		0x00					//����״̬�Ĵ���1
#define ColReg2		0x01					//����״̬�Ĵ���2
#define SSReg			0x02					//��Ĵ���
#define MMReg			0x03					//�ּĴ���
#define HHReg			0x04					//Сʱ�Ĵ���
#define DayReg		0x05					//���ڼĴ���
#define WeekReg		0x06					//���ڼĴ���
#define MCReg			0x07					//�º����ͼĴ���
#define YearReg		0x08					//��Ĵ���

#define ValidL7		0x7F					//��7λ��Ч
#define ValidL6		0x3F					//��6λ��Ч
#define ValidL3		0x07					//��3λ��Ч
#define ValidL5		0x1F					//��5λ��Ч

static u8 OutRtc_pDisc[] = "LuoHuaiXiang_OutRtc\r\n";				//�豸�����ַ�					����static

typedef struct
{
	u8				DeviceAddr;						//������ַ
	I2C_Cfg		I2Cx;									//I2C����
}OutRtc_Cfg;											//OutRTC����

static OutRtc_Cfg OutRtcReal = 			//�ⲿRTC�豸
{
	0xA2,				//�ⲿRTC��ַ
	{
		0,
		GPIO_Pin_6,		//SCK
		GPIOB,
		GPIO_Pin_7,		//SDA
		GPIOB
	}
};

/****************************************************************************
* ��	�ƣ�u8 OutRTC_Set(OutTime_Body* OutTime_t)
* ��	�ܣ��ⲿRTC��ʼ��
* ��ڲ�����OutTime_Body* OutTime_t			ʱ��ṹ��
* ���ڲ�����u8													�Ƿ�ɹ�		1�ɹ�	0ʧ��
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
u8 OutRTC_Set(OutTime_Body* OutTime_t)
{
	u8 Temp = 0;
	
	Temp = BINToBCD(OutTime_t->SS);			//������
	if(I2CSendByteADDRData(&OutRtcReal.I2Cx,OutRtcReal.DeviceAddr,SSReg,&Temp,1) != 1) 
		return 0;			
	Temp = BINToBCD(OutTime_t->MM);			//���÷�
	if(I2CSendByteADDRData(&OutRtcReal.I2Cx,OutRtcReal.DeviceAddr,MMReg,&Temp,1) != 1) 
		return 0; 	
	Temp = BINToBCD(OutTime_t->HH);			//����Сʱ
	if(I2CSendByteADDRData(&OutRtcReal.I2Cx,OutRtcReal.DeviceAddr,HHReg,&Temp,1) != 1) 
		return 0;
	Temp = BINToBCD(OutTime_t->Day);		//������
	if(I2CSendByteADDRData(&OutRtcReal.I2Cx,OutRtcReal.DeviceAddr,DayReg,&Temp,1) != 1)	
		return 0;
	Temp = BINToBCD(OutTime_t->Week);		//������
	if(I2CSendByteADDRData(&OutRtcReal.I2Cx,OutRtcReal.DeviceAddr,WeekReg,&Temp,1) != 1) 
		return 0;
	if((OutTime_t->Year) < 2000)
		Temp = BINToBCD(1);								//��������	19xx
	else
		Temp = BINToBCD(0);								//��������	20xx
	Temp = Temp << 7;
	Temp += BINToBCD(OutTime_t->Month);	//������
	if(I2CSendByteADDRData(&OutRtcReal.I2Cx,OutRtcReal.DeviceAddr,MCReg,&Temp,1) != 1) 
		return 0; 
	Temp = BINToBCD(OutTime_t->Year % 100);				//������
	if(I2CSendByteADDRData(&OutRtcReal.I2Cx,OutRtcReal.DeviceAddr,YearReg,&Temp,1) != 1) 
		return 0; 
	
	return 1;
}

/****************************************************************************
* ��	�ƣ�u8 OutRTC_Get(OutTime_Body* OutTime_t)
* ��	�ܣ��ⲿRTC��ʼ��
* ��ڲ�����OutTime_Body* OutTime_t			ʱ��ṹ��
* ���ڲ�����u8													�Ƿ�ɹ�		1�ɹ�	0ʧ��
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
u8 OutRTC_Get(OutTime_Body* OutTime_t)
{
	u8 Temp = 0;
	
	if(I2CReceiveByteADDRData(OutRtcReal.DeviceAddr,SSReg,&Temp,1) != 1)	
		return 0;	
	OutTime_t->SS = BCDToBIN(Temp& ValidL7);			//�����
	Temp = 0;
	
	if(I2CReceiveByteADDRData(OutRtcReal.DeviceAddr,MMReg,&Temp,1) != 1)	
		return 0;	
	OutTime_t->MM = BCDToBIN(Temp & ValidL7);			//��÷�
	Temp = 0;
	
	if(I2CReceiveByteADDRData(OutRtcReal.DeviceAddr,HHReg,&Temp,1) != 1)	
		return 0;	
	OutTime_t->HH = BCDToBIN(Temp & ValidL6);			//���Сʱ
	Temp = 0;
	
	if(I2CReceiveByteADDRData(OutRtcReal.DeviceAddr,DayReg,&Temp,1) != 1)	
		return 0;	
	OutTime_t->Day = BCDToBIN(Temp & ValidL6);		//�����
	Temp = 0;
	
	if(I2CReceiveByteADDRData(OutRtcReal.DeviceAddr,WeekReg,&Temp,1) != 1)	
		return 0;	
	OutTime_t->Week = BCDToBIN(Temp & ValidL3);		//�����		��7 = 0
	Temp = 0;
	
	if(I2CReceiveByteADDRData(OutRtcReal.DeviceAddr,MCReg,&Temp,1) != 1)	
		return 0;
	OutTime_t->Month = BCDToBIN(Temp & ValidL5);		//�����
	
	if((Temp >> 7) > 0)									//�������
		OutTime_t->Year = 1900;		
	else 
		OutTime_t->Year = 2000;
	Temp = 0;
	
	if(I2CReceiveByteADDRData(OutRtcReal.DeviceAddr,YearReg,&Temp,1) != 1)	
		return 0;													//�����
	OutTime_t->Year += BCDToBIN(Temp);	//�긳ֵ
	
	return 1;														//��ȡ�ɹ�
}


/****************************************************************************
* ��	�ƣ�u8 OutRTC_Show(void)
* ��	�ܣ���ʾʱ��
* ��ڲ�������
* ���ڲ�����u8 ��ȡ��ʾʱ���Ƿ�ɹ�		0ʧ��	1�ɹ�
* ˵	������ȡ����ʾ ������
****************************************************************************/
u8 OutRTC_Show(void)
{
	OutTime_Body TimeTemp = {0};			//��ȡ��ʱ��
	
	if(OutRTC_Get(&TimeTemp) == 0)		//��ȡʱ��
		return 0;		//ʧ��
	
	printf("%d-%d-%d %d:%d:%d %d\r\n",TimeTemp.Year,		//��
																	TimeTemp.Month,			//��
																	TimeTemp.Day,				//��
																	TimeTemp.HH,				//ʱ
																	TimeTemp.MM,				//��
																	TimeTemp.SS,				//��
																	TimeTemp.Week);			//�� 0-6
	
	return 1;			//�ɹ�
}

/****************************************************************************
* ��	�ƣ�static s8 OUTRTC_Open(u32 lParam)
* ��	�ܣ�ͳһ�ӿ�
* ��ڲ�����u32 lParam		��������
* ���ڲ�����u8						�򿪲�������ֵ	��HVL_ERR_CODE��
* ˵	������
****************************************************************************/
static s8 OUTRTC_Open(u32 lParam)
{
	return 0;	
}

/****************************************************************************
* ��	�ƣ�static s8 OUTRTC_Close(u32 lParam)
* ��	�ܣ�ͳһ�ӿ�
* ��ڲ�����u32 lParam		��������
* ���ڲ�����u8						�򿪲�������ֵ	��HVL_ERR_CODE��
* ˵	������
****************************************************************************/
static s8 OUTRTC_Close(u32 lParam)
{
	return 0;
}

/****************************************************************************
* ��	�ƣ�static s32 OUTRTC_Read(u8* buffer,u32 len,u32* Reallen,u32 lParam) 
* ��	�ܣ���
* ��ڲ�����u8* buffer		���ջ�����			����
						u32 len				ϣ�����յĳ���	����
						u32* Reallen	ʵ�ʽ��յĳ���	����
						u32 lParam		����						����
* ���ڲ�����s32						�����Ƿ�ɹ�		0�ɹ�	1ʧ��
* ˵	������
****************************************************************************/
static s32 OUTRTC_Read(u8* buffer,u32 len,u32* Reallen,u32 lParam) 
{
	s32 i = 0;
	
	SysSchedLock();											//������
	i = OutRTC_Get((OutTime_Body*)lParam);		
	SysSchedUnlock();										//��������	
	
	return i;
}

/****************************************************************************
* ��	�ƣ�static s32 OUTRTC_Write(u8* buffer,u32 len,u32* Reallen,u32 lParam) 
* ��	�ܣ���������
* ��ڲ�����u8* buffer		���ջ�����			����
						u32 len				ϣ�����յĳ���	����
						u32* Reallen	ʵ�ʽ��յĳ���	����
						u32 lParam		����						����
* ���ڲ�����s32						�����Ƿ�ɹ�		0�ɹ�	1ʧ��
* ˵	����
****************************************************************************/
static s32 OUTRTC_Write(u8* buffer,u32 len,u32* Reallen,u32 lParam) 
{
	s32 i = 0;
	
	SysSchedLock();											//������
	i = OutRTC_Set((OutTime_Body*)lParam);		
	SysSchedUnlock();										//��������	
	
	return i;
}

/****************************************************************************
* ��	�ƣ�static s32 OUTRTC_Ioctl(u32 cmd, u32 lParam)
* ��	�ܣ����ڿ���
* ��ڲ�����u32 cmd				����
						u32 lParam		����
* ���ڲ�����s32						��������Ĳ�ͬ���ص�ֵ���岻ͬ
* ˵	����	��
****************************************************************************/
static s32 OUTRTC_Ioctl(u32 cmd, u32 lParam)
{
	s32 i = 0;
	
	switch(cmd)
	{
		case (u32)OUTRTCCMD_SetTime:									//����ʱ��
		{
			SysSchedLock();															//������
			i = OutRTC_Set((OutTime_Body*)lParam);		
			SysSchedUnlock();														//��������	
			return i;
		}
		case (u32)OUTRTCCMD_GetTime:									//��ȡʱ��
		{
			SysSchedLock();															//������
			i = OutRTC_Get((OutTime_Body*)lParam);		
			SysSchedUnlock();														//��������	
			return i;
		}
		case (u32)OUTRTCCMD_ShowTime:									//��ʾʱ��
		{
			SysSchedLock();															//������
			OutRTC_Show();		
			SysSchedUnlock();														//��������	
			return 1;
		}
	}
	return 0;	//��������	
}
/****************************************************************************
* ��	�ƣ�u8 OutRTC_init(OutTime_Body* Time_t)
* ��	�ܣ��ⲿRTC��ʼ��
* ��ڲ�����OutTime_Body* Time_t			���õ�ʱ��	0��ʾ����Ҫʱ������
* ���ڲ�����u8												�Ƿ�ɹ�		1�ɹ�	0ʧ��
* ˵	������
****************************************************************************/
u8 OutRTC_init(OutTime_Body* Time_t)
{
	u8 Val = 0;
	
	//ע���ñ���
	DEV_REG outrtc = 			//�豸ע����Ϣ��						����static		
	{
		CHAR_RTC,						//�豸ID��
		0,  								//�Ƿ���			0:���ɹ���ʹ��, 1:�ɹ���ʹ��
		0, 									//���ڹ����豸���򿪴���
		1,									//��������
		1,									//���д����
		OutRtc_pDisc,				//��������			�Զ���
		20120001,						//�����汾			�Զ���
		(u32*)OUTRTC_Open,		//�豸�򿪺���ָ��
		(u32*)OUTRTC_Close, 	//�豸�رպ���ָ��
		(u32*)OUTRTC_Read,		//�ַ�������
		(u32*)OUTRTC_Write,		//�ַ�д����
		(u32*)OUTRTC_Ioctl		//���ƺ���
	};	
	
	if(I2Cx_Init() == 0)		//I2C��ʼ��
		return 0;
	
	if(I2CSendByteADDRData(&OutRtcReal.I2Cx,OutRtcReal.DeviceAddr,ColReg1,&Val,1) != 1)	//����״̬�Ĵ���1		��ͨģʽ��ʱ�����С���Դ��λʧЧ
		return 0;	
	
	if(I2CSendByteADDRData(&OutRtcReal.I2Cx,OutRtcReal.DeviceAddr,ColReg2,&Val,1) != 1) //����״̬�Ĵ���2		������Ч����ʱ��Ч
		return 0;	
	
	if(DeviceInstall(&outrtc) != HVL_NO_ERR)		//�豸ע��
		return 0;
	
	if(Time_t != (OutTime_Body*)0)							//��Ҫ����ʱ������
	{	
		if(OutRTC_Set(Time_t) == 0) 							//����ʱ��
			return 0;					
	}
	
	return 1;
}


