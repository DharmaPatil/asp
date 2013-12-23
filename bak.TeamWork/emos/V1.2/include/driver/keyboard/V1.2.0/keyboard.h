#ifndef	_KEY_INIT_H_
#define	_KEY_INIT_H_
#include <stdbool.h>
#include "sys_cpu.h"

#define 	KEYCMD_SETONOFF										0		//�豸��������
#define 	KEYCMD_LookONOFF									1		//�豸���ػ�ȡ
#define		KEYCMD_LookLCount									2		//��ѯ��������
#define		KEYCMD_LookHCount									3		//��ѯ��������
#define		KEYCMD_LookDevMode									4		//��ѯ�豸����ģʽ	1�ж�	0һֱɨ��
#define		KEYCMD_LookEventFlag								5		//��ѯ�ж�ģʽ���Ƿ������ź�������	1����	0û����

typedef enum
{
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

/****************************************************************************
* ��	�ƣ�Init_Matrix_Key()
* ��	�ܣ���ʼ���������
* ��ڲ�����
* ���ڲ�����bool	TRUE:	�ɹ�		
					FALSE:	ʧ��
* ˵	������
****************************************************************************/
extern BOOL Key_Config(void);


#endif


