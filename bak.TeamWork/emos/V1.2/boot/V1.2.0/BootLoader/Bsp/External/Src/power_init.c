#include "power_init.h"			
#include "static_init.h"		//Delay_MS

#define DELAYCNT  50			//���ʱ����1����ʱ1MS

void Power_init(void)
{
	GPIO_InitTypeDef P; 
  int i;						 //�ػ�����������
    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	P.GPIO_Pin = GPIO_Pin_3;			   	//������
  P.GPIO_Mode  = GPIO_Mode_OUT;			//���
  P.GPIO_Speed = GPIO_Speed_25MHz;	//����Ƶ��
  P.GPIO_OType = GPIO_OType_PP;			//�������
  P.GPIO_PuPd  = GPIO_PuPd_NOPULL; 	//����������
	GPIO_Init(GPIOE, &P);
	
  Power_Close;						   				//��ʼ��Ϊ�ػ�

	P.GPIO_Pin = GPIO_Pin_4;			  	//�����
  P.GPIO_Mode  = GPIO_Mode_IN;			//����
  P.GPIO_Speed = GPIO_Speed_25MHz;	//����Ƶ��
  P.GPIO_OType = GPIO_OType_OD;			//��©���
  P.GPIO_PuPd  = GPIO_PuPd_NOPULL; 	//����������
	GPIO_Init(GPIOE, &P);

  for(i = 0;i < DELAYCNT;i++) 			  //�������50��
	{
		if(PowerLine_Read != 0) 		  //�������£�����������
			break;						  //�����ɿ���������ֹͣ���˳��������
		Delay_MS(1);					  //�����ʱ��
	}

	if(i >= DELAYCNT) 					  //�����������ﵽ�������
		Power_Open;		//��Դ��
	else 								  //����������û�ﵽ�������
	{
		Power_Close;	//���ɹػ�
		while(1);
	}
}


