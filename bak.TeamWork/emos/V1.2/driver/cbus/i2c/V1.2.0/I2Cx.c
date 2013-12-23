#include "I2Cx.h"
#include "dt3102_IO.h"

#define IIC_SCK_PORT	GPIOB						//SCK
#define IIC_SCK_PIN		GPIO_Pin_6
#define IIC_SDA_PORT	GPIOB						//SDA
#define IIC_SDA_PIN		GPIO_Pin_7

// #define SET_IIC_SCK		

/****************************************************************************
* ��	�ƣ�void I2Cm_SDAO(I2C_Cfg* Ix,u8 Flag)
* ��	�ܣ�I2C�����������������  ģ��
* ��ڲ���: I2C_Cfg* Ix		I2C����
						u8 Flag				�ߵ�	1�� 0��
* ���ڲ�������
* ˵	������
****************************************************************************/
void I2Cm_SDAO(u8 Flag)
{
	if(Flag == 1)
// 		GPIO_SetBits(Ix->SDA_Port,Ix->SDA_Pin);			//����
	IIC_SDA_PORT->BSRRL = IIC_SDA_PIN;
	else
// 		GPIO_ResetBits(Ix->SDA_Port,Ix->SDA_Pin);		//����
	IIC_SDA_PORT->BSRRH = IIC_SDA_PIN;
}

/****************************************************************************
* ��	�ƣ�u8 I2Cm_SDAI(I2C_Cfg* Ix)
* ��	�ܣ�I2C��������			ģ��
* ��ڲ���: I2C_Cfg* Ix		I2C����
* ���ڲ�����u8						����������
* ˵	������
****************************************************************************/
u8 I2Cm_SDAI()
{
// 	return GPIO_ReadInputDataBit(IIC_SDA_PORT,IIC_SDA_PIN);	
	return GetIo(IIC_SDA);
}

/****************************************************************************
* ��	�ƣ�void I2Cm_SCKO(I2C_Cfg* Ix,u8 Flag)
* ��	�ܣ�ʱ����������		ģ��
* ��ڲ���: I2C_Cfg* Ix		I2C����
						u8 Flag				�ߵ�	1�� 0��
* ���ڲ�������
* ˵	������
****************************************************************************/
void I2Cm_SCKO(u8 Flag)
{
	if(Flag == 1)
// 		GPIO_SetBits(Ix->SCK_Port,Ix->SCK_Pin);
	IIC_SCK_PORT->BSRRL = IIC_SCK_PIN;
	else
// 		GPIO_ResetBits(Ix->SCK_Port,Ix->SCK_Pin);
	IIC_SCK_PORT->BSRRH = IIC_SCK_PIN;
}

/****************************************************************************
* ��	�ƣ�void I2CmDelay(u32 nCount)
* ��	�ܣ�һ��������Ƶ����ʱ		ģ��
* ��ڲ�����u32 nCount	��ʱ��
* ���ڲ�������
* ˵	������
****************************************************************************/
void I2CmDelay(u32 nCount)
{
  while(nCount--);
}

/****************************************************************************
* ��	�ƣ�u8 I2Cx_Init(I2C_Cfg* I2C_t)
* ��	�ܣ�I2C���ų�ʼ��
* ��ڲ�����I2C_Cfg* I2C_t		I2C������Ϣ
* ���ڲ�����u8							��ʼ���Ƿ�ɹ�	1�ɹ�	0ʧ��
* ˵	����I2C���ű�������Ϊ��©��� 
****************************************************************************/
u8 I2Cx_Init()
{
// 	GPIO_InitTypeDef P; 										//����GPIO�ṹ��

// 	if((IIC_SDA_PORT == GPIOB) || (IIC_SCK_PORT == GPIOB))
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	
// 	P.GPIO_Pin = IIC_SDA_PIN;					//SDA
// 	P.GPIO_Mode = GPIO_Mode_OUT;					//��������Ϊ���
// 	P.GPIO_OType = GPIO_OType_OD;					//���ÿ�©���
// 	P.GPIO_Speed = GPIO_Speed_50MHz; 			//������Ӧ�ٶ�Ϊ50M
// 	GPIO_Init(IIC_SDA_PORT, &P);
	IoSetMode(IIC_SDA,IO_MODE_OUTPUT);
	IoOpenDrain(IIC_SDA);
	IoSetSpeed(IIC_SDA,IO_SPEED_50M);

// 	P.GPIO_Pin = IIC_SCK_PIN;					//SCK
// 	P.GPIO_Mode = GPIO_Mode_OUT;					//��������Ϊ���
// 	P.GPIO_OType = GPIO_OType_OD;					//���ÿ�©���
// 	P.GPIO_Speed = GPIO_Speed_50MHz; 			//������Ӧ�ٶ�Ϊ50M
// 	GPIO_Init(IIC_SCK_PORT, &P);
	IoSetMode(IIC_SCK,IO_MODE_OUTPUT);
	IoOpenDrain(IIC_SCK);
	IoSetSpeed(IIC_SCK,IO_SPEED_50M);


// 	I2Cm_SCKO(1);		 								//��ʼ������
// 	I2Cm_SDAO(1);										//��ʼ������
	IoHi(IIC_SCK);
	IoHi(IIC_SDA);
	return 1;
}

/****************************************************************************
* ��	�ƣ�u8 I2Cx_Start(I2C_Cfg* Ix)
* ��	�ܣ�����I2C����
* ��ڲ�����
* ���ڲ�����u8									�����Ƿ�ɹ�		1�ɹ�	0ʧ��
* ˵	��������I2C����ͨѶ	   SCLH��SDAH-SDAL��ʱ�Ӹ����ݸߵ������俪ʼ
****************************************************************************/
u8 I2Cx_Start()				//I2C����	
{
// 	I2Cm_SDAO(1);
	IoHi(IIC_SDA);
	I2CmDelay(20);

// 	I2Cm_SCKO(1);
	IoHi(IIC_SCK);
	I2CmDelay(20);

	if(I2Cm_SDAI() == 0)		//SDA��δ������������æ,�˳�
		return 0;
				
// 	I2Cm_SDAO(0);					//������
	IoLo(IIC_SDA);
	I2CmDelay(20);						// ����0.6uS  //72������1us

	if(I2Cm_SDAI() == 1) 		//SDA��δ�����������߳���,�˳�
		return 0;
				
// 	I2Cm_SDAO(0);
	IoLo(IIC_SDA);
	I2CmDelay(20);
	
	return 1;						//�����ɹ�
}

/****************************************************************************
* ��	�ƣ�u8 I2Cx_Stop(I2C_Cfg* Ix)
* ��	�ܣ��ر�I2C����
* ��ڲ�����I2C_Cfg* Ix						I2C����
* ���ڲ�����u8		�ر��Ƿ�ɹ�		1�ɹ�	0ʧ��
* ˵	��������I2C����ͨѶ	   SCLH��SDAL-SDAH��ʱ�Ӹ����ݵ͵����������
****************************************************************************/
u8 I2Cx_Stop()								//I2C�ر�
{
// 	I2Cm_SDAO(0);
	IoLo(IIC_SDA);
	I2CmDelay(20);

// 	I2Cm_SCKO(1);
	IoHi(IIC_SCK);
	I2CmDelay(20);						//����0.6uS

// 	I2Cm_SDAO(1);					//������
	IoHi(IIC_SDA);
	I2CmDelay(20);						//����1.3uS

// 	I2Cm_SDAO(0);
	IoLo(IIC_SDA);
	I2CmDelay(20);
	return 1;
}

/****************************************************************************
* ��	�ƣ�u8 I2Cx_Ack(I2C_Cfg* Ix)
* ��	�ܣ�����һ��Ӧ���ź�  ��Ӧ��
* ��ڲ�����I2C_Cfg* Ix								I2C����
* ���ڲ�����u8		����ִ���Ƿ�ɹ�		1�ɹ�	0ʧ��
* ˵	����ʱ���ɵ�-��-�����䣬����SDA_L����
****************************************************************************/
u8 I2Cx_Ack()								//Ӧ��	   ʱ���ɵ�-��-�����䣬����һ������
{	
// 	if(Ix == (I2C_Cfg*)0)						//ָ����
// 		return 0;
		
// 	if(Ix->I2Cx == (I2C_TypeDef*)0)	//ģ��I2C
// 	{
		I2Cm_SCKO(0);
		I2CmDelay(20);

		I2Cm_SDAO(0);										//��������SDA_L��Ӧ��
		I2CmDelay(20);											//����1.3uS

		I2Cm_SCKO(1);		
		I2CmDelay(20);											//����1uS

		I2Cm_SCKO(0);
		I2CmDelay(20);
// 	}
// 	else												//Ӳ��I2C
// 	{
// 		return 0;									//δʵ��
// 	}
	return 1;
}

/****************************************************************************
* ��	�ƣ�u8 I2Cx_NoAck(I2C_Cfg* Ix)
* ��	�ܣ�������Ӧ���ź�	 �߲�Ӧ��
* ��ڲ�����I2C_Cfg* Ix								I2C����
* ���ڲ�����u8		����ִ���Ƿ�ɹ�		1�ɹ�	0ʧ��
* ˵	����ʱ���ɵ�-��-�����䣬����SDA_H����
****************************************************************************/
u8 I2Cx_NoAck()				//��Ӧ��
{	
// 	if(Ix == (I2C_Cfg*)0)						//ָ����
// 		return 0;
		
// 	if(Ix->I2Cx == (I2C_TypeDef*)0)	//ģ��I2C
// 	{
		I2Cm_SCKO(0);
		I2CmDelay(20);

		I2Cm_SDAO(1);		   				//��������SDA_H����Ӧ��
		I2CmDelay(20);

		I2Cm_SCKO(1);
		I2CmDelay(20);

		I2Cm_SCKO(0);
		I2CmDelay(20);
// 	}
// 	else												//Ӳ��I2C
// 	{
// 		return 0;									//δʵ��
// 	}
	return 1;
}

/****************************************************************************
* ��	�ƣ�u8 I2Cx_WaitAck(I2C_Cfg* Ix)
* ��	�ܣ��ȴ�Ӧ���ź�
* ��ڲ�����I2C_Cfg* Ix								I2C����
* ���ڲ�����u8		Ӧ����Ӧ��������
* ˵	����Ӧ��1����Ӧ��0
****************************************************************************/
u8 I2Cx_WaitAck()
{
	u8 ack;	  //Ӧ���ź�
// 	I2Cm_SCKO(0);
// 	I2Cm_SDAO(1);
	IoLo(IIC_SCK);
	IoHi(IIC_SDA);
					
	I2CmDelay(20);
// 	I2Cm_SCKO(1);
	IoHi(IIC_SCK);
	I2CmDelay(20);

	if(I2Cm_SDAI() == 1) 	  //��Ӧ���ȡΪ�ߵ�ƽ
		ack = 0;
	else 
		ack = 1;	 //��ȡӦ���ź�	 1��Ӧ��  0��Ӧ��

// 	I2Cm_SCKO(0);
	IoLo(IIC_SCK);
	return ack;
}

/****************************************************************************
* ��	�ƣ�u8 I2Cx_SendByte(I2C_Cfg* Ix,u8 SendByte) 
* ��	�ܣ���I2C�ӿ��Ϸ����ֽ�����
* ��ڲ�����I2C_Cfg* Ix								I2C����
						SendByte				Ҫ���͵��ֽ�
* ���ڲ�����u8							ִ���Ƿ�ɹ�	1�ɹ�	0ʧ��
* ˵	�������ֽ�ʱ�ظ����ͼ���
****************************************************************************/
u8 I2Cx_SendByte(u8 SendByte) //���ݴӸ�λ����λ
{
	u8 i = 8;
	while(i--)		  							//����8λ����
	{
// 		I2Cm_SCKO(0);
		IoLo(IIC_SCK);

		I2CmDelay(20);

		if((SendByte & 0x80) == 0x80)
// 			I2Cm_SDAO(1);  
			IoHi(IIC_SDA);
		else 
// 			I2Cm_SDAO(0); 
			IoLo(IIC_SDA);

		SendByte <<= 1;
		I2CmDelay(20);					//����1.3uS

// 		I2Cm_SCKO(1);
		IoHi(IIC_SCK);
		I2CmDelay(20);					//����1uS
	}
// 	I2Cm_SCKO(0);
	IoLo(IIC_SCK);
	return 1;
}

/****************************************************************************
* ��	�ƣ�u8 I2Cx_ReceiveByte(I2C_Cfg* Ix)
* ��	�ܣ���I2C�ӿ��Ͻ����ֽ�����
* ��ڲ�����I2C_Cfg* Ix								I2C����
* ���ڲ�����u8								1�ֽڽ�������
* ˵	�������ֽ��ظ����ռ���
****************************************************************************/
u8 I2Cx_ReceiveByte()  //���ݴӸ�λ����λ
{ 
	u8 i = 8;				  							//8λ
	u8 ReceiveByte = 0;	  					//1�ֽڽ������ݻ���
	
// 	if(Ix == (I2C_Cfg*)0)						//ָ����
// 		return 0;
		
// 	if(Ix->I2Cx == (I2C_TypeDef*)0)	//ģ��I2C
// 	{
		I2Cm_SDAO(1);
					
		while(i--)
		{
			ReceiveByte <<= 1; 
					 
			I2Cm_SCKO(0);
			I2CmDelay(20);					//����1.3uS

			I2Cm_SCKO(1);
			I2CmDelay(20);
				
			if(I2Cm_SDAI() == 1)
				ReceiveByte |= 0x01;
		}

		I2Cm_SCKO(0);
		return ReceiveByte;	   		//���ؽ��յ�������
// 	}
// 	else												//Ӳ��I2C
// 	{
// 		return 0;									//δʵ��
// 	}
}

/****************************************************************************
* ��	�ƣ�u32 I2CSendByteADDRData(I2C_Cfg* Ix,u8 DeviceAddr,u8 ADDR,u8* WData,u32 Length)
* ��	�ܣ�����8λ��ַ����
* ��ڲ�����I2C_Cfg* Rx					I2C������Ϣ
						u8 DeviceAddr				������ַ
						u8 ADDR							Ŀ��Ĵ���
						u8* WData						����
* ���ڲ�����u8									�������ݵĸ���
* ˵	������Ŀ��������ַ��Ŀ��Ĵ�������������				���֧��256��ַ
****************************************************************************/
u32 I2CSendByteADDRData(I2C_Cfg* Ix,u8 DeviceAddr,u8 ADDR,u8* WData,u32 Length)
{
	u32 Cnt = 0;											//ʵ�ʷ��͵����ݸ���
	
// 	if(Ix == (I2C_Cfg*)0)							//ָ����
// 		return 0;
	
	if(I2Cx_Start() == 0)						//I2C��ʼʱ�Է�����æ
		return 0;	  		

  I2Cx_SendByte(DeviceAddr);			//������ַ 
  if(I2Cx_WaitAck() == 0)					//�ȴ�Ӧ��
	{
		I2Cx_Stop(); 
		return 0;	  									
	}

  I2Cx_SendByte(ADDR);						//����8λ��ַ    
  if(I2Cx_Ack() == 0)							//�ȴ�Ӧ��
	{
		I2Cx_Stop();
		return 0;	  						
	}

	while(Length)		  								//����ָ����������
	{
		I2Cx_SendByte(*WData);				//��������
		if(I2Cx_Ack() == 0)						//�ȴ�Ӧ��
		{
			I2Cx_Stop();
			return 0;	  				
		}
		WData++;
		Cnt++;
		Length--;
	}
	
  I2Cx_Stop(); 		  							//�ر�I2C	  //�������ر�
	return Cnt;		  									//���͵����ݸ���
}

/****************************************************************************
* ��	�ƣ�u8 I2CSendWordADDRData(u8 DeviceAddr,u16 ADDR,u8* WData,u32 Length)
* ��	�ܣ�����16λ��ַ����
* ��ڲ�����
						u8 DeviceAddr				������ַ
						u16 ADDR						Ŀ��Ĵ���
						u8 WData						����
* ���ڲ�����u32									�Ƿ�ɹ�		1�ɹ�	0ʧ��
* ˵	������Ŀ��������ַ��Ŀ��Ĵ�������������			���֧��65536����ַ
****************************************************************************/
u32 I2CSendWordADDRData(u8 DeviceAddr,u16 ADDR,u8* WData,u32 Length)
{	
	u32 Cnt = 0;											//ʵ�ʷ��͵����ݸ���
		
	if(I2Cx_Start() == 0)						//I2C��ʼʱ�Է�����æ
		return 0;	  		

  I2Cx_SendByte(DeviceAddr);			//������ַ 
  if(I2Cx_WaitAck() == 0)					//�ȴ�Ӧ��
	{
		I2Cx_Stop(); 
		return 0;	  							
	}

  I2Cx_SendByte((u8)(ADDR >> 8));			//���ø�8��ַ 
	if(I2Cx_WaitAck() == 0)							//�ȴ�Ӧ��
	{
		I2Cx_Stop(); 
		return 0;	  											
	}
	
  I2Cx_SendByte((u8)(ADDR % 256));		//���õ�8��ַ 
	if(I2Cx_WaitAck() == 0)							//�ȴ�Ӧ��
	{
		I2Cx_Stop();
		return 0;	  		
	}
	
	while(Length)		  									//����ָ����������
	{
		I2Cx_SendByte(*WData);					//��������
		if(I2Cx_Ack() == 0)							//�ȴ�Ӧ��
		{
			I2Cx_Stop();
			return 0;	  							
		}
		WData++;
		Cnt++;
		Length--;
	}
	
  I2Cx_Stop(); 		  							//�ر�I2C	  //�������ر�
  return Cnt;		  									//���͵����ݸ���
}

/****************************************************************************
* ��	�ƣ�u32 I2CReceiveByteADDRData(I2C_Cfg* Ix,u8 DeviceAddr,u8 ADDR,u8* RData,u32 Length)
* ��	�ܣ���������
* ��ڲ�����I2C_Cfg* Ix					I2C������Ϣ
						u8 DeviceAddr				������ַ
						u8 ADDR							Ŀ��Ĵ���
						u8* RData						��ȡ������
						u32 Length					ϣ����ȡ�����ݳ���
* ���ڲ�����u32									ʵ�ʶ�ȡ�����ݸ���
* ˵	������Ŀ��������ַ��Ŀ��Ĵ�������������
****************************************************************************/
u32 I2CReceiveByteADDRData(u8 DeviceAddr,u8 ADDR,u8* RData,u32 Length)
{		
	u32 Cnt = 0;											//ʵ�ʽ��յ����ݸ���
	
// 	if(Ix == (I2C_Cfg*)0)							//ָ����
// 		return 0;
	
	if(I2Cx_Start() == 0)	  				//I2C����
		return 0;	   										//�Է�����æ

	I2Cx_SendByte(DeviceAddr);			//������ַ
	if(I2Cx_WaitAck() == 0)					//�ȴ�Ӧ��
	{
		I2Cx_Stop();
		return 0;	
	}
	
	I2Cx_SendByte(ADDR);						//����8λ��ַ      
	if(I2Cx_WaitAck() == 0)					//�ȴ�Ӧ��
	{
		I2Cx_Stop();
		return 0;	   
	}
	
	I2Cx_Stop();
	
	I2Cx_SendByte(DeviceAddr | 0x01);					//������ַ ��
	if(I2Cx_WaitAck() == 0)
	{
		I2Cx_Stop();
		return 0;	   //δ�ܽ��յ�Ӧ��
	}

	while(Length)		  //����ָ����������
	{
		*RData = I2Cx_ReceiveByte();
		if(Length == 1)
		{
			if(I2Cx_NoAck() == 0)	   //���ݽ�����Ͳ���Ӧ��
			{
				I2Cx_Stop();
				return 0;	   //δ�ܽ��յ�Ӧ��
			}
		}
		else 
		{
			if(I2Cx_Ack() == 0)
			{
				I2Cx_Stop();
				return 0;	   //δ�ܽ��յ�Ӧ��
			}
		}
		RData++;
		Cnt++;
		Length--;
	}

	I2Cx_Stop();			   //�ر�I2C
	return Cnt;		   			//ʵ�ʽ��յ����ݸ���
}

/****************************************************************************
* ��	�ƣ�u32 I2CReceiveWordADDRData(I2C_Cfg* Ix,u8 DeviceAddr,u16 ADDR,u8* RData,u32 Length)
* ��	�ܣ���������
* ��ڲ�����I2C_Cfg* Ix					I2C������Ϣ
						u8 DeviceAddr				������ַ
						u16 ADDR						Ŀ��Ĵ���
						u8* RData						��ȡ������
						u32 Length					ϣ����ȡ�����ݳ���
* ���ڲ�����s32									��ȡ�����ݸ���
* ˵	������Ŀ��������ַ��Ŀ��Ĵ�������������				���֧��65536����ַ
****************************************************************************/
s32 I2CReceiveWordADDRData(u8 DeviceAddr,u16 ADDR,u8* RData,u32 Length)
{		
	u32 Cnt = 0;											//��ȡ�����ݸ���
	
	
	if(I2Cx_Start() == 0)	  				//I2C����
		return -1;	   										//�Է�����æ

	I2Cx_SendByte(DeviceAddr);			//������ַ
	if(I2Cx_WaitAck() == 0)					//�ȴ�Ӧ��
	{
		I2Cx_Stop();
		return -1;	
	}
	
	I2Cx_SendByte((u8)(ADDR >> 8));	//���ø�8λ��ַ      
	if(I2Cx_WaitAck() == 0)						//�ȴ�Ӧ��
	{
		I2Cx_Stop();
		return -1;	   
	}
	
	I2Cx_SendByte((u8)(ADDR % 256));	//���õ�8λ��ַ      
	if(I2Cx_WaitAck() == 0)						//�ȴ�Ӧ��
	{
		I2Cx_Stop();
		return -1;	   
	}
	
	I2Cx_Stop();
	
	I2Cx_SendByte(DeviceAddr | 0x01);					//������ַ ��
	if(I2Cx_WaitAck() == 0)
	{
		I2Cx_Stop();
		return -1;	   //δ�ܽ��յ�Ӧ��
	}

	while(Length)		  //����ָ����������
	{
		*RData = I2Cx_ReceiveByte();
		if(Length == 1)
		{
			if(I2Cx_NoAck() == 0)	   //���ݽ�����Ͳ���Ӧ��
			{
				I2Cx_Stop();
				return -1;	   //δ�ܽ��յ�Ӧ��
			}
		}
		else 
		{
			if(I2Cx_Ack() == 0)
			{
				I2Cx_Stop();
				return -1;	   //δ�ܽ��յ�Ӧ��
			}
		}
		RData++;				//��ַƫ��
		Cnt++;					//��ȡ�����ݸ���
		Length--;				//��Ҫ���յ����ݸ���
	}

	I2Cx_Stop();		//�ر�I2C
	return Cnt;		   	//ʵ�����ݸ���
}
