#ifndef __SPEED_CHECK_H__
#define __SPEED_CHECK_H__

#include "includes.h"

/* �ٶȼ��㺯�� */
void Check_CarSpeed (void);
/* �ٶȻ�ȡ���� */
uint16 Get_CarSpeed (void);

void Set_Speed_Length(uint16 len);
uint16 Get_Speed_Length(void);


void Set_CarSpeed(int32 speed);


#endif //__SPEED_CHECK_H__
