#include "beep_thread.h"
#include "beep_init.h"
#include "driver.h"
  
/****************************************************************************/
//Ϊ�˲����� Beep_init.h
extern void Beep(u32 count,u16 time);			//����������
extern void SetBeepFlag(u8 on);						//���÷���������
extern u8 LookBeepFlag(void);							//�鿴����������
extern void Beep_Config(void);						//��������ʼ��

/****************************************************************************/

static INT8U BeepJob_PRIO = 0;			//�������ȼ�
static SYS_EVENT* BeepJobEvent = (SYS_EVENT*)0;										//������Ϣ����
static u8 BeepJob_pDisc[] = "LuoHuaiXiang_BeepJob\r\n";						//�豸�����ַ�					����static

//�����ջ��С
#define BeepJobStackSize 128	  														//BEEPJOB�����ջ��С

//�����ջ
__align(8) static SYS_STK 		BeepJobStack[BeepJobStackSize];			//BEEPJOB�����ջ

/****************************************************************************
* ��	�ƣ�void BeepJob_Default(void *nouse)
* ��	�ܣ�Ĭ��BeepJob
* ��ڲ�������
* ���ڲ�������
* ˵	������			
****************************************************************************/
void BeepJob_Default(void *nouse)
{
	u8 err = 0;	
	void* pMsg;														//��Ϣ������ֵ��ַ
	
	if(LookBeepFlag() == 0)								//������������ڹر�״̬
		SetBeepFlag(1);											//�򿪷�����

	while(1)
	{
		pMsg = SysQPend(BeepJobEvent, 0, &err);	//�ȴ���Ϣ
		if(err != SYS_NO_ERR)	//����		���յ�����Ϣ����״̬
			continue;
		else
			Beep(((BEEP_STRUCTURE*)pMsg)->count,((BEEP_STRUCTURE*)pMsg)->time);										//����
	}
}

/****************************************************************************
* ��	�ƣ�static s8 BEEPJOB_Open(u32 lParam)
* ��	�ܣ�ͳһ�ӿ�
* ��ڲ�����u32 lParam		��������
* ���ڲ�����u8						�򿪲�������ֵ	��HVL_ERR_CODE��
* ˵	������
****************************************************************************/
static s8 BEEPJOB_Open(u32 lParam)
{
	INT8U TaskStatus = 0;	//��������SysTaskCreate����ֵ    	SYS_NO_ERR(0x00�ɹ�)	SYS_PRIO_EXIST(0x28���ȼ���ͬ) 

	TaskStatus |= SysTaskCreate(BeepJob_Default,						//������
										(void *)0,														//�������
										&BeepJobStack[BeepJobStackSize - 1],	//�����ջ
										BeepJob_PRIO);												//�������ȼ�
 	if(TaskStatus != SYS_NO_ERR)
		return 1;																							//��ʧ��
	else
		return 0;																							//�򿪳ɹ�
}

/****************************************************************************
* ��	�ƣ�static s8 BEEPJOB_Close(u32 lParam)
* ��	�ܣ�ͳһ�ӿ�
* ��ڲ�����u32 lParam		��������
* ���ڲ�����u8						�򿪲�������ֵ	��HVL_ERR_CODE��
* ˵	������
****************************************************************************/
static s8 BEEPJOB_Close(u32 lParam)
{
	if(SysTaskDel(BeepJob_PRIO) == SYS_ERR_NONE)				//����������
		return 0;
	return 1;
}

/****************************************************************************
* ��	�ƣ�static s32 BEEPJOB_Read(u8* buffer,u32 len,u32* Reallen,u32 lParam) 
* ��	�ܣ���
* ��ڲ�����u8* buffer		���ջ�����			����
						u32 len				ϣ�����յĳ���	����
						u32* Reallen	ʵ�ʽ��յĳ���	����
						u32 lParam		����						����
* ���ڲ�����s32						�����Ƿ�ɹ�		0�ɹ�	1ʧ��
* ˵	������
****************************************************************************/
static s32 BEEPJOB_Read(u8* buffer,u32 len) 
{
	return 0;						
}

/****************************************************************************
* ��	�ƣ�static s32 BEEPJOB_Write(u8* buffer,u32 len,u32* Reallen,u32 lParam) 
* ��	�ܣ���������
* ��ڲ�����u8* buffer		���ջ�����			����
						u32 len				ϣ�����յĳ���	����
						u32* Reallen	ʵ�ʽ��յĳ���	����
						u32 lParam		����						BEEP_STRUCTURE
* ���ڲ�����s32						д������Ƿ�ɹ�	1�ɹ� 0ʧ��			(ʧ��Ϊ�ź�������)
* ˵	����
typedef struct
{
	u32 count;				//��������
	u16 time;					//ÿ�η���ʱ�䣬��λ����
}BEEP_STRUCTURE;
****************************************************************************/
static s32 BEEPJOB_Write(u8* buffer,u32 len) 
{
	return 0;
}

/****************************************************************************
* ��	�ƣ�static s32 BEEPJOB_Ioctl(u32 cmd, u32 lParam)
* ��	�ܣ����ڿ���
* ��ڲ�����u32 cmd				����
						u32 lParam		����
* ���ڲ�����s32						��������Ĳ�ͬ���ص�ֵ���岻ͬ
* ˵	����	��
****************************************************************************/
static s32 BEEPJOB_Ioctl(u32 cmd, u32 lParam)
{
	switch(cmd)
	{
		case BEEPCMD_SETONOFF:
		{
			if(lParam > 0)
				SetBeepFlag(1);
			else
				SetBeepFlag(0);
		}
		case BEEPCMD_GETONOFF:
			return (u32)LookBeepFlag();
		case BEEPCMD_ON:
		{
				if(BeepJobEvent != NULL) 										//�����������Ϣ�����ź���
				{
					SysQPost(BeepJobEvent, (void*)lParam);	//������Ϣ����
					return 0;
				}
				break;
		}
	}
	return 1;
}

/****************************************************************************
* ��	�ƣ�u8 BEEPJOB_Init(u8 PRIO_t,SYS_EVENT* JOBEVENT)
* ��	�ܣ�BeepJOB��ʼ��
* ��ڲ�����u8 PRIO_t								�������ȼ�
						SYS_EVENT* JOBEVENT			������Ϣ����
* ���ڲ�����u8 					�Ƿ�ɹ�	1�ɹ�	0ʧ��
* ˵	������
****************************************************************************/
u8 BEEPJOB_Init(u8 PRIO_t,SYS_EVENT* JOBEVENT)
{
	//ע���ñ���
	DEV_REG beepjob = 		//�豸ע����Ϣ��						����static		
	{
		CHAR_BEEPJOB,				//�豸ID��
		0,  								//�Ƿ���			0:���ɹ���ʹ��, 1:�ɹ���ʹ��
		0, 									//���ڹ����豸���򿪴���
		1,									//��������
		1,									//���д����
		BeepJob_pDisc,			//��������			�Զ���
		20120001,						//�����汾			�Զ���
		(u32*)BEEPJOB_Open,		//�豸�򿪺���ָ��
		(u32*)BEEPJOB_Close, 	//�豸�رպ���ָ��
		(u32*)BEEPJOB_Read,		//�ַ�������
		(u32*)BEEPJOB_Write,	//�ַ�д����
		(u32*)BEEPJOB_Ioctl		//���ƺ���
	};		
	
	Beep_Config();																			//��������ʼ��
	BeepJob_PRIO = PRIO_t;															//���ȼ�����
	BeepJobEvent = JOBEVENT;														//��Ϣ���б���
	if(DeviceInstall(&beepjob) != HVL_NO_ERR)						//ע���豸
		return 0;	
	
	return 1;
}
