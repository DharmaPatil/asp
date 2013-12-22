#ifndef _INCLUDES_H_	
#define _INCLUDES_H_

#ifdef __cplusplus		   		//�����CPP����C���� //��ʼ����
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "stm32f4xx.h"			//STM32F4xx�̼���
#include "C2.h"	
#include "sys_cpu.h"				

#include "debug_info.h"
#include "driver.h"

//Ӧ�ò��Զ������ʱ����
static __inline RDelay(int32 cnt)
{
	while(cnt--);
}
	
#ifdef __cplusplus		   		//�����CPP����C���� //��������
}
#endif

#endif
