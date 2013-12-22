#include "includes.h"
#include "sys_config.h"
#include "sys_param.h"
#include "debug_info.h"
#include "app_beep.h"

static SysMod AppMode = SCS_ZC_7;									//Ӧ��ģʽģʽ

static volatile LongCarMod LongCarMode = HandleLongCarMode;			//��������ģʽ
static volatile BOOL TrafficSignalFlag = FALSE;	//���̵Ʊ�־
static volatile BOOL BGFlag = FALSE; 				//��բ��־
static volatile BOOL BeepFlag = FALSE;				//����������
static volatile FollowCarMod FollowMode = FollowAtOnce;
static u16 APPResetCount = 0;						//ϵͳ��λ����

BOOL Set_System_Mode(SysMod mode)
{
	if(mode >= SCS_CNT || mode == SCS_ZC_4)
		mode = SCS_ZC_7;
	AppMode = mode;
	return Param_Write(GET_U32_OFFSET(ParamAppMod), &mode, sizeof(uint32));
}

//��ȡϵͳģʽ
SysMod Get_System_Mode(void)
{
	return AppMode;
}

LongCarMod Get_LongCar_Mode(void)
{
	return LongCarMode;
}

void Set_LongCar_Mode(LongCarMod mode)
{
	LongCarMode = mode;
	Param_Write(GET_U8_OFFSET(ParamLongCarMode), &mode, sizeof(uint8));
}


BOOL Get_TrafficSignal_Flag(void)		//return ture: �к��̵� FALSE: û��
{
	return TrafficSignalFlag;
}

void Set_TrafficSignal_Flag(BOOL type)		//return ture: �к��̵� FALSE: û��
{
	TrafficSignalFlag = type;
	Param_Write(GET_U8_OFFSET(ParamTrafficSignalFlag), &type, sizeof(uint8));
}


BOOL Get_BGFlag(void)
{
	return BGFlag;
}

void Set_BGFlag(BOOL type)
{
	BGFlag = type;
	Param_Write(GET_U8_OFFSET(ParamBGFlag), &type, sizeof(uint8));
}

FollowCarMod Get_FollowCarMode(void)
{
	return FollowMode;
}

void Set_FollowCarMode(char type)
{	
	if((type < 0) || (type > FollowFarPay)) return;

	FollowMode = (FollowCarMod)type;
	Param_Write(GET_U8_OFFSET(ParamFollowCarMode), &type, sizeof(uint8));
}

BOOL Get_Beep_Flag(void)
{
	return BeepFlag;
}

void Set_Beep_Flag(BOOL type)
{
	BeepFlag = type;
	Param_Write(GET_U8_OFFSET(ParamVoicePowerOn), &type, sizeof(uint8));
	Beep_Power(type);
}

unsigned short Get_AppReset_Count(void)
{
	return APPResetCount;
}

void System_Config_Init(void)
{
//	BOOL err = FALSE;

	Param_Interface_Init();
	RDelay(50);

	//����ϵͳ��λ����
	Param_Read(GET_U16_OFFSET(ParamResetCnt), &APPResetCount, sizeof(u16));
	if(APPResetCount == 0xffff) 
		APPResetCount = 0;

	APPResetCount++;
	Param_Write(GET_U16_OFFSET(ParamResetCnt), &APPResetCount, sizeof(u16));
	printf("APPResetCnt = %d\r\n", APPResetCount);
	
	//����ģʽ
	Param_Read(GET_U32_OFFSET(ParamAppMod), &AppMode, sizeof(uint32));
	if(((unsigned int)AppMode > SCS_CNT) || ((unsigned int)AppMode == SCS_ZC_4))
		AppMode = SCS_ZC_7;

	//��������ģʽ
	Param_Read(GET_U8_OFFSET(ParamLongCarMode), (void *)&LongCarMode, sizeof(uint8));
	if((unsigned int)LongCarMode > HandleLongCarMode) LongCarMode = AutoLongCarMode;

	//���̵�����
	Param_Read(GET_U8_OFFSET(ParamTrafficSignalFlag), (void *)&TrafficSignalFlag, sizeof(uint8));
	if((unsigned int)TrafficSignalFlag > 1) TrafficSignalFlag = TRUE;
	

	//��բ����
	Param_Read(GET_U8_OFFSET(ParamBGFlag), (void *)&BGFlag, sizeof(uint8));
	if((unsigned int)BGFlag > 1) BGFlag = FALSE;

	//����������
	Param_Read(GET_U8_OFFSET(ParamVoicePowerOn), (void *)&BeepFlag, sizeof(uint8));
	if((unsigned int)BeepFlag > 1) BeepFlag = TRUE;

	//�����ȼ�
	Param_Read(GET_U8_OFFSET(ParamFollowCarMode), (void *)&FollowMode, sizeof(uint8));
	if((unsigned char)FollowMode > FollowFarPay) FollowMode = FollowAtOnce;
	
	//������Ϣ��ӡ�ȼ�
	Print_Debug_Init();
}

