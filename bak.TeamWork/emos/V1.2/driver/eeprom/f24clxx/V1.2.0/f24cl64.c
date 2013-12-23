#include "f24cl64.h"
#include "C2.h"						//������
#include "driver.h"				//ע��
#include "stm32f4_crc.h"			//crc16
#include "String.h"				//memset

#define EEPROM_ADDR		0xA0

static SYS_EVENT *EEPROM_Mutex = 0;		 			//�����ȡ������

static u8 FM24_pDisc[] = "LuoHuaiXiang_FM24CL64\r\n";		//�豸�����ַ�

/****************************************************************************
* ��	�ƣ�u8 FM24CL64_Write(u32 Addr,u8* pData,u32 len)
* ��	�ܣ���оƬ����ҳд����
* ��ڲ���
						u32 Addr						оƬ��ŵ�ַ				0-65534		128/ҳ * 512ҳ
						u8* pData						��Ҫ�洢������
						u32 len							�洢�����ݳ���
* ���ڲ�����s32 �Ƿ�洢�ɹ�	-1:ʧ��  ������0��ʵ��д�����ݳ���
* ˵	������
****************************************************************************/
s32 FM24CL64_Write(u32 Addr,u8* pData,u32 len)
{
	u8 err;						//��ȡ�������
	s32 reallen = 0;
	
	if(EEPROM_Mutex == 0) 
		return -1;
	SysMutexPend(EEPROM_Mutex,0,&err);
	if(err != SYS_ERR_NONE)
		return -1;
	
	reallen = I2CSendWordADDRData(EEPROM_ADDR,Addr,pData,len);		//��������
// 	if(*Reallen != len)
// 		return 0;

	if(EEPROM_Mutex == 0) 
		return -1;
	SysMutexPost(EEPROM_Mutex);

	return reallen;
}

/****************************************************************************
* ��	�ƣ�u32 FM24CL64_Erase(u32 Addr,u8 pData,u32 len)
* ��	�ܣ���оƬ����ҳд����
* ��ڲ�����
						u32 Addr						оƬ��ŵ�ַ				0-65534		128/ҳ * 512ҳ
						u8* pData						��Ҫ�洢������
						u32 len							�洢�����ݳ���
* ���ڲ�����s32									ʵ�ʲ��������ݳ��� ����-1ʧ��
* ˵	������
****************************************************************************/
s32 FM24CL64_Erase(u32 Addr,u8 pData,u32 len)
{
	u8 err;						//��ȡ�������
	u32 i = 0;				//�ۼ���
	u32 realcnt = 0;	//ʵ��д������ݸ���
	
	if(EEPROM_Mutex == 0) 
		return -1;
	SysMutexPend(EEPROM_Mutex,0,&err);
	if(err != SYS_ERR_NONE)
		return -1;
	
	for(i = 0; i < len ; i++)  //һ������д
	{
// 		Reallen = I2CSendWordADDRData(EEPROM_ADDR,Addr + i,&pData,1);		//��������
		if(I2CSendWordADDRData(EEPROM_ADDR,Addr + i,&pData,1) != 1)
			return -1;
// 		Reallen = 0;
		realcnt++;
	}
	
	if(EEPROM_Mutex == 0) 
		return -1;
	SysMutexPost(EEPROM_Mutex);
	
	return realcnt;
}

/****************************************************************************
* ��	�ƣ�u8 FM24CL64_Read(EEPROM_Cfg* Ex,u32 Addr,u8* pData,u32 len,u32* Reallen)
* ��	�ܣ���оƬ����ҳ������
* ��ڲ�����EEPROM_Cfg* Ex			EEPROM�豸����
						u32 Addr						оƬ��ŵ�ַ				0-65534		128/ҳ * 512ҳ
						u8* pData						��Ҫ��ȡ������
						u32 len							��ȡ�����ݳ���
* ���ڲ�����s32 �Ƿ��ȡ�ɹ�	�ɹ�����ʵ�ʶ�ȡ�����ݳ���  ʧ�ܷ���-1
* ˵	������
****************************************************************************/
s32 FM24CL64_Read(u32 Addr,u8* pData,u32 len)
{
	u8 err = 0;					//����״̬
	s32 reallen = 0;
	
	if(EEPROM_Mutex == 0)									//������δ����
		return -1;
	SysMutexPend(EEPROM_Mutex, 0, &err);	//��ȡ������
	if(err != SYS_ERR_NONE)
		return -1;
	
	reallen = I2CReceiveWordADDRData(EEPROM_ADDR,Addr,pData,len);		//��������
// 	if(*Reallen != len)										//������յ����ݸ���������ϣ�����յ����ݸ���
// 		return -1;
	
	if(EEPROM_Mutex == 0)									//������δ����
		return -1;
	SysMutexPost(EEPROM_Mutex);						//�ͷŻ�����
	
// 	*Reallen = len;
	return reallen;
}

/****************************************************************************
* ��	�ƣ�static s8 FM24CL64_Open(u32 lParam)
* ��	�ܣ�ͳһ�ӿ�
* ��ڲ�����u32 lParam		��������
* ���ڲ�����u8						�򿪲�������ֵ	��HVL_ERR_CODE��
* ˵	������
****************************************************************************/
static s8 FM24CL64_Open(u32 lParam)
{
	return 0;					
}

/****************************************************************************
* ��	�ƣ�static s8 FM24CL64_Close(void)
* ��	�ܣ�ͳһ�ӿ�
* ��ڲ�������
* ���ڲ�����s8						�رղ�������ֵ	��HVL_ERR_CODE��
* ˵	������
****************************************************************************/
static s8 FM24CL64_Close(void)
{
	return 0;		
}

/****************************************************************************
* ��	�ƣ�static static s32 FM24CL64Read(u32 offset,u8* Buffer,u32 len,u32* Reallen,u32 lParam)
* ��	�ܣ����豸��
* ��ڲ�����u32 offset		ƫ�Ƶ�ַ
						u8* Buffer		��ȡ���ݴ��λ��
						u32 len				ϣ����ȡ�����ݳ���
* ���ڲ�����s32						�����Ƿ�ɹ�		0�ɹ�	1ʧ��
* ˵	������
****************************************************************************/
static s32 FM24CL64Read(u32 offset,u8* Buffer,u32 len)
{
	return FM24CL64_Read(offset,Buffer,len);
}

/****************************************************************************
* ��	�ƣ�static s32 FM24CL64Write(u32 offset,u8* Buffer,u32 len,u32* Reallen,u32 lParam)
* ��	�ܣ����豸д
* ��ڲ�����u32 offset		ƫ�Ƶ�ַ
						u8* Buffer		д������Դ���λ��
						u32 len				ϣ��д������ݳ���
* ���ڲ�����s32						-1:ʧ��  ������0��ʵ��д�����ݳ���
* ˵	������������+2(crc)����
****************************************************************************/
static s32 FM24CL64Write(u32 offset,u8* Buffer,u32 len)
{	
	return FM24CL64_Write(offset,Buffer,len);							//�洢�ɹ�
}

/****************************************************************************
* ��	�ƣ�static s32 FM24CL64_Ioctl(u32 cmd,u32 lParam)
* ��	�ܣ����ڿ���
* ��ڲ�����u32 cmd				����
						u32 lParam		����
* ���ڲ�����s32						��������Ĳ�ͬ���ص�ֵ���岻ͬ
* ˵	����	��
****************************************************************************/
static s32 FM24CL64_Ioctl(u32 cmd,u32 lParam)
{
	switch(cmd)
	{
		case FM24CL64_CMDERASE:				//оƬ����
		{
			return FM24CL64_Erase(((EEPROM_EraseCfg*)lParam)->StartAddr,
																((EEPROM_EraseCfg*)lParam)->DefaultVal,
																((EEPROM_EraseCfg*)lParam)->Len);
		}
	}
	return 0;	//��������	
}

/****************************************************************************
* ��	�ƣ�u8 FM24CL64_Init(u8 prio)
* ��	�ܣ������ʼ��
* ��ڲ�����u8 prio								���ȼ�
* ���ڲ�����u8 										�Ƿ�ɹ�		1�ɹ�	0ʧ��
* ˵	������
****************************************************************************/
u8 FM24CL64_Init(u8 prio)
{
	u8 err = 0;

	//ע���ñ���
	DEV_REG fm24 = 										//�豸ע����Ϣ��						����static		
	{
		BLOCK_FRAMEEPROM,								//�豸ID��
		0,  														//�Ƿ���			0:���ɹ���ʹ��, 1:�ɹ���ʹ��
		0, 															//���ڹ����豸���򿪴���
		1,															//��������
		1,															//���д����
		FM24_pDisc,											//��������			�Զ���
		20120001,												//�����汾			�Զ���
		(u32*)FM24CL64_Open,						//�豸�򿪺���ָ��
		(u32*)FM24CL64_Close, 					//�豸�رպ���ָ��
		(u32*)FM24CL64Read,							//�������
		(u32*)FM24CL64Write,						//��д����
		(u32*)FM24CL64_Ioctl						//���ƺ���
	};
	
	if(I2Cx_Init() == 0)		//EEPROM��ʼ��
		return 0;									//ʧ��
	
	if(EEPROM_Mutex == 0)
		EEPROM_Mutex = SysMutexCreate(prio, &err);							//����������  //�����������ȼ��������ȼ����ܱ�ռ��
	if(err != 0)																							//����������ʧ��
		return 0;
	
	if(DeviceInstall(&fm24) != HVL_NO_ERR)										//�豸ע��
		return 0;
	
	CRC32_init();
	return 1;										//�ɹ�
}
