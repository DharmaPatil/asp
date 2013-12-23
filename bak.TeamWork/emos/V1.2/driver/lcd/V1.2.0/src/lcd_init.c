#include "lcd_init.h"
#include "includes.h"

#define LCD_SX 								240				//��
#define LCD_SY 								128				//��
#define LCD_BYTES_X  					30 				//��ʾ�����
#define TEXT_HOME_ADDRESS 		0x0000 		//�ı���ʾ���׵�ַ
#define GRAPHIC_HOME_ADDRESS 	0x01E0 		//ͼ����ʾ���׵�ַ

#define RowFlag			0											//˳���־
#define ChaosFlag		1											//�����־

typedef struct
{
	GPIO_TypeDef* LCDPor;				//�˿ں�			GPIOA - GPIOI
	u16 LCDPin;									//���ź�			GPIO_Pin_0 - GPIO_Pin_15
}LCD_IO;											//LCD IO����

//��������
static LCD_IO LCD_DataLine[8] = 							
{
	{GPIOG,GPIO_Pin_0},					//0
	{GPIOG,GPIO_Pin_1},					//1
	{GPIOG,GPIO_Pin_2},					//2
	{GPIOG,GPIO_Pin_3},					//3
	{GPIOG,GPIO_Pin_4},					//4
	{GPIOG,GPIO_Pin_5},					//5
	{GPIOG,GPIO_Pin_6},					//6
	{GPIOG,GPIO_Pin_7}					//7	
};
static u8 DataLineOffset = 0;	//������ƫ����	1 = 1������

//��������
static LCD_IO WR 			= {GPIOG,GPIO_Pin_8};			//LCD_WR				//д�ź�			L��Ч
static LCD_IO RD 			= {GPIOG,GPIO_Pin_9};			//LCD_RD				//���ź�			L��Ч
static LCD_IO CE 			= {GPIOG,GPIO_Pin_10};		//LCD_CE				//ʹ��				L��Ч
static LCD_IO CD 			= {GPIOG,GPIO_Pin_11};		//LCD_CD				//����or����	H���� L����
static LCD_IO RST 		= {GPIOG,GPIO_Pin_12};		//LCD_RST				//��λ				L��Ч
static LCD_IO FS 			= {GPIOG,GPIO_Pin_13};		//LCD_FS				//����				
static LCD_IO LED 		= {GPIOG,GPIO_Pin_15}; 		//LCD_LED				//����				H������ L������		
static LCD_IO AD_CTRL = {GPIOC,GPIO_Pin_8};			//LCD_AD_CTRL		//��������

static u8 DataLineChaosRowFlag = RowFlag;				//˳��������	�Զ��ж�
static GPIO_TypeDef* DataLineSamePort = 0;			//ͬһ�˿ں�		�Զ��ж�

/****************************************************************************
* ��	�ƣ�void LCD_Delay(u16 nDly)
* ��	�ܣ���ʱ
* ��ڲ�������ʱʱ��
* ���ڲ�������
* ˵	������		
****************************************************************************/
void LCD_Delay(u16 nDly)
{
	u32 temp = nDly * 2;
	while(temp--);
}

/****************************************************************************
* ��	�ƣ�u8 LCD_DataLineRead(void)
* ��	�ܣ����������ϵ�����
* ��ڲ�������
* ���ڲ�����u8		8λ�������ϵ�8λ����
* ˵	������
****************************************************************************/
u8 LCD_DataLineRead(void)
{
	u16 Readu16 = 0;
	u8 Readu8 = 0;
	
	GPIO_SetBits(AD_CTRL.LCDPor,AD_CTRL.LCDPin);		
	
	if(DataLineSamePort != (GPIO_TypeDef*)0)		//�����ͬһ�˿�
	{
		if(DataLineChaosRowFlag == RowFlag)				//�����˳������
		{
			Readu16 = GPIO_ReadInputData(DataLineSamePort);					//���˿��ϵ�16λ����
			Readu8 = (u8)((Readu16 >> DataLineOffset) & 0x00FF);		//ȡ��8λ����
		}
		else		//��˳��
		{
			Readu8 |=  GPIO_ReadInputDataBit(DataLineSamePort,LCD_DataLine[0].LCDPin);
			Readu8 |= (GPIO_ReadInputDataBit(DataLineSamePort,LCD_DataLine[1].LCDPin) << 1);
			Readu8 |= (GPIO_ReadInputDataBit(DataLineSamePort,LCD_DataLine[2].LCDPin) << 2);
			Readu8 |= (GPIO_ReadInputDataBit(DataLineSamePort,LCD_DataLine[3].LCDPin) << 3);
			Readu8 |= (GPIO_ReadInputDataBit(DataLineSamePort,LCD_DataLine[4].LCDPin) << 4);
			Readu8 |= (GPIO_ReadInputDataBit(DataLineSamePort,LCD_DataLine[5].LCDPin) << 5);
			Readu8 |= (GPIO_ReadInputDataBit(DataLineSamePort,LCD_DataLine[6].LCDPin) << 6);
			Readu8 |= (GPIO_ReadInputDataBit(DataLineSamePort,LCD_DataLine[7].LCDPin) << 7);
		}
	}
	else	//��ͬ�˿�
	{
		Readu8 |=  GPIO_ReadInputDataBit(LCD_DataLine[0].LCDPor,LCD_DataLine[0].LCDPin);
		Readu8 |= (GPIO_ReadInputDataBit(LCD_DataLine[1].LCDPor,LCD_DataLine[1].LCDPin) << 1);
		Readu8 |= (GPIO_ReadInputDataBit(LCD_DataLine[2].LCDPor,LCD_DataLine[2].LCDPin) << 2);
		Readu8 |= (GPIO_ReadInputDataBit(LCD_DataLine[3].LCDPor,LCD_DataLine[3].LCDPin) << 3);
		Readu8 |= (GPIO_ReadInputDataBit(LCD_DataLine[4].LCDPor,LCD_DataLine[4].LCDPin) << 4);
		Readu8 |= (GPIO_ReadInputDataBit(LCD_DataLine[5].LCDPor,LCD_DataLine[5].LCDPin) << 5);
		Readu8 |= (GPIO_ReadInputDataBit(LCD_DataLine[6].LCDPor,LCD_DataLine[6].LCDPin) << 6);
		Readu8 |= (GPIO_ReadInputDataBit(LCD_DataLine[7].LCDPor,LCD_DataLine[7].LCDPin) << 7);
	}
	
	GPIO_ResetBits(AD_CTRL.LCDPor,AD_CTRL.LCDPin);	
	
	return Readu8;																					//���ص�8λ����
}

/****************************************************************************
* ��	�ƣ�u8 LCDCheckBusy(u8 nMask)
* ��	�ܣ�æ���
* ��ڲ�����u16 nMask			æ�ؼ���
* ���ڲ�����u8 	�Ƿ�æ		1æ 0��
* ˵	������
****************************************************************************/
u8 LCDCheckBusy(u8 Mask)
{
	u8 Val = 0;			//LCD״ֵ̬
	u16 i;					//�ۼ���

	GPIO_SetBits(CD.LCDPor,CD.LCDPin);			//����
	GPIO_ResetBits(RD.LCDPor,RD.LCDPin);		//������
	
  for(i = 0;i < 10000;i++)								//����ʧ�ܴ���
	{
		Val = LCD_DataLineRead();							//��ȡ8λ����
		if((Val & Mask) == Mask)
			break;
	}
	GPIO_SetBits(RD.LCDPor,RD.LCDPin);			//��������
	
	if(i > 10000)
		return 1;					//æ
	else
		return 0;					//��
}

/****************************************************************************
* ��	�ƣ�void LCD_DataLineOut(void)
* ��	�ܣ���LCD����������Ϊͨ�����ģʽ
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void LCD_DataLineOut(void)
{
	u8 i = 0;			//�ۼ���
	u8 No = 0;		//���ź�
	u16 Pin = 0;	//��ʱ����ֵ
	
	if(DataLineSamePort != (GPIO_TypeDef*)0)		//�����ͬһ�˿�
	{
		if(DataLineChaosRowFlag == RowFlag)				//�����˳������
			DataLineSamePort->MODER |= 0x00005555 << (DataLineOffset * 2); 							//����������ģʽΪ���01
		else																			//��˳��
		{
			for(i = 0; i < 8; i++)									//��������
			{
				No = 0;																//���ź�
				Pin = LCD_DataLine[i].LCDPin;					//��ʱ���ź�
				
				while(1)
				{
					if((Pin & 0x01) == 0x01)						//�Ƿ��ҵ�����
						break;
					else
					{	
						Pin = Pin >> 1;										//���ź�λ��
						if(No < 15)												//û�г���������ź�
							No++;														//���ź�
						else
							break;
					}
				}
				DataLineSamePort->MODER |= 0x0001 << (No * 2); 		//������������ģʽ���01
			}
		}
	}
	else	//��ͬ�˿�
	{
		for(i = 0; i < 8; i++)									//��������
		{
			No = 0;																//���ź�
			Pin = LCD_DataLine[i].LCDPin;					//��ʱ���ź�
			
			while(1)
			{
				if((Pin & 0x01) == 0x01)						//�Ƿ��ҵ�����
					break;
				else
				{	
					Pin = Pin >> 1;										//���ź�λ��
					if(No < 15)												//û�г���������ź�
						No++;														//���ź�
					else
						break;
				}
			}
			LCD_DataLine[i].LCDPor->MODER |= 0x0001 << (No * 2); 		////��ͬ�˿�����ģʽ���01
		}
	}
}

/****************************************************************************
* ��	�ƣ�void LCD_DataLineIn(void)
* ��	�ܣ���LCD����������Ϊͨ������ģʽ
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void LCD_DataLineIn(void)
{
	u8 i;							//�ۼ���
	u16 uTmp = 0;			//��λ��Ĳ���
	u16 PinNo = 0;		//�ܽź�
	
	if(DataLineSamePort != (GPIO_TypeDef*)0)		//�����ͬһ�˿�
	{
		if(DataLineChaosRowFlag == RowFlag)				//�����˳������
		{	
			for(i = 0;i < DataLineOffset;i++)				//������λ���������
				uTmp = (uTmp << 2) + 1;
			DataLineSamePort->MODER &= 0xFFFF0000 << (DataLineOffset * 2) | uTmp; 		//�ı�����ģʽΪ����00
		}
		else						//��˳��
		{
			for(i = 0; i < 8; i++)		//��������
			{
				PinNo = LCD_DataLine[i].LCDPin / 2;		//�����ź�
				for(i = 0;i < PinNo;i++)							//������λ���������
					uTmp = (uTmp << 2) + 1;
				DataLineSamePort->MODER |= 0x00 << (PinNo * 2) | uTmp; 									//������������ģʽ���00
			}
		}
	}
	else	//��ͬ�˿�
	{
		for(i = 0; i < 8; i++)		//��������
		{
			PinNo = LCD_DataLine[i].LCDPin / 2;		//�����ź�
			for(i = 0;i < PinNo;i++)							//������λ���������
				uTmp = (uTmp << 2) + 1;
			LCD_DataLine[i].LCDPor->MODER |= 0x00 << (PinNo * 2) | uTmp; 							//��ͬ�˿�����ģʽ���00
		}
	}
}

/****************************************************************************
* ��	�ƣ�void LCD_DataLineWrite(u8 uData)
* ��	�ܣ���������д����
* ��ڲ�����u16 uData				8λ����
* ���ڲ�����
* ˵	������
****************************************************************************/
void LCD_DataLineWrite(u8 uData)
{
	u8 i = 0;						//�ۼ���
	u16 nTmp = uData;		//��Ҫ���͵�����
	
	if(DataLineSamePort != (GPIO_TypeDef*)0)		//�����ͬһ�˿�
	{
		if(DataLineChaosRowFlag == RowFlag)				//�����˳������
		{
			DataLineSamePort->BSRRH = 0x00FF << DataLineOffset;			//�������
			DataLineSamePort->BSRRL = nTmp << DataLineOffset ;			//д������
		}
		else											//��˳��
		{
			for(i = 0;i < 8;i++)		//��������
			{
				if((uData >> i) & 0x01 == 1)
					GPIO_SetBits(DataLineSamePort,LCD_DataLine[i].LCDPin);			//����
				else
					GPIO_ResetBits(DataLineSamePort,LCD_DataLine[i].LCDPin);		//����
			}
		}
	}
	else												//��ͬ�˿�
	{
		for(i = 0;i < 8;i++)			//��������
		{
			if((uData >> i) & 0x01 == 1)
				GPIO_SetBits(LCD_DataLine[i].LCDPor,LCD_DataLine[i].LCDPin);			//����
			else
				GPIO_ResetBits(LCD_DataLine[i].LCDPor,LCD_DataLine[i].LCDPin);		//����
		}
	}
}

/****************************************************************************
* ��	�ƣ�	u8 LCD_WriteData(u8 nData)
* ��	�ܣ�	дLCD����
* ��ڲ�����u8	nData		����
* ���ڲ�����u8	���������Ƿ�ɹ�			1�ɹ�	0ʧ��
* ˵	����	ʧ��Ϊæ״̬
****************************************************************************/
u8 LCD_WriteData(u8 nData)
{
	if(LCDCheckBusy(0x03) == 1)						//æ���
		return 0;
	
	LCD_DataLineOut();										//�������л������ģʽ
	GPIO_ResetBits(CD.LCDPor,CD.LCDPin);	//����
	GPIO_ResetBits(WR.LCDPor,WR.LCDPin);	//д����
	
	LCD_DataLineWrite(nData);							//��������
	LCD_Delay(80);												//��ʱ			�ȴ�д���

  GPIO_SetBits(WR.LCDPor,WR.LCDPin);		//������д
	LCD_DataLineIn();											//�������л�������ģʽ
		
	return 1;
}

/****************************************************************************
* ��	�ƣ�	u8 LCD_WriteCommand(u8 nCmd)
* ��	�ܣ�	дLCD����
* ��ڲ�����u8	nCmd 		����
* ���ڲ�����u8	���������Ƿ�ɹ�			1�ɹ�	0ʧ��
* ˵	����	ʧ��Ϊæ״̬
****************************************************************************/
u8 LCD_WriteCommand(u8 nCmd)
{
	if(LCDCheckBusy(0x03) == 1)						//æ���
		return 0;
	
	LCD_DataLineOut();										//�������л������ģʽ
	GPIO_SetBits(CD.LCDPor,CD.LCDPin);		//����
	GPIO_ResetBits(WR.LCDPor,WR.LCDPin);	//д����
	
	LCD_DataLineWrite(nCmd);							//��������
	LCD_Delay(80);												//��ʱ	
	
  GPIO_SetBits(WR.LCDPor,WR.LCDPin);		//������д
	LCD_DataLineIn();											//�������л�������ģʽ	
	return 1;
}

/****************************************************************************
* ��	�ƣ�	u8 LCD_WriteDataAuto(u8 nData)
* ��	�ܣ�	дLCD�����Զ�
* ��ڲ�����u8	nData		����
* ���ڲ�����u8	���������Ƿ�ɹ�			1�ɹ�	0ʧ��
* ˵	����	��
****************************************************************************/
u8 LCD_WriteDataAuto(u8 nData)
{
	if(LCDCheckBusy(0x08) == 1)
		return 0;
	
	LCD_DataLineOut();										//�������л������ģʽ
	GPIO_ResetBits(CD.LCDPor,CD.LCDPin);	//����
	GPIO_ResetBits(WR.LCDPor,WR.LCDPin);	//д����
	
	LCD_DataLineWrite(nData);							//��������
	LCD_Delay(80);												//��ʱ

  GPIO_SetBits(WR.LCDPor,WR.LCDPin);		//������д
	LCD_DataLineIn();											//�������л�������ģʽ
	
	return 1;
}

/****************************************************************************
* ��	�ƣ�	u8 send_2data_and_1cmd_to_lcm(u8 DataL,u8 DataH,u8 Cmd)
* ��	�ܣ�	д2��8λ���ݺ�1�����LCM
* ��ڲ�����u8 DataL			��8λ����
						u8 DataH			��8λ����
						u8 Cmd				����
* ���ڲ�����u8	���������Ƿ�ɹ�			1�ɹ�	0ʧ��
* ˵	����	��
****************************************************************************/
u8 send_2data_and_1cmd_to_lcm(u8 DataL,u8 DataH,u8 Cmd)
{
	if(LCD_WriteData(DataL) == 0)	return 0;		//��8λ����
	if(LCD_WriteData(DataH) == 0)	return 0;		//��8λ����
	if(LCD_WriteCommand(Cmd) == 0) return 0;	//8λ����
	
	return 1;
}

/****************************************************************************
* ��	�ƣ�	u8 send_1hex_data_and_1cmd_to_lcm(u16 Data,u8 Cmd)
* ��	�ܣ�	д1��16λ���ݺ�1�����LCM
* ��ڲ�����u8 DataL			��8λ����
						u8 DataH			��8λ����
						u8 Cmd				����
* ���ڲ�����u8	���������Ƿ�ɹ�			1�ɹ�	0ʧ��
* ˵	����	��
****************************************************************************/
u8 send_1hex_data_and_1cmd_to_lcm(u16 Data,u8 Cmd)
{
	u8 DataH = Data >> 8;
	u8 DataL = Data;
	
	if(LCD_WriteData(DataL) == 0)	return 0;		//��8λ����
	if(LCD_WriteData(DataH) == 0)	return 0;		//��8λ����
	if(LCD_WriteCommand(Cmd) == 0) return 0;	//8λ����
	
	return 1;
}

/****************************************************************************
* ��	�ƣ�	u8 ClearScreen(void)
* ��	�ܣ�	����
* ��ڲ�������
* ���ڲ�����u8 �����Ƿ�ɹ�		1�ɹ� 0ʧ��
* ˵	����	��
****************************************************************************/
u8 ClearScreen(void)
{
	u8 i,j;

	//�ı���ʾ���׵�ַ	//��ַָ��λ��(���ö�д����ָ��)
	if(send_1hex_data_and_1cmd_to_lcm(TEXT_HOME_ADDRESS,LCD_ADR_POS) == 0)	
		return 0;		
	
	if(LCD_WriteCommand(LCD_AUT_WR) == 0)		//�Զ�д���� 
		return 0;

	for(j = 0; j < 144; j++)	   						//����Ϊ144ͬʱ���ͼ��������
	{
		for(i = 0; i < 30; i++)
		{
			if(LCD_WriteDataAuto(0x00) == 0)		//�Զ�д	���RAM
				return 0;
		}
	}

	if(LCD_WriteCommand(LCD_AUT_OVR) == 0)	//�Զ���д���� 
		return 0;
	
	return 1;
}

/****************************************************************************
* ��	�ƣ�void DataLine_Check(void)
* ��	�ܣ������߼��
* ��ڲ�������
* ���ڲ�������
* ˵	����1������Ƿ���ͬһ�˿���
					2�����ź��Ƿ�����
****************************************************************************/
void DataLine_Check(void)
{
	u8 i;		//�ۼ���
	u8 CRFlag = 0;				//˳�������ѯ��־
	u8 PortFlag = 0;			//�˿ڲ�ѯ��־
	
	DataLineChaosRowFlag = RowFlag;						//˳��������
	DataLineSamePort = (GPIO_TypeDef*)0;			//ͬһ�˿ں�
	
	//��������
	for(i = 0 ; i < 8 ; i++)
	{	
		if(CRFlag == 0)			//û���ҹ�˳��������
		{
			if(LCD_DataLine[0].LCDPin << i != LCD_DataLine[i].LCDPin)		//���GPIO	Pin����������������
			{
				DataLineChaosRowFlag = ChaosFlag;		//����
				CRFlag = 1;												//���ҹ�˳������
			}
		}
		if(PortFlag == 0)		//û���ҹ��˿��Ƿ�һ��
		{
			if(LCD_DataLine[0].LCDPor != LCD_DataLine[i].LCDPor)				//���GPIO PORT��һ��������
			{
				DataLineSamePort = (GPIO_TypeDef*)0;			//��ͬһ�˿ں�		�˿����
				PortFlag = 1;															//���ҹ��˿�һ����
			}
		}
	}
	
	//��������������
	if(CRFlag == 0)		//ǰ��û��������
		DataLineChaosRowFlag = RowFlag;								//˳��
	
	if(PortFlag == 0)	//ǰ��û���ֶ˿ڷ�һ����
		DataLineSamePort = LCD_DataLine[0].LCDPor;		//һ��
}

/****************************************************************************
* ��	�ƣ�u8 LCD_PortInit(void)
* ��	�ܣ����ų�ʼ��
* ��ڲ�������
* ���ڲ�����u8	��ʼ���Ƿ�ɹ�			1�ɹ�	0ʧ��
* ˵	������		
****************************************************************************/
u8 LCD_PortInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//��������ʱ��
	if((LCD_DataLine[0].LCDPor == GPIOA) || (LCD_DataLine[1].LCDPor == GPIOA) || (LCD_DataLine[2].LCDPor == GPIOA) || (LCD_DataLine[3].LCDPor == GPIOA) || (LCD_DataLine[4].LCDPor == GPIOA) || (LCD_DataLine[5].LCDPor == GPIOA) || (LCD_DataLine[6].LCDPor == GPIOA) || (LCD_DataLine[7].LCDPor == GPIOA)   
		||(WR.LCDPor == GPIOA) || (RD.LCDPor == GPIOA) || (CE.LCDPor == GPIOA) || (CD.LCDPor == GPIOA) || (RST.LCDPor == GPIOA) || (FS.LCDPor == GPIOA) || (LED.LCDPor == GPIOA) || (AD_CTRL.LCDPor == GPIOA) )
		{RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);}		//��������ʱ��A	
	if((LCD_DataLine[0].LCDPor == GPIOB) || (LCD_DataLine[1].LCDPor == GPIOB) || (LCD_DataLine[2].LCDPor == GPIOB) || (LCD_DataLine[3].LCDPor == GPIOB) || (LCD_DataLine[4].LCDPor == GPIOB) || (LCD_DataLine[5].LCDPor == GPIOB) || (LCD_DataLine[6].LCDPor == GPIOB) || (LCD_DataLine[7].LCDPor == GPIOB)   
		||(WR.LCDPor == GPIOB) || (RD.LCDPor == GPIOB) || (CE.LCDPor == GPIOB) || (CD.LCDPor == GPIOB) || (RST.LCDPor == GPIOB) || (FS.LCDPor == GPIOB) || (LED.LCDPor == GPIOB) || (AD_CTRL.LCDPor == GPIOB) )
		{RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);}		//��������ʱ��B	
	if((LCD_DataLine[0].LCDPor == GPIOC) || (LCD_DataLine[1].LCDPor == GPIOC) || (LCD_DataLine[2].LCDPor == GPIOC) || (LCD_DataLine[3].LCDPor == GPIOC) || (LCD_DataLine[4].LCDPor == GPIOC) || (LCD_DataLine[5].LCDPor == GPIOC) || (LCD_DataLine[6].LCDPor == GPIOC) || (LCD_DataLine[7].LCDPor == GPIOC)   
		||(WR.LCDPor == GPIOC) || (RD.LCDPor == GPIOC) || (CE.LCDPor == GPIOC) || (CD.LCDPor == GPIOC) || (RST.LCDPor == GPIOC) || (FS.LCDPor == GPIOC) || (LED.LCDPor == GPIOC) || (AD_CTRL.LCDPor == GPIOC) )
		{RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);}		//��������ʱ��C	
	if((LCD_DataLine[0].LCDPor == GPIOD) || (LCD_DataLine[1].LCDPor == GPIOD) || (LCD_DataLine[2].LCDPor == GPIOD) || (LCD_DataLine[3].LCDPor == GPIOD) || (LCD_DataLine[4].LCDPor == GPIOD) || (LCD_DataLine[5].LCDPor == GPIOD) || (LCD_DataLine[6].LCDPor == GPIOD) || (LCD_DataLine[7].LCDPor == GPIOD)   
		||(WR.LCDPor == GPIOD) || (RD.LCDPor == GPIOD) || (CE.LCDPor == GPIOD) || (CD.LCDPor == GPIOD) || (RST.LCDPor == GPIOD) || (FS.LCDPor == GPIOD) || (LED.LCDPor == GPIOD) || (AD_CTRL.LCDPor == GPIOD) )
		{RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);}		//��������ʱ��D
	if((LCD_DataLine[0].LCDPor == GPIOE) || (LCD_DataLine[1].LCDPor == GPIOE) || (LCD_DataLine[2].LCDPor == GPIOE) || (LCD_DataLine[3].LCDPor == GPIOE) || (LCD_DataLine[4].LCDPor == GPIOE) || (LCD_DataLine[5].LCDPor == GPIOE) || (LCD_DataLine[6].LCDPor == GPIOE) || (LCD_DataLine[7].LCDPor == GPIOE)   
		||(WR.LCDPor == GPIOE) || (RD.LCDPor == GPIOE) || (CE.LCDPor == GPIOE) || (CD.LCDPor == GPIOE) || (RST.LCDPor == GPIOE) || (FS.LCDPor == GPIOE) || (LED.LCDPor == GPIOE) || (AD_CTRL.LCDPor == GPIOE) )
		{RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);}		//��������ʱ��E
	if((LCD_DataLine[0].LCDPor == GPIOF) || (LCD_DataLine[1].LCDPor == GPIOF) || (LCD_DataLine[2].LCDPor == GPIOF) || (LCD_DataLine[3].LCDPor == GPIOF) || (LCD_DataLine[4].LCDPor == GPIOF) || (LCD_DataLine[5].LCDPor == GPIOF) || (LCD_DataLine[6].LCDPor == GPIOF) || (LCD_DataLine[7].LCDPor == GPIOF)   
		||(WR.LCDPor == GPIOF) || (RD.LCDPor == GPIOF) || (CE.LCDPor == GPIOF) || (CD.LCDPor == GPIOF) || (RST.LCDPor == GPIOF) || (FS.LCDPor == GPIOF) || (LED.LCDPor == GPIOF) || (AD_CTRL.LCDPor == GPIOF) )
		{RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);}		//��������ʱ��F
	if((LCD_DataLine[0].LCDPor == GPIOG) || (LCD_DataLine[1].LCDPor == GPIOG) || (LCD_DataLine[2].LCDPor == GPIOG) || (LCD_DataLine[3].LCDPor == GPIOG) || (LCD_DataLine[4].LCDPor == GPIOG) || (LCD_DataLine[5].LCDPor == GPIOG) || (LCD_DataLine[6].LCDPor == GPIOG) || (LCD_DataLine[7].LCDPor == GPIOG)   
		||(WR.LCDPor == GPIOG) || (RD.LCDPor == GPIOG) || (CE.LCDPor == GPIOG) || (CD.LCDPor == GPIOG) || (RST.LCDPor == GPIOG) || (FS.LCDPor == GPIOG) || (LED.LCDPor == GPIOG) || (AD_CTRL.LCDPor == GPIOG) )
		{RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);}		//��������ʱ��G
	if((LCD_DataLine[0].LCDPor == GPIOH) || (LCD_DataLine[1].LCDPor == GPIOH) || (LCD_DataLine[2].LCDPor == GPIOH) || (LCD_DataLine[3].LCDPor == GPIOH) || (LCD_DataLine[4].LCDPor == GPIOH) || (LCD_DataLine[5].LCDPor == GPIOH) || (LCD_DataLine[6].LCDPor == GPIOH) || (LCD_DataLine[7].LCDPor == GPIOH)   
		||(WR.LCDPor == GPIOH) || (RD.LCDPor == GPIOH) || (CE.LCDPor == GPIOH) || (CD.LCDPor == GPIOH) || (RST.LCDPor == GPIOH) || (FS.LCDPor == GPIOH) || (LED.LCDPor == GPIOH) || (AD_CTRL.LCDPor == GPIOH) )
		{RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);}		//��������ʱ��H
	if((LCD_DataLine[0].LCDPor == GPIOI) || (LCD_DataLine[1].LCDPor == GPIOI) || (LCD_DataLine[2].LCDPor == GPIOI) || (LCD_DataLine[3].LCDPor == GPIOI) || (LCD_DataLine[4].LCDPor == GPIOI) || (LCD_DataLine[5].LCDPor == GPIOI) || (LCD_DataLine[6].LCDPor == GPIOI) || (LCD_DataLine[7].LCDPor == GPIOI)   
		||(WR.LCDPor == GPIOI) || (RD.LCDPor == GPIOI) || (CE.LCDPor == GPIOI) || (CD.LCDPor == GPIOI) || (RST.LCDPor == GPIOI) || (FS.LCDPor == GPIOI) || (LED.LCDPor == GPIOI) || (AD_CTRL.LCDPor == GPIOI) )
		{RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE);}		//��������ʱ��I		

	//����ģʽ�������
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;  
		
	//��������
	GPIO_InitStructure.GPIO_Pin   = WR.LCDPin;				//LCD_WR
	GPIO_Init(WR.LCDPor, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin   = RD.LCDPin;				//LCD_RD
	GPIO_Init(RD.LCDPor, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin   = CE.LCDPin;				//LCD_CE
	GPIO_Init(CE.LCDPor, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin   = CD.LCDPin;				//LCD_CD
	GPIO_Init(CD.LCDPor, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin   = RST.LCDPin;				//LCD_RST
	GPIO_Init(RST.LCDPor, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin   = FS.LCDPin;				//LCD_FS
	GPIO_Init(FS.LCDPor, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin   = LED.LCDPin;				//LCD_LED
	GPIO_Init(LED.LCDPor, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin   = AD_CTRL.LCDPin;		//LCD_AD_CTRL
	GPIO_Init(AD_CTRL.LCDPor, &GPIO_InitStructure);
	
	//��������
	GPIO_InitStructure.GPIO_Pin   = LCD_DataLine[0].LCDPin;		//LCD_D0
	GPIO_Init(LCD_DataLine[0].LCDPor, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin   = LCD_DataLine[1].LCDPin;		//LCD_D1
	GPIO_Init(LCD_DataLine[1].LCDPor, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin   = LCD_DataLine[2].LCDPin;		//LCD_D2
	GPIO_Init(LCD_DataLine[2].LCDPor, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin   = LCD_DataLine[3].LCDPin;		//LCD_D3
	GPIO_Init(LCD_DataLine[3].LCDPor, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin   = LCD_DataLine[4].LCDPin;		//LCD_D4
	GPIO_Init(LCD_DataLine[4].LCDPor, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin   = LCD_DataLine[5].LCDPin;		//LCD_D5
	GPIO_Init(LCD_DataLine[5].LCDPor, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin   = LCD_DataLine[6].LCDPin;		//LCD_D6
	GPIO_Init(LCD_DataLine[6].LCDPor, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin   = LCD_DataLine[7].LCDPin;		//LCD_D7
	GPIO_Init(LCD_DataLine[7].LCDPor, &GPIO_InitStructure);
	
	GPIO_ResetBits(RST.LCDPor,RST.LCDPin);			//��λ
	GPIO_SetBits(RST.LCDPor,RST.LCDPin);				//��λ��Ч
	GPIO_SetBits(CE.LCDPor,CE.LCDPin);					//оƬ��Ч
	GPIO_SetBits(WR.LCDPor,WR.LCDPin);					//д��Ч
	GPIO_SetBits(RD.LCDPor,RD.LCDPin);					//����Ч
	GPIO_SetBits(CD.LCDPor,CD.LCDPin);					//����ģʽ
	GPIO_ResetBits(FS.LCDPor,FS.LCDPin);				//ʹ��8*8����ģʽ

	DataLine_Check();				//�˿�һ���ԺͶ˿������Լ��
	
	GPIO_ResetBits(RST.LCDPor,RST.LCDPin);						//��λ
	SysTimeDly(1);					//����5������ʱ�����ڼ��ɸ�λ
  GPIO_SetBits(RST.LCDPor,RST.LCDPin);							//��λ���
	
	GPIO_ResetBits(CE.LCDPor,CE.LCDPin);							//ʹ��LCD
	
	//��ʼ������
	if(send_1hex_data_and_1cmd_to_lcm(TEXT_HOME_ADDRESS, LCD_TXT_STP) == 0) 		return 0; //�ı���ʾ���׵�ַ
	if(send_2data_and_1cmd_to_lcm(LCD_BYTES_X, 0x00, LCD_TXT_WID) == 0) 				return 0;	//�ı���ʾ�����	
	if(send_1hex_data_and_1cmd_to_lcm(GRAPHIC_HOME_ADDRESS, LCD_GRH_STP) == 0) 	return 0;	//ͼ����ʾ���׵�ַ	
	if(send_2data_and_1cmd_to_lcm(LCD_BYTES_X, 0x00, LCD_GRH_WID) == 0) 				return 0; //ͼ����ʾ�����	
	if(send_2data_and_1cmd_to_lcm(0x02,0x00,LCD_CGR_POS) == 0) 									return 0; //CGRAMƫ�õ�ַ����	
	if(LCD_WriteCommand(LCD_MOD_OR) == 0) 																			return 0;	//�߼�"��"	
	if(ClearScreen() == 0)																											return 0;	//���������ٿ���ʾ		
	if(LCD_WriteCommand(LCD_DIS_GON) == 0) 																			return 0; //����ʾ  												//����ͼ����ʾ
		
	return 1;
}

/****************************************************************************
* ��	�ƣ�void SetLCDLED(u8 OnOff)
* ��	�ܣ����ñ���
* ��ڲ�����u8 OnOff		���⿪��		1��0��
* ���ڲ�������
* ˵	������		
****************************************************************************/
void SetLCDLED(u8 OnOff)
{
	if(OnOff > 0)
		GPIO_SetBits(LED.LCDPor,LED.LCDPin);				//�򿪱���
	else
		GPIO_ResetBits(LED.LCDPor,LED.LCDPin);			//�رձ���
}

/****************************************************************************
* ��	�ƣ�u8 LCD_init(void)
* ��	�ܣ����ų�ʼ��
* ��ڲ�������
* ���ڲ�����u8	��ʼ���Ƿ�ɹ�			1�ɹ�	0ʧ��
* ˵	�����ޱ������п��ܳ�ʼ��δͨ��	
****************************************************************************/
u8 LCD_init(void)
{
	if(LCD_PortInit() == 0)			//���ų�ʼ��
		return 0;	
	SetLCDLED(1);								//��ʼ����ͨ������򿪱���	
	return 1;
}

/****************************************************************************
* ��	�ƣ�	u8 SetAddr(u16 offset)
* ��	�ܣ�	ͼ����ʾ���׵�ַƫ����
* ��ڲ�����u16 offset			//ƫ�Ƶ�ַ
* ���ڲ�����u8		�Ƿ�ִ�гɹ�		1�ɹ�	0ʧ��
* ˵	����	��
****************************************************************************/
u8 SetAddr(u16 offset)
{
	if(send_1hex_data_and_1cmd_to_lcm(GRAPHIC_HOME_ADDRESS + offset,LCD_ADR_POS) == 0)		//ͼ����ʾ���׵�ַ	//��ַָ��λ��(���ö�д����)  
		return 0;
	else
		return 1;
}
