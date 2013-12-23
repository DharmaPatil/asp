/******  file description   ******
 *	created time:	2012-11-10 22:00
 *  author:			
 *	file type:		C/C++ source file
 *	path:			src/wj_test
 *	latest modified:2012-12-05
**********************************/
#include "includes.h"
#include "app_rtc.h"
#include "serial_protocol.h"		//���ļ���ͷ�ļ�
#include "sdk_serial.h"
#include "overtime_queue.h"			//���������ͳ������ݶ���
#include "task_msg.h"						//��Ϣ������غ���
#include "app_msg.h"
#include "data_store_module.h"	//�������ݷ����洢ģ��
#include "device_info.h"
#include "debug_info.h"
#include "stm32f4_crc.h"
#include "crc.h"
#include "usart6.h"
#include "dt3102_io.h"
#include "task_def.h"
#include "weight_param.h"
#include "car_queue.h"

#define PROTOCOL_HEAD		0xA3		//Э������֡ͷ
#define PROTOCOL_VERSION	0x20		//Э��汾��

#define MAX_SDATA_LENGTH	  (sizeof(OvertimeData) + 10)	//������ݳ���

uint16 g_bPayFlag = 0;

//Э�����ģ���õ��ľ�̬ȫ�ֱ���
static uint16 RepeatCmdID = 0;						//��ǰ���²ɼ�������ˮ��
static uint16 ForceCmdID = 0;						//��ǰǿ�Ʋɼ�������ˮ��
static uint16 LongCarCmdID = 0;					//��ǰ�ֶβɼ�������ˮ��


//����ķ������ݻ�����
static char write_buffer[MAX_SDATA_LENGTH];			//����������ص���Ҫͨ�����ڷ��͸�SDK��Э������

static TaskMsg msg;
//====================================================================================
//��sdkͨ�����ݷ��ʹ���

//����sdk��Ӧ
//flag 1:���ͳ���14,��Ҫ��Ӧret 0:���ͳ���13,����Ҫ��Ӧret
void Send_SDK_Ack(uint8 flag, uint8 cmd, uint16 id, uint8 ret)
{
	int status = 0; 
	int16 tmp16 = 0;
	char buf[14] = {0};		//����̵߳���ʹ��
	char len = 0;
	
	buf[0] = PROTOCOL_HEAD;		//֡ͷ
	buf[3] = PROTOCOL_VERSION;	//�汾��
	
	buf[4] = id >> 8;			//��ˮ��
	buf[5] = id;

	buf[6] = cmd;				//�����

	len = 7;
	if(flag == 1)
	{
		buf[len++] = ret;
	}
	status = Get_Device_Status();	 //�豸״̬
	buf[len++] = status >> 24;
	buf[len++] = status >> 16;
	buf[len++] = status >> 8;
	buf[len++] = status;

	tmp16 = len+2;				//���ݰ�����										
	buf[1] = tmp16;
	tmp16 = ~tmp16 + 1;			//���Ȳ���
	buf[2] = tmp16;

	tmp16 = Reentrent_CRC16((u8 *)buf, len);	//CRC
	buf[len++] = tmp16 >> 8;
	buf[len++] = tmp16;

	Usart6_WriteData(buf, len);
}


extern int32 Get_Static_Weight(void);
//����sdk��Ӧ
//���ͳ���14
void send_SDK_ack_static_wet(uint8 cmd, uint16 id)
{
	CarInfo *pCar = NULL;
	int static_weight = 0; 
	int16 tmp16 = 0;
	char buf[14] = {0};		//����̵߳���ʹ��
	char len = 0;
	
	buf[0] = PROTOCOL_HEAD;		//֡ͷ
	buf[3] = PROTOCOL_VERSION;	//�汾��
	
	buf[4] = id >> 8;			//��ˮ��
	buf[5] = id;

	buf[6] = cmd;				//�����

	len = 7;
// 	static_weight = Get_Static_Weight();	 //��ȡ����
// 	static_weight = WetDrvProc(static_weight, gWetPar.StaticDivValue);//�ֶ�ֵ����
	static_weight = Get_Scaler_DrvWeight();	//���ֶ�ֵ����������
	buf[len++] = static_weight >> 24;
	buf[len++] = static_weight >> 16;
	buf[len++] = static_weight >> 8;
	buf[len++] = static_weight;
	
	//pCar = OverQueue_Get_Tail();
	pCar = CarQueue_Get_Tail();
	if(NULL == pCar)
	{
		buf[len++] = 0;	//����
	}
	else
	{
		buf[len++] = pCar->nAxleNum;//����
	}

	tmp16 = len+2;				//���ݰ�����										
	buf[1] = tmp16;
	tmp16 = ~tmp16 + 1;			//���Ȳ���
	buf[2] = tmp16;

	tmp16 = Reentrent_CRC16((u8 *)buf, len);	//CRC
	buf[len++] = tmp16 >> 8;
	buf[len++] = tmp16;

	Usart6_WriteData(buf, len);
}


//���ʹ��������ݶ���Ԫ��
void Send_SDK_OverQueueData(OvertimeData *pCar)
{
	int status = 0, i = 0; 
	int16 tmp16 = 0;
	u8 tmp8 = 0, slen = 0;

	if(pCar == NULL) return;

	//������Ӧ
	if(pCar->cmd == BackCarCmd) 
	{
		Send_SDK_Ack(0, BackCarCmd, pCar->SerialID, 0);
		//���ѷ��ͱ�־
		pCar->bSend = 1;
		return;
	}

	//�޸�sdk���������������Ϣ��ˮ��
	if((pCar->cmd != NormalCarCmd) && (pCar->bSend == 0))
	{
		//�����ݱ�����Ӧ��־λ
		pCar->bSendAttr = 1;
		
 		if(pCar->cmd == RepeatCarCmd) pCar->SerialID = RepeatCmdID;
		else if(pCar->cmd == ForceCarCmd) pCar->SerialID = ForceCmdID;
		else if(pCar->cmd == LongCarCmd) pCar->SerialID = LongCarCmdID;
	}
	
	//���ѷ��ͱ�־
	pCar->bSend = 1;
	
	//�������ֶ����쳣���ݲ���Ҫ��������
	if((pCar->cmd != LongCarCmd) && (pCar->AxleNum > 0))
	{
		if(pCar->AxleNum < 2) pCar->AxleNum = 2;
		if(pCar->AxleNum > gWetPar.nMaxAxleNum) pCar->AxleNum = gWetPar.nMaxAxleNum;
	}

	write_buffer[0] = PROTOCOL_HEAD;		//֡ͷ
	write_buffer[3] = PROTOCOL_VERSION;	//�汾��
	
	tmp16 = pCar->SerialID;					//��ˮ��
	write_buffer[4] = tmp16 >> 8;
	write_buffer[5] = tmp16;

	write_buffer[6] = pCar->cmd;			//�����

	tmp16 = pCar->year;						//ʱ��
	write_buffer[7] = tmp16 >> 8;			
	write_buffer[8] = tmp16;				
	write_buffer[9] = pCar->mon;
	write_buffer[10] = pCar->day;
	write_buffer[11] = pCar->hour;
	write_buffer[12] = pCar->min;
	write_buffer[13] = pCar->sec;
	
	write_buffer[14] = pCar->CarStatus;	//����״̬
	
	status = Get_Device_Status();			//�豸״̬
	write_buffer[15] = status >> 24;
	write_buffer[16] = status >> 16;
	write_buffer[17] = status >> 8;
	write_buffer[18] = status;
	
	tmp16 = pCar->speed;
	write_buffer[19] = tmp16 >> 8;			//�ٶ�
	write_buffer[20] = tmp16;				
	write_buffer[21] = pCar->speeda;		//���ٶ�
	
	write_buffer[22] = pCar->AxleNum;			//����
	write_buffer[23] = pCar->AxleGroupNum;	//������

	slen = 24;
	if(pCar->AxleNum > 0)
	{
		for(i = 0; i < pCar->AxleNum; i++)		//����
		{
			write_buffer[slen]   = pCar->AxleWet[i] >> 8;
			write_buffer[slen+1] = pCar->AxleWet[i];
			slen += 2;
		}

		for(i = 0; i < pCar->AxleGroupNum; i++)		//��������
		{
			write_buffer[slen] = pCar->AxleGroupType[i];
			slen++;
		}

		for(i = 0; i < pCar->AxleNum - 1; i++) //����
		{
			tmp16 = pCar->AxleLen[i];
			write_buffer[slen]   = tmp16 >> 8;
			write_buffer[slen+1] = tmp16;
			slen += 2;
		}
	}
	
	tmp8 = slen + 2;						//���ݰ�����										
	write_buffer[1] = tmp8;
	tmp8 = ~tmp8 + 1;						//���Ȳ���
	write_buffer[2] = tmp8;

	tmp16 = Reentrent_CRC16((u8 *)write_buffer, slen);	//CRC
	write_buffer[slen] = tmp16 >> 8;
	write_buffer[slen+1] = tmp16;
	slen += 2;

	Usart6_WriteData(write_buffer, slen);
}


//���ʱ���Ƿ���Ч
static BOOL Check_Date_Valid(CSysTime *time)
{
	if(time)
	{
		if(time->year < 1990 || time->year > 2100) return FALSE;
		if(time->mon < 1 || time->mon > 12) return FALSE;
		if(time->day > 31) return FALSE;
		if(time->hour > 23) return FALSE;
		if(time->min > 59) return FALSE;
		if(time->sec > 59) return FALSE;
		
		return TRUE;
	}
	return FALSE;
}

extern char m_USART6_test_flag;
extern void Clear_Axle_Recoder(void);
//����һ��ģ��ĳ�����Ϣ
extern void Send_Simulate_CarInfo(uint8 axle, int32 wet, int cmd);
/*******************************************************
* Э�����ݽ������
* �������ƣ�SDK_DataPacket_Parse
* �������ܣ������ڽ��յ���SDKЭ�����ݽ�����õ�Ӧ�ò�����
* ������    1��uint8 *data ���յ��Ĵ�������
*           2��uint8 length  ���յ������ݳ���
* ����ֵ��  void
*******************************************************/
void SDK_DataPacket_Parse(uint8 *data, uint8 length)
{
	CSysTime proTime = {0};
	uint8 cmd = 0;													//����õ��������
	uint16 id;														//����õ�����ˮ��
	char isSuccess = 0;										//���������Ƿ�ɹ���־
	OvertimeData *pCar = NULL;
	DeviceStatus device = {0};
	static uint16 pay_id = 0;

	id = data[1];						//��ˮ��
	id <<= 8;
	id += data[2];
	cmd = data[3];						//�����

	memset(&msg,0,sizeof(msg));		//�����Ϣ

	switch(cmd)
	{
		case IORequstCmd:				//IO״̬����
			Send_SDK_Ack(0, cmd, id, 0);
			break;
			
		case NormalCarCmd:				//�����������ݻ�Ӧ
		case BackCarCmd:				//������Ϣ��Ӧ
			pCar = OverQueue_Get_Head();									
			if(pCar != NULL) 
			{
				if((pCar->SerialID == id) &&
					(pCar->cmd == cmd) &&
					(pCar->bSend == 1))
				{
					OverQueue_Remove_Head();				
					debug(Debug_Business,"//Recv SDK car ack, SerialID=%d!\r\n", id);
				}
			}
			break;
			
		case RepeatCarCmd:				//���²ɼ�����
			//������ˮ��
			RepeatCmdID = id;
			
			//��ҵ�����߳�Ͷ�����²ɼ�������Ϣ
			msg.msg_id = Msg_Cmd_Repeat;				
			Task_QPost(&BusinessMsgQ, &msg);
			break;
			
		case ForceCarCmd:				//ǿ�Ʋɼ�����
			//���µ�ǰǿ�Ʋɼ�������ˮ��
			ForceCmdID = id;
			
			//��ҵ�����߳�Ͷ�ݹ�Ļ�ͷ���Ϣ
			msg.msg_id = Msg_Cmd_Force;					
			Task_QPost(&BusinessMsgQ, &msg);
			break;
			
		case LongCarCmd:				//�ֶβɼ�����
			//���µ�ǰ�ֶβɼ�������ˮ��stepSampleID
			LongCarCmdID = id;
			
			//��ҵ�����߳�Ͷ�ݷֶβɼ���Ϣ
			msg.msg_id = Msg_Cmd_PlusSave;		//�ֶβɼ���Ϣ
			msg.msg_ctx = data[4];				//�ֶ���
			Task_QPost(&BusinessMsgQ, &msg);
			break;
			
		case PayNoticeCmd:				//�׳��ɷ����֪ͨ
			if(pay_id != id) 
			{
				//��ˮ�Ų��ظ�, ��ҵ�����߳�Ͷ���׳��ɷ����֪ͨ
				msg.msg_id = Msg_Cmd_PayNotify;				
				Task_QPost(&BusinessMsgQ, &msg);
				pay_id = id;
				g_bPayFlag = 1;
			}
			else
			{
				debug(Debug_Warning, "//�ɷ�֪ͨ��ˮ���ظ�!\r\n");
			}

			//������Ϣ�ɷ�֪ͨ���Ͳ����ø�
			device.bitValue.carinfo_send_flag = 1;
			Update_CarInfo_SendFlag(device);

			//���ͻ�Ӧ
			Send_SDK_Ack(0, cmd, id, 0);
			
			//������Ϣ�ɷ�֪ͨ���Ͳ����õ�
			device.bitValue.carinfo_send_flag = 0;
			Update_CarInfo_SendFlag(device);
			break;
			
		case IOContrlCmd:				//�����������
			//1���������裬�õ����ƽ��isSuccess
			switch(data[4])
			{
				case 1:			//���ƺ��̵�
					if((data[5] & 0x04) && (data[5] & 0x08))	//���̵��Զ�����
					{
							;		//��ʱû��ʵ��
					}
					else
					{
						isSuccess = fPeripheral_Control(TrafficSignal,data[5]);
					}
					break;
					
				case 2:			//���Ƶ�բ
					if((data[5] & 0x10) && (data[5] & 0x20))	//ǰ��բ�Զ�����
					{
						;		//��ʱû��ʵ��
					}
					else
					{
						isSuccess = fPeripheral_Control(BarrierGate,data[5] >> 1);
					}
					break;
					
				case 3:								
					break;
					
				default: break;
			}
			//���ݿ��ƽ�������
			Send_SDK_Ack(1, cmd, id, isSuccess);
			break;

		case TimeSetCmd:				//SDKʱ��ͬ������
			proTime.year = (data[4] << 8) + data[5];
			proTime.mon = data[6];
			proTime.day = data[7];
			proTime.hour = data[8];
			proTime.min = data[9];
			proTime.sec = data[10];
			if(Check_Date_Valid(&proTime))		
			{
				Set_System_Time(&proTime);
			}

			Send_SDK_Ack(0, cmd, id, 0);
			break;		
			
		case USARTSDKTest:		//�յ�SDK���ڲ�������
			m_USART6_test_flag = 1;
			break;

		case ACKStaticWetAndAxle:		//�ɽ��Ŀ-�ظ���ǰ��̬����������
			send_SDK_ack_static_wet(cmd,id);
			break;
		case ClearLastCarAxle:			//�ɽ��Ŀ-������һ����������
		{
			CarInfo *pCar2 = NULL;
			#if SYS_CRITICAL_METHOD == 3  	 
			SYS_CPU_SR	cpu_sr = 0;
			#endif	
			SYS_ENTER_CRITICAL();		
			Clear_Axle_Recoder();	//������¼
			pCar2 = CarQueue_Get_Tail();
		 	pCar2->nAxleNum = 0;
			SYS_EXIT_CRITICAL();	
			Send_SDK_Ack(0, cmd, id, 0);//����Ӧ��
			//Send_Simulate_CarInfo(pCar->AxleNum, Get_Scaler_DrvWeight(), 0x02);	//���GUI����
		}
			break;
		default:
			break;											
	}
}

#define COMHEADLEN 9

void SDK_Protocol_Parse(char *buf, int *blen)
{
	char dlen = 0, inv_dlen = 0;
	int i, len = *blen;

	if(buf == NULL) return;
	if(len < COMHEADLEN) return;
	
	//�ְ�
	while(len >= COMHEADLEN) 
	{
		//���ұ���ͷ
		for(i = 0; i < len; i++)
		{
			if(buf[i] == PROTOCOL_HEAD)
				break;
		}
		//��������ͷ֮ǰ������
		if(i > 0)
		{
			len -= i;
			memmove(buf, &buf[i], len);
		}
		if(len < COMHEADLEN) break;
	
		//����У��
		dlen = buf[1];
		inv_dlen = buf[2];
		inv_dlen = ~inv_dlen + 1;
		if(dlen != inv_dlen)
		{
			len -= 1;		//����֡ͷ
			memmove(buf, &buf[1], len);
			continue;
		}
		//���ݳ��Ȳ���,��������
		if(len < dlen) break;
	
		//���ݰ���Ч�Լ��
		if(((dlen < COMHEADLEN) || (dlen > 64)) ||	//���ݳ����ж�
			(buf[3] != PROTOCOL_VERSION) ||			//Э��汾�ż��
			(Reentrent_CRC16((unsigned char*)buf, dlen) != 0))			//crc����
		{
			len -= 7;		//���������֮ǰ������
			memmove(buf, &buf[7], len);
			continue;
		}
		
		//�������
		debug(Debug_Warning, "//Uart6 received sdk cmd:%d\r\n", buf[6]);
		SDK_DataPacket_Parse((unsigned char*)&buf[3], dlen - 3);
	
		//�����ѽ���������
		len -= dlen;
		memmove(buf, &buf[dlen], len);
	}//end while(len >= COMHEADLEN)	

	*blen = len;
}

