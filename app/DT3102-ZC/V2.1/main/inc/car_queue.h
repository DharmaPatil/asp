/* 
 * File:    car_queue.h
 * Brief:   carinfo FSM queue
 *
 * History:
 * 1. 2012-11-16 ���������� ����п�������Ŀ��ֲ	 river
 */

#ifndef CAR_QUEUE_H
#define CAR_QUEUE_H

#include "includes.h"
#include "app_msg.h"

#define MAX_AXLE_NUM    20

struct  _CarInfo;
typedef struct  _CarInfo  CarInfo;
typedef void (*CarBusinessFun)(CarInfo *pCar, TaskMsg *pMsg);

struct _AxleInfo 
{
	int32	axle_wet;
	int32   axle_len;
	char 	axle_type;
};

struct  _CarInfo
{
	struct  _CarInfo* pPrev;
	struct  _CarInfo* pNext;
	struct  _CarInfo* pHeadWhenComing;
	CarBusinessFun CarFSMFun;
	
	BOOL	bUse;	  
	BOOL	bResponse;					//�Ƿ��ѷ���
	BOOL	bCarIsDead;			
	BOOL	bStateChanged;				//����״̬�ı��־
	BOOL	bAxleValid;				//����Ч��־
	BOOL	bWetVaild;					//�����Ƿ���Ч
	BOOL	bWetTimeFlag;				//ȡ������ʱ��־

	uint8	cmd;						//�����
	uint8	CutOffState;				//������β״̬
	uint8   nStatus;					//����״̬
	uint8	nPreStatus;				//��һ��״̬
	int32   nStatusKeepTime;			//��״̬���м�ʱ
	int32	nLiveTime;					//������ϵͳ�еĴ��ʱ��
	int32   nHeadStatusWhenComing;		//��Ļ����ʱͷ��״̬
	int32   nHeadWetWhenComing;			//��Ļ����ʱͷ��������
	int32	nHeadAlexsWhenComing;      	//��Ļ����ʱͷ���Ѿ��³ӵ�����
	int32   nScalerKgComing;			//��Ļ�����ĳ�̨����
	int32	nScalerKgDown;				//��Ļ�ͷ�ʱ�ĳ�̨����
	
	uint8   WetLevel;				    //ȡ�������Ŷ�
	uint8   WetFlag;                  	//ȡ��������
	int32   nWetKeepTime;				//������ʱ��ʱ;

	int8    nAxleNum; 					//��������
	int8    nBackAxleNum;				//�����³�����
	int32   nBestKg;					//��������
	int32 	nLongCarHalfWet;			//�����Զ�����ǰ�������
	int8	nLongCarHalfAxle;			//����ǰ�������

	//������ 
	BOOL   bLongCarReady;				//�����ֶγ�����ɱ�־
	uint8  nLongCarSegs;				//�����ֶ���

	uint16	 year;						//ʱ��
	uint8	 mon;
	uint8	 day;
	uint8	 hour;
	uint8	 min;
	uint8	 sec;
	uint16  speed; 
	struct _AxleInfo AxleInfo[MAX_AXLE_NUM];		//����
}; 

//��������״̬
enum {
	NoneWetFlag = 0,
	SingleWetFlag,
	SingleUnstableFlag,
	FollowWetFlag
};
	
/* ���г�ʼ�� */
void CarQueue_Init(void);

/* ��������, ���س�����Ϣָ�� */
CarInfo* CarQueue_Create_Car(void);

/* �Ӷ�β���һ���� */
void CarQueue_Add_Tail(CarInfo* pCar);

/* �Ӷ�ͷ���һ���� */
void CarQueue_Add_Head(CarInfo* pCar);

/* �Ƴ���ͷ����, ���ض�ͷ����ָ�� */
CarInfo*  CarQueue_Remove_Head(void);	

/* �Ƴ���β����, ���ض�β����ָ�� */
CarInfo*  CarQueue_Remove_Tail(void);	

/* �ж��Ƿ���β�� */
BOOL CarQueue_Car_Is_Tail(CarInfo* pCar);

/* �ж��Ƿ���ͷ�� */
BOOL CarQueue_Car_Is_Head(CarInfo* pCar);

/* �ж϶����Ƿ�Ϊ�ա�*/
BOOL CarQueue_Is_Empty(void);				

/* ��ȡ��ͷ����ָ�� */
CarInfo* CarQueue_Get_Head(void);

/* ��ȡ��β����ָ�� */
CarInfo* CarQueue_Get_Tail(void);

/* �жϳ���״̬�Ƿ�ı� */
BOOL CarQueue_CarState_Is_Change(CarInfo* pCar);

int32 CarQueue_Get_CanPayCount(void);


CarInfo *CarQueue_Get_OnScaler_FirstCar(void);


/* ���ó���ɾ����־bIsDead */
void CarQueue_Car_Kill(CarInfo* pCar);

//�жϳ��Ƿ���ɾ��
BOOL CarQueue_Car_Is_Dead(CarInfo* pCar);


/* ɾ��������������ɾ����־�ĳ��� */
void CarQueue_Remove_Dead(void);

/* ���ض��г������� */
int32 CarQueue_Get_Count(void);

/* �����ڳ��ϵĳ��� */
int32 CarQueue_Get_OnScaler_Count(void);


#endif

