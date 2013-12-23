#ifndef _MX251608D_INIT_H_
#define _MX251608D_INIT_H_

#ifdef __cplusplus		   		//�����CPP����C���� //��ʼ����
extern "C" {
#endif
	
#include "includes.h"	
	
/*
MX25Lxx08D
һҳ		256		Byte
һ����	4096	Byte		256*16ҳ
һ��		65536	Byte		4096*16����		256*256ҳ
1608D = 2M = 2097152 Byte
*/
	
#define MX251608D_CMDERASE							0			//оƬ����	
#define MX251608D_CMDBLOCKERASE					1			//�����			
#define MX251608D_CMDSECTORERASE				2			//��������
#define MX251608D_CMDISBUSY							3			//æ���	
#define MX251608D_CMDBUSYWAIT						4			//æ�ȴ�
	
/****************************************************************************
* ��	�ƣ�u8 MX251608D_Init(u8 prio)
* ��	�ܣ�DataFlash��ʼ��
* ��ڲ�����u8 prio			�洢���������ȼ�
* ���ڲ�����u8		�Ƿ�ɹ�		1�ɹ�	0ʧ��
* ˵	������
****************************************************************************/
u8 MX251608D_Init(u8 prio);		//�豸��ʼ��	

/****************************************************************************
* ��	�ƣ�HANDLE DeviceOpen(DEV_ID ID,u32 lParam,HVL_ERR_CODE* err)
* ��	�ܣ��豸�򿪻���豸���
* ��ڲ�����DEV_ID ID						�豸ID��
						u32 lParam					����				����
						HVL_ERR_CODE* err		����״̬
* ���ڲ�����HANDLE 							�豸���
* ˵	������
****************************************************************************/	

/****************************************************************************
* ��	�ƣ�HVL_ERR_CODE DeviceClose(HANDLE IDH)
* ��	�ܣ��豸�ر�
* ��ڲ�����HANDLE IDH					�豸���
* ���ڲ�����HVL_ERR_CODE* err		����״̬
* ˵	������
****************************************************************************/	

/****************************************************************************
* ��	�ƣ�s32 BlockDeviceRead(HANDLE IDH,u32 offset,u8* Buffer,u32 len,u32* Reallen,u32 lParam)
* ��	�ܣ��豸������
* ��ڲ�����HANDLE IDH		�豸���
						u32 offset		��ƫ�Ƶ�ַ
						u8* Buffer		������ݵ�ַ				
						u32 len				ϣ����ȡ�����ݸ���
						u32* Reallen	ʵ�ʶ�ȡ�����ݸ���
						u32 lParam		����									����
* ���ڲ�����s32						��ȡ�����Ƿ�ɹ�			1�ɹ�	0ʧ��
* ˵	������
****************************************************************************/	

/****************************************************************************
* ��	�ƣ�s32 BlockDeviceWrite(HANDLE IDH,u32 offset,u8* Buffer,u32 len,u32* Reallen,u32 lParam)
* ��	�ܣ��豸д����
* ��ڲ�����HANDLE IDH		�豸���							
						u32 offset		дƫ�Ƶ�ַ
						u8* Buffer		д����Դ��ַ					
						u32 len				ϣ��д������ݸ���		
						u32* Reallen	ʵ��д������ݸ���		
						u32 lParam		����									����
* ���ڲ�����s32						д������Ƿ�ɹ�			1�ɹ�	0ʧ��
* ˵	����
****************************************************************************/	

/****************************************************************************
* ��	�ƣ�s32 DeviceIoctl(HANDLE IDH,u32 cmd,u32 lParam)
* ��	�ܣ��豸����
* ��ڲ�����HANDLE IDH		�豸���
						u32 cmd				�豸����
						u32 lParam		���������˵��
* ���ڲ�����s32 					�����ֵ��˵��
* ˵	����
����											����												����ֵ
MX251608D_CMDERASE				����												����
MX251608D_CMDBLOCKERASE		��ַ(�����õ�ַ�����Ŀ�)		����					
MX251608D_CMDSECTORERASE	��ַ(�����õ�ַ����������)	����
MX251608D_CMDISBUSY				����												1æ0��
MX251608D_CMDBUSYWAIT			����												����
****************************************************************************/	
	
#ifdef __cplusplus		   		//�����CPP����C���� //��������
}
#endif

#endif

/*
	u8 W[300] = {0};	  //д����
	u8 R[300] = {0};	  //������
	u8 i = 0;		  			//������
	u32 len = 0;	  		//����
	u32 addr = 0;	  		//��ʼ��ַ
	u32 RealLen = 0;		//ʵ�ʶ�д����

#include "mx251608d_init.h"
#include "String.h"				//memcpy

	HVL_ERR_CODE err = HVL_NO_ERR;								//����״̬
	
	HANDLE MX251608D_Dev = 0;													//�豸���
	
	USART_STRUCT usart1cfg = 
	{
		57600,																	//������
		USART_WordLength_8b,										//����λ
		USART_StopBits_1,												//ֹͣλ
		USART_Parity_No													//Ч��λ
	};

	USART1_Config(&usart1cfg);								//���ڳ�ʼ��
	Set_PrintfPort(1);												//���ô�ӡ����
	printf("���Կ�ʼ\r\n");
	
	if(MX251608D_Init(2) == 0)								//�豸A��ʼ��				���������ȼ�2
	{
		printf("��ʼ��ʧ��\r\n");
		while(1);																										
	}
	
	MX251608D_Dev = DeviceOpen(BLOCK_MX25,0,&err);								//�豸��
	if(err != HVL_NO_ERR)
	{
		printf("�豸��ʧ��\r\n");
		while(1);																										
	}
	
	if(DeviceIoctl(MX251608D_Dev,MX251608D_CMDERASE,0) == 0)			//оƬ����
	{
		printf("������������\r\n");
		while(1);																										
	}
	if(DeviceIoctl(MX251608D_Dev,MX251608D_CMDBUSYWAIT,0) == 0)		//æ�ȴ�
	{
		printf("æ�ȴ���������\r\n");
		while(1);																										
	}
	
	if(BlockDeviceRead(MX251608D_Dev,0,R,256,&RealLen,0) == 0)		//�������
	{
		printf("��ȡ����\r\n");
		while(1);																										//��ȡʧ��
	}
	if(RealLen != 256)																						//��ȡ�����ݸ�����ϣ����ȡ�����ݸ�����ƥ��
	{
		printf("���ݲ�ƥ�����\r\n");
		while(1);																										
	}																		
	RealLen = 0;
	
	//д��
	addr = 0;
	len = 0;				//������1-231�������

	while(1)
	{
		len++;
		if(len > 231)
			len = 1;
		

		for(i = 0; i < len; i++)
			W[i] = i;	

		if(BlockDeviceWrite(MX251608D_Dev,addr,W,len,&RealLen,0) == 0)		//д������
		{
			printf("д���������\r\n");
			while(1);																										
		}	
		if(RealLen != len)																						//д������ݸ�����ϣ��д������ݸ�����ƥ��
		{
			printf("д�����ݲ�ƥ�����\r\n");
			while(1);
		}
		RealLen = 0;
		
		addr += len;  	
		
		if(addr >= 0x00100000) 			//��ַ������
			break;  		
	}

	//��ȡ
	addr = 0;
	len = 0;
	while(1)
	{
		len++;
		if(len > 231)
			len = 1;
		

		for(i = 0; i < len; i++)
			W[i] = i;	

		if(BlockDeviceRead(MX251608D_Dev,addr,R,len,&RealLen,0) == 0)	//��ȡ���
		{
			printf("��ȡ����\r\n");
			while(1);																										//��ȡʧ��
		}
		if(RealLen != len)																						//��ȡ�����ݸ�����ϣ����ȡ�����ݸ�����ƥ��
		{
			printf("���ݲ�ƥ�����\r\n");
			while(1);																										
		}																		
		RealLen = 0;
		
		//�Ƚ�
		if(memcmp(W, R, len) != 0)
			break;

		addr += len;  
		
		if(addr >= 0x00100000) 		//��ַ������
			break;  		  		
	}
*/
