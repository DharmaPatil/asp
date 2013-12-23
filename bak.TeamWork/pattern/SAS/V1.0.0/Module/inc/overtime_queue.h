/* 
 * File:    overtime_data.h
 * Brief:   overtime carinfo queue
 *
 * History:
 * 1. 2012-11-14 �����ļ�������ӿ� river
 * 2. 2012-11-24 ʵ�ֶ��� ���
 */
#ifndef OVERTIME_DATA_H
#define OVERTIME_DATA_H
	 
#ifdef __cplusplus
	 extern "C" {
#endif

#include "includes.h"

#define MAX_AXIS_NUM 20

typedef struct  {
	BOOL	 SendMod;		//�������� �������ͱ�־λ
	uint8	 cmd; 			//�����
	uint8	 bSendAttr;	//��������: ���� ����
	uint8	 SendNum;  		//���ʹ���
	uint16   SerialID;		//��ˮ��
	uint8	 bSend;			//���ͱ�־
	uint8    bSave;			//�����洢��־
	uint8	 LongCarSegs; 	//���������� ��������0ʱΪ����
	uint16	 year;	    	//ʱ��
	uint8	 mon;
	uint8	 day;
	uint8	 hour;
	uint8	 min;
	uint8	 sec;
	uint8	 CarStatus;				//����״̬��(��β״̬)
	uint8	 CarDirection;				//�����н�����(˫��̨)
	uint8  	 ScalerOverWetFlag;		//��̨���ر�־
	uint8	 OverWetFlag;				//�������ر�־
	uint32   TotalWet;                 //����	
	uint8	 AxleNum;                  	//����
	uint8	 AxleGroupNum;				//������
	uint16   AxleWet[MAX_AXIS_NUM];  	//��������
	uint8	 AxleType[MAX_AXIS_NUM];	//����, ��˫��
	uint8	 AxleGroupType[MAX_AXIS_NUM]; 	//��������
	uint16	 AxleLen[MAX_AXIS_NUM];			//����
	uint16   speed;					   		//����
	uint16	 speeda;						//���ٶ�
	uint16	 sCheck;						//������Ϣ��У���, �洢У��ʹ��
}OvertimeData;


//������Ϣ����
enum {
	IORequstCmd = 1,
	NormalCarCmd,
	BackCarCmd,
	RepeatCarCmd,
	ForceCarCmd,
	LongCarCmd,
	PayNoticeCmd,
	IOContrlCmd,
	TimeSetCmd,
	USARTSDKTest,
	ACKStaticWetAndAxle,	//�ɽ��Ŀ
	ClearLastCarAxle,			//�ɽ��Ŀ-������һ����������
	SetZeroCmd						//�ɽ��Ŀ-����
};

enum {
	SendActiveMod = 1,		//��������
	SendUnActiveMod		//��������
};

/* ���г�ʼ��*/
void OverQueue_Init(void);

//��ȡ���к�
unsigned short OverQueue_GetSeriID(void);

/* ������� */
int OverQueue_Enqueue(OvertimeData *data);

/* �Ƴ���ͷԪ�� */
void OverQueue_Remove_Head(void);

/* �Ƴ���βԪ�� */
void OverQueue_Remove_Tail(void);

/* ��ȡ��ͷԪ�� */
OvertimeData *OverQueue_Get_Head(void);

/* ��ȡ��βԪ�� */
OvertimeData *OverQueue_Get_Tail(void);

/* ���Ӷ�ͷԪ�ط��ʹ��� */
void OverQueue_Add_HeadSendNum(void);

/* ��ȡ��ͷԪ�ط��ʹ��� */
int OverQueue_Get_HeadSendNum(void);

/* �����Ƿ�Ϊ�� */
//BOOL OverQueue_Empty(void);

//��ȡ���г���
int OverQueue_Get_Size(void);

OvertimeData *OverQueue_Get_Index(uint8 index);


#ifdef __cplusplus
}
#endif

#endif



