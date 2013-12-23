#include "system_init.h"	//AppRunAddr
#include "driver.h"				//DevList_init
#include <String.h>

static u8 EMOS_Versions[] = "EMOS-V001-001-001-001";				//ƽ̨�汾��  //21���ַ�

static u32 AppRunAddr = 0x08000000;						//������ת��ַ				����ͨ���ӿ�����

//�̼����ݵ�ַ��
#define AppBakSpaceSize				262144 - 1				//�̼��������ռ��С	256K	��Сת���ɿռ���Ҫ-1 �ռ��0��ʼ���
#define StartAddr 						0x8060000				//Ӧ�ó��򱸷ݿ�ʼ��ַ(�������ݴ��)
#define EndAddr 						0x809FFFF				//Ӧ�ó��򱸷ݽ�����ַ(����Ч�顢���ȡ���Դ���)
#define AppCRCAddress					EndAddr - 3				//CRCЧ�����ַ	0x809FFFC - 0x809FFFF		4Byte
#define AppSizeAddress					EndAddr - 7				//��С��ŵ�ַ	0x809FFF8 - 0x809FFFB		4Byte
#define AppUpSrcAddress				EndAddr - 11			//�̼���Դ  	0x809FFF4 - 0x809FFF7		4Byte	


#define DelUPFlagSector FLASH_Sector_2							//ɾ����־����
#define DelAPPbakSector1 FLASH_Sector_7						//ɾ��Ӧ�ó��򱸷�����
#define DelAPPbakSector2 FLASH_Sector_8						//ɾ��Ӧ�ó��򱸷�����

#define UPBOOTFlag 						0x1004				//������־		�д˱�־�����¹̼��� û������Ҫ����

#define AppUPAddress					0x8008000							//�̼����±�־�� 					0x8008000 - 0x80BFFFF		64K

/****************************************************************************
* ��	�ƣ�u8 Get_EMOS_Versions(u8* Versions)
* ��	�ܣ���ȡƽ̨�汾��
* ��ڲ�����Versions		�汾�ŵ�ַ
* ���ڲ�����u8					�汾���ַ�����
* ˵	�����汾��21���ַ�
****************************************************************************/
u8 Get_EMOS_Versions(u8* Versions)
{	 
	memcpy(Versions,EMOS_Versions,sizeof(EMOS_Versions) - 1);
	return sizeof(EMOS_Versions) - 1;
}

/****************************************************************************
* ��	�ƣ�INT32U SYS_CPU_SysTickClkFreq(void)
* ��	�ܣ���ȡϵͳ��Ƶʱ��Ƶ��
* ��ڲ�������
* ���ڲ�������ƵƵ��
* ˵	����Systick��ʼ���л��õ�
****************************************************************************/
INT32U SYS_CPU_SysTickClkFreq(void)
{	 
	RCC_ClocksTypeDef rcc_clocks;								//Ƭ�ϸ���ʱ��Ƶ��
	RCC_GetClocksFreq(&rcc_clocks);							//��ȡƬ��ʱ��Ƶ��
	return ((INT32U)rcc_clocks.HCLK_Frequency);	//����Ƭ����ƵƵ��
}

/****************************************************************************
* ��	�ƣ�uint32_t Get_CPUSysTime(void)
* ��	�ܣ���ȡϵͳ��ʱ��Ƶ��
* ��ڲ�������
* ���ڲ�����uint32_t	��ʱ��Ƶ��
* ˵	����
0 = SYSCLK clock frequency expressed in Hz
1 = HCLK clock frequency expressed in Hz
2 = PCLK1 clock frequency expressed in Hz
3 = PCLK2 clock frequency expressed in Hz
��������0
****************************************************************************/
uint32_t Get_CPUSysTime(u8 Flag)
{
	RCC_ClocksTypeDef rcc_clocks;								//Ƭ�ϸ���ʱ��Ƶ��
	RCC_GetClocksFreq(&rcc_clocks);							//��ȡƬ��ʱ��Ƶ��
	
	switch(Flag)
	{
		case 0:
			return rcc_clocks.SYSCLK_Frequency;			/*!<  SYSCLK clock frequency expressed in Hz */
		case 1:
			return rcc_clocks.HCLK_Frequency;   		/*!<  HCLK clock frequency expressed in Hz */
		case 2:
			return rcc_clocks.PCLK1_Frequency;  		/*!<  PCLK1 clock frequency expressed in Hz */
		case 3:
			return rcc_clocks.PCLK2_Frequency;  		/*!<  PCLK2 clock frequency expressed in Hz */
	}
	return 0;
}

/****************************************************************************
* ��	�ƣ�void SetAppRunAddr(u32 addr)
* ��	�ܣ�������ת��ַ
* ��ڲ�����u32 addr				��ת��ַ
* ���ڲ�������
* ˵	������
****************************************************************************/
void SetAppRunAddr(u32 addr)
{
	AppRunAddr = addr;
}

/****************************************************************************
* ��	�ƣ�u32 LookAppRunAddr(void)
* ��	�ܣ���ȡ��ת��ַ
* ��ڲ�������
* ���ڲ�����u32 						��ת��ַ
* ˵	������
****************************************************************************/
u32 LookAppRunAddr(void)
{
	return AppRunAddr;
}

/****************************************************************************
* ��	�ƣ�void Nvic_init(void)
* ��	�ܣ��жϹ����ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void Nvic_init(void)
{
	#ifdef  VECT_TAB_RAM	//�ж��������ŵ�ַ��SRAM 0x20000000  
	  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
	#else					//�ж��������ŵ�ַ��FLASH	0x08000000
		NVIC_SetVectorTable(NVIC_VectTab_FLASH, AppRunAddr);   
	#endif
}

/****************************************************************************
* ��	�ƣ�void GetCpuIdHex(void)
* ��	�ܣ���ȡCPUоƬIDֵ16����   
* ��ڲ�������
* ���ڲ�������
* ˵	����6��u16��ֵ���CPUID
			u16 x[6] = {0};
			u8 x[12] = {0};
****************************************************************************/
void GetCpuIdHex(u8* HCpuId)
{
	u32 CpuId[3] = {0};

	CpuId[0] = (*(u32*)(0x1FFF7A10));
	CpuId[1] = (*(u32*)(0x1FFF7A14));
	CpuId[2] = (*(u32*)(0x1FFF7A18));
	
	HCpuId[0] = (u8)(CpuId[0] >> 24);
	HCpuId[1] = (u8)(CpuId[0] >> 16); 
	HCpuId[2] = (u8)(CpuId[0] >> 8);
	HCpuId[3] = (u8)(CpuId[0]);

	HCpuId[4] = (u8)(CpuId[1] >> 24);
	HCpuId[5] = (u8)(CpuId[1] >> 16); 
	HCpuId[6] = (u8)(CpuId[1] >> 8);
	HCpuId[7] = (u8)(CpuId[1]);

	HCpuId[8] = (u8)(CpuId[2] >> 24);
	HCpuId[9] = (u8)(CpuId[2] >> 16); 
	HCpuId[10] = (u8)(CpuId[2] >> 8);
	HCpuId[11] = (u8)(CpuId[2]);
}

/****************************************************************************
* ��	�ƣ�void System_Init(void)
* ��	�ܣ�ϵͳ��ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void System_Init(void)
{
	Nvic_init();							//�жϹ����ʼ��
	SYS_CPU_SysTickInit();		//��ʼ��ϵͳ�δ�ʱ��	(10���뼶)
	SysInit();								//ϵͳ��ʼ��
	DevList_init();						//�豸����Ԫ��ʼ��
}

/****************************************************************************
* ��	�ƣ�u32 BeiNumber(u32 num_temp,u8 len_temp)
* ��	�ܣ�10����
* ��ڲ�����u32 num_temp	��Ҫ��������
						u8 len_temp		�����ٱ� ��1������ٸ�0
* ���ڲ�����u32 					������ı���ֵ
* ���Դ��룺
	u32 x = 2;
	x = BeiNumber(x,3);
	//x = 2000;
****************************************************************************/
u32 BeiNumber(u32 num_temp,u8 len_temp)
{
	u32 Bei = 1;		  			//Ĭ�ϱ���ֵ

	while(len_temp--)				//���������з���
		Bei *= 10;	  				//��������
	return Bei * num_temp;	//���ر���
}

/****************************************************************************
* ��	�ƣ�u32 ASCIItoNumber(u8* data_temp,u8 len_temp)
* ��	�ܣ���ASCII������ת����Ϊu32������
* ��ڲ�����u8* data_temp	ASCII������
						u8 len_temp		ASCII�����ָ���
* ���ڲ�����u32 unsigned 	int������
* ���Դ��룺
u32 num = 0;
num = ASCIItoNumber("1234",4);
//num = 1234;
****************************************************************************/
u32 ASCIItoNumber(u8* data_temp,u8 len_temp)
{
	u8 i = len_temp;											//����
	u32 val = 0; 
	u32 len = 0;                      		//���ճ��� 
	
	while(1)
	{
		val = data_temp[i - 1] - 0x30;     	//ȡ���� 
		val = BeiNumber(val,len_temp - i);  //�õ�λ������
		len += val;                    			//λ��������� 
		
		i--;
		if(i == 0)
			return len;
	} 
}

/****************************************************************************
* ��	�ƣ�u32 BCDToBIN(u32 BCD)
* ��	�ܣ���ʮ��������ת��������10������
* ��ڲ�����u32 ʮ��������
* ���ڲ�����u32 ����������
* ���Դ��룺
u32 BcdVal = 101;
u32 BinVal = 0;
BinVal = BCDToBIN(BcdVal);
//BinVal = 65;
****************************************************************************/
u32 BCDToBIN(u32 BCD)
{
	return (BCD & 0x0f) + (BCD >> 4)	* 10;
}

/****************************************************************************
* ��	�ƣ�u32 BINToBCD(u32 BIN)
* ��	�ܣ�����������10������תʮ��������
* ��ڲ�����u32 ����������
* ���ڲ�����u32 ʮ��������
* ���Դ��룺
u32 BinVal = 65;
u32 BcdVal = 0;
BcdVal = BINToBCD(BinVal);
//BcdVal = 101;
****************************************************************************/
u32 BINToBCD(u32 BIN)
{
	return ((BIN / 10) << 4) + BIN % 10;
}

/****************************************************************************
* ��	�ƣ�void FLASH_Erase_Delay(u32 cnt)
* ��	�ܣ���ʱ
* ��ڲ�������
* ���ڲ�����u32		��ʱʱ��
* ˵	����һ��Ҫ����Ƿ�����ɹ������ɹ�������һ�Ρ�
****************************************************************************/
void FLASH_Erase_Delay(u32 cnt)
{
	while(cnt--);
}

/****************************************************************************
* ��	�ƣ�u8 FLASH_APPBakOne_Erase(void)
* ��	�ܣ�����Ӧ�����ݿռ�1
* ��ڲ�������
* ���ڲ�����u8		�����Ƿ�ɹ� 1�ɹ� 0ʧ��
* ˵	����һ��Ҫ����Ƿ�����ɹ������ɹ�������һ�Ρ�
****************************************************************************/
u8 FLASH_APPBakOne_Erase(void)
{
	FLASH_Unlock(); 	//����ǰ���FLASH����
	//2013-05-10 ���Ӳ���flash��־λ
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR |
						FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR |
						FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
 	if(FLASH_EraseSector(DelAPPbakSector1, VoltageRange_3) != FLASH_COMPLETE)
 	{
 		FLASH_Lock();	  //����
 		return 0;				//����ʧ��
 	}
  FLASH_Lock();	  	//����
  return 1;					//�����ɹ�
}	

/****************************************************************************
* ��	�ƣ�u8 FLASH_APPBakTwo_Erase(void)
* ��	�ܣ�����Ӧ�����ݿռ�2
* ��ڲ�������
* ���ڲ�����u8		�����Ƿ�ɹ� 1�ɹ� 0ʧ��
* ˵	����һ��Ҫ����Ƿ�����ɹ������ɹ�������һ�Ρ�
****************************************************************************/
u8 FLASH_APPBakTwo_Erase(void)
{
	FLASH_Unlock(); 	//����ǰ���FLASH����
	//2013-05-10 ���Ӳ���flash��־λ
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR |
						FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR |
						FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
	if(FLASH_EraseSector(DelAPPbakSector2, VoltageRange_3) != FLASH_COMPLETE)
	{
		FLASH_Lock();	  //����
		return 0;				//����ʧ��
	}
  FLASH_Lock();	  	//����
  return 1;					//�����ɹ�
}	

/****************************************************************************
* ��	�ƣ�u8 FLASH_UPFlag_Erase(void)
* ��	�ܣ��������±�־�ռ�
* ��ڲ�������
* ���ڲ�����u8		�����Ƿ�ɹ� 1�ɹ� 0ʧ��
* ˵	����һ��Ҫ����Ƿ�����ɹ������ɹ�������һ�Ρ�
****************************************************************************/
u8 FLASH_UPFlag_Erase(void)
{
	FLASH_Unlock(); 	//����ǰ���FLASH����
 	if(FLASH_EraseSector(DelUPFlagSector, VoltageRange_3) != FLASH_COMPLETE)
 	{
 		FLASH_Lock();	  //����
 		return 0;				//����ʧ��
 	}
  FLASH_Lock();	  	//����
  return 1;					//�����ɹ�
}	

/****************************************************************************
* ��	�ƣ�u8 Flash_APPBak_Store(u32 Addri,u8* Data,u32 DataSize)
* ��	�ܣ�������д��ָ����FLASH�ռ�
* ��ڲ�����u32 Addri			ƫ�Ƶ�ַ	4�ı���(Flash4�ֽڲ���)
						u8* Data����
						u32 DataSize	���ݴ�С
* ���ڲ�����u8	0�洢ʧ��
				1�洢�ɹ�
* ˵	������32λ���ݳ��Ƚ��д洢	 ����Ҫ�Ȳ������ܸ�д
****************************************************************************/
u8 Flash_APPBak_Store(u32 Addri,u8* Data,u32 DataSize)
{
	u32 i = 0;										//�ۼ���
	u32	DataSize32 = 0;						//32λ�����ݳ���
	u32 err = 0;									//����״̬
  u8 ucTemp[4] = {0};
  u32 startOffsetAddr = 0;			//��ƫ�����Ŀ�ʼ��ַ
  
	FLASH_Status status = FLASH_COMPLETE;	  	//����һ��FLASH��־

	if(Addri % 4 != 0)												//FLASH����4�ֽڲ���
		return 0;
	
	startOffsetAddr = StartAddr + Addri;			
	if(startOffsetAddr > EndAddr)							//�������Χ
		return 0;
	
	DataSize32 = DataSize / 4;								//8λ�����ݳ���ת��32λ�����ݳ���
	
	while(1)
	{
		FLASH_Unlock();	  						//����
		status = FLASH_ProgramWord(startOffsetAddr + i * 4, *(((u32*)Data) + i));   //����д����
		FLASH_Lock();	  							//����
		if(status == FLASH_COMPLETE) 	//FLASH��æ
		{
			err = 0;
			if((*((u32*)(startOffsetAddr + i * 4))) != *(((u32*)Data) + i))	 //����Ч��
				return 0;								 	//����Ч�����

			i++;							  				//��һ��д�����
			if(i >= DataSize32)				  //�����һ�β�������������Ҫ�ķ�Χ
				break;						  			//�˳�д�����
		}
		else 
		{
			if(++err > 50)
				return 0;
		}
	}	
  i = DataSize32 * 4;

	while(i < DataSize) 
	{
		memcpy(ucTemp,Data + i,(DataSize - i));
		FLASH_Unlock();	  			//����
		status = FLASH_ProgramWord(startOffsetAddr + i,*((u32*)ucTemp));   //����д����
		FLASH_Lock();	  			//����
		if(status == FLASH_COMPLETE) 	//FLASH��æ
		{
			err = 0;
			if((*((u32*)(startOffsetAddr + i))) != *((u32*)ucTemp))	 //����Ч��
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

/****************************************************************************
* ��	�ƣ�u8 Flash_APPInfo_Store(u32 FILECRC,u32 FILESize)
* ��	�ܣ�������д��ָ����FLASH�ռ�
* ��ڲ�����u8* Data����
						u32 DataSize	���ݴ�С
* ���ڲ�����u8	0�洢ʧ��
				1�洢�ɹ�
* ˵	������32λ���ݳ��Ƚ��д洢	 ����Ҫ�Ȳ������ܸ�д
****************************************************************************/
u8 Flash_APPInfo_Store(u32 FILECRC,u32 FILESize, u32 flag)
{
	u32 FileUpSrc = flag; //NETBOOT;	
	
	FLASH_Unlock();	  			//����
	if(FLASH_ProgramWord(AppCRCAddress,FILECRC) != FLASH_COMPLETE)							//���±�־
	{
		FLASH_Lock();	  			//����
		return 0;
	}
	if(FLASH_ProgramWord(AppSizeAddress,FILESize) != FLASH_COMPLETE)						//���±�־
	{
		FLASH_Lock();	  			//����
		return 0;
	}
	if(FLASH_ProgramWord(AppUpSrcAddress,FileUpSrc) != FLASH_COMPLETE)					//���±�־
	{
		FLASH_Lock();	  			//����
		return 0;
	}
	FLASH_Lock();	  			//����
	return 1;
}

/****************************************************************************
* ��	�ƣ�u8 Flash_UPFlag(void)
* ��	�ܣ��޸ĸ��±�־
* ��ڲ�������
* ���ڲ�����u8	0�洢ʧ��
								1�洢�ɹ�
* ˵	������
****************************************************************************/
u8 Flash_UPFlag(void)
{
	u8 err = 0;
	u32 newbootflag = UPBOOTFlag;			//���±�־
	
	while(FLASH_UPFlag_Erase() == 0)	//����
	{
		err++;
		
		if(err > 5)
			return 0;
		
		FLASH_Erase_Delay(1000);				//��ʱ�������һ�β���
	}
	
	FLASH_Unlock();	  			//����
	if(FLASH_ProgramWord(AppUPAddress,newbootflag) != FLASH_COMPLETE)					//���±�־
	{
		FLASH_Lock();	  			//����
		return 0;
	}
	FLASH_Lock();	  			//����
	return 1;
}
