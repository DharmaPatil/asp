/* 
 * File:    debug_info.h
 * Brief:   print debug info
 *
 * History:
 * 1. 2012-11-16 �����ļ�, ��������Ŀ��ֲ river
 * 2. 2012-11-28 ��ӷּ���ӡ����debug    river
 */

#ifndef DEBUG_INFO_H
#define DEBUG_INFO_H
	 
#ifdef __cplusplus
	 extern "C" {
#endif

#include "includes.h"
#include "app_msg.h"
#include "task_msg.h"

//��ӡ�ȼ�
enum {
	Debug_None = 0,
	Debug_Error,
	Debug_Business,
	Debug_Warning,
	Debug_Notify,
	Debug_Constant			// �����Ĵ�ӡ���Ӳ�ֹͣ
};	

void Set_Debug_Level(int8 level);
int8 Get_Debug_Level(void);


void debug(int8 level, const char *fnt, ...);

void MsgInfo_Print(TaskMsg *pMsg);
void Print_Debug_Init(void);
void Debug_Init(void);

const char *Get_Car_StatusName(int32 id);
const char *Get_Car_LevelName(uint8 id);
const char *Get_Car_TryeType(uint8 nCount);

#ifdef __cplusplus
}
#endif

#endif
