 //Ӳ���ӿڲ�ͬʱ�����ܻ���ĵ����ò���
#ifndef _BEEP_CFG_H	
#define _BEEP_CFG_H

#include "includes.h"

//IOCTL����
#define 	BEEPCMD_SETONOFF										0		//��������������
#define 	BEEPCMD_GETONOFF										1		//���������ػ�ȡ
#define		BEEPCMD_LookDevVer									2		//��ѯ�豸�汾��
#define		BEEPCMD_LookDevDisc									3		//��ѯ�豸����
#define		BEEPCMD_LookDevShared								4		//��ѯ�豸����
#define		BEEPCMD_LookDevOpenCount						5		//��ѯ�豸�򿪴���

//IOCTL����
#define 	BEEPCMD_SETONOFF										0		//��������������
#define 	BEEPCMD_GETONOFF										1		//���������ػ�ȡ
#define 	BEEPCMD_ON											    2   //��������ָ��ms
typedef struct
{
	u32 count;							//��������
	u16 time;								//ÿ�η���ʱ�䣬��λ����
}BEEP_STRUCTURE;


#endif	//_BEEP_CFG_H

/*
���ԣ�
	#include "beep_init.h"			//��������ʼ��
	#include "beep_usercfg.h"		//BEEP_STRUCTURE

	//����������
	static BEEPCTL_STRUCT Beep_User =
	{
		GPIOE,					//���Ŷ˿ں�
		GPIO_Pin_15			//���ź�
	};

	HVL_ERR_CODE err = HVL_NO_ERR;	//�豸��������
	HANDLE BEEP_HANDLE;							//�������豸���
	u8* DevPt = (u8*)0;							//�豸��Ϣָ��		����ʱ��ֹ���Ż���

	BEEP_STRUCTURE Beep_test = {3,100};			//����������

	if(Beep_Init(Beep_User) == 0)						//��������ʼ��
		return;
	
	BEEP_HANDLE	= CharDeviceOpen(CHAR_BEEP,0,&err);					//�򿪷�����
	if(err != HVL_NO_ERR)
		while(1);
		
	DevPt = (u8*)CharDeviceIoctl(BEEP_HANDLE,BEEPCMD_LookDevDisc,0);		//��ȡ�������豸����
	CharDeviceWrite(BEEP_HANDLE,(INT8S*)&Beep_test,0);									//����������
*/
