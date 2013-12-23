/* 
 * File:    sys_param.h
 * Brief:   FRAM ������ַ���� 
 *
 * History:
 * 1. 2012-11-24 �����ļ�, �ӳ������ϳ�����ֲ river 
 * 2. 2012-11-28 ���debug��ӡ�ȼ���־
 * 3. 2012-12-26 ���Ӻ��̵�������ֵ����
 */

#ifndef SYSPARAM_H	
#define SYSPARAM_H

#ifdef __cplusplus		   	
extern "C" {
#endif

#include "sys_cpu.h"
#include "stm32f4xx.h"
#include "app.h"
	
	typedef struct  {
    u8 unit;            // 0:kg  1:t
    u8 Beep;            // 0:close 1:open
    u8 password[10];    // ����
	u8 second;					//��
} SSYSSET;


//FRAM������
//����ʽ��������    0 ~ 1023;
//˫��̨�������� 1024 ~ 3072

/****************************************����ʽ������****************************************/

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
	ParamFollowCarMode,			//�����ȼ�
	ParamLittleFollowCarMode,		//С�������ȼ�
	ParamMaxAxleNum,
    
/********2�ֽ�����, ���洢64������********/
    ParamBdr = 64,				//����2������
    ParamOverWetCarNum,		//dataflash���س�����
    ParamNormalCarNum,		//dataflash��������������
    ParamUpWet,				//�ϳ���ֵ
    ParamDownWet,				//�³���ֵ
    ParamPreAxleUpWet,			//��1ʶ����������ֵ
    ParamPreAxleDownWet,
    ParamBackAxleUpWet,			//��2ʶ����������ֵ
    ParamBackAxleDownWet,
    ParamWbAxleUpWet,			//��3ʶ����������ֵ  //���-->>PreAxle2
    ParamWbAxleDownWet,
    ParamSpeedLength,			//��Ļ�ظо��룬��λcm
    ParamAxleLength,
    ParamZeroTruckTick,		//��λ����ʱ��

	ParamSpeed0k,				//�ٶ�����ϵ��
    ParamSpeed5k,				
    ParamSpeed10k,
    ParamSpeed15k,
    ParamSpeed20k,
    ParamSpeed25k,
    ParamSpeed30k,
    ParamResetCnt,				//ϵͳ��λ����

    ParamCarNumFront,			//��ʷ�������ݶ�ͷλ��
    ParamCarNumRear,			//��ʷ�������ݶ�βָ��

	ParamDeviceSignal, 			//�����ƽ����
  
	ParamFollow0k,				//���ٶ�����ϵ��
    ParamFollow2P5k,				
    ParamFollow5k,
    ParamFollow7P5k,				
    ParamFollow10k,
	ParamFollow12P5k,		
	ParamFollow15k,
    ParamFollow20k,
    ParamFollow25k,
    ParamFollow30k,


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

#define GET_U8_OFFSET(PARAMX)  (U8_PARAM_BASE + PARAMX - U8_PARAM_OFFSET)
#define GET_U16_OFFSET(PARAMX) (U16_PARAM_BASE + ((PARAMX - U16_PARAM_OFFSET)<<1))
#define GET_F32_OFFSET(PARAMX) (F32_PARAM_BASE + ((PARAMX - F32_PARAM_OFFSET)<<2))
#define GET_U32_OFFSET(PARAMX) (U32_PARAM_BASE + ((PARAMX - U32_PARAM_OFFSET)<<2))

/********************************************************************************************/

/****************************************˫��̨������****************************************/

//˫��̨�����汾��100Ϊ������ʼ�汾�ţ� 
#define SCS_SC_VER_INIT 100
#define SCS_SC_VER	 100
// #define WETPARAM_VER    100


//������Ϣ2012.11.21 lxj
#define  SYSPARAM_VER_ADDR      1024		//0x00    // �汾��ַ 4
#define  SYSPARAM_SYSSET        1028		//0x10    // ϵͳ���� 12
#define  SYSPARAM_DEVSET      	  1040   	//0x30      // �������� 7

#define  WETPARAM_VER_ADDR      1064			//0x1000  // �汾��ַ 4
#define  WETPARAM_FLASHADDR     1068			//0x1004  // ����ƫ�Ƶ�ַ 4
#define  WETPARAM_DATAPACKNO    1072			//0x1008  // ���ݰ���� 4
#define  WETPARAM_MATRIC         1076			//0x1010  // ���ز��� 50
#define  WETPARAM_CALIBPARAM_A  1140			//0x1200  // A�궨    16
#define  WETPARAM_CALIBPARAM_B  1200			//0x1280  // B�궨    16
#define  WETPARAM_OVERLOAD       1300 		//0x1300  // ������ֵ  14
#define  WETPARAM_SPEEDMOD       1320			//0x1310  // �ٶ�����  13
#define  WETPARAM_CARMOD         1340		//0x1320  // ��������  24*20
//#define  WETPARAM_CARMOD        0x1500  // ��������  24*20

#define  CALIB_VER_ADDR         1900		//0x1A00   // �궨ԭʼ���ݰ汾 4
#define  CALIB_DATA_A           1904		//	0x1C00   // A�궨ԭʼ����    121
#define  CALIB_DATA_B           2048		//0x1E00   // B�궨ԭʼ����    121

/********************************************************************************************/

void Param_Interface_Init(void);

/*
 * ���ܣ�д�����
 * ������addr ��ַ
         data д�������ַ
         size ��������
 * ���أ�true �ɹ� FALSE ʧ��
 */
BOOL   Param_Write(int addr,  void* data, uint32 size);

/*
 * ���ܣ���ȡ����
 * ������addr ��ַ
         data ��ȡ������ַ
         size ��������
 * ���أ�true �ɹ� FALSE ʧ��
 */
BOOL   Param_Read(int addr,  void* data, uint32 size);

//���������������Ϣ
u8 SysSetParamSave(void);
u8 SysInitParam(void);

u8 GetSysStatus(u32 bit);
u8 SetSysStatus(u32 bit,u8 en);

//===============================
//IO
u16 Get_Device_Signal(void);
BOOL Set_Device_Signal(u16 value);


#ifdef __cplusplus		   	
}
#endif

#endif
