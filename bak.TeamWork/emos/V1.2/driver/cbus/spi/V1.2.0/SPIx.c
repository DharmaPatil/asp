#include "SPIx.h"
#include "dt3102_IO.h"

#define FLASH_SCK_PIN		GPIO_Pin_10
#define FLASH_SCK_PORT	GPIOC
#define FLASH_MISO_PIN	GPIO_Pin_11
#define FLASH_MISO_PORT	GPIOC
#define FLASH_MOSI_PIN	GPIO_Pin_12
#define FLASH_MOSI_PORT	GPIOC


/****************************************************************************
* ��	�ƣ�u8 SPIxbyteSendGet(SPI_TypeDef* SPIt,u8 byte)	
* ��	�ܣ�Ӳ��SPIx���ͽ���byte
* ��ڲ�����SPI_TypeDef* 	SPIt	SPI �˿ں�
						u8 						byte	�������� or �����ַ�
* ���ڲ�����u8									���յ�������
* ˵	������			
****************************************************************************/
u8 SPIxbyteSendGet(SPI_TypeDef* SPIt,u8 byte)		 
{
	while (SPI_I2S_GetFlagStatus(SPIt, SPI_I2S_FLAG_TXE) == RESET);		 	//������������Ϊ��
	SPI_I2S_SendData(SPIt, byte);			   																//��SPI���ڷ�������0xFF
	while (SPI_I2S_GetFlagStatus(SPIt, SPI_I2S_FLAG_RXNE) == RESET);	 	//������ջ�������Ϊ��
	return (u8)SPI_I2S_ReceiveData(SPIt);
}

/****************************************************************************
* ��	�ƣ�void SPIxWordSend(SPI_TypeDef* SPIt,u8 Word)
* ��	�ܣ�Ӳ��SPIx����Word
* ��ڲ�����SPI_TypeDef* SPIt		SPI �˿ں�	
						u8 Word							��������
* ���ڲ�������
* ˵	������			
****************************************************************************/
void SPIxWordSend(SPI_TypeDef* SPIt,u32 Word)		 
{
	SPIxbyteSendGet(SPIt,(Word & 0xff000000) >> 24);
	SPIxbyteSendGet(SPIt,(Word & 0x00ff0000) >> 16);
	SPIxbyteSendGet(SPIt,(Word & 0x0000ff00) >> 8);
	SPIxbyteSendGet(SPIt,(Word & 0x000000ff) >> 0);
}

/****************************************************************************
* ��	�ƣ�u32 SPIxWordGet(SPI_TypeDef* SPIt)	
* ��	�ܣ�Ӳ��SPIxWord����
* ��ڲ�����SPI_TypeDef* SPIt		SPI �˿ں�
* ���ڲ��������յ���32λ����
* ˵	������			
****************************************************************************/
u32 SPIxWordGet(SPI_TypeDef* SPIt)		 
{
	u32 Word = 0;
	
	Word = (SPIxbyteSendGet(SPIt,0xA5) << 24);
	Word |= (SPIxbyteSendGet(SPIt,0xA5) << 16);
	Word |= (SPIxbyteSendGet(SPIt,0xA5) << 8);
	Word |= (SPIxbyteSendGet(SPIt,0xA5) << 0);
	
	return Word;
}

/****************************************************************************
* ��	�ƣ�void SPIS_Delay(u32 nCount)
* ��	�ܣ�һ��������Ƶ����ʱ
* ��ڲ�����u32 nCount	��ʱ��
* ���ڲ�������
* ˵	������
****************************************************************************/
static void SPIS_Delay(u32 nCount)
{
  while(nCount--);
}

/****************************************************************************
* ��	�ƣ�void SPISSendcmd8(SPI_Cfg* Sx,u8 cmd)
* ��	�ܣ�ģ��SPI����8λ����
* ��ڲ�����SPI_Cfg* Sx			SPI����
						u8 cmd					���͵�����
* ���ڲ�������
* ˵	������
****************************************************************************/
void SPISSendcmd8(SPI_Cfg* Sx,u8 cmd)
{
	u8 i;	//8λ�ۼ���
	
	GPIO_ResetBits(Sx->SPIx_SCK_Por,Sx->SPIx_SCK_Pin);								//ʱ�Ӹ�λ
	for(i = 0;i < 8;i++)			//�Ӹ�λ��ʼ��
	{
		if((cmd << i) & 0x80)   //װ������
			GPIO_SetBits(Sx->SPIx_MOSI_Por,Sx->SPIx_MOSI_Pin); 
    else
			GPIO_ResetBits(Sx->SPIx_MOSI_Por,Sx->SPIx_MOSI_Pin);	

	 	GPIO_ResetBits(Sx->SPIx_SCK_Por,Sx->SPIx_SCK_Pin);							//ʱ��׼�� 
	 	SPIS_Delay(2);
	 	GPIO_SetBits(Sx->SPIx_SCK_Por,Sx->SPIx_SCK_Pin);								//��������
	 	SPIS_Delay(2);
	 	GPIO_ResetBits(Sx->SPIx_SCK_Por,Sx->SPIx_SCK_Pin);							//�������
	}
}

/****************************************************************************
* ��	�ƣ�u8 SPISReciveData8(SPI_Cfg* Sx)
* ��	�ܣ�ģ��SPI����8λ����
* ��ڲ�����SPI_Cfg* Sx				SPI����
* ���ڲ�����u8								���յ���8λ����
* ˵	������
****************************************************************************/
u8 SPISReciveData8(SPI_Cfg* Sx)
{
	u8 temp = 0;						//��ȡ��������
	u8 i;										//8λ�ۼ���
	
	for(i = 0;i < 8;i++)		//�Ӹ�λ��ʼ��
	{
		GPIO_ResetBits(Sx->SPIx_SCK_Por,Sx->SPIx_SCK_Pin);										//ʱ��׼�� 
		SPIS_Delay(2);
		GPIO_SetBits(Sx->SPIx_SCK_Por,Sx->SPIx_SCK_Pin);											//��ʼ����
		temp <<= 1; 
		temp |= GPIO_ReadInputDataBit(Sx->SPIx_MISO_Por,Sx->SPIx_MISO_Pin);	 	//����
		SPIS_Delay(2);
		GPIO_ResetBits(Sx->SPIx_SCK_Por,Sx->SPIx_SCK_Pin);										//�������
	}
	return temp;
}

/****************************************************************************
* ��	�ƣ�u8 SPISbyteSendGet(SPI_Cfg* Sx,u8 cmd)
* ��	�ܣ�ģ��SPI���շ���8λ����
* ��ڲ�����SPI_Cfg* Sx			SPI�ӿ�
						u8 cmd					���͵�����
* ���ڲ�����u8							���յ�����
* ˵	������
****************************************************************************/
u8 SPISbyteSendGet(u8 cmd)		 
{
	u8 i;																		//8λ�ۼ���
	u8 temp = 0;														//��ȡ��������
	
	IoLo(FLASH_SCK);
	for(i = 0;i < 8;i++)										//�Ӹ�λ��ʼ��
	{
		if((cmd << i) & 0x80)   							//װ������
		{
			IoHi(FLASH_MOSI);
		}
    else
		{
			IoLo(FLASH_MOSI);	
		}			

		IoLo(FLASH_SCK);
	 	SPIS_Delay(2);
		IoHi(FLASH_SCK);
		temp <<= 1;
		temp |= GetIo(FLASH_MISO);
	 	SPIS_Delay(2);
	}
	return temp;
}

/****************************************************************************
* ��	�ƣ�void SPISWordSend(SPI_Cfg* Sx,u8 Word)
* ��	�ܣ�ģ��SPI Word����
* ��ڲ�����SPI_Cfg* Sx				SPI�ӿ�
						Word							���͵�����
* ���ڲ�������
* ˵	������			
****************************************************************************/
void SPISWordSend(SPI_Cfg* Sx,u32 Word)		 
{
	SPISbyteSendGet((Word & 0xff000000) >> 24);
	SPISbyteSendGet((Word & 0x00ff0000) >> 16);
	SPISbyteSendGet((Word & 0x0000ff00) >> 8);
	SPISbyteSendGet((Word & 0x000000ff) >> 0);
}

/****************************************************************************
* ��	�ƣ�u32 SPISWordGet(SPI_Cfg* Sx)	
* ��	�ܣ�ģ��SPI Word����
* ��ڲ�����SPI_Cfg* Sx				SPI�ӿ�
* ���ڲ��������յ���32λ����
* ˵	������			
****************************************************************************/
u32 SPISWordGet(SPI_Cfg* Sx)		 
{
	u32 Word = 0;
	
	Word = (SPISbyteSendGet(0xA5) << 24);
	Word |= (SPISbyteSendGet(0xA5) << 16);
	Word |= (SPISbyteSendGet(0xA5) << 8);
	Word |= (SPISbyteSendGet(0xA5) << 0);
	
	return Word;
}

/****************************************************************************
* ��	�ƣvvoid SPIx_Init(SPI_Cfg* Sx)
* ��	�ܣ�ͨ��SPI���ų�ʼ��
* ��ڲ�����SPI_Cfg* Sx			SPI���ò���
* ���ڲ�������
* ˵	������
****************************************************************************/
void SPIx_Init()
{
	GPIO_InitTypeDef G;						//���ű���

	//ѡ����IO��ʱ��
// 	if((FLASH_SCK_PORT == GPIOC) || (FLASH_MISO_PORT == GPIOC) || (FLASH_MOSI_PORT == GPIOC))
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);		//��������ʱ��C
	
// 	G.GPIO_Pin = FLASH_SCK_PIN;			//SCK
// 	G.GPIO_Mode = GPIO_Mode_OUT;
// 	G.GPIO_OType = GPIO_OType_PP;
// 	G.GPIO_Speed = GPIO_Speed_100MHz;
// 	G.GPIO_PuPd = GPIO_PuPd_NOPULL;
// 	GPIO_Init(FLASH_SCK_PORT, &G);
	
	IoSetMode(FLASH_SCK,IO_MODE_OUTPUT);
	IoPushPull(FLASH_SCK);
	IoSetSpeed(FLASH_SCK,IO_SPEED_100M);
	
	G.GPIO_Pin = FLASH_MOSI_PIN;			//SCK MOSI
	GPIO_Init(FLASH_MOSI_PORT, &G);		
	
// 	G.GPIO_Pin = FLASH_MISO_PIN;		//MISO
// 	G.GPIO_Mode = GPIO_Mode_IN;
// 	G.GPIO_OType = GPIO_OType_OD;
// 	G.GPIO_Speed = GPIO_Speed_2MHz;
// 	G.GPIO_PuPd = GPIO_PuPd_NOPULL;
// 	GPIO_Init(FLASH_MISO_PORT, &G);	

	IoSetMode(FLASH_MISO,IO_MODE_INPUT);
	IoOpenDrain(FLASH_MISO);
	IoSetSpeed(FLASH_MISO,IO_SPEED_2M);
}

/****************************************************************************
* ��	�ƣ�u32 SPIbyteSendGet(SPI_Cfg* Sx)	
* ��	�ܣ�ģ��SPIӲ��SPI���ֽڽ���
* ��ڲ�����SPI_Cfg* Sx		SPI����
* ���ڲ��������յ���8λ����
* ˵	������			
****************************************************************************/
u8 SPIbyteSendGet(u8 cmd)	 
{
		return SPISbyteSendGet(cmd);								//��Ҫ�õ�����
}

/****************************************************************************
* ��	�ƣ�u32 SPIWordGet(SPI_Cfg* Sx)	
* ��	�ܣ�ģ��SPIӲ��SPI�ֽ���
* ��ڲ�����SPI_Cfg* Sx		SPI����
* ���ڲ��������յ���32λ����
* ˵	������			
****************************************************************************/
u32 SPIWordGet(SPI_Cfg* Sx)		 
{
	if(Sx->SPIx == (SPI_TypeDef*)0)					//ģ��SPI
		return SPISWordGet(Sx);								//��Ҫ�õ�����
	else																		//Ӳ��SPI
		return SPIxWordGet(Sx->SPIx);					//ֻ���õ�SPI�˿ں�
}

/****************************************************************************
* ��	�ƣ�void SPIWordSend(SPI_Cfg* Sx,u32 Word)	
* ��	�ܣ�ģ��SPIӲ��SPI�ַ���
* ��ڲ�����SPI_Cfg* Sx				SPI����
* ���ڲ���������32λ����
* ˵	������			
****************************************************************************/
void SPIWordSend(SPI_Cfg* Sx,u32 Word)		 
{
	if(Sx->SPIx == (SPI_TypeDef*)0)						//ģ��SPI
		SPISWordSend(Sx,Word);									//��Ҫ�õ�����
	else																			//Ӳ��SPI
		SPIxWordSend(Sx->SPIx,Word);						//ֻ���õ�SPI�˿ں�
}
