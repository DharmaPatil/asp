/* 
 * File:    debug_info.h
 * Brief:   print debug info
 *
 * History:
 * 1. 2012-11-16 �����ļ�, ��������Ŀ��ֲ river
 * 2. 2012-11-28 ��ӷּ���ӡ����debug    river
 * 3. 2013-3-7   ���Ӵ���1 shell
 */

#ifndef DEBUG_INFO_H
#define DEBUG_INFO_H
	 
#ifdef __cplusplus
	 extern "C" {
#endif

#include "includes.h"
#include "app_msg.h"

//��ӡ�ȼ�
enum {
	Debug_None = 0,
	Debug_Error,
	Debug_Business,
	Debug_Warning,
	Debug_Notify,
	Debug_Constant			// �����Ĵ�ӡ���Ӳ�ֹͣ
};	

//���ڽ���shell
typedef int (*DebugFun)(char *arg);
int Register_Debug_Cmd(char *name, DebugFun fun, char *help, BOOL bParam);

//���Դ������, ע��ú������ɶ���̵߳���
int Usart1_WriteData(char *pbuf, u32 len);
int Usart1_ReadData(char *pbuf, u32 len);
void Usart1_Clear_Buffer(void);
void Usart1_Set_OvertimeTick(uint32 tick);



//�������������Ϣ
uint8 Debug_Mem_Free(void *pblk);
int debug_get_info(char *buf);

//������Ϣ��ӡ�ȼ�����
void Set_Debug_Level(uint8 level);
int8 Get_Debug_Level(void);

//���������Ϣ���
void debug(int8 level, const char *fnt, ...);

//���Դ��ڳ�ʼ��
void Print_Debug_Init(void);
void Debug_Init(void);

#ifdef __cplusplus
}
#endif

#endif
