
#include "Job_USB.h"
#include "driver.h"


#include "usb_conf.h"
#include "usb_core.h"
#include "usbh_core.h"
#include "usbh_msc_core.h"

typedef void (*fFeedDog)(void);									//����Ӧ�ò�ע���ι��������
fFeedDog pfFeedDog = NULL;
/** @defgroup USBH_USR_MAIN_Private_Variables
* @{
*/
__ALIGN_BEGIN USB_OTG_CORE_HANDLE      USB_OTG_Core __ALIGN_END;
__ALIGN_BEGIN USBH_HOST                USB_Host __ALIGN_END;

//Դ��usbh_usr.h
extern  USBH_Usr_cb_TypeDef USR_cb;
extern 	s32	fUSB1_StateRead(void);
extern	s32	fUSB1_CapacityReturn(void);


//����USB�����ջ��С
#define 	JOB_USB1_STACK_SIZE			256

//�����ջ
__align(8) static SYS_STK 		Stack_JobUSB[JOB_USB1_STACK_SIZE];		//JOB USB1�����ջ
static u8 JobUSB1_PRIO = 0;												//�������ȼ�

static u8 sJOB_USB1_pDisc[] = "USB1\r\n";								//�豸�����ַ�		����static

/****************************************************************************
* ��	�ƣ�USB1Thread
* ��	�ܣ�USB1�߳�
* ��ڲ�����
* ���ڲ�����
* ˵	������
****************************************************************************/
static void USB1Thread(void *nouse)
{
	for(;;)
	{
		/* Host Task handler */
		USBH_Process(&USB_OTG_Core, &USB_Host);
		SysTimeDly(10);
	}
}
/****************************************************************************
* ��	�ƣ�static s8 USB1JOB_Open(u32 lParam)
* ��	�ܣ�ͳһ�ӿ�
* ��ڲ�����u32 lParam		��������
* ���ڲ�����u8						�򿪲�������ֵ	��HVL_ERR_CODE��
* ˵	������
****************************************************************************/
static s8 USB1JOB_Open(u32 lParam)
{
	INT8U TaskStatus = 0;	//��������SysTaskCreate����ֵ    	SYS_NO_ERR(0x00�ɹ�)	SYS_PRIO_EXIST(0x28���ȼ���ͬ) 

	TaskStatus |= SysTaskCreate(USB1Thread,											//������
										(void *)0,									//�������
										&Stack_JobUSB[JOB_USB1_STACK_SIZE - 1],		//�����ջ
										JobUSB1_PRIO);								//�������ȼ�
 	if(TaskStatus != SYS_NO_ERR)
		return 1;																	//��ʧ��
	else
		return 0;																	//�򿪳ɹ�
}
/****************************************************************************
* ��	�ƣ�static s8 USB1JOB_Close(u32 lParam)
* ��	�ܣ�ɾ��USB1����
* ��ڲ�����u32 lParam		��������
* ���ڲ�����u8						�򿪲�������ֵ	��HVL_ERR_CODE��
* ˵	������
****************************************************************************/
static s8 USB1JOB_Close(u32 lParam)
{
	if( SYS_NO_ERR != SysTaskDel(JobUSB1_PRIO) )
	{return 1;}
	else
	{return 0;}
		
}
extern char sProductString[20];
/****************************************************************************
* ��	�ƣ�static s32 USB1JOB_Read(u8* buffer,u32 len,u32* Reallen,u32 lParam) 
* ��	�ܣ���
* ��ڲ�����u8* buffer		���ջ�����			����
						u32 len				ϣ�����յĳ���	����
						u32* Reallen	ʵ�ʽ��յĳ���	����
						u32 lParam		����						����
* ���ڲ�����s32						�����Ƿ�ɹ�		0�ɹ�	1ʧ��
* ˵	��������Ӧ�ò�����
****************************************************************************/
static s32 USB1JOB_Read(u32 offset , u8* buffer,u32 len,u32* Reallen,u32 lParam) 
{
	u8 i;
	*Reallen = len;
	if(len>20)
	{
		len = 20;
		*Reallen = 20;
	}
	for( i=0 ; i<len ; i++ )
	{
		(buffer[i]) = sProductString[i];
	}
	return 1;						
}
/****************************************************************************
* ��	�ƣ�static s32 USB1JOB_Write(u8* buffer,u32 len,u32* Reallen,u32 lParam) 
* ��	�ܣ�д
* ��ڲ�����u8* buffer		���ջ�����			����
						u32 len				ϣ�����յĳ���	����
						u32* Reallen	ʵ�ʽ��յĳ���	����
						u32 lParam		����						����
* ���ڲ�����s32						�����Ƿ�ɹ�		0�ɹ�	1ʧ��
* ˵	����
****************************************************************************/
static s32 USB1JOB_Write(u8* buffer,u32 len,u32* Reallen,u32 lParam) 
{
	return 0;
}
/****************************************************************************
* ��	�ƣ�static s32 USB1JOB_Ioctl(u32 cmd, u32 lParam)
* ��	�ܣ�����
* ��ڲ�����u32 cmd				����
						u32 lParam		����
* ���ڲ�����s32						��������Ĳ�ͬ���ص�ֵ���岻ͬ
* ˵	����	��
****************************************************************************/
static s32 USB1JOB_Ioctl(u32 cmd, u32 lParam)
{
	switch(cmd){
		case 	CMD_USB1_READ_STATE:{
				return fUSB1_StateRead();}
		case	CMD_USB1_READ_CAPACITY:{
				return	fUSB1_CapacityReturn();}
		case	CMD_USB1_REGISTER_FEED_DOG:{
				pfFeedDog = (fFeedDog)lParam;
		}
	}
	return 1;
}
/****************************************************************************
* ��	�ƣ�u8 USB1Job_Init(u8 PRIO_t)
* ��	�ܣ�USB1�����ʼ��
* ��ڲ�����u8 PRIO_t							���ȼ�
* ���ڲ�����u8		�Ƿ�ɹ�   1�ɹ�  0ʧ��
* ˵	������
****************************************************************************/
u8 USB1Job_Init(u8 PRIO_t)
{	
	//ע���ñ���
	DEV_REG USBjob = 		  	//�豸ע����Ϣ��		����static		
	{
		BLOCK_USB1,				//�豸ID��
		0,  					//�Ƿ���			0:���ɹ���ʹ��, 1:�ɹ���ʹ��
		0, 						//���ڹ����豸���򿪴���
		1,						//��������
		1,						//���д����
		sJOB_USB1_pDisc,		//��������			�Զ���
		20120001,				//�����汾			�Զ���
		(u32*)USB1JOB_Open,		//�豸�򿪺���ָ��
		(u32*)USB1JOB_Close, 	//�豸�رպ���ָ��
		(u32*)USB1JOB_Read,		//�ַ�������
		(u32*)USB1JOB_Write,	//�ַ�д����
		(u32*)USB1JOB_Ioctl		//���ƺ���
	};	
	
	JobUSB1_PRIO = PRIO_t;		//�������ȼ�����
	
	//USB��ʼ��
	USBH_Init(&USB_OTG_Core, 
						USB_OTG_FS_CORE_ID,
						&USB_Host,
						&USBH_MSC_cb, 
						&USR_cb);	

	if(DeviceInstall(&USBjob) != HVL_NO_ERR)	//ע���豸
		return 0;	
	
	return 1;
}





