#ifndef _DOG_INIT_H_
#define _DOG_INIT_H_

#ifdef __cplusplus		   		//�����CPP����C���� //��ʼ����
extern "C" {
#endif

#include "includes.h"				

#define DOGCMD_FEED			0		//ι��
	
void FeedDog(void); //ֱ�ӵ���ι������
/****************************************************************************
* ��	�ƣ�u8 Dog_Init(void)
* ��	�ܣ����Ź���ʼ��
* ��ڲ�������
* ���ڲ�����u8	��ʼ���Ƿ�ɹ�	1�ɹ� 0ʧ��
* ˵	������
****************************************************************************/
u8 Dog_Init(void);

/****************************************************************************
* ��	�ƣ�HANDLE DeviceOpen(DEV_ID ID,u32 lParam,HVL_ERR_CODE* err)
* ��	�ܣ��豸�򿪻���豸���
* ��ڲ�����DEV_ID ID						�豸ID��
						u32 lParam					����				�������ȼ�
						HVL_ERR_CODE* err		����״̬
* ���ڲ�����HANDLE 							�豸���
* ˵	������������������
****************************************************************************/	

/****************************************************************************
* ��	�ƣ�HVL_ERR_CODE DeviceClose(HANDLE IDH)
* ��	�ܣ��豸�ر�
* ��ڲ�����HANDLE IDH					�豸���
* ���ڲ�����HVL_ERR_CODE* err		����״̬
* ˵	����ɾ������������
****************************************************************************/	

/****************************************************************************
* ��	�ƣ�s32 DeviceIoctl(HANDLE IDH,u32 cmd,u32 lParam)
* ��	�ܣ��豸����
* ��ڲ�����HANDLE IDH		�豸���
						u32 cmd				�豸����
						u32 lParam		���������˵��
* ���ڲ�����s32 					�����ֵ��˵��
* ˵	����
����											����							����ֵ
DOGCMD_FEED								����							����
****************************************************************************/	

#ifdef __cplusplus		   		//�����CPP����C���� //��������
}
#endif

#endif

/*
	HANDLE DOG_HANDLE;												//�豸���
	
	USART1_Config(&usart1cfg);								//���ڳ�ʼ��
	Set_PrintfPort(1);												//���ô�ӡ����
	printf("���Կ�ʼ\r\n");
	SysTimeDly(10);
	
	Dog_Init();		//���Ź���ʼ��		��ʼ����1.6������Ҫ����ι��
	
	DOG_HANDLE	= DeviceOpen(CHAR_DOG,0,&err);	//�򿪿��Ź��豸
	if(err != HVL_NO_ERR)
		while(1);
	DeviceIoctl(DOG_HANDLE,DOGCMD_FEED,0);			//ι��
	
	while(1)
	{
		printf("����� %d \r\n",cnt++);
		DeviceIoctl(DOG_HANDLE,DOGCMD_FEED,0);		//ι��
		SysTimeDly(140);			//1.6S  ����ι��������λ
	}
*/
