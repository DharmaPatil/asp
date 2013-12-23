#ifndef __DATA_STORE_MODULE_H__
#define __DATA_STORE_MODULE_H__

#include "includes.h"

#define MAX_NORMAL_VEHICLE_NUMBER					4096					//�洢�������������ֵ
#define MAX_OVERWEIGHT_VEHICLE_NUMBER				4096					//�洢���س��������ֵ

//DataFlash��ʼ������
void init_flash(void);

//��ʷ����������
uint16 Get_Flash_OverWetCarNum(void);
uint16 Get_Flash_NormalCarNum(void);
uint16 Get_Flash_NormalCarNumFront(void);
uint16 Get_Flash_NormalCarNumRear(void);

void Erase_Flash_Chip(void);


//�洢������������
uint8 Store_Normal_Vehicle(const void *data, uint8 len);

//�洢���س�������
uint8 Store_Overweight_Vehicle(const void *data, uint8 len);

//��ѯ������������
void *Query_Normal_Vehicle(uint16 id, uint8 len);

//��ѯ���س�������
void *Query_Overweight_Vehicle(uint16 id, uint8 len);


#endif //__DATA_STORE_MODULE_H__

