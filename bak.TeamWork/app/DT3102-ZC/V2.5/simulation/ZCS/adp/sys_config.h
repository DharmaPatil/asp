#ifndef _SYS_CONFIG_H_ 
#define _SYS_CONFIG_H_ 

#include "includes.h"
#include "stdlib.h"
#include "string.h"
#include "weight_param.h"

#define DYNAMIC_AD_RATE 800									//��̬ad������
#define WEIGHT_CYCLE    32     								//��������
#define STATIC_AD_RATE  25    //(DYNAMIC_AD_RATE/WEIGHT_CYCLE)		//��̬ad������

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif


#define SYS_ERR_NONE                   0u



 //5��ϵͳģʽ
 typedef enum {
 	SCS_ZC_1 = 0,		
 	SCS_ZC_2,
 	SCS_ZC_3,
 	SCS_ZC_4,
 	SCS_ZC_5,
 	SCS_ZC_6,
	SCS_ZC_7
 }SysMod;

 //��������ģʽ
typedef enum {
	AutoLongCarMode,
	HandleLongCarMode
}LongCarMod;


typedef struct {
	u8 ad1_err:1;		//��1·AD����
	u8 ad2_err:1;
	u8 ad3_err:1;
	u8 ad4_err:1;
}ADStatus;
//����1��2Ϊ�ϳƷ����˳�� 
typedef union {
	struct {
		u8 LC1_err:1;		//��Ļ1  0-normal status,1-err status
		u8 LC1_status:1;	//	 0-release,1-trigger
		u8 LC2_err:1;		//��Ļ2 �ֳ���Ļ����
		u8 LC2_status:1;	//�ֳ���Ļ״̬	
		u8 GC1_err:1;		//ǰ�ظ�		
		u8 GC1_status:1;	
		u8 GC2_err:1;		//��ظ�
		u8 GC2_status:1;
		
		u8 tireSen_err:1;		//��̥ʶ��������
		u8 single_double:1;		//��˫��   0-signal,1-double
		u8 ud1:6;			//���� 6λ
		
		u8 ad1_err:1;		//��1·AD����
		u8 ad2_err:1;
		u8 ad3_err:1;
		u8 ad4_err:1;
		u8 axle11_valid:1;		//ǰ���һ·��Ч��־
		u8 axle12_valid:1;
		u8 axle2_valid:1;		//����
		u16 ud2:9;			//���� 4λ		 
	}bitValue;

	u32 value;
}DeviceStatus;	//����״̬��Ϣ 


typedef struct {
	s32 Wet;				//���ӷֶ�����
	s32 Axle1_Wet;			//ǰ������
    s32 Axle2_Wet;			//��������
	float PreAxle1AD;		//adֵ
	float PreAxle2AD;
    float ScalerAD;
	float BackAxleAD;
	u16  ScalerStableFlag;				//��̨�ȶ���־
	u16	ScalerZeroFlag;				//��̨��λ��־
}WetStatue; 

#define GET_U8_OFFSET(PARAMX)  (U8_PARAM_BASE + PARAMX - U8_PARAM_OFFSET)
#define GET_U16_OFFSET(PARAMX) (U16_PARAM_BASE + ((PARAMX - U16_PARAM_OFFSET)<<1))
#define GET_F32_OFFSET(PARAMX) (F32_PARAM_BASE + ((PARAMX - F32_PARAM_OFFSET)<<2))
#define GET_U32_OFFSET(PARAMX) (U32_PARAM_BASE + ((PARAMX - U32_PARAM_OFFSET)<<2))

#define U8_PARAM_BASE 		0		//���ֽ�����, ����64�ֽڳ���
#define U16_PARAM_BASE		64		//2�ֽ�����, ����128�ֽڳ���
#define F32_PARAM_BASE		192		//�����������, ����256�ֽڳ���
#define U32_PARAM_BASE		448	    //���Ͳ������򣬶���256�ֽڳ���
#define USR_PARAM_BASE		704

#define U8_PARAM_OFFSET         0
#define U16_PARAM_OFFSET        64
#define F32_PARAM_OFFSET        128
#define U32_PARAM_OFFSET        192


enum {
/********���ֽ�����, ���洢64������********/
    ParamPowerZero = 0,			//�������㷶Χ,���20%
    ParamHandlZero,				//�ֶ����㷶Χ,���4%
    ParamZeroTruckRang,			//�����ٷ�Χ
    ParamZeroTruckSpeed,			//�������ٶ�
    ParamDynamicDivValue,			//��̬�ֶ�ֵ
    ParamStaticDivValue,			//��̬�ֶ�ֵ
    ParamDynamicFilter,			//��̬�˲��ȼ�
    ParamStaticFilter,			//��̬�˲��ȼ�
    ParamSuperSpeedLevel,			//������ֵ
    ParamLongCarMode,				//����ģʽѡ��
    ParamTrafficSignalFlag,		//���̵Ʊ�־
    ParamBGFlag,					//��բ��־
    ParamVoicePowerOn,			//����������
    ParamPreLCFlag,				//ǰ��Ļ��־��ǰ��Ļ��ǰ�ظж�����һ
    ParamDebugLevel,				//������Ϣ��ӡ�ȼ�
    
/********2�ֽ�����, ���洢64������********/
    ParamBdr = 64,				//����2������
    ParamOverWetCarNum,		//dataflash���س�����
    ParamNormalCarNum,		//dataflash��������������
    ParamUpWet,				//�ϳ���ֵ
    ParamDownWet,				//�³���ֵ
    ParamAxle1UpWet,			//ǰ��ʶ����������ֵ
    ParamAxle1DownWet,
    ParamAxle2UpWet,			//����ʶ����������ֵ
    ParamAxle2DownWet,
    ParamSpeedLength,			//��Ļ�ظо��룬��λcm
    ParamAxleLength,
    ParamZeroTruckTick,		//��λ����ʱ��

    ParamSpeed5k,				//�ٶ�����ϵ��
    ParamSpeed10k,
    ParamSpeed15k,
    ParamSpeed20k,
    ParamSpeed25k,
    ParamSpeed30k,


    
/********�����������, ���洢64������********/
    ParamPreAxle1_K	= 128,	//4·ad Kϵ��
    ParamPreAxle2_K,
    ParamScaler_K,
    ParamBackAxle_K,	
    ParamTotal_K,
    ParamDyncK,				//��̬ϵ��
    ParamPreAxle1Zero,		//4·ad���
    ParamPreAxle2Zero,
    ParamScalerZero,
    ParamBackAxleZero,
    
/********���Ͳ�������, ���洢64������********/
    ParamAppVer  =  192,		//����汾��
    ParamParVer,				//����ʽ�����汾�����ݲ����汾�����Ƿ����Ĭ�ϲ���
    ParamAppMod,				//����ģʽ
    ParamMaxWet,				//�����������
    ParamTrafficWet, 			//���̵���ֵ
    ParamMax2Wet,				//2�ᳵ�������
    ParamMax3Wet,
    ParamMax4Wet,
    ParamMax5Wet,
    ParamMax6Wet,
    ParamAxleMaxWet,
    
/********������������, ��ʼ��ַ704********/
    ParamNet	 = USR_PARAM_BASE, 	//�������
    ParamOther = USR_PARAM_BASE + 40
};

//�ֳ�ѡ��
typedef enum {
	DevideAtOnce,		//��Ļ��β1.2��������ȡ��������
	DevideAtDown		//��������ӽɷ�
}FollowCarMod;


extern gWetParam gWetPar;

SysMod Get_System_Mode();
int Updata_AD_Status(u8 chanel,u8 status);
void Set_CarSpeed(int32 speed);
void Update_AxleDevice_Status(DeviceStatus status);
int Param_Write(uint32 nAddr, char* pBuff, int nLen);
void Scaler_Set_Zero(BOOL bAuto);
void Update_Scaler_WetPar(WetStatue *wet);
unsigned char Get_NetStatus(void);
LongCarMod Get_LongCar_Mode(void);
BOOL Get_LC_Status(u8 no);
uint16 Get_CarSpeed (void);
uint16 Get_Speed_Length(void);
BOOL Get_TrafficSignal_Flag(void);
void CarQueue_Print_Info(void);
void Clear_ScalerEmpty_Count(void);
BOOL Get_BGFlag(void);
FollowCarMod Get_FollowCarMode(void);
FollowCarMod Get_LittleFollowCarMode(void);


//��Ϣ����
uint8 Task_QPost(MsgQ *pMsgQ, void *pMsg);
uint8 Task_QPend(MsgQ *pMsgQ, void *pMsg, int len, uint16 timeout);

void barriergate_open(void);
#endif

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
}
#endif


