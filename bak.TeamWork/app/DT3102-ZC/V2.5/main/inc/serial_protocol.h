/******  file description   ******/
/*	created time: 2012-11-10 22:00
 *  author:  
 *	file type:	C/C++ header file
 *	path:		src/wj_test
 *	latest modified:	2012-11-29
 *********************************/

#ifndef __SERIAL_PROTOCOL_H__
#define __SERIAL_PROTOCOL_H__

#include "includes.h"
#include "usart_cfg.h"			//�������ú���
#include "usart6.h"				//����6����SDKͨ���߳�
#include "usart1.h"				
#include "overtime_queue.h"

//sdkЭ�����
void SDK_Protocol_Parse(char *buf, int *blen);


/*���º���ΪЭ�������������,����Э������ݸ�ʽ������ݰ�, ͨ�����ڷ��͸�SDK*/
/*
 * Function:���ʹ��������ݶ���Ԫ��
 * Param: 
 */
void Send_SDK_OverQueueData(OvertimeData *pCar);

/*
 * Function: ����sdk��Ӧ
 * Param:
 *	flag 1:���ͳ���14,��Ҫ��Ӧret 0:���ͳ���13,����Ҫ��Ӧret
 *	ret: ��Ӧ���; cmd: �����; id: ��ˮ��
 */
void Send_SDK_Ack(uint8 flag, uint8 cmd, uint16 id, uint8 ret);

#define SDK_OVERTIME_TICK 50 //Ĭ��Э����500ms, ���ճ�ʱ��2��
#endif //__SERIAL_PROTOCOL_H__
