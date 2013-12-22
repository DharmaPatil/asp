#ifndef	_KEY_INIT_H_
#define	_KEY_INIT_H_
#include <stdbool.h>
#include "sys_cpu.h"
#include "stm32f4xx.h"

#define 	KEYCMD_SETONOFF										0		//�豸��������
#define 	KEYCMD_LookONOFF									1		//�豸���ػ�ȡ
#define		KEYCMD_LookLCount									2		//��ѯ��������
#define		KEYCMD_LookHCount									3		//��ѯ��������
#define		KEYCMD_LookDevMode									4		//��ѯ�豸����ģʽ	1�ж�	0һֱɨ��
#define		KEYCMD_LookEventFlag								5		//��ѯ�ж�ģʽ���Ƿ������ź�������	1����	0û����


#define	MATRIX_KEY_LINE	4	//��������
#define	MATRIX_KEY_ROW	7	//��������

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
	
	
	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	
}VKEY_VAL; 									//����ֵ

/****************************************************************************
* ��	�ƣ�Init_Matrix_Key()
* ��	�ܣ���ʼ���������
* ��ڲ�����
* ���ڲ�����bool	TRUE:	�ɹ�		
					FALSE:	ʧ��
* ˵	������
****************************************************************************/
extern BOOL Key_Config(void);
BOOL KeyCheck(u8* K_H,u8* K_L);
void Key_Inif_KeyHard(void);
VKEY_VAL Key_Get_KeyValue(u8 line, u8 row);

#endif


