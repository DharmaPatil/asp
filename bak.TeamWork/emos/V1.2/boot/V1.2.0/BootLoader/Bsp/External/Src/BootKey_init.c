#include "BootKey_init.h"
#include "ConnectCfg.h"				//BootKeyPin

#ifdef BootKey								//����������
/****************************************************************************
* ��	�ƣ�void BootKey_init(void)
* ��	�ܣ�����������ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void BootKey_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(BootKeyRCC, ENABLE);					//�򿪶˿�ʱ��

	//����� Keyһ������
	GPIO_InitStructure.GPIO_Pin = 	BootKeyPin;					//�����
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;				//����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;		//Ƶ��
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;			//��©
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;  	//����������
	GPIO_Init(BootKeyPort, &GPIO_InitStructure);
    
	//���͵�ƽ	Key��һ������
	GPIO_InitStructure.GPIO_Pin = 	GPIO_Pin_9;					//��ƽ��
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;			//���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;		//Ƶ��
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//����
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;  	//����������
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	BootKeyDownVal == Bit_RESET ? GPIO_SetBits(GPIOE,GPIO_Pin_9) : GPIO_ResetBits(GPIOE,GPIO_Pin_9);	//����Ϊ�͵�ƽ
}
#endif
