/*
*********************************************************************************************************
*
*	ģ������ : ��������ģ��
*	�ļ����� : bsp_key.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#define KEY_COUNT    30	   					/* �������� */

/*
	�����˲�ʱ��50ms, ��λ10ms��
	ֻ��������⵽50ms״̬�������Ϊ��Ч����������Ͱ��������¼�
	��ʹ������·����Ӳ���˲������˲�����Ҳ���Ա�֤�ɿ��ؼ�⵽�����¼�
*/
#define KEY_FILTER_TIME   2
#define KEY_LONG_TIME     20			/* ��λ10ms�� ����1�룬��Ϊ�����¼� */

/*
	ÿ��������Ӧ1��ȫ�ֵĽṹ�������
*/
typedef struct
{
	/* ������һ������ָ�룬ָ���жϰ����ַ��µĺ��� */
	uint8_t (*IsKeyDownFunc)(void); /* �������µ��жϺ���,1��ʾ���� */

	uint8_t  Count;			/* �˲��������� */
	uint16_t LongCount;		/* ���������� */
	uint16_t LongTime;		/* �������³���ʱ��, 0��ʾ����ⳤ�� */
	uint8_t  State;			/* ������ǰ״̬�����»��ǵ��� */
	uint8_t  RepeatSpeed;	/* ������������ */
	uint8_t  RepeatCount;	/* �������������� */
}KEY_T;

/*
	�����ֵ����, ���밴���´���ʱÿ�����İ��¡�����ͳ����¼�

	�Ƽ�ʹ��enum, ����#define��ԭ��
	(1) ����������ֵ,�������˳��ʹ���뿴���������
	(2) �������ɰ����Ǳ����ֵ�ظ���
*/

#if 0
typedef enum
{
	KEY_NONE = 0, 
	KEY_ERR = 0,         							//���󰴼� 
	KEY_0,											//0
	KEY_1,											//1
	KEY_2,											//2
	KEY_3,											//3
	KEY_4,											//4
	KEY_5,											//5
	KEY_6,											//6
	KEY_7,											//7
	KEY_8,											//8
	KEY_9,											//9
	KEY_Del,										//ɾ��
	KEY_UNIT,										//��λ
	KEY_OK,											//ȷ��
	KEY_QUERY,										//��ѯ
	KEY_ADD,										//+-
	KEY_DOT,										//.
	KEY_PRINT,										//��ӡ
	KEY_UP,											//��
	KEY_LEFT,										//��
	KEY_RIGHT,										//��
	KEY_MENU,										//�˵�
	KEY_BACK,										//����
	KEY_DOWN,										//��
	KEY_CAR,										//����
	KEY_AXIS,										//����
	KEY_DIAGNOSIS,									//���
	KEY_ZERO,										//����
}VKEY_VAL; 									//����ֵ

#endif 

/* ����FIFO�õ����� */
#define KEY_FIFO_SIZE	40

#define KEY_DOWN	1
#define KEY_UP		2
#define KEY_LONG	3 

typedef struct
{
	uint8_t KeyCode;					/* ��������ָ�� */
	uint8_t KeyValue;					/* ������дָ�� */
}KEY_INFO_T;

typedef struct
{
	KEY_INFO_T 	Buf[KEY_FIFO_SIZE];		/* ��ֵ������ */
	uint8_t 		Read;					/* ��������ָ�� */
	uint8_t 		Write;					/* ������дָ�� */
}KEY_FIFO_T;

/* ���ⲿ���õĺ������� */
void bsp_InitKey(void);
void bsp_KeyScan(void);
void bsp_PutKey(uint8_t _KeyCode, uint8_t _KeyValue);
uint16_t bsp_GetKey(void);
uint8_t bsp_GetKeyState(VKEY_VAL _ucKeyID);

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
