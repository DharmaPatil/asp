
#ifndef __STATICTESTIFPROC_H_
#define __STATICTESTIFPROC_H_

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif

//adͨ��
enum {
	ScalerChanle,				//��̨
	BackAxleChanle	,			//����
	PreAxle1Chanle,		//ǰ��
	PreAxle2Chanle,		//С��̨�ڶ�·ad
};

enum {
	KERR_NONE = 0,
	KERR_MIN,		//�궨kֵ̫С
	KERR_MAX,			//�궨kֵ̫��
	KERR_PARM,			//kֵδд��flash
	KERR_ADERR,
};

#define DYNAMIC_AD_RATE 800									//��̬ad������
#define WEIGHT_CYCLE    32     								//��������
#define STATIC_AD_RATE  25    //(DYNAMIC_AD_RATE/WEIGHT_CYCLE)		//��̬ad������

//�㷨ҵ��
void Weight_Business(void *buf);

//�㷨ģ���ʼ��
int Weight_Init(void);

//��ȡ��̨��̬����
int32 Get_Dynamic_Weight(void);

//���ó�̨����, ����ģ��ʹ��
void Set_Static_Weight(int32 wet);		

//��ȡ��̨��̬����
int32 Get_Static_Weight(void);	
int32 Get_Static_MaxWeight(void);	
void Clear_ScalerMaxWeight(void);	

//��̨����
void Scaler_Set_Zero(BOOL bAuto);

//�ж�Kֵ�쳣
uint8 Check_K_Value(float k);

//����ADͨ�����,�궨���
BOOL Weight_Calc_Zero(void);

//����ADͨ��Kϵ��
uint8 Weight_Cal_K(int inWet, int chanle);

//���ɱ궨
BOOL SCSZC1_Cal_K(int inWet, int chanle, int place);


//���ó�̨��̬�ֶ�ֵ
int Set_Scaler_StaticDrv(uint8 drv);

//���ó�̨��̬�ֶ�ֵ
int Set_Scaler_DynamicDrv(uint8 drv);


//���ص�ǰadͨ����
int32 Get_ADChanle_Num(void);

//��ʼ���㷨����
void Init_Wet_Par(void);

int Set_ScalerStatic_K(float k);




#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
}
#endif

#endif
