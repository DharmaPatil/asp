/*
 * 2013-3-16 �洢����ʱ���ٿ�����������, �����Ƿ���Ч�ɴ洢ģ���ж���Ϣ��У���ʵ��; 
 *		   У��͵Ĵ洢��ַ����ڴ洢��Ϣ������ֽ�, �洢�ռ��ɴ���Ĵ洢��Ϣ�ṩ
 */

#include "data_store_module.h"
#include "mx251608d.h"
#include "task_def.h"							//priorty
#include "sys_param.h"

#define START_ADDR_OF_NORMAL_VEHICLE					0x100000				//�洢�����������ݵ���ʼ��ַ
#define END_ADDR_OF_NORMAL_VEHICLE					0x1FFFFF				//�洢�����������ݵĽ�����ַ
#define START_ADDR_OF_OVERWEIGHT_VEHICLE			0x0						//�洢���س������ݵ���ʼ��ַ
#define END_ADDR_OF_OVERWEIGHT_VEHICLE				0xFFFFF					//�洢���س������ݵĽ�����ַ

static HVL_ERR_CODE err = HVL_NO_ERR;		//����״̬
static HANDLE MX251608D_Dev = 0;			//�豸���

static uint16 gOverWeight_Num = 0;		//��ǰ�洢�ĳ��س�����

//����У���
static uint16 Calc_Check_Sum(uint8 *data, uint8 len)
{
	int i = 0;
	uint16 tmp = 0;

	for(; i < len; i++)
	{
		tmp += data[i];
	}

	return tmp;
}

//���ݵ�ַ�洢���ݵ�flash
static BOOL Store_CarInfo_Flash(u32 addr, const void *data, uint8 len)
{
	int rlen = 0;	//ʵ�ʶ�дFlash�ĳ���
	uint16 verify = 0;
	u8 *pdata = (u8 *)data;

	verify = Calc_Check_Sum(pdata, len - 2);
	memcpy(&pdata[len-2], &verify, 2);
	
	rlen = BlockDeviceWrite(MX251608D_Dev, addr, pdata, len);
	if(rlen < 0)		//д������
	{
		debug(Debug_Error,"дFlash��������,addr:0x%06x\r\n",addr);
		return FALSE;																																//�洢ʧ�ܷ���																									
	}	
	
	if(rlen != len)																						//д������ݸ�����ϣ��д������ݸ�����ƥ��
	{
		debug(Debug_Notify,"дFlash���ݲ�ƥ�����\r\n");
		return FALSE;																																//�洢ʧ�ܷ���
	}

	return TRUE;
}

//����flash
static BOOL Erase_CarInfo_Flash(u32 erase_addr)
{
	DeviceIoctl(MX251608D_Dev,MX251608D_CMDSECTORERASE,erase_addr);

	//��������60ms
	while(DeviceIoctl(MX251608D_Dev,MX251608D_CMDISBUSY,0))		//æ�ȴ�
	{
		SysTimeDly(2);																	
	}
	
	return TRUE;
}

static BOOL Check_Data_Valid(uint8 *buf, uint8 len)
{
	uint16 verify = 0, tverify = 0xff;

	verify = Calc_Check_Sum(buf, len - 2);
	memcpy(&tverify, &buf[len-2], 2);

	if(tverify != verify) return FALSE;

	return TRUE;
}

/*******   FLASH�豸��ʼ������   ********/
void init_flash(void)
{
	if(MX251608D_Init(FlashMutexPrio) == 0)								//�豸A��ʼ��				���������ȼ�9
	{
		debug(Debug_None,"��ʼ��Flashʧ��\r\n");
		SysTaskSuspend(SYS_PRIO_SELF);
	}
	else
	{
		debug(Debug_Notify,"��ʼ��flash�ɹ�!\r\n");
	}
	
	MX251608D_Dev = DeviceOpen(BLOCK_DATAFLASH,0,&err);								//�豸��
	if(err != HVL_NO_ERR)
	{
		debug(Debug_Notify,"Flash�豸��ʧ��\r\n");
	}
	else
	{
		debug(Debug_Notify,"flash�豸�򿪳ɹ�!\r\n");
	}
}

void Erase_Flash_Chip(void)
{
	DeviceIoctl(MX251608D_Dev,MX251608D_CMDERASE,0);

	//����оƬ14s
	while(DeviceIoctl(MX251608D_Dev,MX251608D_CMDISBUSY,0)) 	//æ�ȴ�
	{
		SysTimeDly(100);																	
	}
}

uint16 Get_Flash_NormalCarNum(void)
{
	uint16 tmp = 0;
	
	Param_Read(GET_U16_OFFSET(ParamNormalCarNum), &tmp, sizeof(uint16));
	if(tmp > MAX_NORMAL_VEHICLE_NUMBER) tmp = 0;
	
	return tmp;		
}

uint16 Get_Flash_OverWetCarNum(void)
{
	uint16 tmp = 0;
	
	Param_Read(GET_U16_OFFSET(ParamOverWetCarNum), &tmp, sizeof(uint16));
	if(tmp > MAX_OVERWEIGHT_VEHICLE_NUMBER) tmp = 0;
	
	return tmp;		
}

uint16 Get_Flash_NormalCarNumFront(void)
{
	uint16 tmp = 0;
	
	Param_Read(GET_U16_OFFSET(ParamCarNumFront), &tmp, sizeof(uint16));
	if(tmp > MAX_NORMAL_VEHICLE_NUMBER) tmp = 0;

	return tmp;
}

uint16 Get_Flash_NormalCarNumRear(void)
{
	uint16 tmp = 0;
	
	Param_Read(GET_U16_OFFSET(ParamCarNumRear), &tmp, sizeof(uint16));
	if(tmp > MAX_NORMAL_VEHICLE_NUMBER) tmp = 0;

	return tmp;
}

//�洢������������
//������ʷ�������ɳ�������дָ�롢��ͷ����λ��ά��
uint8 Store_Normal_Vehicle(const void *data, uint8 len)
{
#if 1
	int j = 0,i = 0;						//������ʱ���Ʊ���
	uint32 addr = 0;						//����Flashʱ�ṩ�ĵ�ַ
	BOOL bErase = FALSE;					//�߼��������Ƿ���Ҫ����dataFlash����ʼ��Ϊfalse
	uint16 rear = 0, carnum = 0;
	uint16 front = 0;

	if((data == NULL) || (len < 2)) return FALSE;

	//��ȡ��βָ��
	rear = Get_Flash_NormalCarNumRear();
		
	/*******�жϴ洢ʱ�Ƿ���Ҫ����flash********/
	/*******�����Ҫ�������õ�������ַ*********/
	/*******���ݲ�����ַ������4KBflash*********/
	i = rear/15;			//�����Ŀ���
	
	j = rear%15;			//�Ƿ�Ӧ�ò���
	if(j == 0) bErase = TRUE;
	
	if(bErase)
	{
		addr = i*4096 + START_ADDR_OF_NORMAL_VEHICLE;			//��4KB����ʱ�ṩ����ʼ��ַ
		if(!Erase_CarInfo_Flash(addr)) 
			return FALSE;
	}
	
	//��Flashд�복�����ݣ�ʵ�������������ݴ洢����
	//������д��ַ
	addr = rear*256 + START_ADDR_OF_NORMAL_VEHICLE;
	if(Store_CarInfo_Flash(addr, data, len))	
	{
		//����дָ��
		rear++;
		if(rear >= MAX_NORMAL_VEHICLE_NUMBER) rear = 0;
		Param_Write(GET_U16_OFFSET(ParamCarNumRear), &rear, sizeof(uint16));

		//��������, ���ӳ�����
		carnum = Get_Flash_NormalCarNum();
		carnum++;
		if(carnum > MAX_NORMAL_VEHICLE_NUMBER)
		{
			//���������󶪵�һ�������ĳ���
			carnum = MAX_NORMAL_VEHICLE_NUMBER - 16 + 1;
			
			//����,����ͷָ��
			front = Get_Flash_NormalCarNumFront();
			front += 16;
			if(front >= MAX_NORMAL_VEHICLE_NUMBER) front = 0;
			Param_Write(GET_U16_OFFSET(ParamCarNumFront), &front, sizeof(uint16));
		}
		//д������
		Param_Write(GET_U16_OFFSET(ParamNormalCarNum), &carnum, sizeof(uint16));
		
		debug(Debug_Notify,"��Flashд��%d����������,addr:0x%06x\r\n",carnum,addr);		

		return TRUE;																																//�洢�ɹ�����
	}

	return FALSE;
#else
	return TRUE;
#endif
}


//�洢���س�������
uint8 Store_Overweight_Vehicle(const void *data, uint8 len)
{
#if 1
	int j = 0,i = 0;						//������ʱ���Ʊ���
	uint32 addr = 0;						//����Flashʱ�ṩ�ĵ�ַ
	BOOL bErase = FALSE;					//�߼��������Ƿ���Ҫ����dataFlash����ʼ��Ϊfalse

	if((data == NULL) || (len < 2)) return FALSE;

	Param_Read(GET_U16_OFFSET(ParamOverWetCarNum), &gOverWeight_Num, sizeof(uint16));

	//���޳����������ڻ���
	if(gOverWeight_Num >= MAX_OVERWEIGHT_VEHICLE_NUMBER)	return FALSE;
		
	/*******�жϴ洢ʱ�Ƿ���Ҫ����flash********/
	/*******�����Ҫ�������õ�������ַ*********/
	/*******���ݲ�����ַ������4KBflash*********/
	i = gOverWeight_Num/15;			//�����Ŀ���
	
	j = gOverWeight_Num%15;			//�Ƿ�Ӧ�ò���
	if(j == 0) bErase = TRUE;
	
	if(bErase)
	{
		addr = i*4096 + START_ADDR_OF_OVERWEIGHT_VEHICLE;	//��4KB����ʱ�ṩ����ʼ��ַ
		if(!Erase_CarInfo_Flash(addr)) 
			return FALSE;
	}
	
	//��Flashд�복�����ݣ�ʵ�������������ݴ洢����
	//������д��ַ
	addr = gOverWeight_Num*256 + START_ADDR_OF_OVERWEIGHT_VEHICLE;
	if(Store_CarInfo_Flash(addr, data, len))	
	{	
		//д�복����
		gOverWeight_Num++;
		if(gOverWeight_Num > MAX_OVERWEIGHT_VEHICLE_NUMBER) gOverWeight_Num = MAX_OVERWEIGHT_VEHICLE_NUMBER;
		Param_Write(GET_U16_OFFSET(ParamOverWetCarNum), &gOverWeight_Num, sizeof(uint16));

		debug(Debug_Notify,"��Flashд��%d�����س�������,addr:0x%06x\r\n",gOverWeight_Num,addr);
		return TRUE;	
	}

	return FALSE;
#else
	return TRUE;
#endif
}

static uint8 TmpCarinfoBuf[256] = {0};			//�洢ȡ�����ĳ�����Ϣ

//��ѯ������������
void *Query_Normal_Vehicle(uint16 id, uint8 len)
{
	uint32 rlen = 0;
	uint32 readAddr = 0;				//��DataFlashʱ�ṩ�ĵ�ַ

	if((id < 1) || (id > MAX_NORMAL_VEHICLE_NUMBER)) return NULL;
	if(len < 2) return NULL;

	while(DeviceIoctl(MX251608D_Dev,MX251608D_CMDISBUSY,0)) 	//æ�ȴ�
	{
		SysTimeDly(3);																	
	}
	
	readAddr = (id-1)*256 + START_ADDR_OF_NORMAL_VEHICLE;		//��DataFlashʱ�ṩ�ĵ�ַ

	rlen = BlockDeviceRead(MX251608D_Dev, readAddr, TmpCarinfoBuf, len);
	if(rlen <= 0)	//��ȡ
	{
		debug(Debug_Error,"Flash��ȡ����\r\n");
		return NULL;
	}
	
	if(rlen != len)																						//��ȡ�����ݸ�����ϣ����ȡ�����ݸ�����ƥ��
	{
		debug(Debug_Notify,"Flash���ݲ�ƥ�����\r\n");
		return NULL;
	}		
	else
	{
		debug(Debug_Notify,"��Flash����%d��������������,addr:0x%06x\r\n",id,readAddr);
	}

	if(!Check_Data_Valid(TmpCarinfoBuf, len))
	{
		debug(Debug_Error, "Query CarInfo invalid!\r\n");
		return NULL;
	}

	return TmpCarinfoBuf;
}

// ��ѯ���س�������
void *Query_Overweight_Vehicle(uint16 id, uint8 len)
{
	uint32 rlen = 0;
	uint32 readAddr = 0;				//��DataFlashʱ�ṩ�ĵ�ַ
	
	if((id < 1) || (id > MAX_OVERWEIGHT_VEHICLE_NUMBER)) return NULL;
	if(len < 2) return NULL;
	
	Param_Read(GET_U16_OFFSET(ParamOverWetCarNum), &gOverWeight_Num, sizeof(uint16));
	if(id > gOverWeight_Num)	return NULL;

	while(DeviceIoctl(MX251608D_Dev,MX251608D_CMDISBUSY,0)) 	//æ�ȴ�
	{
		SysTimeDly(3);																	
	}
	
	readAddr = (id-1)*256 + START_ADDR_OF_OVERWEIGHT_VEHICLE;		//��DataFlashʱ�ṩ�ĵ�ַ

	rlen = BlockDeviceRead(MX251608D_Dev,readAddr,TmpCarinfoBuf, len);
	if(rlen <= 0)	//��ȡ
	{
		debug(Debug_Notify,"Flash��ȡ����\r\n");
		return NULL;
	}
	if(rlen != len)																						//��ȡ�����ݸ�����ϣ����ȡ�����ݸ�����ƥ��
	{
		debug(Debug_Notify,"Flash���ݲ�ƥ�����\r\n");
		return NULL;
	}		
	else
	{
		debug(Debug_Notify,"��Flash����%d�����س�������,addr:0x%06x\r\n",id,readAddr);
	}

	if(!Check_Data_Valid(TmpCarinfoBuf, len))
	{
		debug(Debug_Error, "Query CarInfo invalid!\r\n");
		return NULL;
	}

	return TmpCarinfoBuf;
}

