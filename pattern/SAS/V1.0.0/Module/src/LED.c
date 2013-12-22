
#include "LED.h"



/****************************************************************************
* ��	�ƣ�void fIO_Init(void)
* ��	�ܣ���ʼ��IO
* ��ڲ�������
* ���ڲ�������
* ˵	����
****************************************************************************/	
void fLED_IO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//��������ܽ�
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOD,ENABLE);	 	//�����������ʱ��	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;  
	GPIO_InitStructure.GPIO_Pin   = LED4_PIN;
	GPIO_Init(LED4_PORT, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin   = LED1_PIN;
	GPIO_Init(LED1_PORT, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin   = LED2_PIN;
	GPIO_Init(LED2_PORT, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin   = LED3_PIN;
	GPIO_Init(LED3_PORT, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin   = LED5_PIN;
	GPIO_Init(LED5_PORT, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin   = LED6_PIN;
	GPIO_Init(LED6_PORT, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin   = LED7_PIN;
	GPIO_Init(LED7_PORT, &GPIO_InitStructure);	
	LED3_OFF;
	LED4_OFF;
	LED5_OFF;
	LED6_OFF;
	LED7_OFF;
}




