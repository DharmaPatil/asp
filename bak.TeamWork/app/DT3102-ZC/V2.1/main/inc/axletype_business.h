#ifndef AXLETYPE_BUSINESS_H
#define AXLETYPE_BUSINESS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "dt3102_io.h"

typedef struct _TyreDevice 
{
	int32  direct;				
	int32  nCntInterval;		//��¼����һ����̥�ļ��ʱ��
	int32  nCntPulse;			//����ʱ�䣬��¼��ѹ����̥ʶ�����ϵ�ʱ��
	char   bDouble;				//��˫��
}TyreDevice;


void AxleType_Business(sAxisType1_State_TypeDef *status);
void Car_Business_WBAxle(void);


//��̥ʶ���������ź�
char Get_TyreDevice_Event(void);
char Get_TyreDevice_bDouble(void);



//��̥����ѭ������
int TyreQueue_Get(TyreDevice *data);
void TyreQueue_Put(TyreDevice *data);
u32 TyreQueue_len(void);
int TyreQueue_Get_Rear(TyreDevice *data);
void TyreQueue_Init(void);


#ifdef __cplusplus
}
#endif

#endif

