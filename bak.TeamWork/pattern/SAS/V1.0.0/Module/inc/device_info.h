#ifndef _DEVICE_INFO_H
#define _DEVICE_INFO_H

#include "includes.h"

/*****
�豸�����涨��
��Ļ1���ϳӷ����һ����Ļ���ù�Ļһ�����ڲ��٣���ѡ��
��Ļ2���ϳӷ���ڶ�����Ļ���ù�Ļ���ڳ������롣
�ظ���Ȧ1���ϳ�̨����ĵظ���Ȧ
�ظ���Ȧ2���³�̨����ĵظ���Ȧ

BIT0: 
0--����
1--��Ļ1����
BIT 1: 
0--��Ļ1δ��ס
1--��Ļ1��ס
BIT 2: 
0--����
1--��Ļ2����
BIT 3: 
0����Ļ2δ��ס
1����Ļ2��ס
BIT 4��
0--����
1--�ظ���Ȧ1����
BIT 5��
0--�ظ���Ȧ1�����ͷ�״̬
1--�ظ���Ȧ1���ڴ���״̬
BIT 6��
0--����
1--�ظ���Ȧ2����
BIT 7��
0--�ظ���Ȧ2�����ͷ�״̬
1--�ظ���Ȧ2���ڴ���״̬
BIT8: 
0��	����
1��	��̥ʶ�������� 
BIT9: 
0-	����
1-	˫��
BIT10-BIT15��Ϊ�������豣��

BIT16����1·AD����
0-	����
1-	����
BIT 17����2·AD����
0������
1������
BIT 18����3·AD����
0������
1������
BIT 19����4·AD����
0������
1������
BIT 20-BIT31 ��Ϊ������AD���ϱ���
**********/
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
		u8 single_double:1;	//��˫��   0-signal,1-double
		u8 tireSen_Signal:1;  //��̥ʶ�����ź� 0 �� 1 ��
		u8 ud1:5;			//���� 6λ
		
		u8 ad1_err:1;		//��1·AD����
		u8 ad2_err:1;
		u8 ad3_err:1;
		u8 ad4_err:1;
		u8 axle11_valid:1;		//ǰ���һ·��Ч��־
		u8 axle12_valid:1;
		u8 axle2_valid:1;			//����
		u8 carinfo_send_flag:1;		//�������ͱ�־
		u8 ud2:8;			//���� 4λ		 
	}bitValue;

	u32 value;
}DeviceStatus;	//����״̬��Ϣ 



//���ü̵�����״̬ PE0-3 �͵�ƽʱ�����Ϊ�ߵ�ƽ
u8 Set_Relay_Status(u8 no,BOOL status);

BOOL Get_LC_Status(u8 no);
// ��	����no��Ϊ1(ǰ��Ļ)��2(���Ļ)
// ����ֵ��true������false�ͷ�
// ˵	������ȡ��Ļ����״̬

BOOL Get_LC_ErrStatus(u8 no);
// ��	����no��Ϊ1(ǰ��Ļ)��2(���Ļ)
// ����ֵ��true������false����
// ˵	������ȡ��Ļ����״̬

BOOL Get_Tire_Status(void);
// ��	������
// ����ֵ��true���� false˫��
// ˵	������ȡ̥ʶ������˫״̬

BOOL Get_Tire_ErrStatus(void);
// ��	������
// ����ֵ��true���� false����
// ˵	������ȡ̥ʶ��������״̬

BOOL Get_GC_Status(u8 no);
// ��	����no��Ϊ1(ǰ�ظ�)��2(��ظ�)
// ����ֵ��true���� false�ͷ�
// ˵	������ȡ�ظ���Ȧ����״̬ 

BOOL Get_GC_ErrStatus(u8 no);
// ��	����no��Ϊ1(ǰ�ظ�)��2(��ظ�)
// ����ֵ��true���� false����
// ˵	������ȡ�ظ���Ȧ����״̬ 


u32 Get_Device_Status(void);
// ����ֵ���Ѱ������ؿ��ƶ���SDKͨѶЭ�顷���кõ����ֽ�����
// ˵	������ȡ�����豸״̬��Ϣ

void Updata_Device_Status(DeviceStatus newStatus);
// ˵	�������������豸״̬��Ϣ����̬�������Ѱ������ؿ��ƶ���SDKͨѶЭ�顷���к�

//���º�������źţ�����ǰ�ظ�
void Updata_HW_Status(DeviceStatus newStatus);

//������ʶ����״̬, ����ʹ��
void Update_AxleDevice_Status(DeviceStatus status);
//���³������ͱ�־,��λ������ʹ��
void Update_CarInfo_SendFlag(DeviceStatus status);


BOOL Updata_AD_Status(u8 chanel,u8 status);
BOOL Get_AD_ErrStatus(u8 no);

//�㷨�̸߳���AD����״̬
//chanel 1-4 ADͨ�����,status:0-����,1Ϊ����

void UpDate_ADChip_Status(int status);
int Get_ADChip_ErrStatus(void);


#endif		//_DEVICE_INFO_H
