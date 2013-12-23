#include "includes.h"
#include "task_timer.h"
#include "task_msg.h"										//��Ϣ����
#include "app_msg.h"
#include "serial_protocol.h"								//����Э�����
#include "sdk_serial.h"
#include "overtime_queue.h"									//�����ͳ������ݶ��в���
#include "data_store_module.h"							//�������ݷ����洢ģ��
#include "sys_param.h"									//�洢�������������������
#include "debug_info.h"
#include "device_info.h"
#include "task_timer.h"

//��sdkͨ��Э����ˮ��,��Χ60000~65535; 1~59999�ɳ�����Ϣ�뵹����Ϣʹ��
static uint16 OtherSerialID = 60000;
static uint16 Get_SerialID(void)
{
	uint16 tmp;

	tmp = OtherSerialID;
	OtherSerialID++;

	//����
	if(OtherSerialID == 0) OtherSerialID = 60000;

	return tmp;
}

static void Data_Manager_Business(TaskMsg *msg)
{
	OvertimeData *pCar = NULL;
	uint16 type = msg->msg_id;

	switch(type)										//���ݲ�ͬ����Ϣ�Ž������̴���
	{
		case Msg_Device_Fault:							//���輴ʱ������Ϣ
		case Msg_Device_Heart:							//10s��ʱ�ϱ�����״̬, OverQueueΪ��ʱ�ϴ�
			Send_SDK_Ack(0, 0x01, Get_SerialID(), 0);
			break;
			
		case Msg_Overtime:								//��ʱ��Ϣ
			//1����ͷԪ���������
			pCar = OverQueue_Get_Head();				//��ȡ��ͷ��������
			if(pCar == NULL) break;
			
			Send_SDK_OverQueueData(pCar);
			//���ӷ��ʹ���
			if(pCar->SendNum < 255) pCar->SendNum++;
			break;
			
		case Msg_BackCarInfo:							//������Ϣ
		case Msg_CarInfo_Ready:							//������Ϣ��׼�����֪ͨ��	
			//==============================================
			//��ȡ��ͷ��������,���͵�sdk
			pCar = OverQueue_Get_Head();							
			if(pCar == NULL) 
			{
				debug(Debug_Business, "ERROR: OverQueue_Get_Head NULL!");
				break;
			}

			if(pCar->SendMod == SendActiveMod)
			{	
				debug(Debug_Business, "@@Send carinfo to SDK,cmd:%d,SendNum:%d,OverQueueLen:%d\r\n", 
					pCar->cmd, pCar->SendNum,OverQueue_Get_Size());

				//�����ͷ����δ����,���������ͳ�����Ϣ,�����ɳ�ʱ���Ʒ���
				if(pCar->bSend == 0)
				{
					Send_SDK_OverQueueData(pCar);

					//���ӷ��ʹ���
					if(pCar->SendNum < 255) pCar->SendNum++;

					//��λ��ʱ����
					Reset_OvertimeTick();
				}
			}
			//����Ϊ0���ô洢
			if(pCar->AxleNum > 0) 
			{				
				//==============================================
				//��ȡ��β��������, �洢��flash
				pCar = OverQueue_Get_Tail();	
				if(pCar == NULL) 
				{
					debug(Debug_Business, "ERROR: OverQueue_Get_Tail NULL!");
					break;
				}
				
				//�����β��������,�洢���޳�������
				if(pCar->ScalerOverWetFlag)		
				{
					if(pCar->bSave == 0)
						Store_Overweight_Vehicle(pCar, sizeof(OvertimeData));	
				}

				//�洢������Ϣ����ʷ������Ϣ����
				if(pCar->cmd != BackCarCmd)
				{
					if(pCar->bSave == 0)
						Store_Normal_Vehicle(pCar, sizeof(OvertimeData));	
				}
				
				//���ô洢��־
				if(pCar->bSave == 0) 
				{
					debug(Debug_Business, "@@Store CarInfo completed!\r\n");
					pCar->bSave = 1;		
				}
				debug(Debug_Business, "\r\n");
			}

			pCar = OverQueue_Get_Head();	
			if(pCar != NULL)
			{
				//�������͵ĳ�����������ɾ��
				if(pCar->bSendAttr) OverQueue_Remove_Head();
			}	
			break;
			
		case Msg_USARTSDKTest:
			Send_SDK_Ack(0, USARTSDKTest, 0, 0);
			break;
			
		default: 
			break;
	}
}

void DataManager_Thread(void *arg)
{
	uint8 err;
	TaskMsg msg = {0};											

	while(1) 
	{
		err = Task_QPend(&DataManagerMsgQ, &msg, sizeof(TaskMsg), 0);		
		if(err == SYS_ERR_NONE) 
		{
			Data_Manager_Business(&msg);
		}
	}
}

