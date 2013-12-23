#ifndef _TIM5_INIT_H_	
#define _TIM5_INIT_H_

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif

#include "stm32f4xx.h"

#define Mode_1US				0							//1΢��
#define Mode_10US				1							//10΢��
#define Mode_100US			2							//100΢��
#define Mode_500US			3							//500΢��

#define TIM5_CMDClearCnt			0				//��ռ�����
#define TIM5_CMDTimOpen			1				//������ʱ�������ж���Ӧ
#define TIM5_CMDTimClose			2				//�رն�ʱ���ر��ж���Ӧ
#define TIM5_CMDSetCntFlag		3				//���ü�������
#define TIM5_CMDSetCaller		4				//���ûص�����

typedef void(*TIM5_fun)(void); 			//����ָ��
	
/****************************************************************************
* ��	�ƣ�u8 TIM5_Config(TIM5_fun Cfun,u16 TimeUS,u8 mode)
* ��	�ܣ���ʱ������
* ��ڲ�����TIM5_fun Cfun		�ص�����
						u16 TimeUS	��Ҫ��ʱ�ĵ�λ����	΢�뵥λ
						u8 mode			ģʽ 1 10 100 500΢��
* ���ڲ�����u8	�����Ƿ�ɹ�		1�ɹ� 0ʧ��
* ˵	������
****************************************************************************/
u8 TIM5_Config(TIM5_fun Cfun,u16 TimeUS,u8 mode);	 	//��ʱ����ʼ��

/****************************************************************************
* ��	�ƣ�HANDLE DeviceOpen(DEV_ID ID,u32 lParam,HVL_ERR_CODE* err)
* ��	�ܣ��豸�򿪻���豸���
* ��ڲ�����DEV_ID ID						�豸ID��
						u32 lParam					����				����
						HVL_ERR_CODE* err		����״̬
* ���ڲ�����HANDLE 							�豸���
* ˵	������
****************************************************************************/	

/****************************************************************************
* ��	�ƣ�HVL_ERR_CODE DeviceClose(HANDLE IDH)
* ��	�ܣ��豸�ر�
* ��ڲ�����HANDLE IDH					�豸���
* ���ڲ�����HVL_ERR_CODE* err		����״̬
* ˵	������
****************************************************************************/	

/****************************************************************************
* ��	�ƣ�s32 CharDeviceRead(HANDLE IDH,u8* Buffer,u32 len,u32* Reallen,u32 lParam)
* ��	�ܣ��豸������
* ��ڲ�����HANDLE IDH		�豸���
						u8* Buffer		������ݵ�ַ					����		
						u32 len				ϣ����ȡ�����ݸ���		����
						u32* Reallen	ʵ�ʶ�ȡ�����ݸ���		���ʱ����ʱֵ
						u32 lParam		����									����
* ���ڲ�����s32						��ȡ�����Ƿ�ɹ�			����
* ˵	������
****************************************************************************/	

/****************************************************************************
* ��	�ƣ�s32 CharDeviceWrite(HANDLE IDH,u8* Buffer,u32 len,u32* Reallen,u32 lParam)
* ��	�ܣ��豸д����
* ��ڲ�����HANDLE IDH		�豸���							����
						u8* Buffer		д����Դ��ַ					����
						u32 len				ϣ��д������ݸ���		����
						u32* Reallen	ʵ��д������ݸ���		����
						u32 lParam		����									����
* ���ڲ�����s32						д������Ƿ�ɹ�			����
* ˵	����
****************************************************************************/	

/****************************************************************************
* ��	�ƣ�s32 DeviceIoctl(HANDLE IDH,u32 cmd,u32 lParam)
* ��	�ܣ��豸����
* ��ڲ�����HANDLE IDH		�豸���
						u32 cmd				�豸����
						u32 lParam		���������˵��
* ���ڲ�����s32 					�����ֵ��˵��
* ˵	����
����											����							����ֵ
TIM7_CMDClearCnt					����							����
TIM7_CMDTimOpen						����							����
TIM7_CMDTimClose					����							����
TIM7_CMDSetCntFlag				1��0��						����
TIM7_CMDSetCaller					������ַ					����
****************************************************************************/	

#ifdef __cplusplus		   //�����CPP����C���� //��������
}
#endif

#endif

/*
#include "driver.h"
#include "tim7_init.h"

//�ص�����1
void TIM7_CallerTest1(void)
{
	int i = 0;
	i += 3;
	return;
}

//�ص�����2
void TIM7_CallerTest2(void)
{
	int i = 0;
	i += 3;
	return;
}

	u32 cnt = 0;																	//��ȡ����ֵ
	HVL_ERR_CODE err = HVL_NO_ERR;								//����״̬
	
	HANDLE TIM7_Dev = 0;													//�豸���
	
	TIM7_Config(TIM7_CallerTest1,10,Mode_500US);	//500΢���ж�һ��	10�μ���һ���ж���Ӧ
	
	TIM7_Dev = DeviceOpen(CHAR_TIM7,0,&err);			//�豸��
	if(err != HVL_NO_ERR)
		while(1);
	
	if(DeviceIoctl(TIM7_Dev,TIM7_CMDSetCntFlag,1) == 0)		//���ü�������
		while(1);
		
	if(DeviceIoctl(TIM7_Dev,TIM7_CMDTimOpen,0) == 0)			//������ʱ�������ж���Ӧ
		while(1);
	
	SysTimeDly(5000);																			//5S

	if(DeviceIoctl(TIM7_Dev,TIM7_CMDSetCaller,(u32)&TIM7_CallerTest2) == 0)			//�л��ص�����
		while(1);
	
	SysTimeDly(5000);																			//5S
	
	if(DeviceIoctl(TIM7_Dev,TIM7_CMDTimClose,0) == 0)			//������ʱ�������ж���Ӧ
		while(1);
	
	if(CharDeviceRead(TIM7_Dev,0,0,&cnt,0) == 0)					//��ȡ����ֵ
		while(1);
	
	if(DeviceIoctl(TIM7_Dev,TIM7_CMDClearCnt,0) == 0)			//��λ������
		while(1);
*/
