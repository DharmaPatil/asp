#include "ConnectCfg.h"
#include "printf_init.h"		//DebugPf
#include "crc_init.h"				//CRC
#include "static_init.h"		//ADDR
#include "rtc_init.h"				//RTC_AlarmA

/****************************************************************************
* ��	�ƣ�void BootLoaderOver(void)
* ��	�ܣ�BootLoader������ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void BootLoaderOver(void)
{
	//�رն�ʱ��6
	TIM_ITConfig(TIM6, TIM_FLAG_Update, DISABLE);						//�رն�ʱ���ж�
	TIM_Cmd(TIM6, DISABLE);																	//�رն�ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, DISABLE);		//�رն�ʱ����Դ
	
	//�ر�RTC
	RTC_ITConfig(RTC_IT_ALRA,DISABLE);											//�رձ���A�ж�
	RTC_AlarmCmd(RTC_Alarm_A,DISABLE);											//�رձ���A
	RTC_ITConfig(RTC_IT_ALRB,DISABLE);											//�رձ���B�ж�
	RTC_AlarmCmd(RTC_Alarm_B,DISABLE);											//�رձ���A
	PWR_BackupAccessCmd(DISABLE);														//�ر�RTC�󱸼Ĵ���ͨ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, DISABLE);		//�ر�PWRʱ��	
	RCC_RTCCLKCmd(DISABLE);																	//�ر�RTCʱ����Դ
	
	//�رմ���1
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);					//�رս����ж�
	USART_Cmd(USART1, DISABLE);  														//�رմ���1����
	
	//�رմ���2
	USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);					//�رս����ж�
	USART_Cmd(USART3, DISABLE);  														//�رմ���3����

	//�رմ���6
	USART_ITConfig(USART6, USART_IT_RXNE, DISABLE);					//�رս����ж�
	USART_Cmd(USART6, DISABLE);  														//�رմ���6����
}

/****************************************************************************
* ��	�ƣ�void TimDelayMS(const u16 DelayMS)
* ��	�ܣ���ʱ���ĺ��뼶�ӳ�
* ��ڲ�����const u16 DelayMS	�ӳ�ʱ����		����
* ���ڲ�������
* ˵	������
****************************************************************************/
void TimDelayMS(const u16 DelayMS)
{
	u8 Sone = 0;																//һ��ʱ��
	u8 STwo = 0;																//����ʱ��
	
	Sone = (u8)(GetTimeMSecCnt() / DelayMS);		//һ��ʱ���ȡ
	
	do
	{
		STwo = (u8)(GetTimeMSecCnt() / DelayMS);	//����ʱ���ȡ
	}while(STwo == Sone);												//δ����1��
}

/****************************************************************************
* ��	�ƣ�void BlockDataCheck(void)
* ��	�ܣ����ݿ���
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void BlockDataCheck(void)
{
	u32 RUNCRCOne = 0;
	u32 RUNCRCOneBak = 0;
	u32 RUNCRCDataSize = 0;

	//Ӧ����������
	DebugPf(0,"�������̼�Ч��..........");
	RUNCRCOne = ReadAppCRCAddress();				//��ȡ�洢��CRCЧ��ֵ
	RUNCRCDataSize = ReadAppSizeAddress();	//��ȡ���ݴ�С
	if(RUNCRCDataSize < AppSpaceSize)				//������ݴ�С�Ƿ񳬹�������ռ䣬������û�����ݣ�û������ʱ����ȫFF
	{
		if(ReadAppUpSrcAddress() == PCBOOT)
			RUNCRCOneBak = CRC32H_Chack8(1,(u8*)((volatile u8*)AppStartAddress),RUNCRCDataSize);		//Ӧ���������õ��ļ�У��ֵ		  //STM32Ӳ��Ч��
		else if(ReadAppUpSrcAddress() == NETBOOT)
			;
		
		if(RUNCRCOne == RUNCRCOneBak)					//CRC�Ƚ�
		{
			DebugPf(0,"OK!\r\n");									//��������
		}
		else
			DebugPf(0,"Error!\r\n");							//��������
	}
	else
		DebugPf(0,"No Data!\r\n");							//û������
	
	RUNCRCOne = 0;
	RUNCRCOneBak = 0;
	RUNCRCDataSize = 0;
	
	//GB�ֿ���
	DebugPf(0,"GB�ֿ�������Ч��........");			
	RUNCRCOne = ReadGBCRCAddress();					//��ȡ�洢��CRCЧ��ֵ
	RUNCRCDataSize = ReadGBSizeAddress();		//��ȡ���ݴ�С
	if(RUNCRCDataSize < GBFontSpaceSize)		//������ݴ�С�Ƿ񳬹�������ռ䣬������û�����ݣ�û������ʱ����ȫFF
	{
		if(ReadGBUpSrcAddress() == PCBOOT)
			RUNCRCOneBak = CRC32H_Chack8(1,(u8*)((volatile u8*)GBFontStartAddress),RUNCRCDataSize);		//GB�ֿ��������õ��ļ�У��ֵ
		else if(ReadGBUpSrcAddress() == NETBOOT)
			;
		
		if(RUNCRCOne == RUNCRCOneBak)					//CRC�Ƚ�
		{
			DebugPf(0,"OK!\r\n");									//��������
		}
		else
			DebugPf(0,"Error!\r\n");							//��������
	}
	else
		DebugPf(0,"No Data!\r\n");							//û������
	
	RUNCRCOne = 0;
	RUNCRCOneBak = 0;
	RUNCRCDataSize = 0;
	
	//ASCII�ֿ���
	DebugPf(0,"ASCII�ֿ�������Ч��.....");
	RUNCRCOne = ReadASCIICRCAddress();				//��ȡ�洢��CRCЧ��ֵ
	RUNCRCDataSize = ReadASCIISizeAddress();	//��ȡ���ݴ�С
	if(RUNCRCDataSize < ASCIIFontSpaceSize)		//������ݴ�С�Ƿ񳬹�������ռ䣬������û�����ݣ�û������ʱ����ȫFF
	{
		if(ReadASCIIUpSrcAddress() == PCBOOT)
			RUNCRCOneBak = CRC32H_Chack8(1,(u8*)((volatile u8*)ASCIIFontStartAddress),RUNCRCDataSize);		//ASCII�ֿ��������õ��ļ�У��ֵ
		else if(ReadASCIIUpSrcAddress() == NETBOOT)
			;
		
		if(RUNCRCOne == RUNCRCOneBak)						//CRC�Ƚ�
		{	
			DebugPf(0,"OK!\r\n");										//��������
		}
		else
			DebugPf(0,"Error!\r\n");								//��������
	}
	else
		DebugPf(0,"No Data!\r\n");								//û������
}

/****************************************************************************
* ��	�ƣ�u32 AutoBaudRateCheck(const u8 Val)
* ��	�ܣ��Զ�������ʶ����
* ��ڲ�����const u8 Val		ʶ���ַ�
* ���ڲ�����u32				������ֵ
* ˵	������
****************************************************************************/
u32 AutoBaudRateCheck(const u8 Val)
{
	u32 BaudRateVal = BootLoaderUsart_BaudRate;			//������ֵ�ʼΪ�趨ֵ
	u8 selfval[2] = {0};		//����Ӧ��' 'ֵ��ֻ����һ��ֵ������1�������ڲ����ʹ���
	u8 selfCnt = 0;					//����Ӧ�Լ�
	u8 selflen = 0;					//�յ����ַ���
	u8 Time03S = 0;					//0.3����		���20�� = 6��

	while(1)																	//����Ӧ������
	{
		BootLoaderUsart_RX_Clear();							//��ս��ջ�����
		
		TimDelayMS(300);												//���յȴ�0.3��
		Time03S++;
		if(Time03S >= 20)												//0.3/�� * 20�� = 6S
			return 0;															//û��⵽
			
		selflen = BootLoaderUsart_GetString(selfval,2);	//�ӻ�������ȡ�ַ�
		if(selflen >= 1)												//�յ�����
		{
			if(selfval[0] == Val)									//�ж��ַ��Ƿ���ȷ
			{
				BootLoaderUsart_RX_Clear();					//��ս��ջ�����
				return BaudRateVal;									//��ȷ ���ز�����ֵ
			}
		}
		
		selfCnt++;															//�ַ��д� �ı䲨����
		
		switch(selfCnt)
		{
			case 1:		//2400
			{
				BaudRateVal = 2400;
				break;
			}
			case 2:		//4800
			{
				BaudRateVal = 4800;
				break;
			}
			case 3:		//9600
			{
				BaudRateVal = 9600;
				break;
			}
			case 4:		//10416
			{
				BaudRateVal = 10416;
				break;
			}
			case 5:		//19200
			{
				BaudRateVal = 19200;
				break;
			}
			case 6:		//38400
			{
				BaudRateVal = 38400;
				break;
			}
			case 7:		//57600
			{
				BaudRateVal = 57600;
				break;
			}
			case 8:		//115200
			{
				BaudRateVal = 115200;
				break;
			}
			default:
			{
				selfCnt = 0;		//�ָ�2400����
				break;
			}
		}
		
		BootLoaderUsart_Init(BaudRateVal);		//�������ô��ڲ�����
	}
}
