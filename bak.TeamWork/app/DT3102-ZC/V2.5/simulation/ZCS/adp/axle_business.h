/* 
 * File:    axle_business.h
 * Brief:   car's axle business
 *
 * History:
 * 1. 2012-11-27 �����ļ�, ����п�������Ŀ��ֲ	 river
 */

#ifndef AXLE_BUSINESS_H
#define AXLE_BUSINESS_H

#include "includes.h"
#include "car_queue.h"

typedef struct
{
	int32  direct;
	int32  AlexKg;	
	int32  AlexKgRef;			//���ط�ֵ
	int32  AlexRightTopWet;
	int32  AlexRightWet;
	int32  nTimeInterval;		//��¼����һ����ļ��ʱ��
	int32  nTimePulse;			//����ʱ�䣬��¼��ѹ����ʶ�����ϵ�ʱ��
	BOOL   bDouble;				//��˫��
}AlexRecoder;


int32 Apply_Car_Axle(CarInfo* pCar);					//��������
int32 Apply_LongCar_Axle(CarInfo *pCar);

void Save_Axle_Info(AlexRecoder* pAxle);
void SysTimer_Business(void);



//������¼
void Clear_Axle_Recoder(void);

int32 Get_Axle_Weight(uint8 axle_num);
void Save_Car_AxleWet(CarInfo *pCar);




#endif
