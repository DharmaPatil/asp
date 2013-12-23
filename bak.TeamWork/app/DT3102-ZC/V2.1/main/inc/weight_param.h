/* 
 * File:    weight_param.h
 * Brief:   global weight parameter  
 *
 * History:
 * 1. 2012-11-24 �����ļ�, ��������������ֲ river 
 */

#ifndef WEIGH_PARAM_H	
#define WEIGH_PARAM_H

#ifdef __cplusplus		   	
extern "C" {
#endif

#include "sys_cpu.h"

typedef struct {
	u32 MaxWet;					//����������
	u8  PowerZero;				//�������㷶Χ
	u8  HandlZero;				//�ֶ����㷶Χ
	u8  ZeroTruckSpeed;		//�������ٶ�
	u8	DynamicDivValue;		//��̬�ֶ�ֵ
	u8  StaticDivValue;		//��̬�ֶ�ֵ
	u8  ZeroTruckRang;			//�����ٷ�Χ
	u8	StaticFilterLevel;	//��̬�˲��ȼ�
	u8  DynamicFilterLevel;	//��̬�˲��ȼ�
	u8	SuperSpeedLevel;		//������ֵ
	
	u16	ZeroTruckTick;			//��λ����ʱ��
	u16  PreAxleUpWet;			//��1������ֵ
	u16  PreAxleDownWet;
	u16  BackAxleUpWet;			//��2������ֵ
	u16  BackAxleDownWet;
	u16  WbAxleUpWet;			//��3������ֵ
	u16  WbAxleDownWet;
	u16  TotalUpWet;		//�����ϳ���ֵ
	u16  TotalDownWet;
	u16  Length; 			//��Ļ�ظо���
	u16  AxleLength;		//��ʶ�������

	u16 Speed_0k;		//0km/h~5km/h
	u16 Speed_5k;		//5km/h~10km/h����ϵ��
	u16 Speed_10k;		//10km/h
	u16 Speed_15k;
	u16 Speed_20k;
	u16 Speed_25k;
	u16 Speed_30k;		//30km/h����
	
	u16 Follow_0k;		//
	u16 Follow_2P5k;	//
	u16 Follow_5k;		//
	u16 Follow_7P5k;	//
	u16 Follow_10k;		//
	u16 Follow_12P5k;		//
	u16 Follow_15k;		//	
	u16 Follow_20k;		//
	u16 Follow_25k;		//
	u16 Follow_30k;		//

	u32 Max2AxleWet;		//�������������������
	u32 Max3AxleWet;
	u32 Max4AxleWet;
	u32 Max5AxleWet;
	u32 Max6AxleWet;
	u32 MaxTrafficWet;		//���̵�������ֵ

	float DynamicK;	  	//��̬ϵ��
	float PreAxle1_K;		//adͨ��kϵ��
	float PreAxle2_K;
	float Scaler_K;
	float BackAxle_K;
	float Total_K;
	float PreAxle1ZeroAD;	//adͨ��ad���
	float PreAxle2ZeroAD;
	float ScalerZeroAD;
	float BackAxleZeroAD;
	
	u8	nMaxAxleNum;			//���峵���������
}gWetParam;

/*//�������ݽӿ�
 *adͨ���ɳ��ؼ���ģʽ����
 *SCS_ZC_1:
	PreAxle1AD ScalerAD��Ч; PreAxle1AD��ʾ��̨��һ·AD, ScalerAD��ʾ��̨�ڶ�·AD
 *SCS_ZC_2:
	PreAxle1AD ScalerAD��Ч; PreAxleAD��ʾǰ��ʶ����AD, ScalerAD��ʾ��̨AD 
 *SCS_ZC_3:
	PreAxle1AD ScalerAD BackAxle��Ч; PreAxleAD��ʾǰ��ʶ����AD, ScalerAD��ʾ��̨AD,BackAxle��ʾ����ʶ����AD
 *SCS_ZC_5,SCS_ZC_6:
	PreAxle1AD PreAxle2AD ScalerAD BackAxle��Ч; PreAxle1AD,PreAxle2AD��ʾС��̨AD, ScalerAD��ʾ��̨AD,BackAxle��ʾ����ʶ����AD
*/
typedef struct 
{
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

extern gWetParam gWetPar;			//ȫ�ֳ��ز���

int32 Get_Scaler_DrvWeight(void);

//���³�������
void Update_Scaler_WetPar(WetStatue *wet);

//��ȡ��������
void Get_Scaler_WetPar(WetStatue *gWet);

void Weight_Param_Init(void);

//����ʽ�����汾��200Ϊ������ʼ�汾�ţ� 
#define SCS_ZC_VER_INIT 200
#define SCS_ZC_VER	 201		//201�汾������������ֵ���������ٶ�����	

BOOL Set_Max_AxleNum(unsigned char num);

#ifdef __cplusplus		   	
}
#endif

#endif


