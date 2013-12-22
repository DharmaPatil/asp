#ifndef _USART_CFG_H_
#define _USART_CFG_H_

#ifdef __cplusplus		   		//�����CPP����C���� //��ʼ����
extern "C" {
#endif

#include "includes.h"				

// ���յ�ģʽ
#define	RX_PACKET_MODE 0  //���ַ���ʱģʽ����
#define	RX_LENGTH_MODE 1	//����������
#define	RX_KEY_MODE    2	//���ؼ��ֽ���

typedef enum{
	CMD_SET_RX_TIMEOUT ,	//���ý��ճ�ʱʱ�� (ms)
	CMD_GET_RX_TIMEOUT 	, //��ȡ���ճ�ʱʱ�� (ms)
	CMD_SET_TX_TIMEOUT ,	//���÷��ͳ�ʱʱ�� (ms)
	CMD_GET_TX_TIMEOUT 	, //��ȡ���ͳ�ʱʱ�� (ms)
	CMD_SET_RX_MODE 	,		//���ý��յ�ģʽ(0,1,2)
	CMD_GET_RX_MODE 	,		//��ȡ��ǰ���յ�ģʽ
	CMD_SET_KEY_CHAR 	,		//�ؼ��ֽ���ģʽ�£����ùؼ���
	CMD_GET_KEY_CHAR 	,		//�ؼ��ֽ���ģʽ�£���ȡ�ؼ���
	CMD_SET_N_CHAR_TIMEOUT ,		//�ַ���ʱģʽ�£�����N���ַ��Ĵ���ʱ��Ϊ��ʱʱ��
	CMD_GET_N_CHAR_TIMEOUT 		, //�ַ���ʱģʽ�£���ȡN���ַ���ʱ
	CMD_FLUSH_INPUT				,	//��ս��ջ����е�����
	CMD_FLUSH_OUTPUT				, //��շ��ͻ����е�����
	CMD_GET_INPUT_BUF_SIZE 		//��ȡ���ջ����е������ֽ���
}USART_CTRL_CMD;


#ifdef __cplusplus		   		//�����CPP����C���� //��������
}
#endif

#endif
