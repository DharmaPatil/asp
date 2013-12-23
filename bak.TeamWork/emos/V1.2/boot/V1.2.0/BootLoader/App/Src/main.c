#include "main.h"
#include "stm32f4xx.h"		//�̼���
#include "nvic_init.h"		//nvic_init
#include "crc_init.h"			//crc
#include "printf_init.h"	//printf
#include "Flash_bootloader_init.h"	//BootLoader
#include "rtc_init.h"			//RTC_Config															//���Բ鿴ʱ��
#include "static_init.h"	//BootLoader����
#include "Flash_init.h"		//FLASH_DataSpace_Erase
#include "ConnectCfg.h"		//ͨѶ������
#include "usart_cfg.h"		//USART_STRUCT
#include "tim6_init.h"		//TIM6_Config															//��ѡ���ṩ��ȷ��ʱ

#ifdef BootKey						//����������
#include "BootKey_init.h"	//��������
#endif

#ifdef RTCTimeFlag
#include "RTC_Alarm.h"		//Alarm_A_Respond
#endif

#define USART_RX_BUFF_SIZEMAX 128
#define USART_TX_BUFF_SIZEMAX 128
static u8 USARTRxBuffer[USART_RX_BUFF_SIZEMAX] = {0};								//���ڽ��ջ�����
static u8 USARTTxBuffer[USART_TX_BUFF_SIZEMAX] = {0};								//���ڷ��ͻ�����

static u8 NET_Bin_KEY[] = "EMOS_V001_001_001_001";				//��ĿKEY

//�������ñ���
static USART_STRUCT USART_Real = 
{
	BootLoaderUsart_BaudRate,	//������

	USARTRxBuffer,						//���ջ�����
	USART_RX_BUFF_SIZEMAX,		//���ջ�������С

	USARTTxBuffer,						//���ͻ�����
	USART_TX_BUFF_SIZEMAX,		//���ͻ�������С
};

/****************************************************************************
* ��	�ƣ�int main(void)
* ��	�ܣ���
* ��ڲ�������
* ���ڲ�������
* ˵	�����̼���������
1�����չ̼�
2���̼��洢�������������±�־
3������־���Ƿ�����
4����Ҫ���������̼�����������
5�������̼����������뵽�̼�Ӧ����
6�����̼�Ӧ��������
7���������±�־

�����ļ�ʱ��ö���CRC32H_Chack8���ļ�Ч�鲢�洢��
����ʱ�þ���CRC32H_Chack8��������
****************************************************************************/
int main(void)
{
	u32 FileCRC = 0;				//Ӧ��������������ļ�У��ֵ
	u32 FileCRCBak = 0;			//Ӧ�ñ���������������ļ�У��ֵ
	u32 flag = 0;						//������ɱ�־	0���	1Ч�����δ���	 2�̼���С����δ���	

	#ifndef BootKey					//û����������
	#ifdef CmdKey						//�������
	u8 cmdlen = 0;					//���ո���
	u8 cmdVal[2] = {0};			//�����ַ�
	#endif
	#endif

	nvic_init();														//�ж��������ַ
	
//Power_init();														//��Դ����
		
	CRC32_init();														//CRC32��ʼ��
	
	#ifdef BootKey													//��������
	BootKey_init();													//�����������ų�ʼ��
	#endif

	if(BootLoaderUsart_Config(&USART_Real) == 0)				//�����ó�ʼ��ͨѶ�˿�
		return 0;	
	Set_PrintfPort(PrintfPort);													//��ӡ���������	
	
	if(TIM6_Config(0,10,Mode_100US) == 0)								//Ĭ��Ϊ�����ж�
		DebugPf(0,"��ʱ��6����ʧ��\r\n");	
	TIM6_CntFlagOnOff(1);																//������������
	TIM6_ResetCount();																	//��ʱֵ��λ
	TIM6_TimeISR_Open();																//������ʱ�����ж�
	
	//�Ƿ�����ʵʱʱ��	(�����������BootLoaderҲ�޷�����)
	#ifdef RTCTimeFlag
  if(RTC_Config(2012,10,4,4,15,45,30))								//ʵʱʱ�ӳ�ʼ��		��������ʱ����
		RTC_AlarmA(99,0,99,99,99,1,Alarm_A_Respond);			//��������A		ÿ�뱨��A
	#endif
	
	//�Ƿ������Զ�������ʶ��
	#ifdef AutoBaudRateCheckFlag
	USART_Real.BaudRate = AutoBaudRateCheck(' ');				//�Զ���Ⲩ����
	if(USART_Real.BaudRate == 0)												//���û�м�⵽������
	{
		USART_Real.BaudRate = BootLoaderUsart_BaudRate;		//����ΪĬ�ϲ�����
		BootLoaderUsart_Init(BootLoaderUsart_BaudRate);		//��Ĭ�ϲ��������ô���
	}
	#endif
	
	DebugPfEx(0,"BootLoader Start, BaudRate %d\r\n",USART_Real.BaudRate);
	
	BlockDataCheck();																		//���ݿ���
	
	//�Ƿ������������������
	#ifdef BootKey			//����������
	if(GPIO_ReadInputDataBit(BootKeyPort,BootKeyPin) == BootKeyDownVal)
		BOOTLoader(USART_Real.BaudRate);									//������������		������������		//û����������������
	#else
	#ifdef CmdKey				//�������
	TimDelayMS(300);																		//���յȴ�0.3��
	cmdlen = BootLoaderUsart_GetString(cmdVal,2);				//�ӻ�������ȡ�ַ�
	if(cmdlen >= 1)																			//�յ�����
	{
		if(cmdVal[0] == ' ')															//�ж��ַ��Ƿ���ȷ
		{
			BootLoaderUsart_RX_Clear();											//��ս��ջ�����
			BOOTLoader(USART_Real.BaudRate);								//������������		����������
		}
	}
	#endif
	#endif
		
	//�Ƿ����¹̼�
	if(ReadAppUPFlagAddress() == UPBOOTFlag)	
	{
		if(ReadAppUpSrcAddress() == PCBOOT)					   		//�̼���������
		{
			//Set_PrintfPort(1);														//��ӡ�˿��Ƿ���Ҫ�ı�
			DebugPf(0,"��PC�������¹̼�\r\n");
		}
		else if(ReadAppUpSrcAddress() == NETBOOT)	
		{
			//Set_PrintfPort(1);														//��ӡ�˿��Ƿ���Ҫ�ı�
			DebugPf(0,"�����緢�����¹̼�\r\n");
		}
		
		if(ReadAppSizeAddress() <= AppSpaceSize)					//�̼����ܴ��ڹ̼���ſռ�
		{	
			DebugPf(0,"Ч��̼�����\r\n");
			
			//�����ļ�Ч��ֵ
			if(ReadAppUpSrcAddress() == PCBOOT)											//PC�̼�		Ч�鷽��		CRC32H_Chack8
			{	
				FileCRCBak = CRC32H_Chack8(1,(u8*)((volatile u8*)AppStartAddressBak),ReadAppSizeAddress());		//Ӧ�ñ����������õ��ļ�У��ֵ		  //STM32Ӳ��Ч��
			}
			else if(ReadAppUpSrcAddress() == NETBOOT)								//NET�̼�		Ч�鷽��		���Բ���CRC32H_Chack8 �����������ʱ�õ�CRC32H_Chack8һ��
			{
				FileCRCBak = CRC16(1,(u8*)((volatile u8*)AppStartAddressBak),ReadAppSizeAddress());		//Ӧ�ñ����������õ��ļ�У��ֵ
				FileCRCBak = CRC16(0,NET_Bin_KEY,sizeof(NET_Bin_KEY));	
			}
			DebugPfEx(1,"����У��ֵ %X\r\n",FileCRCBak);
				   
			//�������ļ���СУ��ֵ
			if(FileCRCBak == ReadAppCRCAddress())  									//FileCRCBak����	ReadAppCRCAddress��ȡ	
			{	
				DebugPf(0,"Ч��ɹ������¹̼�����........\r\n");				//ÿ����Ч��ͨ�����̼�����������ʼ���³���

				CopyErrJump:		//�����ֿ�������ʱ��ת���˴��½��п���
				if(FLASH_DataSpace_Erase(AppStartAddress,AppSpaceSize) == 0)													//�������й̼���
				{
					DebugPf(0,"Ӧ�ó���ռ����ʧ��!\r\n");																							//Ӧ�ó���ռ����ʧ��
					return 0;
				}
				
				Flash_DataWrite(AppStartAddress,ReadAppSizeAddress(),(u8*)AppStartAddressBak);				//�������ݵ�������	//����С�����ƹ̼���������
				DebugPf(0,"�̼��Ѹ���\r\n��ʼ��Ӧ�����̼��������Ч��...\r\n");

				//��⿽��������Ӧ��������CRC
				
				if(ReadAppUpSrcAddress() == PCBOOT)											//PC�̼�		Ч�鷽��		CRC32H_Chack8
				{	
					FileCRC = CRC32H_Chack8(1,(u8*)((volatile u8*)AppStartAddress),ReadAppSizeAddress());		//Ӧ�ñ����������õ��ļ�У��ֵ		  //STM32Ӳ��Ч��
				}
				else if(ReadAppUpSrcAddress() == NETBOOT)								//NET�̼�		Ч�鷽��		���Բ���CRC32H_Chack8 �����������ʱ�õ�CRC32H_Chack8һ��
				{
					FileCRC = CRC16(1,(u8*)((volatile u8*)AppStartAddress),ReadAppSizeAddress());		//Ӧ�ñ����������õ��ļ�У��ֵ
					FileCRC = CRC16(0,NET_Bin_KEY,sizeof(NET_Bin_KEY));	
				}
				if(FileCRC == ReadAppCRCAddress())
				{
					//Ӧ��������ͱ���������һ��
					flag = Succeed;			 																												//�������	������0
					if(FLASH_DataSpace_Erase(AppFlagAddress,AppFlagSpaceSize) == 0)							//д������ɱ�־
						DebugPf(0,"Ӧ�ó��򱸷ݱ�־�ռ����ʧ��(������ɱ�־)!\r\n");							//Ӧ�ó����־�ռ����ʧ��
					
					Flash_DataWrite(AppFlagAddress,4,(u8*)&flag);																//���½����ִ�гɹ�

					DebugPf(0,"�̼����³ɹ�����תִ��Ӧ�ó���......\r\n");
					
					BootLoaderOver();				//��������
					Jump_IAP(AppStartAddress);	 						//������ת
				}
				else											//Ӧ����Ч��ʧ��	���������뿽����Ӧ����ʱ����
				{
					if(CopyEver == 0)				//��һֱ����
					{
						flag = CopyErr;		  																								//����δ���	������3
	          if(FLASH_DataSpace_Erase(AppFlagAddress,AppFlagSpaceSize) == 0)			//д����δ��ɱ�־
						{
							DebugPf(0,"Ӧ�ó��򱸷ݱ�־�ռ����ʧ��(���������־)!\r\n");			//Ӧ�ó����־�ռ����ʧ��
							return 0;
						}
						Flash_DataWrite(AppFlagAddress,4,(u8*)&flag);												//���½������������
						DebugPf(0,"�������ݳ���Ӧ��������ͱ���Ӧ�������벻һ��\r\n");
					}
					else										//һֱ���п�������
						goto CopyErrJump;			//��ת������������ת��
				}
			}
			else 				   							//Ч��ʧ��
			{
        flag = CRCErr;		   			//����δ���	������1
        if(FLASH_DataSpace_Erase(AppFlagAddress,AppFlagSpaceSize) == 0)					//д����δ��ɱ�־
				{
					DebugPf(0,"Ӧ�ó��򱸷ݱ�־�ռ����ʧ��(CRC�����־)!\r\n");					//Ӧ�ó����־�ռ����ʧ��
					return 0;
				}
				Flash_DataWrite(AppFlagAddress,4,(u8*)&flag);														//���½����CRC����
				DebugPf(0,"�̼���CRC��ⲻͨ����������\r\n");
			}
		}
		else 					   							//�̼���С��ͨ��
		{
      flag = SizeErr;			   			//�̼���С��ͨ����������2
      if(FLASH_DataSpace_Erase(AppFlagAddress,AppFlagSpaceSize) == 0)						//д�̼���С��ͨ����־
			{
				DebugPf(0,"Ӧ�ó��򱸷ݱ�־�ռ����ʧ��(�̼���С�����־)!\r\n");				//Ӧ�ó����־�ռ����ʧ��
				return 0;
			}
			Flash_DataWrite(AppFlagAddress,4,(u8*)&flag);															//���½�����̼���̫С
			DebugPf(0,"�̼�����С��������\r\n");
		}

		DebugPf(0,"ִ��ԭ����\r\n");
		
		BootLoaderOver();												//��������
		Jump_IAP(AppStartAddress);	 						//������ת
		return 0;
	}
	
	DebugPf(0,"��תִ��Ӧ�ó���......\r\n");
	
	BootLoaderOver();													//��������
	Jump_IAP(AppStartAddress);																//������ת
	return 0;																	//��������ʧ��
}

