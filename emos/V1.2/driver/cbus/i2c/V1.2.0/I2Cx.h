#ifndef _I2CX_H_
#define _I2CX_H_

#ifdef __cplusplus		   		//�����CPP����C���� //��ʼ����
extern "C" {
#endif

#include "stm32f4xx.h"
	
typedef struct
{
	I2C_TypeDef*		I2Cx;						//I2Cѡ��0-IO��ģ��I2C��1-I2C1��2-I2C2��3-I2C3��������Ч
	u16 						SCK_Pin;				//SCK���ź�			GPIO_Pin_0 - GPIO_Pin_15
	GPIO_TypeDef* 	SCK_Port;				//SCK�˿ں�			GPIOA - GPIOI
	u16 						SDA_Pin;				//SDA���ź�			GPIO_Pin_0 - GPIO_Pin_15
	GPIO_TypeDef* 	SDA_Port;				//SDA�˿ں�			GPIOA - GPIOI
}I2C_Cfg;													//I2C����	

/****************************************************************************
* ��	�ƣ�u8 I2Cx_Init(I2C_Cfg* I2C_t)
* ��	�ܣ�I2C���ų�ʼ��
* ��ڲ�����I2C_Cfg* I2C_t		I2C������Ϣ
* ���ڲ�����u8							��ʼ���Ƿ�ɹ�	1�ɹ�	0ʧ��
* ˵	����I2C���ű�������Ϊ��©��� 
****************************************************************************/
u8 I2Cx_Init(void);															//I2C���ų�ʼ��

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
u32 I2CSendByteADDRData(I2C_Cfg* Ix,u8 DeviceAddr,u8 ADDR,u8* WData,u32 Length);	//8λ���͵�ַ����

/****************************************************************************
* ��	�ƣ�u8 I2CSendWordADDRData(I2C_Cfg* Ix,u8 DeviceAddr,u16 ADDR,u8* WData,u32 Length)
* ��	�ܣ�����16λ��ַ����
* ��ڲ�����I2C_Cfg* Rx					I2C������Ϣ
						u8 DeviceAddr				������ַ
						u16 ADDR						Ŀ��Ĵ���
						u8 WData						����
* ���ڲ�����u32									�Ƿ�ɹ�		1�ɹ�	0ʧ��
* ˵	������Ŀ��������ַ��Ŀ��Ĵ�������������			���֧��65536����ַ
****************************************************************************/
u32 I2CSendWordADDRData(u8 DeviceAddr,u16 ADDR,u8* WData,u32 Length);	//16λ���͵�ַ����

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
u32 I2CReceiveByteADDRData(u8 DeviceAddr,u8 ADDR,u8* RData,u32 Length);			//8λ��ַ��������

/****************************************************************************
* ��	�ƣ�u32 I2CReceiveWordADDRData(I2C_Cfg* Ix,u8 DeviceAddr,u16 ADDR,u8* RData,u32 Length)
* ��	�ܣ���������
* ��ڲ�����I2C_Cfg* Ix					I2C������Ϣ
						u8 DeviceAddr				������ַ
						u16 ADDR						Ŀ��Ĵ���
						u8* RData						��ȡ������
						u32 Length					ϣ����ȡ�����ݳ���
* ���ڲ�����s32									��ȡ�����ݸ���
* ˵	������Ŀ��������ַ��Ŀ��Ĵ�������������
****************************************************************************/
s32 I2CReceiveWordADDRData(u8 DeviceAddr,u16 ADDR,u8* RData,u32 Length);		//16λ��ַ��������

#ifdef __cplusplus		   		//�����CPP����C���� //��������
}
#endif

#endif

