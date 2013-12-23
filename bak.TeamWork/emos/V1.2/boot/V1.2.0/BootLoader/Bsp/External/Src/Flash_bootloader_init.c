#include "Flash_bootloader_init.h"
#include "ConnectCfg.h"			//ͨѶ��	//ComPort��
#include "printf_init.h"		//DebugPf
#include "static_init.h"		//CPUID
#include "ymodem_init.h"		//ymodemЭ��
#include "rtc_init.h"				//ʱ����ʾ
#include "crc_init.h"				//CRC32H_Chack8

/***********************
bin			 		2�����ļ�
hex					16�����ļ�
Flash Loader Demo	��hex�ļ�

STM32ʸ����
0x00		Initial Main SP
0x04		Reset
0x08		NMI
0x0C		Hard Fault
0x10		Memory Manage
0x14		Bus Fault
0x18		Usage Fault
0x1C-0x28	Reserved
0x2C		SVCall
0x30		Debug Monitor
0x34		Reserved
0x38		PendSV
0x3C		Svstick
0x40		IRQ0
..			More IRQs

IAP���裺
BootLoader��	������ת��ַ										ApplicationAddress 0x08XX XXXX
Application��	*������ʼ��ַ�ͳ����С								0x08XX XXXX 0xXXXX
				*�����ж�������ƫ�Ƶ�ַ����ʼ��ַһ��				NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0xXXXXX);
				����Linker��R/O Base����ʼ��ַһ��					0x08XX XXXX
				�޸�*.sct�ļ��е�ַ����ʼ��ַһ�£������Сһ��		0x08XX XXXX
				����Bin �������ļ�
				�鿴*.map	RESET��ַ�Ƿ����						0x08XX XXXX  
***********************/

//�����˵���ʾ�շ������ô���
#define BootLoader_SendByte  			ComSendByte							//BootLoader�˵����ַ�����
#define BootLoader_GetByte_Wait  	ComGetByte_Wait					//BootLoader�˵����մ��� �ȴ�
#define BootLoader_SendString 		ComSendString						//BootLoader�˵��ַ�������
#define BootLoader_RX_Clear 			BootLoaderUsart_RX_Clear						//BootLoader�˵��������
#define BootSendFinishWait()			ComSendFinishWait()			//�ȴ��������

/****************************************************************************
* ��	�ƣ�void ShowMenu(const u32 BaudRate)
* ��	�ܣ�BootLoader�˵���ʾ
* ��ڲ�����const u32 BaudRate		������
* ���ڲ�������
* ˵	��������ʼ��ַΪ��0x00000000
****************************************************************************/
void ShowMenu(const u32 BaudRate)
{
	BootLoader_SendByte(0x0C);
	BootLoader_SendByte(0x0C);		//���� 2�θ�����

	DebugPf(0,"*****************************************************\r\n");
	DebugPf(0,"                STM32 Bootloader Menu\r\n");
	DebugPfEx(0,"           (%s)\r\n",BootVersion);
	DebugPf(0,"\r\n");
  DebugPf(0,"                Build Date : ");
	DebugPf(0,__DATE__);		  //��ʾ��������
	DebugPf(0,"\r\n");
	DebugPf(0,"                Build Time : ");
	DebugPf(0,__TIME__);		  //��ʾ����ʱ��
	DebugPf(0,"\r\n");
	DebugPf(0,"\r\n");
	DebugPf(0,"        ( 1 ) : Download Application Program From Ymodem (256K)\r\n");							//�ն˻�ȡӦ�ó���̼��ļ�
	DebugPf(0,"        ( 2 ) : Download GB Font From Ymodem (256K)\r\n");													//�ն˻�ȡGB�ֿ��ļ�
	DebugPf(0,"        ( 3 ) : Download ASCII Font From Ymodem (16K)\r\n");												//�ն˻�ȡASCII�ֿ��ļ�
	DebugPf(0,"        ( 4 ) : Download Auxiliary Application Program From Ymodem (64K)\r\n");		//�ն˻�ȡ����Ӧ�ó����ļ�
	DebugPf(0,"        ( 5 ) : Block Data Check\r\n");															//�����е�����
	DebugPf(0,"        ( 6 ) : CPU ID\r\n");																				//CPU ID
	DebugPf(0,"        ( 7 ) : Show Time\r\n");																			//��ʾ��ǰʱ��
	DebugPf(0,"        ( 8 ) : Reset System\r\n");																	//����
	DebugPf(0,"        ( E ) : Exit\r\n");																					//�˳�
	DebugPf(0,"        ( D ) : Auxiliary\r\n");																			//����Ӧ�ó���
	DebugPf(0,"\r\n");
	DebugPfEx(0,"-----------  %d(S) Wait....  ",(u8)MenuWaitTime);									//��ʾ�ȴ�ʱ��Ϊ��
	DebugPfEx(0,"BaudRate��%d----------\r\n",BaudRate);															//��ʾ�ȴ�ʱ��Ϊ��
	DebugPf(0,"*****************************************************\r\n");
	DebugPf(0,"Input Number:");
}

/****************************************************************************
* ��	�ƣ�u8 Key_Input(const u8 timeS)
* ��	�ܣ���������
* ��ڲ����const �u8 timeS	n*5��
* ���ڲ�����u8	1	����ˢ�²˵�
				0	�˳����û��Բ˵�
* ˵	����1	���г���
			2	���س���
			3	�˳�BOOTLOADER
			4	YMODEM�����ļ�
		����	���󰴼�
****************************************************************************/
u8 Key_Input(const u8 timeS)
{
	u8 num = 0;								//���뷬��
	u8 flag = 0;							//״̬��־	
	u8 CPUID[12] = {0};				//CPUID
	u32 TimeCnt = 0;					//���ӳټ�����

	u32 AuxiliaryAppCRCOne = 0;					//���������ȡЧ����
	u32 AuxiliaryAppCRCOneBak = 0;			//�����������Ч����
	u32 AuxiliaryAppCRCDataSize = 0;		//��������Ч�����ݴ�С

	while(1)
	{
		flag = BootLoader_GetByte_Wait(&num,11200000);		//�������ݣ��ӳ�1S (RTC���ʱ�������)
		if(flag == 0)		  								//δ���յ�����	   	//2��δ������ת
		{
			TimeCnt++;											//����ʱ����
			
			if(TimeCnt >= timeS)						//ʱ����
			{
				DebugPfEx(0,"\r\n%d���޲������Զ��˳�BootLoader......\r\n",timeS);
				return 0;
			}
		}
		else			 											 //���յ�����		//2���а���
		{
			TimeCnt = 0;									//���¼�ʱ
	
			DebugPfEx(0,"%c\r\n",num);		//���Լ�����ַ�����
			switch(num)
			{
				case '1':
				case '2':
				case '3':
				case '4':
				{
					switch(num)
					{
						case '1':
						{
							DebugPfEx(0,"Download Application Program From Ymodem.....'B' or 'b' = EXIT!  %d(S) Wait...\r\n",(u32)ShowGetFileWait);		//������ɺ󷵻ز˵�
							flag = Ymodem_START(1);	//���չ̼��ļ�
							break;
						}
						case '2':
						{
							DebugPfEx(0,"Download GB Font From Ymodem.....'B' or 'b' = EXIT!  %d(S) Wait...\r\n",(u32)ShowGetFileWait);		//������ɺ󷵻ز˵�
							flag = Ymodem_START(2);	//����GB�ֿ��ļ�
							break;
						}
						case '3':
						{
							DebugPfEx(0,"Download ASCII Font From Ymodem.....'B' or 'b' = EXIT!  %d(S) Wait...\r\n",(u32)ShowGetFileWait);		//������ɺ󷵻ز˵�
							flag = Ymodem_START(3);	//����ASCII�ֿ��ļ�
							break;
						}
						case '4':
						{
							DebugPfEx(0,"Download Auxiliary Application Program Font From Ymodem.....'B' or 'b' = EXIT!  %d(S) Wait...\r\n",(u32)ShowGetFileWait);		//������ɺ󷵻ز˵�
							flag = Ymodem_START(4);	//���ո���Ӧ�ó����ļ�
							break;
						}
						default:
						{
							DebugPf(0,"Option Error.....\r\n");		//������ɺ󷵻ز˵�
							flag = 0;								//����
							break;
						}	
					}
					
					if(flag == 1)		 //���ճɹ�
					{
						DebugPf(0,"****************************\r\n");
						DebugPf(0,"File Name : ");
						BootLoader_SendString(GetFlieNameDataAddr(),GetFlieNameLen());		 //�ļ�����
						BootSendFinishWait();													//�ȴ�ISR�����жϽ����ݷ�����	
						DebugPf(0,"\r\n");
						DebugPfEx(0,"File Size : %d\r\n",GetFlieSize());	 	//�ļ���С	
						DebugPf(0,"****************************\r\n");
						DebugPf(0,"Receive File Complete!! Press Key To Return...\r\n");
	
						BootLoader_GetByte_Wait(&num,168000000);	//��ʾͣ�� 15��
					}
					return 1;		   //���ز˵�
				}
				case '5':
				{
					BlockDataCheck();													//������ݿ�
					BootLoader_GetByte_Wait(CPUID,168000000);	//��ʾͣ�� Լ15��
					return 1;			//���ز˵�
				}
				case '6':
				{
				 	GetCpuIdHex(CPUID);			//��ȡCPUID
					DebugPfEx(0,"CPU ID��%X-",CPUID[0]);
					DebugPfEx(0,"%X-",CPUID[1]);
					DebugPfEx(0,"%X-",CPUID[2]);
					DebugPfEx(0,"%X-",CPUID[3]);
					DebugPfEx(0,"%X-",CPUID[4]);
					DebugPfEx(0,"%X-",CPUID[5]);
					DebugPfEx(0,"%X-",CPUID[6]);
					DebugPfEx(0,"%X-",CPUID[7]);
					DebugPfEx(0,"%X-",CPUID[8]);
					DebugPfEx(0,"%X-",CPUID[9]);
					DebugPfEx(0,"%X-",CPUID[10]);
					DebugPfEx(0,"%X\r\n",CPUID[11]);
					BootLoader_GetByte_Wait(CPUID,168000000);	//��ʾͣ�� Լ15��
					return 1;
				}
				case '7':
				{
				 	ShowTime();
					BootLoader_GetByte_Wait(&num,168000000);	//��ʾͣ�� Լ15��
					return 1;
				}
				case '8':
				{
					NVIC_SystemReset();	//�̼����Դ���������
					return 1;			//���ز˵�
				}
				case 'E':		 //�˳�
				case 'e':		 //�˳�
				{		  
				 	DebugPf(0,"Exit.....\r\n");	//Bye			
					return 0;
				}
				case 'D':		 //ִ�и�������
				case 'd':		 //ִ�и�������
				{
					//Ӧ����������
					DebugPf(0,"��������Ч��..........");
					AuxiliaryAppCRCOne = ReadAuxiliaryAppCRCAddress();				//��ȡ�洢��CRCЧ��ֵ
					AuxiliaryAppCRCDataSize = ReadAuxiliaryAppSizeAddress();	//��ȡ���ݴ�С
					if(AuxiliaryAppCRCDataSize < AuxiliaryAppSpaceSize)				//������ݴ�С�Ƿ񳬹�������ռ䣬������û�����ݣ�û������ʱ����ȫFF
					{
						if(ReadAuxiliaryAppUpSrcAddress() == PCBOOT)
							AuxiliaryAppCRCOneBak = CRC32H_Chack8(1,(u8*)((volatile u8*)AuxiliaryAppStartAddress),ReadAuxiliaryAppSizeAddress());		//Ӧ���������õ��ļ�У��ֵ		  //STM32Ӳ��Ч��
						else if(ReadAuxiliaryAppUpSrcAddress() == NETBOOT)
							;
						
						if(AuxiliaryAppCRCOne == AuxiliaryAppCRCOneBak)					//CRC�Ƚ�
						{
							DebugPf(0,"\r\n����Ӧ�ó���Ч��ͨ������תִ�и�������......\r\n");
							
							BootLoaderOver();													//��������
							Jump_IAP(AuxiliaryAppStartAddress);				//������ת	
						}
						else
						{
							DebugPf(0,"Error!\r\n");									//��������
						}
					}
					else
						DebugPf(0,"No Data!\r\n");									//û������
				 		
					return 0;
				}
				
				default:			   //ˢ�²˵�
					return 1;		   //���ز˵�
			}
		}
	}	
}

/****************************************************************************
* ��	�ƣ�void BOOTLoader(const u32 BaudRate)
* ��	�ܣ�BootLoaderִ�г���
* ��ڲ�����const u32 BaudRate		������
* ���ڲ�������
* ˵	��������ʼ��ַΪ��0x00000000
****************************************************************************/
void BOOTLoader(const u32 BaudRate)
{
	u8 flag = 0;								//ִ�з��ر�־

	while(1)
	{
		ShowMenu(BaudRate);										//��ʾ�˵�

		CRC_ResetDR();	  										//�������ʱ���CRC
		BootLoader_RX_Clear();								//��ս��ջ�����

		flag = Key_Input((u8)MenuWaitTime);		//�ȴ���������
		if(flag == 0)													//�˳����û��Բ˵�
		{	
			DebugPf(0,"BYE!\r\n");
			break;															//�˳�ѭ��
		}
	}
}
 
/****************************************************************************
* ��	�ƣ�void Jump_IAP(const u32 MainAppAddr)
* ��	�ܣ�IAP��ת
* ��ڲ�����const u32 MainAppAddr		�������ַ
* ���ڲ�������
* ˵	��������Ӧ�ó���ִ��
(volatile u32*) ���� (__IO uint32_t*) ����
****************************************************************************/
void Jump_IAP(const u32 MainAppAddr)
{
	typedef void(*pFunction)(void);					//����ָ���û�����ĺ���ָ��
	u32 JumpAddress = 0;										//��ת��ַ
	pFunction Jump_To_Application;					//�����û���������

	JumpAddress = *(volatile u32*)(MainAppAddr + 4);	  //Reset��ַ (�׵�ַ+4)
	Jump_To_Application = (pFunction)JumpAddress;				//����Ӧ�ó������ڵ�ַ 
	__set_MSP(*(volatile u32*)MainAppAddr);			  			//��ʼ��Ӧ�ó���Ķ�ջָ��	 //ע��˴��ĵ�ַ���׵�ַ������Reset��ַ
	Jump_To_Application();									  					//��ת��Main����
}

