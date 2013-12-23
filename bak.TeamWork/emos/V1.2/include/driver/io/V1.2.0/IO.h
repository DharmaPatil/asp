/****************************************Copyright (c)**************************************************
**                              
**                                 
**                                  
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: IO.h
**��   ��   ��: 
**�� �� ��  ��: 
**��        ��: 
**����޸�����: 
**��        ��: �����˹���CPU��һЩ������IO����(STM32F40Xϵ��)
********************************************************************************************************/
#ifndef		_IO_ACCESS_H_
#define		_IO_ACCESS_H_

//�������ѡ��
#define IO_MODE_INPUT 		0x00000000UL
#define IO_MODE_OUTPUT		0x55555555UL
#define IO_MODE_ALTER			0xAAAAAAAAUL
#define IO_MODE_ANALOG		0xFFFFFFFFUL

//�����ʽ
#define IO_PUSH_PULL			0x0000UL
#define IO_OPEN_DRAIN			0xFFFFUL

//�ٶ�ѡ��
#define IO_SPEED_2M 		0x00000000UL
#define IO_SPEED_25M		0x55555555UL
#define IO_SPEED_50M		0xAAAAAAAAUL
#define IO_SPEED_100M		0xFFFFFFFFUL




//����IO�˿�����
#define IoSetMode(bit, mode) (bit##_PORT##->MODER) &= ~(((unsigned long)(bit##_PIN) * (unsigned long)(bit##_PIN)) | (((unsigned long)(bit##_PIN) * (unsigned long)(bit##_PIN))<<1)); \
														 (bit##_PORT##->MODER) |= ((((unsigned long)(bit##_PIN) * (unsigned long)(bit##_PIN)) | (((unsigned long)(bit##_PIN) * (unsigned long)(bit##_PIN))<<1)) & mode)

#define IoSetSpeed(bit, speed) (bit##_PORT##->OSPEEDR) &= ~(((unsigned long)(bit##_PIN) * (unsigned long)(bit##_PIN)) | (((unsigned long)(bit##_PIN) * (unsigned long)(bit##_PIN))<<1)); \
																(bit##_PORT##->OSPEEDR) |= ((((unsigned long)(bit##_PIN) * (unsigned long)(bit##_PIN)) | (((unsigned long)(bit##_PIN) * (unsigned long)(bit##_PIN))<<1)) & speed)

#define IoPushPull(bit) 		(bit##_PORT##->OTYPER) &= (~(bit##_PIN))
#define IoOpenDrain(bit) 		(bit##_PORT##->OTYPER) |= (bit##_PIN)


//����IO״̬
#define IoHi(bit)							(bit##_PORT##->BSRRL) = bit##_PIN
#define IoLo(bit)							(bit##_PORT##->BSRRH) = bit##_PIN 
																		
//��ȡIO״̬
#define GetIo(bit)						(((bit##_PORT##->IDR & (bit##_PIN)) > 0) ? 1 : 0)



/*****************

1���˿ں����Ŷ��巽��
����˿�ʱ������Ҫ���Ƶ���������ΪSCK�����붨��
#define SCK_PORT					GPIOA
#define SCK_PIN						GPIO_Pin_1


2��ʹ�÷���
����ߵ�ƽʱ���� IoHi(SCK);
����͵�ƽʱ���� IoLo(SCK);
��ȡIO��ƽʱ���� GetIo(SCK);
����IO�˿����ԣ���������Ϊ50MHz�ٶ������������
                 IoSetMode(SCK, IO_MODE_OUTPUT);
								 IoSetSpeed(SCK, IO_SPEED_50M);
								 IoPushPull(SCK);

******************/





		
//************************************************************************
#endif

