#include "MX25L3208DM2I_12G_init.h"

/****************************************************************************
* ��	�ƣ�void MX25L3208_CE(u8 Flag)
* ��	�ܣ���Դ����
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void MX25L3208_CE(u8 Flag)
{
	if(Flag == 1)
		GPIO_SetBits(GPIOC, GPIO_Pin_9);
	else
		GPIO_ResetBits(GPIOC, GPIO_Pin_9);
}

/****************************************************************************
* ��	�ƣ�void MX25L3208_Init(void)
* ��	�ܣ�DATAFLASH��ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵	����Ӳ��SPI�ӿ�
****************************************************************************/
void MX25L3208_Init(void)
{
	GPIO_InitTypeDef G;																			//���ű���
	
	if(MX25L3208_CEPort == GPIOA)
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);		//������ʱ��
	else if(MX25L3208_CEPort == GPIOB)
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);		//������ʱ��
	else if(MX25L3208_CEPort == GPIOC)
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);		//������ʱ��
	else if(MX25L3208_CEPort == GPIOD)
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);		//������ʱ��
	else if(MX25L3208_CEPort == GPIOE)
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);		//������ʱ��
	else if(MX25L3208_CEPort == GPIOF)
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);		//������ʱ��
	else if(MX25L3208_CEPort == GPIOG)
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);		//������ʱ��
	else if(MX25L3208_CEPort == GPIOH)
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);		//������ʱ��
	else if(MX25L3208_CEPort == GPIOI)
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE);		//������ʱ��
	
  G.GPIO_Pin = MX25L3208_CEPin;			//���ź�		FLASH_CE
  G.GPIO_Mode = GPIO_Mode_OUT;			//���ģʽ
  G.GPIO_OType = GPIO_OType_PP;			//����
  G.GPIO_Speed = GPIO_Speed_50MHz;	//����Ƶ��
  G.GPIO_PuPd = GPIO_PuPd_NOPULL;		//����
  GPIO_Init(MX25L3208_CEPort, &G);
	
	MX25L3208_CE(1);									//��ʼ����ԴΪ�ر�
}

/****************************************************************************
* ��	�ƣ�void GetMX25L3208ID(SPI_Cfg* spix,u8* ID)
* ��	�ܣ���ȡFLASH ID��
* ��ڲ�����SPI_Cfg* spix					SPI����
						u8* ID								��ȡ��FLASH ID��
* ���ڲ�������
* ˵	����u8 ID[3] = {0};
****************************************************************************/
void GetMX25L3208ID(SPI_Cfg* spix,u8* ID)
{
	//����ģ��SPI�����Ľ��գ������ķ���
	if(spix->SPIx == (SPI_TypeDef*)0)	
	{
		MX25L3208_CE(0);
		SPISSendcmd8(spix,0x9F);
		ID[0] = SPISReciveData8(spix);
		ID[1] = SPISReciveData8(spix);
		ID[2] = SPISReciveData8(spix);
		MX25L3208_CE(1);
	}
	
	ID[0] = 0;ID[1] = 0;ID[2] = 0;		//��λ
	
	//����ģ��SPIӲ��SPI���ֽ��շ�
	MX25L3208_CE(0);
	SPIbyteSendGet(spix,0x9F);
	ID[0] = SPIbyteSendGet(spix,0);
	ID[1] = SPIbyteSendGet(spix,0);
	ID[2] = SPIbyteSendGet(spix,0);
  MX25L3208_CE(1); 
}
