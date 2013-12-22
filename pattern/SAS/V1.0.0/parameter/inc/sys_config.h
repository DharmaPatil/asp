/* 
 * File:    sys_config.h
 * Brief:   system parameter configuration 
 *
 * History:
 * 1. 2012-11-24 �����ļ� river
 */

#ifndef SYSCONFIG_H	
#define SYSCONFIG_H

#ifdef __cplusplus		   	
extern "C" {
#endif
#include "sys_cpu.h"

//5��ϵͳģʽ
typedef enum {
	SCS_ZC_1 = 0,		
	SCS_ZC_2,
	SCS_ZC_3,
	SCS_ZC_4,
	SCS_ZC_5,
	SCS_ZC_6,
	SCS_ZC_7,
	SCS_CNT
}SysMod;

//��������ģʽ
typedef enum {
	AutoLongCarMode,
	HandleLongCarMode
}LongCarMod;

//����ѡ��
typedef enum {
	FollowAtOnce,		//��Ļ��β1.2��������ȡ��������
	FollowNoFarPay, 	//����û����ӽɷ�
	FollowFarPay		//��������ӽɷ�
}FollowCarMod;

void System_Config_Init(void);

/*
 * ϵͳģʽѡ��
 * ������model����ģʽ
 * ˵������5�ֹ���ģʽ��ѡ�����õ�ǰ�Ĺ���ģʽ 
 */
BOOL Set_System_Mode(SysMod mode);
SysMod Get_System_Mode(void);

LongCarMod Get_LongCar_Mode(void);
void Set_LongCar_Mode(LongCarMod mode);

BOOL Get_TrafficSignal_Flag(void);		//return ture: �к��̵� FALSE: û��
void Set_TrafficSignal_Flag(BOOL type);

BOOL Get_BGFlag(void);
void Set_BGFlag(BOOL type);

FollowCarMod Get_FollowCarMode(void);
void Set_FollowCarMode(char type);

BOOL Get_Beep_Flag(void);
void Set_Beep_Flag(BOOL type);

unsigned short Get_AppReset_Count(void);


#ifdef __cplusplus		   	
}
#endif

#endif
