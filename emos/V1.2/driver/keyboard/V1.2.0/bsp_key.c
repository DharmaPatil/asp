/*
*********************************************************************************************************
*
*	ģ������ : ������������ģ��
*	�ļ����� : bsp_key.c
*	��    �� : V1.0
*	˵    �� : ɨ�������������������˲����ƣ����а���FIFO�����Լ�������¼���
*				(1) ��������
*				(2) ��������
*				(3) ������
*				(4) ����ʱ�Զ�����
*
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2013-02-01 armfly  ��ʽ����
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/

//#include <rthw.h>
//#include <rtthread.h>

#include "Keyboard.h"
#include "Key_cfg.h"
#include "stm32f4xx.h"
#include "c2.h"
#include "driver.h"
#include "dt3102_IO.h"
#include "IO.h"


#include "bsp_key.h"
#include "key_cfg.h"
#include "Keyboard.h"

static KEY_T s_tBtn[KEY_COUNT + 1]; 
static KEY_FIFO_T s_tKey;		/* ����FIFO����,�ṹ�� */
 
 u16 KeyPin = 0;	
 GPIO_TypeDef* KeyPort = (GPIO_TypeDef*)0;

/*
	�ó��������ڰ�����STM32-X4��STM32-F4������

	�����������Ӳ�������޸�GPIO����� IsKeyDown1 - IsKeyDown8 ����

	����û��İ�������С��8��������Խ�����İ���ȫ������Ϊ�͵�1������һ��������Ӱ�������
	#define KEY_COUNT    8	  ����� bsp_key.h �ļ��ж���
*/

/*
*********************************************************************************************************
*	�� �� ��: IsKeyDownX
*	����˵��: �жϰ����Ƿ���
*	��    ��: ��
*	�� �� ֵ: ����ֵ1 ��ʾ���£�0��ʾδ����
*********************************************************************************************************
*/
typedef struct
{
	uint16_t KeyPin;		/* ��������*/
	GPIO_TypeDef* KeyPort;				 
}KEY_IO_T;

static KEY_IO_T KeyIO[MATRIX_KEY_ROW + 1] = 
{
	{KEY_L1_PIN,KEY_L1_PORT},
	{KEY_L2_PIN,KEY_L2_PORT},
	{KEY_L3_PIN,KEY_L3_PORT},
	{KEY_L4_PIN,KEY_L4_PORT},
	{KEY_L5_PIN,KEY_L5_PORT},
	{KEY_L6_PIN,KEY_L6_PORT},
	{KEY_L7_PIN,KEY_L7_PORT},
};

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
static s32 bsp_KEY_Read(void)
{
	s8	s8KeyValue;
	u8	u8KeyLine,u8KeyRow,err;
	u8	i = 0;
	
	for(i = 0;i < MATRIX_KEY_ROW;i++)
	{
		KeyPin 	= KeyIO[i].KeyPin;
		KeyPort = KeyIO[i].KeyPort;
	
		//Matrix_Key_IntEnable(FALSE);				//�ر��ж�
		if(KeyCheck(&u8KeyLine,&u8KeyRow) == 0)	//��⵽�������� 
		{
			s8KeyValue = -2;
		}						//���󰴼�ֵ
		else
		{
			//s8KeyValue = (VKEY_VAL)KeyTable[u8KeyLine][u8KeyRow];
			s8KeyValue = Key_Get_KeyValue(u8KeyLine,u8KeyRow);
			return s8KeyValue;
		}
	}
	//Matrix_Key_IntEnable(TRUE);					//���ж�
	return s8KeyValue;
}


static uint8_t IsKeyDown(void)
{
		return 1;
}
/*
*********************************************************************************************************
*	�� �� ��: bsp_InitKeyVar
*	����˵��: ��ʼ����������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void bsp_InitKeyVar(void)
{
	uint8_t i;

	/* �԰���FIFO��дָ������ */
	s_tKey.Read = 0;
	s_tKey.Write = 0;

	/* ��ÿ�������ṹ���Ա������һ��ȱʡֵ */
	for (i = 0; i < KEY_COUNT; i++)
	{
		
		s_tBtn[i].IsKeyDownFunc = IsKeyDown;
		s_tBtn[i].LongTime = KEY_LONG_TIME;			/* ����ʱ�� 0 ��ʾ����ⳤ�����¼� */
		
		s_tBtn[i].Count = KEY_FILTER_TIME / 2;		/* ����������Ϊ�˲�ʱ���һ�� */
		s_tBtn[i].State = 0;							/* ����ȱʡ״̬��0Ϊδ���� */
		//s_tBtn[i].KeyCodeDown = 3 * i + 1;				/* �������µļ�ֵ���� */
		//s_tBtn[i].KeyCodeUp   = 3 * i + 2;				/* ��������ļ�ֵ���� */
		//s_tBtn[i].KeyCodeLong = 3 * i + 3;				/* �������������µļ�ֵ���� */
		s_tBtn[i].RepeatSpeed = 30;						/* �����������ٶȣ�0��ʾ��֧������ */ /* ÿ��50ms�Զ����ͼ�ֵ */

		s_tBtn[i].RepeatCount = 0;						/* ���������� */
		
	}

	/* �����Ҫ��������ĳ�������Ĳ����������ڴ˵������¸�ֵ */
	/* ���磬����ϣ������1���³���1����Զ��ط���ͬ��ֵ */
	s_tBtn[KEY_Del].LongTime = 100;
	s_tBtn[KEY_Del].RepeatSpeed = 5;	/* ÿ��50ms�Զ����ͼ�ֵ */

	s_tBtn[KEY_ADD].LongTime = 100;
	s_tBtn[KEY_ADD].RepeatSpeed = 5;	/* ÿ��50ms�Զ����ͼ�ֵ */

	s_tBtn[KEY_CAR].LongTime = 100;
	s_tBtn[KEY_CAR].RepeatSpeed = 5;	/* ÿ��50ms�Զ����ͼ�ֵ */

	s_tBtn[KEY_AXIS].LongTime = 100;
	s_tBtn[KEY_AXIS].RepeatSpeed = 5;	/* ÿ��50ms�Զ����ͼ�ֵ */

	/* �жϰ������µĺ��� */
// 	s_tBtn[0].IsKeyDownFunc = IsKeyDown1;
// 	s_tBtn[1].IsKeyDownFunc = IsKeyDown2;
// 	s_tBtn[2].IsKeyDownFunc = IsKeyDown3;
// 	s_tBtn[3].IsKeyDownFunc = IsKeyDown4;
// 	s_tBtn[4].IsKeyDownFunc = IsKeyDown5;
// 	s_tBtn[5].IsKeyDownFunc = IsKeyDown6;
// 	s_tBtn[6].IsKeyDownFunc = IsKeyDown7;
// 	s_tBtn[7].IsKeyDownFunc = IsKeyDown8;
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_DetectKey
*	����˵��: ���һ��������������״̬�����뱻�����Եĵ��á�
*	��    �Σ������ṹ����ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void bsp_DetectKey(uint8_t i,uint8_t _keyvalild)
{
	KEY_T *pBtn;

	pBtn = &s_tBtn[i];
	//if (pBtn->IsKeyDownFunc())
	if(_keyvalild)
	{
		if (pBtn->Count < KEY_FILTER_TIME)
		{
			pBtn->Count = KEY_FILTER_TIME;
		}
		else if(pBtn->Count < 2 * KEY_FILTER_TIME)
		{
			pBtn->Count++;
		}
		else
		{
			if (pBtn->State == 0)
			{
				pBtn->State = 1;

				/* ���Ͱ�ť���µ���Ϣ */
				//bsp_PutKey((uint8_t)(3 * i + 1));
				bsp_PutKey((uint8_t)(i),KEY_DOWN);
			}

			if (pBtn->LongTime > 0)
			{
				if (pBtn->LongCount < pBtn->LongTime)
				{
					/* ���Ͱ�ť�������µ���Ϣ */
					if (++pBtn->LongCount == pBtn->LongTime)
					{
						/* ��ֵ���밴��FIFO */
						//bsp_PutKey((uint8_t)(3 * i + 3));
						bsp_PutKey((uint8_t)(i),KEY_LONG);
					}
				}
				else
				{
					if (pBtn->RepeatSpeed > 0)
					{
						if (++pBtn->RepeatCount >= pBtn->RepeatSpeed)
						{
							pBtn->RepeatCount = 0;
							/* ��������ÿ��10ms����1������ */
							//bsp_PutKey((uint8_t)(3 * i + 1));
							bsp_PutKey((uint8_t)(i),KEY_DOWN);
						}
					}
				}
			}
		}
	}
	else
	{
		if(pBtn->Count > KEY_FILTER_TIME)
		{
			pBtn->Count = KEY_FILTER_TIME;
		}
		else if(pBtn->Count != 0)
		{
			pBtn->Count--;
		}
		else
		{
			if (pBtn->State == 1)
			{
				pBtn->State = 0;

				/* ���Ͱ�ť�������Ϣ */
				//bsp_PutKey((uint8_t)(3 * i + 2));
				bsp_PutKey((uint8_t)(i),KEY_UP);
			}
		}

		pBtn->LongCount = 0;
		pBtn->RepeatCount = 0;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitKey
*	����˵��: ��ʼ������. �ú����� bsp_Init() ���á�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitKey(void)
{
	bsp_InitKeyVar();		/* ��ʼ���������� */
	bsp_InitKeyHard();		/* ��ʼ������Ӳ�� */
	
	//Key_Inif_KeyHard();
	
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_PutKey
*	����˵��: ��1����ֵѹ�밴��FIFO��������������ģ��һ��������
*	��    �Σ�_KeyCode : ��������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_PutKey(uint8_t _KeyCode, uint8_t _KeyValue)
{
	s_tKey.Buf[s_tKey.Write].KeyCode = _KeyCode;
	s_tKey.Buf[s_tKey.Write].KeyValue = _KeyValue;

	if (++s_tKey.Write  >= KEY_FIFO_SIZE)
	{
		s_tKey.Write = 0;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_GetKey
*	����˵��: �Ӱ���FIFO��������ȡһ����ֵ��
*	��    �Σ���
*	�� �� ֵ: ��������
*********************************************************************************************************
*/
uint16_t bsp_GetKey(void)
{
	uint16_t ret = 0;

	if (s_tKey.Read == s_tKey.Write)
	{
		return KEY_ERR;
	}
	else
	{
		ret = ( s_tKey.Buf[s_tKey.Read].KeyCode << 8) + s_tKey.Buf[s_tKey.Read].KeyValue;

		if (++s_tKey.Read >= KEY_FIFO_SIZE)
		{
			s_tKey.Read = 0;
		}
		return ret;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_GetKeyState
*	����˵��: ��ȡ������״̬
*	��    �Σ�_ucKeyID : ����ID����0��ʼ
*	�� �� ֵ: 1 ��ʾ���£� 0 ��ʾδ����
*********************************************************************************************************
*/
uint8_t bsp_GetKeyState(VKEY_VAL _ucKeyID)
{
	return s_tBtn[_ucKeyID].State;
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_KeyScan
*	����˵��: ɨ�����а���������������systick�ж������Եĵ���
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_KeyScan(void)
{
	int8_t i;

	//////////������̶�ȡ����ֵ�ķ�ʽ /////////////////
	
	i = bsp_KEY_Read();
	if(i > 0)
	{
		bsp_DetectKey(i,1);
	}
	else
	{
		for(i = 0;i < KEY_COUNT;i ++)
		{
			bsp_DetectKey(i,0);	
		}
	}
}


/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
