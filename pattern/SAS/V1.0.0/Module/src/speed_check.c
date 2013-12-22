#include "includes.h"
#include "device_info.h"
#include "speed_check.h"

static int16 CarSpeed = 66;
static uint16 length = 1500;				//��Ļ�ظо���mm

/* �ٶȼ��㺯�� */
void Check_CarSpeed (void)
{
#if 0
	static uint16 nCount_2ms = 0;						//���ڼ���2ms��ʱ����
	static BOOL carCome = FALSE;						//������־,�ֲ�����
	static BOOL keep = FALSE;

	//��ȡǰ�ظ���Ȧ����ǰ��Ļ����״̬	
	if(TRUE == Get_GC_Status(1) || TRUE == Get_LC_Status(1))		
	{
		if(keep) {
			carCome = TRUE;								//ǰ�ظб�����,��������־
			nCount_2ms++;								//��ʼ2ms����
		}															
	}
	else														//ǰ�ظ�û�б�����
	{
		keep = TRUE;
		if(carCome)			//��Ļ����ǰ�ظл�δ�������Ļ��������
		{
			carCome = FALSE;
			nCount_2ms = 0;
		}
	}
	
	if(Get_LC_Status(2) && (carCome))					//���Ļ����
	{
		carCome = FALSE;								//���������־
		keep = FALSE;
		//��ʱ̫С�������ٶ�
		if(nCount_2ms > 10)
			CarSpeed = 72*(length/nCount_2ms);			//7.2*length/cnt (km/h)���㳵���ٶ�
		nCount_2ms = 0;									//2ms��������
	}
#endif
}

/* �ٶȻ�ȡ���� */
uint16 Get_CarSpeed(void)
{
	if(CarSpeed < 20)
		CarSpeed = 20;

	if(CarSpeed > 800)
		CarSpeed = 300;

	return CarSpeed;
}

void Set_Speed_Length(uint16 len)
{
	length = len;
//	Param_Write();
}

uint16 Get_Speed_Length(void)
{
	return length;
}

void Set_CarSpeed(int32 speed)
{
	CarSpeed = speed;
}


