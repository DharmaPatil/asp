#ifndef APP_MSG_H
#define APP_MSG_H
	 
#ifdef __cplusplus
	 extern "C" {
#endif

#include "includes.h"
#include "task_msg.h"

//������Ϣ�Ż�ַ��������Ҫ�޸�
enum {
	Msg_Data_Base		= 100,
	Msg_Business_Base = 200,
	Msg_NetSend_Base	= 300,
};

//ҵ�����߳̽�����Ϣ
//��Ϣ�ŵ�����Ҫ��scszc_debug_info.c��� strMsgArr �ַ�������ƥ��
enum{
	Msg_None  =  Msg_Business_Base,

/*����ʽ��Ϣ��*/
	Msg_Reset,			//ϵͳ��λ
	Msg_Refresh,		//ϵͳˢ��
	Msg_GC1_Enter,		//ǰ�ظд���
	Msg_GC1_Leave,		//ǰ�ظ�����
	Msg_GC2_Enter,		//��ظ�
	Msg_GC2_Leave,		
	Msg_LC1_Enter,		//ǰ��Ļ����
	Msg_LC1_Leave,		//ǰ��Ļ����
	Msg_LC2_Enter,	  	//�ֳ���Ļ
	Msg_LC2_Leave,
	Msg_PreAlex_Come,		//ǰ��ʶ��������
	Msg_PreAlex_Add,		//ǰ��ʶ��������
	Msg_PreAlex_Remove,	//ǰ��ʶ��������
	Msg_BackAlex,			//����ʶ��������
	Msg_Cmd_Repeat,		//���²ɼ�
	Msg_Cmd_PlusSave,		//�ֶβɼ�����
	Msg_Cmd_PayNotify,
	Msg_Cmd_Force,			//ǿ�Ʋɼ�����
	Msg_Tick,				//10msʱ��

	Msg_WBAxle				//�������Ϣ
};

/*˫��̨��Ϣ��*/
enum {
	VP_GM_TRIG = Msg_Business_Base + 50,      // ��Ļ����  		=250
	VP_GM_RELE,         // ��Ļ�ͷ�			
	VP_DG_TRIG,         // �ظд���		
	VP_DG_RELE,         // �ظ��ͷ�		
	VP_LZ_SINGLE,       // ����			
	VP_LZ_DOUBLE,       // ˫��				
	VP_VEHUP_A,         // A �ϳ�		
	VP_VEHDOWN_A,       // A �³�			
	VP_VEHUP_B,         // B �ϳ�		
	VP_VEHDOWN_B,       // B �³�			
	VP_MSG_TICK,		// 2msʱ����Ϣ		
};

//ҵ�����ݹ����߳̽�����Ϣ
enum {
	Msg_Device_Heart = Msg_Data_Base,	 //10s��ʱ�ϱ�����״̬, �����г����ϴ�
	Msg_Device_Fault,		//���輴ʱ������Ϣ
	Msg_Overtime,			//��ʱ��Ϣ
	Msg_BackCarInfo,		//������Ϣ
	Msg_CarInfo_Ready		//������Ϣ
};

//���緢���߳̽�����Ϣ
enum {
	Msg_NetSend = Msg_NetSend_Base,
	Msg_NetDebug
};
	
typedef struct 
{
	uint32 priv;		//�û��Զ���ָ��
	uint16 msg_id;		//��Ϣ��
	uint16 msg_len;		//���ݳ���
	uint32 msg_ctx;		//����ָ��
}TaskMsg;

//Ӧ�ò���Ϣ����ȫ�ֱ���
extern MsgQ NetMsgQ;
extern MsgQ BusinessMsgQ; 
extern MsgQ DataManagerMsgQ;

/*Ӧ�ò���Ϣ���г�ʼ��*/
void App_Msg_Init(void);

/* ���ʹ�����adֵ���豸״̬  */
BOOL Net_Send_Wave(void *buf);


/* ���յ���Ϣ�ͷ��ڴ� */
uint8 Wave_Mem_Free(void *pblk);

#ifdef __cplusplus
}
#endif

#endif


