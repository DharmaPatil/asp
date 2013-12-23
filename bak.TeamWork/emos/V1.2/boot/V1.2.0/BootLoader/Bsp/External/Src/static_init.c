#include "static_init.h"

/****************************************************************************
* ��	�ƣ�void Delay(const u32 nCount)
* ��	�ܣ�һ��������Ƶ����ʱ
* ��ڲ�����const u32 nCount	��ʱ��
* ���ڲ�������
* ˵	������
****************************************************************************/
void Delay(const u32 nCount)
{
	u32 i = nCount;
  while(i--);
}

/****************************************************************************
* ��	�ƣ�void Delay_MS(const u32 nCount_temp)
* ��	�ܣ����뼶
* ��ڲ�����const u32 nCount_temp	��ʱ��
* ���ڲ�������
* ˵	������
****************************************************************************/
void Delay_MS(const u32 nCount_temp)
{
	u32 nCount = nCount_temp * 8000;
	while(nCount--);
}

/****************************************************************************
* ��	�ƣ�void Delay_S(const u32 nCount_temp)
* ��	�ܣ��뼶
* ��ڲ�����const u32 nCount_temp	��ʱ��
* ���ڲ�������
* ˵	������
****************************************************************************/
void Delay_S(const u32 nCount_temp)
{
	u32 nCount = nCount_temp * 8000000;
	while(nCount--);
}

/****************************************************************************
* ��	�ƣ�void GetCpuIdHex(void)
* ��	�ܣ���ȡCPUоƬIDֵ16����   
* ��ڲ�������
* ���ڲ�������
* ˵	����6��u16��ֵ���CPUID
			u16 x[6] = {0};
			u8 x[12] = {0};
****************************************************************************/
void GetCpuIdHex(u8* HCpuId)
{
	u32 CpuId[3] = {0};

	CpuId[0] = (*(u32*)(0x1FFF7A10));
	CpuId[1] = (*(u32*)(0x1FFF7A14));
	CpuId[2] = (*(u32*)(0x1FFF7A18));
	
	HCpuId[0] = (u8)(CpuId[0] >> 24);
	HCpuId[1] = (u8)(CpuId[0] >> 16); 
	HCpuId[2] = (u8)(CpuId[0] >> 8);
	HCpuId[3] = (u8)(CpuId[0]);

	HCpuId[4] = (u8)(CpuId[1] >> 24);
	HCpuId[5] = (u8)(CpuId[1] >> 16); 
	HCpuId[6] = (u8)(CpuId[1] >> 8);
	HCpuId[7] = (u8)(CpuId[1]);

	HCpuId[8] = (u8)(CpuId[2] >> 24);
	HCpuId[9] = (u8)(CpuId[2] >> 16); 
	HCpuId[10] = (u8)(CpuId[2] >> 8);
	HCpuId[11] = (u8)(CpuId[2]);
}

/****************************************************************************
* ��	�ƣ�u32 BeiNumber(const u32 num_temp,const u8 len_temp)
* ��	�ܣ�10����
* ��ڲ�����const u32 num_temp	��Ҫ��������
			const u8 len_temp	�����ٱ� ��1������ٸ�0
* ���ڲ�����u32 ������ı���ֵ
* ���Դ��룺
	u32 x = 2;
	x = BeiNumber(x,3);
	//x = 2000;
****************************************************************************/
u32 BeiNumber(const u32 num_temp,const u8 len_temp)
{
	u8 i = len_temp;
	u32 Bei = 1;		 					 //Ĭ�ϱ���ֵ

	while(i--)								//���������з���
		Bei *= 10;	  					//��������
	
	return Bei * num_temp;	  //���ر���
}

/****************************************************************************
* ��	�ƣ�u32 ASCIItoNumber(u8* data_temp,const u8 len_temp)
* ��	�ܣ���ASCII������ת����Ϊu32������
* ��ڲ�����u8* data_temp	ASCII������
			const u8 len_temp		ASCII�����ָ���
* ���ڲ�����u32 unsigned int������
* ���Դ��룺
	u32 num=0;
	num=ASCIItoNumber("1234",4);
	//num=1234;
****************************************************************************/
u32 ASCIItoNumber(u8* data_temp,const u8 len_temp)
{
    u8 i = len_temp;													//����
    u32 val = 0; 
    u32 len = 0;                      				//���ճ��� 
    
    while(1)
    {
			val = data_temp[i - 1] - 0x30;     			//ȡ���� 
			val = BeiNumber(val,len_temp - i);     	//�õ�λ������
			len += val;                    					//λ��������� 
			
			i--;
			if(i == 0)
				 return len;
    } 
}

/****************************************************************************
* ��	�ƣ�u32 Get_SyS_CLK(const u8 temp)
* ��	�ܣ���ǰϵͳ����Ƶ��״̬��ȡ	   
* ��ڲ�����const u8 temp ѡ��Ƶ�ʲ���
* ���ڲ�����ʱ��Ƶ��
* ˵	����HCLKƵ��(1) PCLK1Ƶ��(2) PCLK2Ƶ��(3) SYSTEMƵ��(0����)
****************************************************************************/
u32 Get_SyS_CLK(const u8 temp)
{
	RCC_ClocksTypeDef RCC_ClockFreq;
	RCC_GetClocksFreq(&RCC_ClockFreq);	//Ϊʱ�ӱ�������Ƭ�ϸ�ʱ��
	switch (temp)
	{
		case 1:	return RCC_ClockFreq.HCLK_Frequency; 		//HCLKƵ��			168MHZ
		case 2:	return RCC_ClockFreq.PCLK1_Frequency; 	//PCLK1Ƶ��			42MHZ
		case 3:	return RCC_ClockFreq.PCLK2_Frequency; 	//PCLK2Ƶ��			84MHZ
		default: return RCC_ClockFreq.SYSCLK_Frequency;	//SYSTEMƵ��		168MHZ
	}	
}
