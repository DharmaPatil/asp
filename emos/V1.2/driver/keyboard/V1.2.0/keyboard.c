#include "Keyboard.h"
#include "Key_cfg.h"
#include "stm32f4xx.h"
#include "c2.h"
#include "driver.h"
#include "dt3102_IO.h"
#include "IO.h"

#define MATRIX_KEY_ROW1SET		IoHi(KEY_H1)//KEY_H1_PORT->BSRRL = KEY_H1_PIN;
#define MATRIX_KEY_ROW2SET		IoHi(KEY_H2)//KEY_H2_PORT->BSRRL = KEY_H2_PIN;
#define MATRIX_KEY_ROW3SET		IoHi(KEY_H3)//KEY_H3_PORT->BSRRL = KEY_H3_PIN;
#define MATRIX_KEY_ROW4SET		IoHi(KEY_H4)//KEY_H4_PORT->BSRRL = KEY_H4_PIN;
// #define MATRIX_KEY_ROW5SET		IoHi(KEY_H5)//KEY_H5_PORT->BSRRL = KEY_H5_PIN;
// #define MATRIX_KEY_ROW6SET		IoHi(KEY_H6)
#define	MATRIX_KEY_ROW1RESET	IoLo(KEY_H1)//KEY_H1_PORT->BSRRH = KEY_H1_PIN;
#define	MATRIX_KEY_ROW2RESET	IoLo(KEY_H2)//KEY_H2_PORT->BSRRH = KEY_H2_PIN;
#define	MATRIX_KEY_ROW3RESET	IoLo(KEY_H3)//KEY_H3_PORT->BSRRH = KEY_H3_PIN;
#define	MATRIX_KEY_ROW4RESET	IoLo(KEY_H4)//KEY_H4_PORT->BSRRH = KEY_H4_PIN;
// #define	MATRIX_KEY_ROW5RESET	IoLo(KEY_H5)//KEY_H5_PORT->BSRRH = KEY_H5_PIN;
// #define	MATRIX_KEY_ROW6RESET 	IoLo(KEY_H6)


#define	KEY_GPIO_STATE_GET(port,pin)	( ((port->IDR & pin)>0) ? 1:0)
#define	EXTI_GET_IT_STATUS(x)			( ((EXTI->PR & x) != (u32)RESET)? SET : RESET)
static const u8 Key_pDisc[] = "Key\r\n";				//�豸�����ַ�
static SYS_EVENT* KeyEvent = (SYS_EVENT*)0;
extern u16 KeyPin;	
extern GPIO_TypeDef* KeyPort;

static u32 Matrix_Key_IntEnable(BOOL bEn);
static const char KeyTable[MATRIX_KEY_LINE][MATRIX_KEY_ROW] = {
//		A				B			C			D				E			
/*1*/	KEY_1,			KEY_2,			KEY_3,		KEY_LEFT,		KEY_BACK,				KEY_F1,			KEY_MENU,				
/*2*/	KEY_4,			KEY_5,			KEY_6,		KEY_UP,			KEY_DIAGNOSIS,	KEY_F2,			KEY_OK,
/*3*/	KEY_7,			KEY_8,			KEY_9,		KEY_RIGHT,	KEY_ZERO,	    	KEY_F3,			KEY_ERR,
/*4*/	KEY_Del,   	KEY_0,			KEY_DOT,	KEY_DOWN,		KEY_PRINT,			KEY_F4,			KEY_ERR	
/*6*/				
};
//��������ж�ע����Ϣ��
static SDEV_IRQ_REGS Key_IRQ = {
	0,								//�ж��ߺ�
	0,								//�ж��߿�����־
	(pIRQHandle)0,					//�жϾ��
};

/****************************************************************************
* ��	�ƣ�void KEY_EXTI15_IRQHandler(void)
* ��	�ܣ��ж�ִ�к���
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
static void KEY_IRQHandler(void)
{
	if(EXTI_GET_IT_STATUS(KEY_L1_PIN) != RESET){			    //1���ж��ߴ���
		if(KeyPin == 0){ 								//���ǰ��û�л�ȡ������ֵ
			KeyPin = KEY_L1_PIN;						//�жϵõ���ֵ
			KeyPort = KEY_L1_PORT;				
			if(KeyEvent != 0)							//�ź����Ѵ���
				SysSemPost(KeyEvent);					//�����ź���
		}
		EXTI->PR |= KEY_L1_PIN;							//���жϱ�־
	}
	else if(EXTI_GET_IT_STATUS(KEY_L2_PIN) != RESET){			//2���ж��ߴ���
		if(KeyPin == 0){ 								//���ǰ��û�л�ȡ������ֵ
			KeyPin = KEY_L2_PIN;						//�жϵõ���ֵ
			KeyPort = KEY_L2_PORT;
			if(KeyEvent != 0)							//�ź����Ѵ���
				SysSemPost(KeyEvent);					//�����ź���
		}
		EXTI->PR |= KEY_L2_PIN;							//���жϱ�־
	}
	else if(EXTI_GET_IT_STATUS(KEY_L3_PIN) != RESET){			//3���ж��ߴ���
		if(KeyPin == 0){ 								//���ǰ��û�л�ȡ������ֵ
			KeyPin = KEY_L3_PIN;						//�жϵõ���ֵ
			KeyPort = KEY_L3_PORT;
			if(KeyEvent != 0)							//�ź����Ѵ���
				SysSemPost(KeyEvent);					//�����ź���
		}
		EXTI->PR |= KEY_L3_PIN;							//���жϱ�־
	}
	else if(EXTI_GET_IT_STATUS(KEY_L4_PIN) != RESET){			//4���ж��ߴ���
		if(KeyPin == 0){ 								//���ǰ��û�л�ȡ������ֵ
			KeyPin = KEY_L4_PIN;						//�жϵõ���ֵ
			KeyPort = KEY_L4_PORT;
			if(KeyEvent != 0){							//�ź����Ѵ���
				SysSemPost(KeyEvent);					//�����ź���				
			}
		}
		EXTI->PR |= KEY_L4_PIN;							//���жϱ�־
	}
	else if(EXTI_GET_IT_STATUS(KEY_L5_PIN) != RESET){			//5���ж��ߴ���
		if(KeyPin == 0){ 								//���ǰ��û�л�ȡ������ֵ
			KeyPin = KEY_L5_PIN;						//�жϵõ���ֵ
			KeyPort = KEY_L5_PORT;
			if(KeyEvent != 0)							//�ź����Ѵ���
				SysSemPost(KeyEvent);					//�����ź���
		}
		EXTI->PR |= KEY_L5_PIN;							//���жϱ�־
	}
		else if(EXTI_GET_IT_STATUS(KEY_L6_PIN) != RESET){			//6���ж��ߴ���
		if(KeyPin == 0){ 								//���ǰ��û�л�ȡ������ֵ
			KeyPin = KEY_L6_PIN;						//�жϵõ���ֵ
			KeyPort = KEY_L6_PORT;
			if(KeyEvent != 0)							//�ź����Ѵ���
				SysSemPost(KeyEvent);					//�����ź���
		}
		EXTI->PR |= KEY_L6_PIN;							//���жϱ�־
	}
		else if(EXTI_GET_IT_STATUS(KEY_L7_PIN) != RESET){			//7���ж��ߴ���
		if(KeyPin == 0){ 								//���ǰ��û�л�ȡ������ֵ
			KeyPin = KEY_L7_PIN;						//�жϵõ���ֵ
			KeyPort = KEY_L7_PORT;
			if(KeyEvent != 0)							//�ź����Ѵ���
				SysSemPost(KeyEvent);					//�����ź���
		}
		EXTI->PR |= KEY_L7_PIN;							//���жϱ�־
	}
}


/*
*********************************************************************************************************
*	�� �� ��: bsp_InitKeyHard
*	����˵��: ���ð�����Ӧ��GPIO
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/

void bsp_InitKeyHard(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE );
	
	
	//IO����
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//5��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;						//����ģʽ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	 					//�������� 
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | 
									GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_14 | GPIO_Pin_15;		
	GPIO_Init(GPIOE, &GPIO_InitStructure);	
	
	
	//6��
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;	 					//���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;						//����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;					//50MHZS 
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | 
									GPIO_Pin_13 ;	
	GPIO_Init(GPIOE, &GPIO_InitStructure);		
}


/****************************************************************************
* ��	�ƣ�Init_Matrix_Key_Pin()
* ��	�ܣ���ʼ��������̹ܽ�
* ��ڲ�����
* ���ڲ�����
* ˵	������
****************************************************************************/
static void Init_Matrix_Key_Pin(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//ʱ��ʹ��
	RCC_AHB1PeriphClockCmd(KEY_H1_RCC, ENABLE );		
// 	RCC_AHB1PeriphClockCmd(KEY_H2_RCC, ENABLE );		
// 	RCC_AHB1PeriphClockCmd(KEY_H3_RCC, ENABLE );		
// 	RCC_AHB1PeriphClockCmd(KEY_H4_RCC, ENABLE );		
// 	RCC_AHB1PeriphClockCmd(KEY_H5_RCC, ENABLE );		
// 	RCC_AHB1PeriphClockCmd(KEY_L1_RCC, ENABLE );	
// 	RCC_AHB1PeriphClockCmd(KEY_L2_RCC, ENABLE );
// 	RCC_AHB1PeriphClockCmd(KEY_L3_RCC, ENABLE );	
// 	RCC_AHB1PeriphClockCmd(KEY_L4_RCC, ENABLE );
// 	RCC_AHB1PeriphClockCmd(KEY_L5_RCC, ENABLE );	
// 	RCC_AHB1PeriphClockCmd(KEY_L6_RCC, ENABLE );
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);		
	
	//IO����
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//5��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;						//����ģʽ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	 					//�������� 
	
	GPIO_InitStructure.GPIO_Pin   = KEY_L1_PIN;		
	GPIO_Init(KEY_L1_PORT, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin   = KEY_L2_PIN;		
	GPIO_Init(KEY_L2_PORT, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin   = KEY_L3_PIN;		
	GPIO_Init(KEY_L3_PORT, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin   = KEY_L4_PIN;		
	GPIO_Init(KEY_L4_PORT, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin   = KEY_L5_PIN;		
	GPIO_Init(KEY_L5_PORT, &GPIO_InitStructure);		
	GPIO_InitStructure.GPIO_Pin   = KEY_L6_PIN;		
	GPIO_Init(KEY_L6_PORT, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin   = KEY_L7_PIN;		
	GPIO_Init(KEY_L7_PORT, &GPIO_InitStructure);			
	//6��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	 					//���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;						//����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;					//50MHZS 
	
	GPIO_InitStructure.GPIO_Pin   = KEY_H1_PIN;		
	GPIO_Init(KEY_H1_PORT, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin   = KEY_H2_PIN;		
	GPIO_Init(KEY_H2_PORT, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin   = KEY_H3_PIN;		
	GPIO_Init(KEY_H3_PORT, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin   = KEY_H4_PIN;		
	GPIO_Init(KEY_H4_PORT, &GPIO_InitStructure);	
// 	GPIO_InitStructure.GPIO_Pin   = KEY_H5_PIN;		
// 	GPIO_Init(KEY_H5_PORT, &GPIO_InitStructure);	
// 	GPIO_InitStructure.GPIO_Pin   = KEY_H6_PIN;		
// 	GPIO_Init(KEY_H6_PORT, &GPIO_InitStructure);	
}
/****************************************************************************
* ��	�ƣ�Matrix_Key_IntEnable()
* ��	�ܣ��ж�������
* ��ڲ�����BOOL	TRUE-ʹ��	;	FALSE-ʧ��
* ���ڲ�����
* ˵	������
****************************************************************************/
static u32 Matrix_Key_IntEnable(BOOL bEn)
{
	EXTI_InitTypeDef EXTI_InitStructure; 
	//�ⲿ�ж�����
    EXTI_InitStructure.EXTI_Line = KEY_L1_PIN | KEY_L2_PIN | KEY_L3_PIN | KEY_L4_PIN | KEY_L5_PIN | KEY_L6_PIN | KEY_L7_PIN ;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 
    if (TRUE==bEn) {
    	EXTI_InitStructure.EXTI_LineCmd = ENABLE; 
    } else {
    	EXTI_InitStructure.EXTI_LineCmd = DISABLE; 
    }
	EXTI_Init(&EXTI_InitStructure);	
	
	return EXTI_InitStructure.EXTI_Line;
}
/****************************************************************************
* ��	�ƣ�Init_Matrix_Key_IRQ()
* ��	�ܣ���ʼ����������ж�
* ��ڲ�����
* ���ڲ�����
* ˵	������
****************************************************************************/
static BOOL Init_Matrix_Key_IRQ(void)
{
//	EXTI_InitTypeDef EXTI_InitStructure; 
	NVIC_InitTypeDef NVIC_InitStructure;
	//�жϹܽ�ӳ��
	SYSCFG_EXTILineConfig(KEY_L1_EXTI_PORT, KEY_L1_EXTI_PIN);	
	SYSCFG_EXTILineConfig(KEY_L2_EXTI_PORT, KEY_L2_EXTI_PIN);	
	SYSCFG_EXTILineConfig(KEY_L3_EXTI_PORT, KEY_L3_EXTI_PIN);	
	SYSCFG_EXTILineConfig(KEY_L4_EXTI_PORT, KEY_L4_EXTI_PIN);	
	SYSCFG_EXTILineConfig(KEY_L5_EXTI_PORT, KEY_L5_EXTI_PIN);	
	SYSCFG_EXTILineConfig(KEY_L6_EXTI_PORT, KEY_L6_EXTI_PIN);	
	SYSCFG_EXTILineConfig(KEY_L7_EXTI_PORT, KEY_L7_EXTI_PIN);	

	//SYSCFG_EXTILineConfig(KEY_L6_EXTI_PORT, KEY_L6_EXTI_PIN);
	//�ж�������
/*    EXTI_InitStructure.EXTI_Line = */Matrix_Key_IntEnable(TRUE);
	//�жϷ���������
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	Key_IRQ.EXTI_Flag = 1;													//�жϱ�־	
	{
		NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
		NVIC_Init(&NVIC_InitStructure);
		Key_IRQ.EXTI_No = 5;												//�ж��ߺ�??
		Key_IRQ.IRQHandler = KEY_IRQHandler;								//�жϺ���		
		if(EXTIIRQHandlerInstall(&Key_IRQ) != HVL_NO_ERR)					//�ж�ע��
			return FALSE;		//ʧ��
	}	
	Key_IRQ.EXTI_Flag = 1;													//�жϱ�־	
	{
		NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
		NVIC_Init(&NVIC_InitStructure);
		Key_IRQ.EXTI_No = 10;												//�ж��ߺ�??
		Key_IRQ.IRQHandler = KEY_IRQHandler;								//�жϺ���		
		if(EXTIIRQHandlerInstall(&Key_IRQ) != HVL_NO_ERR)					//�ж�ע��
			return FALSE;		//ʧ��
	}		
	return TRUE;
}


 u32 Key_swap(void)
{
	u32 RowKey;
	u32 key ;
	
	key = GPIO_ReadInputData(GPIOE);
	SysTimeDly(20);
	
	GPIO_SetBits(GPIOE, GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 );
	GPIO_ResetBits(GPIOE, GPIO_Pin_10);
	RowKey = GPIO_ReadInputData(GPIOE)&0xc3E0;
	
	GPIO_SetBits(GPIOE, GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 );
	
	if (RowKey != 0xc3E0) {
		
		return (RowKey>>5) + 0x3E00;
	}
	
	GPIO_SetBits(GPIOE, GPIO_Pin_10 | GPIO_Pin_12 | GPIO_Pin_13 );
	GPIO_ResetBits(GPIOE, GPIO_Pin_11);	
	RowKey = GPIO_ReadInputData(GPIOE)&0xc3E0;
	
	GPIO_SetBits(GPIOE, GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 );
	if (RowKey != 0xc3E0) {
		
		return (RowKey>>5) + 0x3D00;
	}	
	GPIO_SetBits(GPIOE, GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_13 );
	GPIO_ResetBits(GPIOE, GPIO_Pin_12);		
	RowKey = GPIO_ReadInputData(GPIOE)&0xc3E0;
	
	GPIO_SetBits(GPIOE, GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 );
	if (RowKey != 0xc3E0) {
		
		return (RowKey>>5) + 0x3B00;
	}	
	GPIO_SetBits(GPIOE, GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12  );
	GPIO_ResetBits(GPIOE, GPIO_Pin_13);	
	RowKey = GPIO_ReadInputData(GPIOE)&0xc3E0;
	
	GPIO_SetBits(GPIOE, GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 );
	if (RowKey != 0xc3E0) {
		
		return (RowKey>>5) + 0x3700;
	}	
	
	
	return 0x3F00 + 0x1F;
}


/****************************************************************************
* ��	�ƣ�u8 KeyScanCheckL(void)
* ��	�ܣ���������м��	��ѯģʽ��
* ��ڲ�������
* ���ڲ�����u8 �����к�			��255�ҵ�				255û�ҵ�
* ˵	����ɨ���ƽ��ʽ����һ�ΰ�����ð���ֵ���ڶ���Ϊȷ�ϰ���ֵ
****************************************************************************/
static u8 KeyScanCheckL(void)
{
	if(0 == KEY_GPIO_STATE_GET(KeyPort, KeyPin)){
		switch(KeyPin){
			
			case KEY_L6_PIN:return 0;
			case KEY_L7_PIN:return 1;			
			case KEY_L1_PIN:return 2;
			case KEY_L2_PIN:return 3;
			case KEY_L3_PIN:return 4;
			case KEY_L4_PIN:return 5;
			case KEY_L5_PIN:return 6;

			//case KEY_L6_PIN:return 5;
			default :break;
		}
	}
	return  255;												//û�ҵ�����ֵ
}
/****************************************************************************
* ��	�ƣ�u8 KeyCheckH(void)
* ��	�ܣ���������м��
* ��ڲ�������
* ���ڲ������к�
* ˵	����ɨ��õ��к�
****************************************************************************/	
static u8 KeyCheckH(void)	
{
	int a = 0;
	//���н���������
	MATRIX_KEY_ROW1SET;			//1������
	SysTimeDly(6);
	if(KEY_GPIO_STATE_GET(KeyPort, KeyPin) == 1){//���ǰ�ڵ��к������Ƿ�Ϊ�� ������Ҳ�������� �򰴼������ڸ���
		a = 0;
		return 0;
	}	
	MATRIX_KEY_ROW2SET;			//2������
	SysTimeDly(6);
	if(KEY_GPIO_STATE_GET(KeyPort, KeyPin) == 1){//���ǰ�ڵ��к������Ƿ�Ϊ�� ������Ҳ�������� �򰴼������ڸ���
				a = 1;
		return 1;
	}	
	MATRIX_KEY_ROW3SET;			//3������
	SysTimeDly(6);
	if(KEY_GPIO_STATE_GET(KeyPort, KeyPin) == 1){//���ǰ�ڵ��к������Ƿ�Ϊ�� ������Ҳ�������� �򰴼������ڸ���
				a = 2;
		return 2;
	}	
	MATRIX_KEY_ROW4SET;			//4������
	SysTimeDly(6);
	if(KEY_GPIO_STATE_GET(KeyPort, KeyPin) == 1){//���ǰ�ڵ��к������Ƿ�Ϊ�� ������Ҳ�������� �򰴼������ڸ���
				a = 3;
		return 3;
	}	
// 	MATRIX_KEY_ROW5SET;			//5������
// 	if(KEY_GPIO_STATE_GET(KeyPort, KeyPin) == 1){//���ǰ�ڵ��к������Ƿ�Ϊ�� ������Ҳ�������� �򰴼������ڸ���
// 		return 4;
// 	}
// 	MATRIX_KEY_ROW6SET;			//6������
// 	if(KEY_GPIO_STATE_GET(KeyPort, KeyPin) == 1){//���ǰ�ڵ��к������Ƿ�Ϊ�� ������Ҳ�������� �򰴼������ڸ���
// 		return 5;
// 	}	
	return 255;
}
/****************************************************************************
* ��	�ƣ�u8 KeyCheck(u8* K_H,u8* K_L)
* ��	�ܣ�����������м��
* ��ڲ�����u8* K_H	�õ�����ֵ
			u8* K_L	�õ�����ֵ
* ���ڲ�����BOOL �Ƿ�ȡ�ð���ֵ	falseû��ȡ��	trueȡ��
* ˵	�����õ�������̵�����ֵ
			δʹ���ж�
****************************************************************************/	
BOOL KeyCheck(u8* K_H,u8* K_L)
{

	if( (*K_L = KeyScanCheckL()) >= MATRIX_KEY_ROW){		//���к� �����ֵ����ȷ
		return FALSE;
	}	
	SysTimeDly(4);											//20������ʱ

 	if(*K_L != KeyScanCheckL()){	 						//���û�м�⵽��ֵ���������µĳ����Բ���
 		return FALSE;
	}
	*K_H = KeyCheckH(); 									//�õ���ֵ
	//�и�λ
	
	SysTimeDly(4);
	MATRIX_KEY_ROW1RESET;			
	MATRIX_KEY_ROW2RESET;			
	MATRIX_KEY_ROW3RESET;			
	MATRIX_KEY_ROW4RESET;		
	SysTimeDly(4);
// 	MATRIX_KEY_ROW5RESET;	
// 	MATRIX_KEY_ROW6RESET;
	
	if( *K_H >= MATRIX_KEY_LINE ){							//��ֵ���
		return FALSE;
	}
	return TRUE;											//����ֵ���ڷ�Χ�ڻ��⵽��������
}
/****************************************************************************
* ��	�ƣ�static s8 KEY_Open(u32 lParam)
* ��	�ܣ��豸��
* ��ڲ�����u32 lParam		�豸���ò���
* ���ڲ�����s8						�򿪲�������ֵ	��HVL_ERR_CODE��
* ˵	������
****************************************************************************/
static s8 KEY_Open(INT32U lParam)
{
	Matrix_Key_IntEnable(TRUE);
	return HVL_NO_ERR;					//���豸�ɹ�
}

/****************************************************************************
* ��	�ƣ�static s8 KEY_Close(u32 lParam)
* ��	�ܣ��豸�ر�
* ��ڲ�����u32 lParam		�豸���ò���
* ���ڲ�����s8						�򿪲�������ֵ	��HVL_ERR_CODE��
* ˵	����
****************************************************************************/
static s8 KEY_Close(u32 lParam)
{
	Matrix_Key_IntEnable(FALSE);
	return HVL_NO_ERR;					//�ر��豸�ɹ�
}
/****************************************************************************
* ��	�ƣ�static s32 BEEP_Read(u8* buffer,u32 len,u32* Reallen,u32 lParam) 
* ��	�ܣ���
* ��ڲ�����u8* buffer		����
						u32 len				����
						u32* Reallen	����
						u32 lParam		����
* ���ڲ�����s32						����ֵ	0û��	��0������ֵ
* ˵	������
****************************************************************************/
static s32 KEY_Read(u8* buffer,u32 len) 
{
	s8	s8KeyValue;
	u8	u8KeyLine,u8KeyRow,err;
	
	SysSemPend(KeyEvent,0,&err);			//�ȴ��жϷ��������͵��ź���
 	if(err != SYS_NO_ERR)					
 		return -1;		
	Matrix_Key_IntEnable(FALSE);				//�ر��ж�
	if(KeyCheck(&u8KeyLine,&u8KeyRow) == FALSE)	//��⵽�������� 
		{s8KeyValue = -2;}						//���󰴼�ֵ
		else{s8KeyValue = (VKEY_VAL)KeyTable[u8KeyLine][u8KeyRow];}
	KeyPin = 0;	
	Matrix_Key_IntEnable(TRUE);					//���ж�
	return s8KeyValue;
}

/****************************************************************************
* ��	�ƣ�static s32 KEY_Write(u8* buffer,u32 len,u32* Reallen,u32 lParam) 
* ��	�ܣ�д
* ��ڲ�����u8* buffer		����Դ������					����
						u32 len				ϣ��д��ĳ���				����
						u32* Reallen	ʵ��д��ĳ���				����
						u32 lParam		����									����
* ���ڲ�����s32						�����Ƿ�ɹ�		0�ɹ�	1ʧ��			����
* ˵	������
****************************************************************************/
static s32 KEY_Write(u8* buffer,u32 len) 
{
	return 0;
}
/****************************************************************************
* ��	�ƣ�static INT32S KEY_CHARIoctl(INT32U cmd, INT32U lParam)
* ��	�ܣ��豸����
* ��ڲ�����INT32U cmd				����
						INT32U lParam			����
* ���ڲ�����INT32S						��������Ĳ�ͬ���ص�ֵ���岻ͬ
* ˵	����	��
****************************************************************************/
static s32 KEY_CHARIoctl(s32 cmd, s32 lParam)
{
	switch(cmd){
		case KEYCMD_SETONOFF:{											//���ÿ���
			Matrix_Key_IntEnable((BOOL)lParam);		
			return 1;								
		}
		case KEYCMD_LookONOFF:											//��ѯ����
			return ( *((volatile uint32_t*)EXTI_BASE)&&KEY_L1_PIN );	//���ص�һ���жϵĿ���״̬
		case (u32)KEYCMD_LookLCount:									//��ѯ��������
			return MATRIX_KEY_ROW;	
		case (u32)KEYCMD_LookHCount:									//��ѯ��������
			return MATRIX_KEY_LINE;	
		case (u32)KEYCMD_LookDevMode:									//��ѯ�豸����ģʽ
			return 1;	
		case (u32)KEYCMD_LookEventFlag:									//��ѯ�ж�ģʽ���ź����Ƿ�����
			return 1;	
		default:break;
	}
	return 1;	
}
/****************************************************************************
* ��	�ƣ�Key_Config()
* ��	�ܣ���ʼ���������
* ��ڲ�����
* ���ڲ�����BOOL	TRUE:	�ɹ�		
					FALSE:	ʧ��
* ˵	������
****************************************************************************/
BOOL Key_Config(void)
{
	DEV_REG Dev_KEY ={
		CHAR_KEY,						//�ַ��豸��		��DEV_CHAR_IDȡƥ���豸ID
		0,								//�Ƿ���			0:���ɹ���ʹ��, 1:�ɹ���ʹ��
		0,								//���ڹ����豸���򿪴���
		1,								//��������
		1,								//���д����
		(u8*)Key_pDisc,					//��������			�Զ���
		20130001,						//�����汾			�Զ���
		(u32*)KEY_Open,					//�豸�򿪺���ָ��
		(u32*)KEY_Close,				//�豸�رպ���ָ��
		(u32*)KEY_Read,					//�豸������ӳ��ָ��
		(u32*)KEY_Write,				//�豸д����ӳ��ָ��
		(u32*)KEY_CHARIoctl				//�豸���ƺ���ӳ��ָ��
	};
	//ע��
	if(DeviceInstall(&Dev_KEY) != HVL_NO_ERR){
		return FALSE;	
	}
	Init_Matrix_Key_Pin();				//�ܽų�ʼ��
	if(FALSE==Init_Matrix_Key_IRQ()){	//�жϳ�ʼ��
		return FALSE;
	}		
	if(KeyEvent == (SYS_EVENT*)0){	
		KeyEvent = SysSemCreate(0); 		//���������ź���
	}
	return TRUE;
}

void Key_Inif_KeyHard(void)
{
		Init_Matrix_Key_Pin();				//�ܽų�ʼ��
}

VKEY_VAL Key_Get_KeyValue(u8 line, u8 row)
{
		s8	s8KeyValue;
		s8KeyValue = (VKEY_VAL)KeyTable[line][row];
	return s8KeyValue;
}

