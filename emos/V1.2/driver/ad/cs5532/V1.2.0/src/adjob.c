#include "adjob.h"
#include "driver.h"
#include "String.h"

#include "ADInterface.h"
#include "spi_cs5532A.h"
#include "spi_cs5532B.h"

static const u8 ADJob_pDisc[] = "ADJob\r\n";					

static SYS_EVENT* ADTimeEvent = (SYS_EVENT*)0;							//����ʱ�����ź���
static HANDLE TIM6_Dev = 0;												//�豸���

//TIM6ͷ�ļ����ݰ���
typedef void(*TIM6_fun)(void); 											//����ָ��
extern u8 TIM6_Config(TIM6_fun Cfun,u16 TimeUS,u8 mode);	 	//��ʱ����ʼ��
#define TIM6_CMDTimOpen				1				//������ʱ�������ж���Ӧ
#define TIM6_CMDTimClose			2				//�رն�ʱ���ر��ж���Ӧ
#define Mode_10US					1				//10΢��

static u8 u8ErrContAD1 = 0;							//����AD�������
static u8 u8ErrContAD2 = 0;
static u8 u8ErrContAD3 = 0;
static u8 u8ErrContAD4 = 0;
static u8 u8StateAD	= 0;							//����AD״̬

static u8 u8ResetTimCntAD1 = 0;						//�������³�ʼ��ADͨ���ļ�ʱ����(16��--20ms)
static u8 u8ResetTimCntAD2 = 0;
static u8 u8ResetTimCntAD3 = 0;
static u8 u8ResetTimCntAD4 = 0;

#define	RE_INIT_AD_COUNT	0x07					//AD����ô������������

/****************************************************************************
* ��	�ƣ�static s8 ADJOB_Open(u32 lParam)
* ��	�ܣ�ͳһ�ӿ�
* ��ڲ�����u32 lParam		��������
* ���ڲ�����u8				�򿪲�������ֵ	��HVL_ERR_CODE��
* ˵	������
****************************************************************************/
static s8 ADJOB_Open(u32 lParam)
{
	if(DeviceIoctl(TIM6_Dev,TIM6_CMDTimOpen,0) == 0)			//������ʱ�������ж���Ӧ
		return 1;												//ʧ��	
	else
		return 0;												//�ɹ�
}

/****************************************************************************
* ��	�ƣ�static s8 ADJOB_Close(u32 lParam)
* ��	�ܣ�ͳһ�ӿ�
* ��ڲ�����u32 lParam		��������
* ���ڲ�����u8				�򿪲�������ֵ	��HVL_ERR_CODE��
* ˵	������
****************************************************************************/
static s8 ADJOB_Close(u32 lParam)
{
	if(DeviceIoctl(TIM6_Dev,TIM6_CMDTimClose,0) == 0)			//�رն�ʱ�������ж���Ӧ
		return 1;				//ʧ��
	return 0;					//�ɹ�
}

/****************************************************************************
* ��	�ƣ�s32 ADJOB_Read(u8* buffer,u32 len) 
* ��	�ܣ���
* ��ڲ�����u8* buffer		���ջ�����			����ADֵ(���ڵ���16�ֽ�)
			u32 len			��Ҫ���յĳ���		��Ҫ���ڵ���16
* ���ڲ�����s32				�����Ƿ�ɹ�		1�ɹ�	-1��-2ʧ��
* ˵	��������Ӧ�ò�����
****************************************************************************/
static s32 ADJOB_Read(u8* buffer,u32 len) 
{
	u8 err = 0;	
	SysSemPend(ADTimeEvent,0,&err);				//�ȴ���ʱʱ����Ϣ
 	if(err != SYS_NO_ERR)					
 		return -1;		
	
	if(len < 16)								//��������������
		return -2;	

	//AD1
	if( 0 == u8ResetTimCntAD1 )					//�ж�AD1�Ƿ�������
	{
		if(CS5532A1_Flag())						//�ж�AD1�Ƿ�ת����
		{				
			u8StateAD &= ~ERROR_AD1;
			u8ErrContAD1 = 0;
			*((long*)buffer) = CS5532Ax_GetValue();//��ȡADֵ
		}
		else
		{//ADδ׼���ã����������1�������������ﵽ��Ҫ���³�ʼ���Ĵ���
			if( 0 == (++u8ErrContAD1&RE_INIT_AD_COUNT) )
			{
				u8ResetTimCntAD1 = 1;		
				fStepConfgAD_Ax(CS5532A1 , 1);	//���³�ʼ��AD1�ĵ�һ��--��λ,��������ģʽ
				if(u8ErrContAD1>10){
					u8ErrContAD1 = 0;
					u8StateAD |= ERROR_AD1;
				}
			}
		}
	}
	else 
	{//��λ��������1
		u8ResetTimCntAD1++;
		if( 16 == u8ResetTimCntAD1 )			//20ms��ʱ��
		{
			fStepConfgAD_Ax(CS5532A1 , 2);		//���³�ʼ��AD1�ĵڶ���--����
			u8ResetTimCntAD1 = 0;
		}	
	}
	//AD2
	if( 0 ==u8ResetTimCntAD2 )
	{
		if(CS5532A2_Flag())
		{
			u8StateAD &= ~ERROR_AD2;
			u8ErrContAD2 = 0;
			*((long*)(buffer+4)) = CS5532Ax_GetValue();
		}
		else
		{
			if( 0 == (++u8ErrContAD2&RE_INIT_AD_COUNT) )
			{
				u8ResetTimCntAD2 = 1;
				fStepConfgAD_Ax(CS5532A2 , 1);		//���³�ʼ��AD2�ĵ�һ��--��λ
				if(u8ErrContAD2>10){
					u8ErrContAD2 = 0;
					u8StateAD |= ERROR_AD2;
				}
			}
		}
	}
	else
	{
		u8ResetTimCntAD2++;
		if( 16 == u8ResetTimCntAD2 )				//20ms��ʱ��
		{
			fStepConfgAD_Ax(CS5532A2 , 2);			//���³�ʼ��AD2�ĵڶ���--����
			u8ResetTimCntAD2 = 0;
		}
	}

	//AD3
	if( 0 == u8ResetTimCntAD3 )
	{
		if(CS5532B1_Flag())
		{				
			u8StateAD &= ~ERROR_AD3;
			u8ErrContAD3 = 0;
			*((long*)(buffer+8)) = CS5532Bx_GetValue();
		}
		else
		{
			if( 0 == (++u8ErrContAD3&RE_INIT_AD_COUNT) )
			{
				u8ResetTimCntAD3 = 1;		
				fStepConfgAD_Bx(CS5532B1 , 1);	//���³�ʼ��AD1�ĵ�һ��--��λ
				if(u8ErrContAD3>10){
					u8ErrContAD3 = 0;
					u8StateAD |= ERROR_AD3;
				}
			}
		}
	}
	else 
	{
		u8ResetTimCntAD3++;
		if( 16 == u8ResetTimCntAD3 )			//20ms��ʱ��
		{
			fStepConfgAD_Bx(CS5532B1 , 2);		//���³�ʼ��AD1�ĵڶ���--����
			u8ResetTimCntAD3 = 0;
		}
	}
	//AD4
	if( 0 ==u8ResetTimCntAD4 )
	{
		if(CS5532B2_Flag())
		{
			u8StateAD &= ~ERROR_AD4;
			u8ErrContAD4 = 0;
			*((long*)(buffer+12)) = CS5532Bx_GetValue();
		}
		else
		{
			if( 0 == (++u8ErrContAD4&RE_INIT_AD_COUNT) )
			{
				u8ResetTimCntAD4 = 1;
				fStepConfgAD_Bx(CS5532B2 , 1);		//���³�ʼ��AD2�ĵ�һ��--��λ
				if(u8ErrContAD4>10){
					u8ErrContAD4 = 0;
					u8StateAD |= ERROR_AD4;
				}
			}
		}
	}
	else
	{
		u8ResetTimCntAD4++;
		if( 16 == u8ResetTimCntAD4 )				//20ms��ʱ��
		{
			fStepConfgAD_Bx(CS5532B2 , 2);			//���³�ʼ��AD2�ĵڶ���--����
			u8ResetTimCntAD4 = 0;
		}
	}
	return 16;						
}

/****************************************************************************
* ��	�ƣ�static s32 ADJOB_Write(u8* buffer,u32 len,u32* Reallen,u32 lParam) 
* ��	�ܣ�д
* ��ڲ�����u8* buffer		���ջ�����			����
			u32 len			ϣ�����յĳ���	����
* ���ڲ�����s32				�����Ƿ�ɹ�		0�ɹ�	
* ˵	����
****************************************************************************/
static s32 ADJOB_Write(u8* buffer,u32 len) 
{
	return 0;
}

/****************************************************************************
* ��	�ƣ�static s32 ADJOB_Ioctl(u32 cmd, u32 lParam)
* ��	�ܣ�����
* ��ڲ�����u32 cmd			����
			u32 lParam		����
* ���ڲ�����s32				��������Ĳ�ͬ���ص�ֵ���岻ͬ
* ˵	����	��
****************************************************************************/
static s32 ADJOB_Ioctl(u32 cmd, u32 lParam)
{	
	switch(cmd)
	{
		case AD_CMDCheck:
		{
			return u8StateAD;											//�豸�Ƿ����		32λ�ĵ�4λ��ʾ��A��D��
		}
	}
	return 1;
}

/****************************************************************************
* ��	�ƣ�void Tim6TimeISR(void)
* ��	�ܣ���ʱ���
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void Tim6TimeISR(void)
{
	SysSemPost(ADTimeEvent);					//����ʱ�����ź���
}

/****************************************************************************
* ��	�ƣ�u8 ADJOB_init(u8 PRIO_t)
* ��	�ܣ�AD���������ʼ��
* ��ڲ�����u8 PRIO_t							���ȼ�
* ���ڲ�����u8		�Ƿ�ɹ�   1�ɹ�  0ʧ��
* ˵	������
****************************************************************************/
bool ADJOB_init(u8 PRIO_t)
{		
 	HVL_ERR_CODE err = HVL_NO_ERR;								//����״̬
	
	//ע���ñ���
	DEV_REG adjob = 		//�豸ע����Ϣ��						����static		
	{
		CHAR_AD,						//�豸ID��
		0,  								//�Ƿ���			0:���ɹ���ʹ��, 1:�ɹ���ʹ��
		0, 									//���ڹ����豸���򿪴���
		1,									//��������
		1,									//���д����
		(u8*)ADJob_pDisc,				//��������			�Զ���
		20120001,						//�����汾			�Զ���
		(u32*)ADJOB_Open,		//�豸�򿪺���ָ��
		(u32*)ADJOB_Close, 	//�豸�رպ���ָ��
		(u32*)ADJOB_Read,		//�ַ�������
		(u32*)ADJOB_Write,	//�ַ�д����
		(u32*)ADJOB_Ioctl		//���ƺ���
	};																		
	
	CS5532A_Init();	   //CS5532���ų�ʼ��
	CS5532B_Init();	   //CS5532���ų�ʼ��										
	if(ADTimeEvent == (SYS_EVENT*)0) 
		 ADTimeEvent = SysSemCreate(0);   					//��������ʱ�����ź���
		
 	if(TIM6_Config(Tim6TimeISR,125,Mode_10US) == 0)			//10΢�����һ��	125�μ���һ���ж���Ӧ  1250us = 1.25ms
		return false;
	
 	TIM6_Dev = DeviceOpen(CHAR_TIM6,0,&err);			//�豸��	//��þ��
	if(err != HVL_NO_ERR)
 		return false;
		
	if(DeviceInstall(&adjob) != HVL_NO_ERR)				//ע���豸
		return false;	


	if(false == fStepConfgAD_Ax(CS5532A1+CS5532A2 , 1))//��λA1A2
		return false;
	if(false == fStepConfgAD_Bx(CS5532B1+CS5532B2 , 1))//��λB1B2
		return false;	
	SysTimeDly(2);									   //20ms��ʱ
	if(false == fStepConfgAD_Ax(CS5532A1+CS5532A2 , 2))//����A1A2
		return false;	
	if(false == fStepConfgAD_Bx(CS5532B1+CS5532B2 , 2))//����B1B2
		return false;		
	else 
		return true;	

}

