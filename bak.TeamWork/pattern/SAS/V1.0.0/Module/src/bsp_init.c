#include "includes.h"
#include "bsp_init.h"			

/****************************************************************************
* ��	�ƣ�void Bsp_Init(void)
* ��	�ܣ�������ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void Bsp_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//����4������ܽ�
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE,ENABLE);	  										//�����������ʱ��
	
	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;  
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_Init(GPIOE, &GPIO_InitStructure);	

	//����10������
	GPIOF->MODER &= 0xffc00000;
	//���ñ궨PD10
	GPIOD->MODER &= 0xffcfffff;
}

