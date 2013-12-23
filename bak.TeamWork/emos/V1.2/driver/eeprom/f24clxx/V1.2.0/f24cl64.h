#ifndef _F24CL64_INIT_H_
#define _F24CL64_INIT_H_

#ifdef __cplusplus		   		//�����CPP����C���� //��ʼ����
extern "C" {
#endif

#include "stm32f4xx.h"
#include "I2Cx.h"

#define FM24CL64_A_SIZE 		8192				//FM24CL64����8KB
	
#define FM24CL64_CMDERASE			0					//оƬ����
	
typedef struct
{
	u8				DeviceAddr;						//������ַ
	I2C_Cfg		I2Cx;									//I2C����
}EEPROM_Cfg;											//EEPROM ����

typedef struct
{
	u32				StartAddr;						//��ʼ��ַ
	u8				DefaultVal;						//Ĭ������
	u32				Len;									//�������ݳ���
}EEPROM_EraseCfg;									//EEPROM �����ò���

/****************************************************************************
* ��	�ƣ�u8 FM24CL64_Init(u8 prio)
* ��	�ܣ������ʼ��
* ��ڲ�����u8 prio			�洢���������ȼ�
* ���ڲ�����u8		�Ƿ�ɹ�		1�ɹ�	0ʧ��
* ˵	������ҪI2C���CRC��
****************************************************************************/
u8 FM24CL64_Init(u8 prio);		//�豸��ʼ��	

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
						u32 lParam		����									0ֱ�Ӷ�	1Ч���
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
						u32* Reallen	ʵ��д������ݸ���		len + 2��(Ч��)
						u32 lParam		����									0ֱ��д	1Ч��д
* ���ڲ�����s32						д������Ƿ�ɹ�			1�ɹ�	0ʧ��
* ˵	��������ÿ�δ�������ݺ���׷��2λЧ������
���磺����1 size = 8
			�洢ռ��λ�� [0]-[7]����1 [8]-[9]Ч��ֵ
			����2�����λ��[10]��ʼ�洢��������ƻ�����1����
****************************************************************************/	

/****************************************************************************
* ��	�ƣ�s32 DeviceIoctl(HANDLE IDH,u32 cmd,u32 lParam)
* ��	�ܣ��豸����
* ��ڲ�����HANDLE IDH		�豸���
						u32 cmd				�豸����
						u32 lParam		���������˵��			EEPROM_EraseCfg
* ���ڲ�����s32 					�����ֵ��˵��
* ˵	����
����											����												����ֵ
FM24CL64_CMDERASE					EEPROM_EraseCfg							�����������ֽ���
****************************************************************************/

#ifdef __cplusplus		   		//�����CPP����C���� //��������
}
#endif

#endif

/*
	u8 val = 0xCD;			//һ�ֽ�����	��������
	u8 W[300] = {0};	  //д����
	u8 R[300] = {0};	  //������
	u8 C[10000] = {0};	//�������
	u8 i = 0;		  			//������
	u32 len = 0;	  		//����
	u32 addr = 0;	  		//��ʼ��ַ
	u32 RealLen = 0;		//ʵ�ʶ�д����

	HVL_ERR_CODE err = HVL_NO_ERR;								//����״̬
	HANDLE FM24CL64_Dev = 0;											//�豸���
	
	EEPROM_EraseCfg F24_Def = 		//F24������Ĭ��ֵ
	{
		0,													//�����Ŀ�ʼ��ַ
		0xCC,												//Ĭ������
		FM24CL64_A_SIZE							//�������ݸ���	8K
	};
	
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

	if(FM24CL64_Init(5) == 0)									//EEPROM��ʼ��			���ȼ�
		return;
	
	FM24CL64_Dev = DeviceOpen(BLOCK_AT24C,0,&err);								//�豸��
	if(err != HVL_NO_ERR)
	{
		printf("�豸��ʧ��\r\n");
		while(1);																										
	}
	
	SysTimeDly(500);		//��ͣ5��		//��ֹ��доƬʱִ��write����
	
	//======================================�ռ��С����
	//���洢�ռ����ݴ洢�������һ�����ݴ洢���鿴������������Ƿ�Ḳ�ǵ�һ��λ������(������ݿ�����������Ƿ�Ḳ��)
	if(DeviceIoctl(FM24CL64_Dev,FM24CL64_CMDERASE,(u32)&F24_Def) != FM24CL64_A_SIZE)		//д0-8191�ֽ�����	��8192�ֽ�����
	{printf("��������\r\n");while(1);}		
	if(BlockDeviceRead(FM24CL64_Dev,0,C,10000,&RealLen,0) == 0)							//��ȡ���ݣ��������оƬ�����ݴ洢���
	{printf("��ȡ����\r\n");while(1);}	
	if(RealLen != 10000)			//��ȡ�����ݸ�����ϣ����ȡ�����ݸ�����ƥ��
	{printf("���ݲ�ƥ�����\r\n");while(1);}		
	
	RealLen = 0;				//��ո�λ
	memset(C,0,10000);	//��ո�λ
	
	if(BlockDeviceWrite(FM24CL64_Dev,FM24CL64_A_SIZE,&val,1,&RealLen,0) == 0)					//д��1���ݵ������ַ	
	{printf("д���������\r\n");while(1);}	
	if(RealLen != 1)												//д������ݸ�����ϣ��д������ݸ�����ƥ��
	{printf("д�����ݲ�ƥ�����\r\n");while(1);}
	
	RealLen = 0;				//��ո�λ
	
	if(BlockDeviceRead(FM24CL64_Dev,0,C,10000,&RealLen,0) == 0)							//��ȡ���ݣ���鱾��ϣ��д��1024�ϵ������Ƿ񸲸���0�ϵ����ݣ��洢���λ��
	{printf("��ȡ����\r\n");while(1);}
	if(RealLen != 10000)										//��ȡ�����ݸ�����ϣ����ȡ�����ݸ�����ƥ��
	{printf("���ݲ�ƥ�����\r\n");while(1);}
	
	RealLen = 0;				//��ո�λ
	
	//��մ洢��
	if(DeviceIoctl(FM24CL64_Dev,FM24CL64_CMDERASE,(u32)&F24_Def) != FM24CL64_A_SIZE)
	{printf("��������\r\n");while(1);}	
	
	//��Ч������洢����
	for(i = 0;i < 100;i++)
		W[i] = i;
	
	if(BlockDeviceWrite(FM24CL64_Dev,0,W,100,&RealLen,1) == 0)					//д��1���ݵ������ַ	
	{printf("д���������\r\n");while(1);}	
	if(RealLen != 102)												//д������ݸ�����ϣ��д������ݸ�����ƥ��
	{printf("д�����ݲ�ƥ�����\r\n");while(1);}
	
	if(BlockDeviceRead(FM24CL64_Dev,0,R,120,&RealLen,0) == 0)						//��ȡ���ݣ��鿴Ч��λ�Ƿ�д��
	{printf("��ȡ����\r\n");while(1);}
	if(RealLen != 120)												//��ȡ�����ݸ�����ϣ����ȡ�����ݸ�����ƥ��
	{printf("���ݲ�ƥ�����\r\n");while(1);}
	
	RealLen = 0;				//��ո�λ
	
	if(BlockDeviceRead(FM24CL64_Dev,0,R,100,&RealLen,1) == 0)							//��ȡ���ݣ���鱾��ϣ��д��1024�ϵ������Ƿ񸲸���0�ϵ����ݣ��洢���λ��
	{printf("��ȡ����\r\n");while(1);}
	if(RealLen != 100)										//��ȡ�����ݸ�����ϣ����ȡ�����ݸ�����ƥ��
	{printf("���ݲ�ƥ�����\r\n");while(1);}
*/
