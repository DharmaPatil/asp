/* 
 * File:    car_business.h
 * Brief:   car business
 *
 * History:
 * 1. 2012-11-24 �����ļ�, ����п�������Ŀ��ֲ	 river
 */

#ifndef CAR_BUS_H
#define CAR_BUS_H

#include "car_queue.h"
#include "app_msg.h"

//#define CAR_STCUTOFF_TIME	800			//������β״̬��ʱʱ��(ms)
#define CAR_TICK			10		    //����״̬��ʱ��λ10ms
#define USCAR_WET_TICK		1000
//#define SCAR_WET_TICK     2000


#define SCALER_MIN_KG		500

//CarStatus����
//ע������ԭ�򣺴ӳ����ӽ��뵽�뿪����
//���Ҫ����״̬��������ѭ��ԭ��
typedef enum
{
	stCarNone = 0,

	//����״̬��ʾ���ڳ���============================
	stCarComing,		//������ʼ�ϳ�״̬

	stCarEnter,			//��������

	stCarInScaler,		//������ȫ���ڳ�̨��

	stCarWaitPay,		//���շ�״̬,����������ظ�

	stLongCar,			//�����ڳ�̨�ϣ�ͷ���Ѿ�������ظ�

	stCarLeaving,		//�����뿪,�����˼Ʒѳɹ�������

	//����״̬��ʾ���ڳ���============================

	stCarFarPay,		//��ӽɷ�	

	stDead				//��ɾ��
}CarStatus;

typedef enum
{
	LevelNoneCar=0,

	//������ʱ1.2���ȡֵ
	LevelMultiCar = 100,

	//�����Զ�����ȡCutOff����
	LevelAutoLongCar,	

	//��������£��󳵴�����Ļ
	LevelSingleCarBy,	

	//����׼���³ӣ������˺�ظ�
	LevelSingleCarBeginDown,

	//�����ֶ�
	LevelLongCar,

	//������β��ʱ1.2s�ȶ�
	LevelSingleCarStable	
}CarKgLevel;

BOOL IsSingleCarComming(CarInfo *pCar);

void CarSetBestKg(CarInfo *pCar, int nKg, unsigned char nLevel);

void FSM_Change_CarState(CarInfo *pCar, int32 state);

void Car_Business_RepeatCmd(CarInfo *pCar, TaskMsg *msg);

void Send_Simulate_CarInfo(uint8 axle, int32 wet, int cmd);

//sdk�������������쳣���ݻ�Ӧ
void AbnormalAckInfo_To_SDK(uint8 cmd, uint8 CutOffState);


void LongCarReset(void);
BOOL IsLongCar(void);
void LongCarRecognize(int32 nWet);

#endif

