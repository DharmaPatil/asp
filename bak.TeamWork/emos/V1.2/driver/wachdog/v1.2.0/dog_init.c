#include "dog_init.h"
#include "driver.h"
#include "dt3102_io.h"
//DOG�豸���ƽṹ��
typedef struct
{
	GPIO_TypeDef*		Port;							//���Ŷ˿ں�
	uint16_t				Pin;								//���ź�
}DOGCTL_STRUCT;

//static DOGCTL_STRUCT Dog_Real = {GPIOD,GPIO_Pin_15};			//��������ʵ��				����static
static u8 DOG_pDisc[] = "LuoHuaiXiang_DOG\r\n";						//�豸�����ַ�	
#define WATCHDOG_PIN	GPIO_Pin_15
#define WATCHDOG_PORT	GPIOD
/****************************************************************************
* ��	�ƣ�void FeedDog(void)
* ��	�ܣ�ι������
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void FeedDog(void)
{
	IoHi(WATCHDOG);
	IoLo(WATCHDOG);
	//GPIO_SetBits(Dog_Real.Port, Dog_Real.Pin);				//������1
	//GPIO_ResetBits(Dog_Real.Port, Dog_Real.Pin);			//������0
}

/****************************************************************************
* ��	�ƣ�static s8 Dog_Open(u32 lParam)
* ��	�ܣ�ͳһ�ӿ�
* ��ڲ�����u32 lParam		��������
* ���ڲ�����u8						�򿪲�������ֵ	��HVL_ERR_CODE��
* ˵	������
****************************************************************************/
static s8 Dog_Open(u32 lParam)
{
	return 0;																							//�򿪳ɹ�
}

/****************************************************************************
* ��	�ƣ�static s8 Dog_Close(u32 lParam)
* ��	�ܣ�ͳһ�ӿ�
* ��ڲ�����u32 lParam		��������
* ���ڲ�����u8						�򿪲�������ֵ	��HVL_ERR_CODE��
* ˵	������
****************************************************************************/
static s8 Dog_Close(u32 lParam)
{
	return 0;
}

/****************************************************************************
* ��	�ƣ�static s32 Dog_Read(u8* buffer,u32 len,u32* Reallen,u32 lParam) 
* ��	�ܣ���
* ��ڲ�����u8* buffer		���ջ�����			����
						u32 len				ϣ�����յĳ���	����
						u32* Reallen	ʵ�ʽ��յĳ���	����
						u32 lParam		����						����
* ���ڲ�����s32						�����Ƿ�ɹ�		0�ɹ�	1ʧ��
* ˵	������
****************************************************************************/
static s32 Dog_Read(u8* buffer,u32 len) 
{
	return 0;						
}

/****************************************************************************
* ��	�ƣ�static s32 Dog_Write(u8* buffer,u32 len,u32* Reallen,u32 lParam) 
* ��	�ܣ���������
* ��ڲ�����u8* buffer		���ջ�����			����
						u32 len				ϣ�����յĳ���	����
						u32* Reallen	ʵ�ʽ��յĳ���	����
						u32 lParam		����						����
* ���ڲ�����s32						�����Ƿ�ɹ�		0�ɹ�	1ʧ��
* ˵	����
****************************************************************************/
static s32 Dog_Write(u8* buffer,u32 len) 
{
	return 0;
}

/****************************************************************************
* ��	�ƣ�static s32 Dog_Ioctl(u32 cmd, u32 lParam)
* ��	�ܣ����ڿ���
* ��ڲ�����u32 cmd				����
						u32 lParam		����
* ���ڲ�����s32						��������Ĳ�ͬ���ص�ֵ���岻ͬ
* ˵	����	��
****************************************************************************/
static s32 Dog_Ioctl(u32 cmd, u32 lParam)
{
	switch(cmd)
	{
		case DOGCMD_FEED:
		{
			FeedDog();			//ι��
			return 1;
		}
		default:
			return 0;
	}
}

/****************************************************************************
* ��	�ƣ�u8 Dog_Init(void)
* ��	�ܣ����Ź���ʼ��
* ��ڲ�������
* ���ڲ�����u8	��ʼ���Ƿ�ɹ�	1�ɹ� 0ʧ��
* ˵	������
****************************************************************************/
u8 Dog_Init(void)
{
//	GPIO_InitTypeDef GPIO_InitStructure;		//�������Žṹ
	
		//ע���ñ���
	DEV_REG dog = 				//�豸ע����Ϣ��						����static		
	{
		CHAR_DOG,						//�豸ID��
		0,  								//�Ƿ���			0:���ɹ���ʹ��, 1:�ɹ���ʹ��
		0, 									//���ڹ����豸���򿪴���
		1,									//��������
		1,									//���д����
		DOG_pDisc,					//��������			�Զ���
		20120001,						//�����汾			�Զ���
		(u32*)Dog_Open,		//�豸�򿪺���ָ��
		(u32*)Dog_Close, 	//�豸�رպ���ָ��
		(u32*)Dog_Read,		//�ַ�������
		(u32*)Dog_Write,	//�ַ�д����
		(u32*)Dog_Ioctl		//���ƺ���
	};
	

	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
#if 0
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;				//��������
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;			//��������Ϊ���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//Ƶ��50MHz
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP; 			//�����ڲ�������
	GPIO_Init(Dog_Real.Port, &GPIO_InitStructure);
#endif
	IoSetMode(WATCHDOG,IO_MODE_OUTPUT);
	IoSetSpeed(WATCHDOG,IO_SPEED_50M);
	IoPushPull(WATCHDOG);
	if(DeviceInstall(&dog) != HVL_NO_ERR)								//ע���豸
		return 0;	
	
	return 1;
}
