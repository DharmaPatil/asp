/* 
 * File:    msg.h
 * Brief:   task message defined
 *
 * History:
 * 1. 2012-11-14 �����ļ� river 
 */


/* 
 * File:    task_msg.h
 * Brief:   task message defined
 */
#ifndef TASK_MSG_H
#define TASK_MSG_H
	 
#ifdef __cplusplus
	 extern "C" {
#endif

#include "includes.h"

#define SYS_MEM		int
#define SYS_EVENT	int

typedef struct _MsgQ 
{
	SYS_MEM *pMem;
	SYS_EVENT *pEvent;
}MsgQ;

int8 Task_Msg_Create(MsgQ *pMsgQ, void ** pSysQ, void *pMemBlock, uint32 nMsgCount, uint32 nMsgSize);

uint8 Task_QPost(MsgQ *pMsgQ, void *pMsg);

uint8 Task_QPostFront(MsgQ *pMsgQ, void *pMsg);

uint8 Task_QPend(MsgQ *pMsgQ, void *pMsg, int len, uint16 timeout);


#ifdef __cplusplus
}
#endif

#endif














//#ifndef MSG_H
//#define MSG_H
//	 
//#ifdef __cplusplus
//	 extern "C" {
//#endif
//
//#include "includes.h"

//������Ϣ�Ż�ַ��������Ҫ�޸�
//enum {
//	Msg_Data_Base		= 100,
//	Msg_Business_Base = 200,
//	Msg_NetSend_Base	= 300,
//};

//ҵ�����߳̽�����Ϣ
//enum{
//	Msg_None  =  Msg_Business_Base,
//
///*����ʽ��Ϣ��*/
//	Msg_Reset,			//ϵͳ��λ
//	Msg_Refresh,		//ϵͳˢ��
//	Msg_GC1_Enter,		//ǰ�ظд���
//	Msg_GC1_Leave,		//ǰ�ظ�����
//	Msg_GC2_Enter,		//��ظ�
//	Msg_GC2_Leave,		
//	Msg_LC1_Enter,		//ǰ��Ļ����
//	Msg_LC1_Leave,		//ǰ��Ļ����
//	Msg_LC2_Enter,	  	//�ֳ���Ļ
//	Msg_LC2_Leave,
//	Msg_PreAlex_Add,		//ǰ��ʶ��������
//	Msg_PreAlex_Remove,	//ǰ��ʶ��������
//	Msg_BackAlex,			//����ʶ��������
//	Msg_Cmd_Repeat,		//���²ɼ�
//	Msg_Cmd_PlusSave,		//�ֶβɼ�����
//	Msg_Cmd_PayNotify,
//	Msg_Cmd_Force,			//ǿ�Ʋɼ�����
//	Msg_Tick				//10msʱ��
//
///*˫��̨��Ϣ��*/
//};
//
////ҵ�����ݹ����߳̽�����Ϣ
//enum {
//	Msg_Device_Heart = Msg_Data_Base,	 //10s��ʱ�ϱ�����״̬, �����г����ϴ�
//	Msg_Device_Fault,		//���輴ʱ������Ϣ
//	Msg_Overtime,			//��ʱ��Ϣ
//	Msg_BackCarInfo,		//������Ϣ
//	Msg_CarInfo_Ready		//������Ϣ
//};
//
////���緢���߳̽�����Ϣ
//#define Msg_NetSend  Msg_NetSend_Base
//
////������OS_EVENT
//enum {
//	MSG_NET_ID = 0,			//������Ϣ����
//	MSG_BUSINESS_ID,			//ҵ������Ϣ����
//	MSG_DATASEND_ID,			//ҵ�����ݹ�����Ϣ����
//
//	MSG_COUNT
//};
//
//#define TaskMsgNetLen		4
//
//#define TaskMsgBusinessLen	32
//#define TaskMsgDataSendLen	24
//#define TaskMsgBufLen			TaskMsgNetLen+TaskMsgBusinessLen+TaskMsgDataSendLen
//	
//typedef struct 
//{
//	uint32 priv;		//�û��Զ���ָ��
//	uint16 msg_id;		//��Ϣ��
//	uint16 msg_len;		//���ݳ���
//	uint32 msg_ctx;		//����ָ��
//}TaskMsg;
//
//void Task_Msg_Init(void);
//
// /* Ͷ����Ϣ */ 
//uint8 Task_QPost(uint8 task_msg_id, TaskMsg* pMsgInfo);
//
///* �ȴ���Ϣ */ 
//void* Task_QPend(uint8 task_msg_id, uint16 to, uint8 *err);
//
///* ���ʹ�����adֵ���豸״̬  */
//BOOL Net_Send_Wave(void *buf);
//
///* ���յ���Ϣ�ͷ��ڴ� */
//uint8 Wave_Mem_Free(void *pblk);
//
//
//
////uint8 Task_SemPost(uint8 task_msg_id);
////void Task_SemPend(uint8 task_msg_id, uint16 to, uint8 *err);
//
//
//#ifdef __cplusplus
//}
//#endif
//
//#endif
//
