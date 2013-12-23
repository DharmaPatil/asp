
#ifndef _WET_PARAM
#define _WET_PARAM
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
#endif
