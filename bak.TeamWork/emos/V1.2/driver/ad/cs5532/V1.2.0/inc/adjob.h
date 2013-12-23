#ifndef _ADJOB_INIT_H_	
#define _ADJOB_INIT_H_

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif

#include <stdbool.h>	
// #define AD_CMDStart			0			//��������
// #define AD_CMDEnd			1			//ֹͣ����
#define AD_CMDCheck			2				//ADоƬ���ϲ�ѯ		�÷���32λ�ĵ�4λ��ʾ��A��B�θ�C�ε�D��		1����0����

#define	ERROR_AD1	0x01
#define	ERROR_AD2	0x02
#define	ERROR_AD3	0x04
#define	ERROR_AD4	0x08

/****************************************************************************
* ��	�ƣ�u8 ADJOB_init(u8 PRIO_t)
* ��	�ܣ�AD���������ʼ��
* ��ڲ�����u8 PRIO_t							���ȼ�
* ���ڲ�����u8		�Ƿ�ɹ�   1�ɹ�  0ʧ��
* ˵	������
****************************************************************************/
bool ADJOB_init(unsigned char PRIO_t);	 	//���������ʼ��

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
AD_CMDStart								����							����
AD_CMDEnd									����							����
AD_CMDCheck								����							����
****************************************************************************/	

#ifdef __cplusplus		   //�����CPP����C���� //��������
}
#endif

#endif

/*
void Job1ManagerProcess(void)		//��������߳�
{
	u32 ErrCnt = 0;								//�������
 	u8 err = 0;
// 	u32 xx = 0;									//���Կ��ز���
// 	u8 yy = 0;
// 	s32 ADstateFlag = 0;				//ת��оƬ�Ƿ����
	
	long AD[4] = {0};						//�õ���ADֵ
	
	HANDLE ADJOB_HANDLE;				//AD�����豸���
	
	USART_STRUCT usart1cfg = 
	{
		57600,																	//������
		USART_WordLength_8b,										//����λ
		USART_StopBits_1,												//ֹͣλ
		USART_Parity_No													//Ч��λ
	};

	USART1_Config(&usart1cfg);								//���ڳ�ʼ��
	Set_PrintfPort(1);												//���ô�ӡ����
	
	printf("��ʼ\r\n");
	
	if(ADJOB_init(5) != 1)	//�豸��ʼ��		//�������ȼ�	AD��ȡ��Ϣ����	//AD��ȡ���ȼ��������AD�������ȼ�
		while(1);
	
	ADJOB_HANDLE	= DeviceOpen(CHAR_AD,0,&err);			//��AD��������
 	if(err != HVL_NO_ERR)
 		while(1);

	while(1)
	{
		if(CharDeviceRead(ADJOB_HANDLE,(u8*)AD,0,0,0) == 0)
			continue;
		
		printf("A = %d\r\nB = %d\r\nC = %d\r\nD = %d ERR = %d\r\n\r\n",AD[0],AD[1],AD[2],AD[3],ErrCnt);
		if(AD[0] == 0 || AD[1] == 0 || AD[2] == 0 || AD[3] == 0)
			ErrCnt++;
		
 		AD[0] = 0;		//CS5532A
 		AD[1] = 0;		//CS5532B
 		AD[2] = 0;		//CS5532C
 		AD[3] = 0;		//CS5532D
 		SysTimeDly(1);
			
		
//  	printf("A = %d\r\n",AD[0]);
//  	printf("B = %d\r\n",AD[1]);
//  	printf("C = %d\r\n",AD[2]);
// 		printf("D = %d\r\n\r\n",AD[3]);
		
// 		AD[0] = 0;
// 		AD[1] = 0;
// 		AD[2] = 0;
// 		AD[3] = 0;
		
// 		if(yy == 1)
// 			ADstateFlag = DeviceIoctl(ADJOB_HANDLE,AD_CMDCheck,0);		//���ת��оƬ�Ƿ���� �÷���32λ�ĵ�4λ��ʾ��A��B�θ�C�ε�D��		1����0����
// 		ADstateFlag = ADstateFlag;
// 		ADstateFlag = 0;
// 		yy = 0;
// 		
// 		xx++;																							
// 		if(xx == 2)			//��ADJOB��43�н�����SysTimeDly(50);	����ʱ�����ź���Ҫ�ۼӣ���ȼ��������ͷ���
// 			DeviceIoctl(ADJOB_HANDLE,AD_CMDEnd,0);					//�رղ���
// 		
// 		if(xx == 1000)
// 			DeviceIoctl(ADJOB_HANDLE,AD_CMDStart,0);				//��������
// 		
// 		if(xx == 2000)
// 			DeviceIoctl(ADJOB_HANDLE,AD_CMDEnd,0);					//�رղ���
// 		
// 		if(xx == 3000)
// 			DeviceIoctl(ADJOB_HANDLE,AD_CMDStart,0);				//��������
// 		
// 		if(xx == 4000)
// 			DeviceClose(ADJOB_HANDLE);				//�ر��豸		
	}
}
*/
