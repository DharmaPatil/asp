#include "Flash_init.h"
#include "String.h"			//memcpy

/****************************************************************************
* FLASH���˵����
			-------------------------------------------------------------
			���洢����ʼ��ַ	0x08000000
			-------------------------------------------------------------
								   
FLASH_Unlock();															//����
FLASH_DataSpace_Erase(0x08000000,Size);			//���� ֻ�ܰ�������������������������С��FindSector����
FLASH_Lock();																//����
FLASH_ProgramWord(0x08030000,0x12345678);		//��ָ����λ�ñ��ָ������	��32λ���ݴ���	�����Ȳ����ٴ���
****************************************************************************/

/****************************************************************************
* ��	�ƣ�u32 FindSector(const u32 Address)
* ��	�ܣ����ݵ�ַ���ҵ�ַ��������
* ��ڲ�����const u32 Address			��ַ
* ���ڲ�����u32 ������
* ˵	����1M ROM����12������
0����:		16K
1����:		16K
2����:		16K
3����:		16K
4����:		64K
5����:		128K
6����:		128K
7����:		128K
8����:		128K
9����:		128K
10����:		128K
11����:		128K
16 + 16 + 16 + 16 + 64 + 128 + 128 + 128 + 128 + 128 + 128 + 128 = 1024
****************************************************************************/
u32 FindSector(const u32 Address)
{
  u32 sector = 0;
  
  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
    sector = FLASH_Sector_0;  
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
    sector = FLASH_Sector_1;  
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
    sector = FLASH_Sector_2;  
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
    sector = FLASH_Sector_3;  
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
    sector = FLASH_Sector_4;  
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
    sector = FLASH_Sector_5;  
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
    sector = FLASH_Sector_6;  
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
    sector = FLASH_Sector_7;  
  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
    sector = FLASH_Sector_8;  
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
    sector = FLASH_Sector_9;  
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
    sector = FLASH_Sector_10;  
  else
    sector = FLASH_Sector_11;  
	
	return sector;
}

/****************************************************************************
* ��	�ƣ�void FLASH_DataSpace_Erase(const u32 StartAddr,const u32 DataSize)
* ��	�ܣ��������ݿռ�
* ��ڲ�����const u32 StartAddr	��ʼ��ַ
						const u32 DataSize	8λ���ݴ�С
* ���ڲ�����u8		�����Ƿ�ɹ� 1�ɹ� 0ʧ��
* ˵	������
****************************************************************************/
u8 FLASH_DataSpace_Erase(const u32 StartAddr,const u32 DataSize)
{
	u32 i = 0;				//�ۼ���
  u32 UserStartSector = 0;			//��ʼ����
  u32 UserEndSector = 0;				//��������

  UserStartSector = FindSector(StartAddr);										//���ݵ�ַ�ҿ�ʼ����
  UserEndSector   = FindSector(StartAddr + DataSize - 1);			//���ݵ�ַ�ҽ�������

	FLASH_Unlock(); 	//����ǰ���FLASH����
  for(i = UserStartSector; i <= UserEndSector; i += 8)		//ָ��ʼ�����еĿ�ʼ��ַ�������ݴ�С�ռ�Ĳ���					
  {
    if (FLASH_EraseSector(i, VoltageRange_3) != FLASH_COMPLETE)
		{
			FLASH_Lock();	  				//����
      return 0;			//����ʧ��
		}
  }
  FLASH_Lock();	  				//����
  return 1;					//�����ɹ�
}	  

/****************************************************************************
* ��	�ƣ�u8 Flash_DataWrite(const u32 StartAddr,const u32 DataSize,u8* Data_temp)
* ��	�ܣ�������д��ָ����FLASH�ռ�
* ��ڲ����const �u32 StartAddr	��ʼ��ַ
			const u32 DataSize	���ݴ�С
			u8* Data_temp	����
* ���ڲ�����u8	0�洢ʧ��
				1�洢�ɹ�
* ˵	������32λ���ݳ��Ƚ��д洢	 ����Ҫ�Ȳ������ܸ�д
****************************************************************************/
u8 Flash_DataWrite(const u32 StartAddr,const u32 DataSize,u8* Data_temp)
{
	u32 i = 0;										//�ۼ���
	u32	DataSize32 = 0;						//32λ�����ݳ���
	u32 err = 0;									//����״̬
  u8 ucTemp[4] = {0};
    
	FLASH_Status status = FLASH_COMPLETE;	  	//����һ��FLASH��־

	DataSize32 = DataSize / 4;					//8λ�����ݳ���ת��32λ�����ݳ���
	
	while(1)
	{
		FLASH_Unlock();	  			//����
		status = FLASH_ProgramWord(StartAddr + i * 4, *(((u32*)Data_temp) + i));   //����д����
		FLASH_Lock();	  				//����
		if(status == FLASH_COMPLETE) 	//FLASH��æ
		{
      err = 0;
			if((*((u32*)(StartAddr + i * 4))) != *(((u32*)Data_temp) + i))	 //����Ч��
				return 0;					 //����Ч�����

			i++;							  //��һ��д�����
			if(i >= DataSize32)				  //�����һ�β�������������Ҫ�ķ�Χ
				break;						  //�˳�д�����
		}
		else 
		{
			if (++err > 50)
				return 0;
		}
	}	
  i = DataSize32 * 4;

	while(i < DataSize) 
	{
		memcpy(ucTemp,Data_temp + i,(DataSize - i));
		FLASH_Unlock();	  			//����
		status = FLASH_ProgramWord(StartAddr + i,*((u32*)ucTemp));   //����д����
		FLASH_Lock();	  			//����
		if(status == FLASH_COMPLETE) 	//FLASH��æ
		{
			err = 0;
			if((*((u32*)(StartAddr + i))) != *((u32*)ucTemp))	 //����Ч��
				return 0;					 //����Ч�����
			break;							  //��һ��д�����
		}
		else
		{
			if (++err > 50)
				return 0;
		}
	}
	return 1;				
}

/*
	u8 DATA[128]={0};			//��Ҫ��ŵ�����
	u8 i=0;

	for(i=0;i<128;i++)			//���ݸ�ֵ
		DATA[i]=i;

	//��ʽһ��
	FLASH_DataSpace_Erase(0x08030000,128);	//����	//��FLASH�� 0x08030000 ��ַ��ʼ����128��8λ����λ��		128*8=1024
	Flash_DataWrite(0x08030000,128,DATA);	//д��	//��FLASH�� 0x08030000 ��ַ��ʼ���128��8λDATA����
	FLASH_DataShow(0x08030000,128);			//����	//��FLASH�� 0x08030000 ��ַ��ʼ��ȡ����ʾ32��32λ����	32*32=1024	4��8λ=32λ

	//��ʽ����
	Flash_SAVEData(0x08030000,DATA,128);	//��д��
*/
