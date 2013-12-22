#include "device_info.h"



static volatile DeviceStatus curStatus={0};
static volatile int ADChipStatus = 0;

/****************************************
* ˵	�������������豸״̬��Ϣ����̬������
* �Ѱ������ؿ��ƶ���SDKͨѶЭ�顷���к�
****************************************/
//2ms��ʱ����������״̬
void Updata_Device_Status(DeviceStatus newStatus)
{
	curStatus.value =(newStatus.value & 0x3ff) | (curStatus.value & 0xfffff800);
}

void Updata_HW_Status(DeviceStatus newStatus)
{
	curStatus.value =(newStatus.value & 0x030) | (curStatus.value & 0xffffffcf);
}

//������ʶ����״̬
void Update_AxleDevice_Status(DeviceStatus status)
{
	curStatus.value =(status.value) | (curStatus.value & 0xff8fffff);
}

//���³�����Ϣ���ͱ�־λ
void Update_CarInfo_SendFlag(DeviceStatus status)
{
	curStatus.value =(status.value) | (curStatus.value & 0xff7fffff);
}


//�㷨�̸߳���AD����״̬
//chanel 1-4 ADͨ�����,status:0-����,1Ϊ����
BOOL Updata_AD_Status(u8 chanel,u8 status)
{
	if(chanel > 3 || status > 1)
		return FALSE;
		
	switch(chanel)
	{
		case 0:curStatus.bitValue.ad1_err = status;
			break;
		case 1:curStatus.bitValue.ad2_err = status;
			break;
		case 2:curStatus.bitValue.ad3_err = status;
			break;
		case 3:curStatus.bitValue.ad4_err = status;
			break;
		default:return TRUE;
	}
	return TRUE;
}


//�㷨�̸߳���adоƬ״̬
void UpDate_ADChip_Status(int status)
{
	ADChipStatus = status & 0x0f;
}

int Get_ADChip_ErrStatus(void)
{
	return ADChipStatus;
}

/****************************************
 *��	����no��Ϊ1(ǰ��Ļ)��2(���Ļ)
 * ����ֵ��true������false�ͷ�
 * ˵	������ȡ��Ļ����״̬
 ****************************************/
BOOL Get_LC_Status(u8 no)
{
	if(no==1)
	{
		return curStatus.bitValue.LC1_status==1;
	}
		
	else 
	{
		return curStatus.bitValue.LC2_status==1;
	}
}

/****************************************
 * ��	����no��Ϊ1(ǰ��Ļ)��2(���Ļ)
 * ����ֵ��true������false����
 * ˵	������ȡ��Ļ����״̬
 ****************************************/
BOOL Get_LC_ErrStatus(u8 no)
{
	if(no==1)
	{
		return curStatus.bitValue.LC1_err==0;
	}
	else 
	{
		return curStatus.bitValue.LC2_err==0;
	}
}

/****************************************
 * ��	����no��Ϊ1-4·ͨ��
 * ����ֵ��true���� false����
 * ˵	������ȡADͨ������״̬
 ***************************************/
BOOL Get_AD_ErrStatus(u8 no)
{
	if((no < 1) || (no > 4)) return FALSE;
	
	if(no == 1)
		return curStatus.bitValue.ad1_err==1;
	else if(no == 2)
		return curStatus.bitValue.ad2_err==1;
	else if(no == 3)
		return curStatus.bitValue.ad3_err==1;
	else if(no == 4)
		return curStatus.bitValue.ad4_err==1;
	else
		;

	return FALSE;
}

/****************************************
 * ��	����no��Ϊ1(ǰ��ʶ����)��2(����ʶ����)
 * ����ֵ��true��ѹ�� false��ѹ��
 * ˵	������ȡ��ʶ��������״̬
 ***************************************/
BOOL Get_Axle_Status(u8 no)
{
	//other
	return TRUE;
}
/****************************************
 * ��	������
 * ����ֵ��true���� false����
 * ˵	������ȡ̥��ʶ��������״̬
 ***************************************/
BOOL Get_Tire_ErrStatus(void)
{
	return curStatus.bitValue.tireSen_err == 0;
}

//��ȡ̥��ʶ�������ź�����״̬��true-���źţ�false-���ź�
BOOL Get_Device_TyreFlag(void)
{
	return curStatus.bitValue.tireSen_Signal;
}

//��ȡ̥��ʶ�����ĵ�˫��״̬��true-˫�֣�false-����
BOOL Get_Tire_Status(void)
{
	return curStatus.bitValue.single_double==1;
}


/****************************************
 * ��	����no��Ϊ1(ǰ�ظ�)��2(��ظ�)
 * ����ֵ��true���� false�ͷ�
 * ˵	������ȡ�ظ���Ȧ����״̬ 
 ****************************************/
BOOL Get_GC_Status(u8 no)
{
	if(no==1)
	{
		return curStatus.bitValue.GC1_status==1;
	}
	else 
	{
		return curStatus.bitValue.GC2_status==1;
	}
}

/****************************************
 * ��	����no��Ϊ1(ǰ�ظ�)��2(��ظ�)
 * ����ֵ��true���� false����
 * ˵	������ȡ�ظ���Ȧ����״̬ 
 ****************************************/
BOOL Get_GC_ErrStatus(u8 no)
{
	if(no==1)
	{
		return curStatus.bitValue.GC1_err==0;
	}
	else 
	{
		return curStatus.bitValue.GC2_err==0;
	}
}


/****************************************
* ����ֵ���Ѱ������ؿ��ƶ���SDKͨѶЭ�顷���кõ����ֽ�����
* ˵	������ȡ�����豸״̬��Ϣ
****************************************/
u32 Get_Device_Status(void)
{
	return curStatus.value;
}



