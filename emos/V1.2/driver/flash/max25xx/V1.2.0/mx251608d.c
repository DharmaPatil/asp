#include "mx251608d.h"
#include "SPIx.h"
#include "C2.h"
#include "IO.h"

/*
MX25Lxx08D
һҳ		256		Byte
һ����	4096	Byte		256*16ҳ
һ��		65536	Byte		4096*16����		256*256ҳ
1608D = 2M = 2097152 Byte
*/

#define PageSize 			256													//ҳ��С
#define PageNo				8192												//ҳ��
#define SectorSize		4096												//������С
#define SectorNo			512													//��������
#define BlockSize			65536												//���С
#define BlockNo				32													//������
#define CapacitySize 	2097152											//������

#define FLASH_FCE_PIN		GPIO_Pin_9
#define FLASH_FCE_PORT	GPIOC

typedef struct
{
	uint16_t 				FCE_Pin;					//Ƭѡ����
	GPIO_TypeDef* 	FCE_Port;					//Ƭѡ���Ŷ˿ں�
	SPI_Cfg 				FSPI;							//�豸����SPI�ӿ�
}MX251608D_Cfg;		//DATAFLASH�ṹ��

static SYS_EVENT *MX251608D_Mutex = 0;		 			//�����ȡ������

#define MX251608DA				1					//MX251608DA					1����		0�ر�

#if(MX251608DA == 1)
static u8 MX25A_pDisc[] = "LuoHuaiXiang_MX25A\r\n";		//�豸�����ַ�
// static MX251608D_Cfg MX25A =
// {
// 	GPIO_Pin_9,					//FCE_PIN
// 	GPIOC,							//FCE_PORT
// 	{	
// 		0,								//IO
// 		GPIO_Pin_10,			//SCK_PIN
// 		GPIOC,						//SCK_PORT
// 		GPIO_Pin_11,			//MISO_PIN
// 		GPIOC,						//MISO_PORT
// 		GPIO_Pin_12,			//MOSI_PIN
// 		GPIOC,						//MOSI_PORT
// 		SPI_BaudRatePrescaler_32		//baudrate
// 	}
// };
#endif

/****************************************************************************
* ��	�ƣ�void MX251608D_CEH(MX251608D_Cfg* Fx)
* ��	�ܣ��豸Ƭѡ����
* ��ڲ�����MX251608D_Cfg* Fx		�豸
* ���ڲ�������
* ˵	������
****************************************************************************/
void MX251608D_CEH()	
{
	IoHi(FLASH_FCE);
	
}

/****************************************************************************
* ��	�ƣ�void MX251608D_CEL(MX251608D_Cfg* Fx)
* ��	�ܣ��豸Ƭѡ����
* ��ڲ�����MX251608D_Cfg* Fx		�豸
* ���ڲ�������
* ˵	������
****************************************************************************/
void MX251608D_CEL()	
{
	IoLo(FLASH_FCE);
}

/****************************************************************************
* ��	�ƣ�void GetMX251608Dx_ID(u8* ID,MX251608D_Cfg* Fx)
* ��	�ܣ���ȡ�豸ID
* ��ڲ�����u8* ID							�豸ID
						MX251608D_Cfg* Fx		�豸
* ���ڲ�������
* ˵	����u8 ID[3] = {0};
****************************************************************************/
void GetMX251608Dx_ID(u8* ID)
{
	if(ID == (u8*)0) 														//���ȱ���
		return;			
	
	MX251608D_CEL();													//�����豸
	
	SPIbyteSendGet(0x9F);
	ID[0] = SPIbyteSendGet(0);
	ID[1] = SPIbyteSendGet(0);
	ID[2] = SPIbyteSendGet(0);
	
	MX251608D_CEH();													//�ر��豸
}

/****************************************************************************
* ��	�ƣ�u8 MX251608D_Read(MX251608D_Cfg* Fx,u32 ReadAddr,u8* pBuffer,u32 Len,u32* RealLen)
* ��	�ܣ�Flash�����Զ�����
* ��ڲ�����
						u32 ReadAddr					��ʼ��ȡλ��
						u8* pBuffer						��ȡ���ݴ�ŵ�ַ
						u32 Len								ϣ����ȡ�����ݸ���
* ���ڲ�����s32 										���ݶ�ȡ�Ƿ�ɹ�		
* ˵	������
****************************************************************************/
s32 MX251608D_Read(u32 ReadAddr,u8* pBuffer,u32 Len)
{
	u8 err = 0;																	//����״̬
  u32 realLen = 0;														//ʵ�ʶ��������ݸ���

	if(pBuffer == 0) 														//ָ�뱣��
		return -1;	
	if(Len == 0) 																//���ȱ���
		return -1;			
// 	if(Fx == (MX251608D_Cfg*)0) 								//���Ʊ���
// 		return 0;			
	
	if(MX251608D_Mutex == 0)		//������δ����
		return -1;
	SysMutexPend(MX251608D_Mutex, 0, &err);		//��ȡ������		δ��ȡ����һֱ�ȴ�
	
  MX251608D_CEL();													//�����豸
	
	SPIbyteSendGet(0x03);
	SPIbyteSendGet((ReadAddr & 0xFF0000) >> 16);
	SPIbyteSendGet((ReadAddr & 0xFF00) >> 8);
	SPIbyteSendGet(ReadAddr & 0xFF);
	
	while (Len--) 
	{
		*pBuffer = SPIbyteSendGet(0);					//��ȡ����
		pBuffer++;																			//��һ��ַ
		realLen++;																			//ʵ�ʶ��������ݸ���
	}
		
  MX251608D_CEH();												//�ر��豸
	
	if(MX251608D_Mutex == 0)									//������δ����
		return -1;
	SysMutexPost(MX251608D_Mutex);						//�ͷŻ�����
	
// 	*RealLen = realLen;												//ʵ�ʻ�ȡ�����ݸ���
  return realLen;																	//��ȡ���
}

/****************************************************************************
* ��	�ƣ�u8 MX251608D_IsBusy(MX251608D_Cfg* Fx)
* ��	�ܣ�æ״̬��ѯ
* ��ڲ�����MX251608D_Cfg* Fx		�豸
* ���ڲ�����u8		1æ	0��
* ˵	������
****************************************************************************/
u8 MX251608D_IsBusy()
{
	u8 Bflag = 0;																//æ��־
  u8 flashstatus = 0;
		
	MX251608D_CEL();													//�����豸
	
	SPIbyteSendGet(0x05);										//��ѯ״̬
	flashstatus = SPIbyteSendGet(0);
	
 	Bflag = flashstatus & 0x01;									//��ȡæ��־	[0]λ�ϵı�־��æ״̬ 1æ 0��æ
	MX251608D_CEH();													//�ر��豸
	
	return Bflag;																//����æ��־
}

/****************************************************************************
* ��	�ƣ�void MX251608D_BusyWait(MX251608D_Cfg* Fx)
* ��	�ܣ�æ�ȴ�
* ��ڲ�����MX251608D_Cfg* Fx		�豸
* ���ڲ�������
* ˵	����ֱ��æ�����Ż��˳�
****************************************************************************/
void MX251608D_BusyWait()
{
  u8 flashstatus = 1;													//��ʼ��Ϊæ״̬
		
	MX251608D_CEL();													//�����豸
	
	SPIbyteSendGet(0x05);							//��ѯ״̬
	
	while ((flashstatus & 0x01) == 1)
		flashstatus = SPIbyteSendGet(0);
	
	MX251608D_CEH();													//�ر��豸
}

/****************************************************************************
* ��	�ƣ�void MX251608D_Erase(MX251608D_Cfg* Fx)
* ��	�ܣ�����оƬ
* ��ڲ�����MX251608D_Cfg* Fx	�豸
* ���ڲ�������
* ˵	��:��
****************************************************************************/
void MX251608D_Erase()
{
	MX251608D_CEL();													//�����豸
	SPIbyteSendGet(0x06);							//дʹ��
	MX251608D_CEH();													//�ر��豸
		
	MX251608D_CEL();													//�����豸
	SPIbyteSendGet(0xC7);							//оƬ����
	
	MX251608D_CEH();													//�ر��豸
	
	MX251608D_CEL();													//�����豸
	SPIbyteSendGet(0x04);							//дʧ��
	
	MX251608D_CEH();													//�ر��豸
}

/****************************************************************************
* ��	�ƣ�void MX251608D_EraseSector(MX251608D_Cfg* Fx,u32 SectorAddr)
* ��	�ܣ��������� 4096 Byte
* ��ڲ�����MX251608D_Cfg* Fx		�豸
						u32 SectorAddr			�ڲ���ַ����������
* ���ڲ�������
* ˵	��:������ǰ��ַ���������е�����
һ����	4096	Byte		256*16ҳ
****************************************************************************/
void MX251608D_EraseSector(u32 SectorAddr)
{
	MX251608D_CEL();												//�����豸
	SPIbyteSendGet(0x06);						//дʹ��
	MX251608D_CEH();												//�ر��豸
	
	MX251608D_CEL();												//�����豸
	SPIbyteSendGet(0x20);						//��������
	SPIbyteSendGet((SectorAddr & 0xFF0000) >> 16);
	SPIbyteSendGet((SectorAddr & 0xFF00) >> 8);
	SPIbyteSendGet(SectorAddr & 0xFF);
	
	MX251608D_CEH();													//�ر��豸
	
	MX251608D_CEL();													//�����豸
	
	SPIbyteSendGet(0x04);							//дʧ��
	
	MX251608D_CEH();													//�ر��豸	
}

/****************************************************************************
* ��	�ƣ�void MX251608D_EraseBlock(MX251608D_Cfg* Fx,u32 BlockAddr)
* ��	�ܣ������� 65536 Byte
* ��ڲ�����BlockAddr  ���ַ
* ���ڲ�������
* ˵	��:һ��		65536	Byte		4096*16����		256*256ҳ
****************************************************************************/
void MX251608D_EraseBlock(u32 BlockAddr)
{	
	MX251608D_CEL();													//�����豸
	SPIbyteSendGet(0x06);							//дʹ��
	MX251608D_CEH();													//�ر��豸
	
	MX251608D_CEL();													//�����豸
	SPIbyteSendGet(0xD8);							//�����
	SPIbyteSendGet((BlockAddr & 0xFF0000) >> 16);
	SPIbyteSendGet((BlockAddr & 0xFF00) >> 8);
	SPIbyteSendGet(BlockAddr & 0xFF);
    
    
	MX251608D_CEH();													//�ر��豸
	
	MX251608D_CEL();													//�����豸
	
	SPIbyteSendGet(0x04);							//дʧ��
	
	MX251608D_CEH();													//�ر��豸
}

/****************************************************************************
* ��	�ƣ�	void MX251608D_PageWrite(MX251608D_Cfg* Fx,u16 WriteAddr,u8* pBuffer,u32 Len)
* ��	�ܣ�	Flash��ҳд����
* ��ڲ�����MX251608D_Cfg* Fx			�豸
						u32 WriteAddr					ҳ�ڵ�ַ
						u8* pBuffer						Ҫд�������
						u32 Len								д������ݳ���
* ���ڲ�������
* ˵	����һҳ256�ֽ�
					����256�ֽڵ����ݻ��ͷ�ٿ�ʼ����д��
					[0][0][0][0][0] <- ��[3]��ʼд��12345
					[3][4][5][1][2]
****************************************************************************/
void MX251608D_PageWrite(u8* pBuffer, u32 WriteAddr, u32 Len)
{
	if(pBuffer == 0) 																	//ָ�뱣��
		return;	
	if(Len == 0) 																			//���ȱ���
		return;			

	MX251608D_CEL();																//�����豸
	SPIbyteSendGet(0x06);										//дʹ��
	MX251608D_CEH();																//�ر��豸
	
	MX251608D_CEL();																//�����豸
	SPIbyteSendGet(0x02);										//ѡ��ҳ
	SPIbyteSendGet((WriteAddr & 0xFF0000) >> 16);	//��ַ
	SPIbyteSendGet((WriteAddr & 0xFF00) >> 8);
	SPIbyteSendGet(WriteAddr & 0xFF);
	
	while (Len--)		//����
	{
		SPIbyteSendGet(*pBuffer);
		pBuffer++;
	}
		
  MX251608D_CEH();															//�ر��豸
	
	MX251608D_BusyWait();													//æ�ȴ�
	
	MX251608D_CEL();															//�����豸
	
	SPIbyteSendGet(0x04);									//дʧ��
	
	MX251608D_CEH();															//�ر��豸
}

/****************************************************************************
* ��	�ƣ�u8 MX251608D_Write(MX251608D_Cfg* Fx,u32 Addr,u8* Buffer,u32 Len,u32* RealLen)
* ��	�ܣ����Դ洢
* ��ڲ�����MX251608D_Cfg* Fx		�豸
						u32 Addr						��ʼ��ַ
						u8* Buffer					����������
						u32 Len							���ݳ���
						u8 Num	 						������
* ���ڲ�����s32 �洢�Ƿ�ɹ�	
* ˵	����
****************************************************************************/
s32 MX251608D_Write(u32 Addr,u8* Buffer,u32 Len)
{
	u8 err;
	u16 PageSurplusLen = 0;  				//ҳʣ��ռ䳤�� ���256
	u16 WLen = 0;										//��ǰ�����ݳ���
	u32 RCnt = 0;										//ʵ��д������ݸ���
	
	if(Buffer == 0) 								//ָ�뱣��
		return -1;	
	if(Len == 0) 										//���ȱ���
		return -1;			
// 	if(Fx == (MX251608D_Cfg*)0) 		//���Ʊ���
// 		return 0;			
	
	//�������洢��Χ
	if((Addr + Len) > CapacitySize)   //0 - 2097152 (256 * 256)
		return -1;
	
	//д����	 	
	while(Len > 0)		//����û������
	{
		PageSurplusLen = PageSize - Addr % PageSize;					//���������ҳ������ҳ�ڵ�ַ��ҳ�������Դ���ٸ�����
		
		if(Len > PageSurplusLen)		//ϣ���洢�����ݸ����Ƿ񳬹���ǰ����ҳʣ������
			WLen = PageSurplusLen;		//��ʣ�������洢
		else
			WLen = Len;								//��ϣ���洢�����ݸ����洢
		
		if(MX251608D_Mutex == 0)		//������δ����
			return -1;
		SysMutexPend(MX251608D_Mutex, 0, &err);		//��ȡ������	δ��ȡ����һֱ�ȴ���ȡ
		
		MX251608D_PageWrite(Buffer,Addr,WLen);	//ҳ�洢����
		MX251608D_BusyWait();										//æ�ȴ�
		
		if(MX251608D_Mutex == 0)		//������δ����
			return -1;
		SysMutexPost(MX251608D_Mutex);						//�ͷŻ�����
		
		RCnt += WLen;								//ʵ��д������ݸ���
		Len -= WLen;								//����ϣ���洢�����ݸ���
		Buffer += WLen;							//�洢����Դƫ��
		Addr += WLen;								//��ʼ�洢��ַƫ��
		
		if(Len != 0)								//����û�д洢��				
		{
			if((Addr % PageSize) != 0)			//��ַ��� ��һ������һ��Ϊҳ��ͷ��ʼȡ
				return -1;	
		}
	}
// 	*RealLen = RCnt;							//ʵ��д������ݸ���
	return RCnt;
}

/****************************************************************************
* ��	�ƣ�static s8 MX251608D_Open(u32 lParam)
* ��	�ܣ�ͳһ�ӿ�
* ��ڲ�����u32 lParam		��������
* ���ڲ�����u8						�򿪲�������ֵ	��HVL_ERR_CODE��
* ˵	������
****************************************************************************/
static s8 MX251608D_Open(u32 lParam)
{
	return 0;					
}

/****************************************************************************
* ��	�ƣ�static s8 MX251608D_Close(void)
* ��	�ܣ�ͳһ�ӿ�
* ��ڲ�������
* ���ڲ�����s8						�رղ�������ֵ	��HVL_ERR_CODE��
* ˵	������
****************************************************************************/
static s8 MX251608D_Close(void)
{
	return 0;		
}

/****************************************************************************
* ��	�ƣ�static s32 MX251608DRead(u32 offset,u8* Buffer,u32 len,u32* Reallen,u32 lParam)
* ��	�ܣ����豸��
* ��ڲ�����u32 offset		ƫ�Ƶ�ַ
						u8* Buffer		��ȡ���ݴ��λ��
						u32 len				ϣ����ȡ�����ݳ���
						u32* Reallen	ʵ�ʶ�ȡ�����ݳ���
						u32 lParam		����
* ���ڲ�����s32					
* ˵	������
****************************************************************************/
static s32 MX251608DRead(u32 offset,u8* Buffer,u32 len)
{
	return MX251608D_Read(offset,Buffer,len);
}

/****************************************************************************
* ��	�ƣ�static s32 MX251608DWrite(u32 offset,u8* Buffer,u32 len,u32* Reallen,u32 lParam)
* ��	�ܣ����豸д
* ��ڲ�����u32 offset		ƫ�Ƶ�ַ
						u8* Buffer		д������Դ���λ��
						u32 len				ϣ��д������ݳ���
* ���ڲ�����s32						�����Ƿ�ɹ�	
* ˵	������
****************************************************************************/
static s32 MX251608DWrite(u32 offset,u8* Buffer,u32 len)
{
	return MX251608D_Write(offset,Buffer,len);
}

/****************************************************************************
* ��	�ƣ�static s32 MX251608D_Ioctl(u32 cmd,u32 lParam)
* ��	�ܣ����ڿ���
* ��ڲ�����u32 cmd				����
						u32 lParam		����
* ���ڲ�����s32						��������Ĳ�ͬ���ص�ֵ���岻ͬ
* ˵	����	��
****************************************************************************/
static s32 MX251608D_Ioctl(u32 cmd,u32 lParam)
{
	switch(cmd)
	{
		case MX251608D_CMDERASE:				//оƬ����
		{
			MX251608D_Erase();
			return 1;							
		}
		case MX251608D_CMDBLOCKERASE:		//�����
		{
			MX251608D_EraseBlock(lParam);
			return 1;							
		}
		case MX251608D_CMDSECTORERASE:	//��������
		{
			MX251608D_EraseSector(lParam);
			return 1;			
		}
		case MX251608D_CMDISBUSY:				//æ���
			return MX251608D_IsBusy();
			
		case MX251608D_CMDBUSYWAIT:			//æ�ȴ�
		{
			MX251608D_BusyWait();
			return 1;
		}
	}
	return 0;	//��������	
}

/****************************************************************************
* ��	�ƣ�u8 MX251608D_Init(u8 prio)
* ��	�ܣ�DataFlash��ʼ��
* ��ڲ�����u8 prio			�洢���������ȼ�
* ���ڲ�����u8		�Ƿ�ɹ�		1�ɹ�	0ʧ��
* ˵	������
****************************************************************************/
u8 MX251608D_Init(u8 prio)
{
	u8 err = 0;
// 	GPIO_InitTypeDef   G;
	
	#if(MX251608DA == 1)
	//ע���ñ���
	DEV_REG mx25a = 									//�豸ע����Ϣ��						����static		
	{
		BLOCK_DATAFLASH,								//�豸ID��
		0,  														//�Ƿ���			0:���ɹ���ʹ��, 1:�ɹ���ʹ��
		0, 															//���ڹ����豸���򿪴���
		1,															//��������
		1,															//���д����
		MX25A_pDisc,										//��������			�Զ���
		20120001,												//�����汾			�Զ���
		(u32*)MX251608D_Open,						//�豸�򿪺���ָ��
		(u32*)MX251608D_Close, 					//�豸�رպ���ָ��
		(u32*)MX251608DRead,						//�������
		(u32*)MX251608DWrite,						//��д����
		(u32*)MX251608D_Ioctl						//���ƺ���
	};
	#endif
	
	#if(MX251608DA == 1)
	SPIx_Init();						//�豸SPI��ʼ��
	
// 	else if(MX25A.FCE_Port == GPIOC)
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);		//��������ʱ��C
	
//   G.GPIO_Pin = FLASH_FCE_PIN;																//CE���ų�ʼ��
//   G.GPIO_Mode = GPIO_Mode_OUT;
//   G.GPIO_OType = GPIO_OType_PP;
//   G.GPIO_Speed = GPIO_Speed_50MHz;
//   G.GPIO_PuPd = GPIO_PuPd_NOPULL;
//   GPIO_Init(FLASH_FCE_PORT, &G);
	
	IoSetMode(FLASH_FCE,IO_MODE_OUTPUT);
	IoPushPull(FLASH_FCE);
	IoSetSpeed(FLASH_FCE,IO_SPEED_50M);
	
	MX251608D_CEH();																		//�ر��豸
	
	if(MX251608D_Mutex == 0)
		MX251608D_Mutex = SysMutexCreate(prio, &err);						//����������  //�����������ȼ��������ȼ����ܱ�ռ��
	if(err != SYS_ERR_NONE)																		//����������ʧ��
		return 0;	
	
	if(DeviceInstall(&mx25a) != HVL_NO_ERR)										//�豸ע��
		return 0;
	#endif
	
	return 1;
}


