
#include "LCD.h"
#include "LCD_cfg.h"
#include <stdbool.h>

#define LCD_BYTES_X  			30 			//��ʾ�����
#define TEXT_HOME_ADDRESS 		0x0000 		//�ı���ʾ���׵�ַ
#define GRAPHIC_HOME_ADDRESS 	0x01E0 		//ͼ����ʾ���׵�ַ

#define	GPIO_SET(x)		{LCD_##x##_PORT -> BSRRL = LCD_##x##_PIN;}
#define	GPIO_RESET(x)	{LCD_##x##_PORT -> BSRRH = LCD_##x##_PIN;}

#define	LCD_WRITE_DATA(data)	LCD_DATA1_PORT -> BSRRH = 0x00FF;LCD_DATA1_PORT -> BSRRL = (data) ;		//д����
#define	LCD_SET_DATA_OUT()		LCD_DATA1_PORT->MODER |= 0x00005555; 									//����������Ϊ���ģʽ
#define	LCD_SET_DATA_IN()		  LCD_DATA1_PORT->MODER &= 0xffff0000; 									//����������Ϊ����ģʽ

/****************************************************************************
* ��	�ƣ�u8 fLCD_ReadLEDstate(void)
* ��	�ܣ�������״̬
* ��ڲ�����
* ���ڲ�����u8	1:�����Ѵ򿪣�	0�������ѹر�
* ˵	������
****************************************************************************/
u8 fLCD_ReadLEDstate(void)
{
	return GPIO_ReadOutputDataBit(LCD_LED_PORT , LCD_LED_PIN);
}
/****************************************************************************
* ��	�ƣ�void fLCD_LEDON(void)
* ��	�ܣ���LCD����
* ��ڲ�����
* ���ڲ�����
* ˵	������
****************************************************************************/
void fLCD_LEDON(void)
{
	GPIO_SET(LED);
}
/****************************************************************************
* ��	�ƣ�void fLCD_LEDOFF(void)
* ��	�ܣ��ر�LCD����
* ��ڲ�����
* ���ڲ�����
* ˵	������
****************************************************************************/
void fLCD_LEDOFF(void)
{
	GPIO_RESET(LED);
}
/****************************************************************************
* ��	�ƣ�void fLCD_ReadData_Row(void)
* ��	�ܣ���LCD���ݣ�LCD������Ϊ������������
* ��ڲ�������
* ���ڲ�����LCD data
* ˵	������
****************************************************************************/
static u8 fLCD_ReadData_Row(void)
{
	u8 uTmp;							//IO������Ϊ����ģʽ
	GPIO_SET(AD_CTRL);
	uTmp = (u8)(LCD_DATA1_PORT ->IDR );	//������
	GPIO_RESET(AD_CTRL);				//IO������Ϊ���ģʽ
	return uTmp;
}

static void WhileDly(u32 nDly)
{
	while(nDly--);
}

static void LCD_PortInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);		//��������ʱ��G
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);		//��������ʱ��G

 	//���ö�д����Ϊ�������
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;  
	GPIO_InitStructure.GPIO_Pin   = LCD_WR_PIN|LCD_RD_PIN|LCD_CE_PIN|LCD_CD_PIN|LCD_RST_PIN|LCD_FS_PIN|LCD_LED_PIN/
									LCD_DATA1_PIN|LCD_DATA2_PIN|LCD_DATA3_PIN|LCD_DATA4_PIN|LCD_DATA5_PIN|LCD_DATA6_PIN|LCD_DATA7_PIN|LCD_DATA8_PIN;		//LCD_WR
	GPIO_Init(LCD_WR_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin   = LCD_AD_CTRL_PIN ;					
	GPIO_Init(LCD_AD_CTRL_PORT, &GPIO_InitStructure);	
					
	GPIO_RESET(RST);
	
	GPIO_SET(CE);
	GPIO_SET(WR);
	GPIO_SET(AD_CTRL);
	GPIO_SET(RD);
	GPIO_SET(CD);
	GPIO_RESET(FS);		//ʹ��8*8ģʽ
	GPIO_SET(LED);		//�򿪱���
}

/****************************************************************************
* ��	�ƣ�	bool CheckBusy(unsigned char nMask)
* ��	�ܣ�	LCDæ���
* ��ڲ�����	u8:	��æ����	
* ���ڲ�����	bool:	false--æ	;	true--��æ
* ˵	����	��
****************************************************************************/
static bool CheckBusy(unsigned char nMask)
{
	unsigned char nState;
	unsigned short int i = 0;

	GPIO_SET(CD);
	GPIO_RESET(RD);
	LCD_Delay(NS100_DLY(1));		 //��ʱ

    do{
		nState = fLCD_ReadData_Row();
		if(++i > 10000)
		{
			GPIO_SET(RD);
			return false;
		}
	}while((nState & nMask) != nMask);

	GPIO_SET(RD);
	return true;
}
/****************************************************************************
* ��	�ƣ�	void LCD_WriteCommand(uint8 nCmd)
* ��	�ܣ�	дLCD����
* ��ڲ�����u8	����
* ���ڲ�������
* ˵	����	��
****************************************************************************/
void LCD_WriteCommand(unsigned char nCmd)
{
	CheckBusy(0x03);
	
	GPIO_SET(CD);
	LCD_WRITE_DATA(nCmd);		//��MCUд�����ݣ�׼����LCD�������
	
	GPIO_RESET(WR);
	LCD_SET_DATA_OUT();
	LCD_Delay(NS100_DLY(2));	//��ʱ	

	GPIO_SET(WR);
	LCD_Delay(NS100_DLY(4));
	LCD_SET_DATA_IN();
}

void LCD_WriteData(unsigned char nData)
{
	CheckBusy(0x03);
	
	GPIO_RESET(CD);
	LCD_WRITE_DATA(nData);		//��MCUд�����ݣ�׼����LCD�������

	GPIO_RESET(WR);
	LCD_SET_DATA_OUT();
	LCD_Delay(NS100_DLY(2));	//��ʱ	

	GPIO_SET(WR);
	LCD_Delay(NS100_DLY(4));
	LCD_SET_DATA_IN();
}

static void LCD_WriteDataAuto(unsigned char nData)
{
	CheckBusy(0x08);
	
	GPIO_RESET(CD);
	LCD_WRITE_DATA(nData);		//��MCUд�����ݣ�׼����LCD�������

	GPIO_RESET(WR);
	LCD_SET_DATA_OUT();
	LCD_Delay(NS100_DLY(2));	//��ʱ	

	GPIO_SET(WR);
	LCD_Delay(NS100_DLY(4));
	LCD_SET_DATA_IN();
}

//д2�����ݺ�1�����LCM..........
static void send_2data_and_1cmd_to_lcm(uint8 lcm_data_l,uint8 lcm_data_h,uint8 lcm_cmd)
{
	LCD_WriteData(lcm_data_l);
	LCD_WriteData(lcm_data_h);
	LCD_WriteCommand(lcm_cmd);
}
//д1��16�������ݺ�1�����LCM..........
static void send_1hex_data_and_1cmd_to_lcm(uint16 lcm_data,uint8 lcm_cmd)
{
	LCD_WriteData(lcm_data);
	LCD_WriteData(lcm_data>>8);
	LCD_WriteCommand(lcm_cmd);
}


void SetAddr(uint16 nAddr)
{
	send_1hex_data_and_1cmd_to_lcm(GRAPHIC_HOME_ADDRESS + nAddr, LCD_ADR_POS);
}

void ClearScreen(void)
{
	uint8 i,j;

	send_1hex_data_and_1cmd_to_lcm(TEXT_HOME_ADDRESS, LCD_ADR_POS);

	LCD_WriteCommand(LCD_AUT_WR);

	for(j=0; j<144; j++)	   //����Ϊ144ͬʱ���ͼ��������
	{
		for(i=0; i<30; i++)
		{
			LCD_WriteDataAuto(0x00);
		}
	}

	LCD_WriteCommand(LCD_AUT_OVR);
}
/****************************************************************************
* ��	�ƣ�void LCD_Init(void)
* ��	�ܣ���ʼ��LCD
* ��ڲ�������
* ���ڲ�����
* ˵	������
****************************************************************************/
void LCD_Init(void)
{
	//LCD�ܽų�ʼ��
	LCD_PortInit();	

	GPIO_RESET(RST);
	SysTimeDly(2);	
	GPIO_SET(RST);
	SysTimeDly(1);

	//ʹ��Ƭѡ
	GPIO_RESET(CE);

	//��ʼ������
	send_1hex_data_and_1cmd_to_lcm(TEXT_HOME_ADDRESS, LCD_TXT_STP);  	//�ı���ʾ���׵�ַ
	send_2data_and_1cmd_to_lcm(LCD_BYTES_X, 0x00, LCD_TXT_WID);  		//�ı���ʾ�����
	send_1hex_data_and_1cmd_to_lcm(GRAPHIC_HOME_ADDRESS, LCD_GRH_STP); 	//ͼ����ʾ���׵�ַ
	send_2data_and_1cmd_to_lcm(LCD_BYTES_X, 0x00, LCD_GRH_WID);  		//ͼ����ʾ�����
	send_2data_and_1cmd_to_lcm(0x02,0x00,LCD_CGR_POS);   				//CGRAMƫ�õ�ַ����
	LCD_WriteCommand(LCD_MOD_OR);      									//�߼�"��"

	//���������ٿ���ʾ
	ClearScreen();

	//����ʾ
	LCD_WriteCommand(LCD_DIS_GON);      								//����ͼ����ʾ
}

