#ifndef _BEEPJOB_INIT_H	
#define _BEEPJOB_INIT_H

#ifdef __cplusplus		   		//�����CPP����C���� //��ʼ����
extern "C" {
#endif

#include "includes.h"
#include "beep_init.h"
	
/****************************************************************************
* ��	�ƣ�u8 BEEPJOB_Init(u8 PRIO_t,SYS_EVENT* JOBEVENT)
* ��	�ܣ�BeepJOB��ʼ��
* ��ڲ�����u8 PRIO_t								�������ȼ�
						SYS_EVENT* JOBEVENT			������Ϣ����
* ���ڲ�����u8 					�Ƿ�ɹ�	1�ɹ�	0ʧ��
* ˵	������
****************************************************************************/
u8 BEEPJOB_Init(u8 PRIO_t,SYS_EVENT* JOBEVENT);
	


#ifdef __cplusplus		   		//�����CPP����C���� //��������
}
#endif

#endif


