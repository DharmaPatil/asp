#include "driver.h"
#include "ccm_mem.h"

typedef struct
{
	u32 					ver;					//�����汾			�Զ���
	u8* 					pDisc;				//��������			�Զ���
	u8 						canShared;  	//�Ƿ���			0:���ɹ���ʹ��, 1:�ɹ���ʹ��
	u8						OpenMax; 			//�����豸���򿪴���
	u8 						OpenCount; 		//�򿪼���
	u8 						ReadMax; 			//��������
	u8 						ReadCount;		//������
	u8 						WriteMax; 		//���д����
	u8 						WriteCount;		//д����
	u8 						UseFlag;  		//�豸���ñ�־				0��δ����	1������
	u32*					devOpen;    	//�豸�򿪺���ָ��
	u32*			 		devClose;   	//�豸�رպ���ָ��
	u32*		  		devRead;			//�豸������ӳ��ָ��
	u32*			 		devWrite;			//�豸д����ӳ��ָ��
	u32*			 		devIoctl;			//�豸���ƺ���ӳ��ָ��
}DEV_SPACE;										//�豸�ռ���Ϣ

//�豸ע�����Ԫ
//static DEV_SPACE DEV_List[DEV_MAX - 1];			//�豸�ռ�	�и��߽�ֵ
static DEV_SPACE *DEV_List = NULL; 				

//�ж�ע�����Ԫ
static SDEV_IRQ_REGS EXTI_List[16] = 				//�ж�ӳ���		�豸ID���ж��ߺţ��жϺ���ָ��		//�ַ��Ϳ鹲�ô��ж�ע��
{
	{0,0,(pIRQHandle)0},			//0
	{0,0,(pIRQHandle)0},			//1
	{0,0,(pIRQHandle)0},			//2
	{0,0,(pIRQHandle)0},			//3
	{0,0,(pIRQHandle)0},			//4
	{0,0,(pIRQHandle)0},			//5
	{0,0,(pIRQHandle)0},			//6
	{0,0,(pIRQHandle)0},			//7
	{0,0,(pIRQHandle)0},			//8
	{0,0,(pIRQHandle)0},			//9
	{0,0,(pIRQHandle)0},			//10
	{0,0,(pIRQHandle)0},			//11
	{0,0,(pIRQHandle)0},			//12
	{0,0,(pIRQHandle)0},			//13
	{0,0,(pIRQHandle)0},			//14
	{0,0,(pIRQHandle)0}				//15
};								

static Dev_Info QDevInfo = 					//�豸��ѯ��
{
	0,				//�����汾			�Զ���
	(u8*)0,		//����������ַ	�Զ���
	0,  			//�Ƿ���			0:���ɹ���ʹ��, 1:�ɹ���ʹ��
	0, 				//���ڹ����豸���򿪴���
	0, 				//���ڹ����豸Ŀǰ�򿪴���
	0, 				//��������
	0,				//Ŀǰ������
	0, 				//���д����
	0					//Ŀǰд����
};

/****************************************************************************
* ��	�ƣ�HVL_ERR_CODE DeviceInstall(DEV_REG *dev)
* ��	�ܣ��豸ע��
* ��ڲ�����DEV_REG *dev		�豸ע����Ϣ
* ���ڲ�����HVL_ERR_CODE		�Ƿ�ע��ɹ�  �ɹ�HVL_NO_ERR		����ʧ��
* ˵	�������������͵��豸ע��		
****************************************************************************/
HVL_ERR_CODE DeviceInstall(DEV_REG *dev)
{
	if(dev->ID > (DEV_MAX - 1))										//��������豸ID��
		return HVL_INSTALL_FAIL;										//ע��ʧ��
		
	if(dev->ID > CHAR_BLOCK_BOUNDARY)							//���豸��
		dev->ID -= 1;																//��ȥһ���豸ռλ����
	
	if(DEV_List[dev->ID].UseFlag > 0)							//����Ѿ�ע���˸��豸
		return HVL_INSTALL_FAIL;										//ע��ʧ��
	
	DEV_List[dev->ID].canShared = dev->canShared;	//�Ƿ���			0:���ɹ���ʹ��, 1:�ɹ���ʹ��
	DEV_List[dev->ID].OpenMax = dev->OpenMax;			//�����豸���򿪴���
	DEV_List[dev->ID].ReadMax = dev->ReadMax;			//��������
	DEV_List[dev->ID].WriteMax = dev->WriteMax;		//���д����
	DEV_List[dev->ID].pDisc = dev->pDisc;					//��������			�Զ���
	DEV_List[dev->ID].ver = dev->ver;							//�����汾			�Զ���
	DEV_List[dev->ID].devOpen = (u32*)dev->devOpen;			//�򿪺���
	DEV_List[dev->ID].devClose = (u32*)dev->devClose;		//�رպ���
	DEV_List[dev->ID].devRead = (u32*)dev->devRead;			//������
	DEV_List[dev->ID].devWrite = (u32*)dev->devWrite;		//д����
	DEV_List[dev->ID].devIoctl = (u32*)dev->devIoctl;		//���ƺ���
	DEV_List[dev->ID].OpenCount = 0;								//�򿪼���
	DEV_List[dev->ID].ReadCount = 0;								//������
	DEV_List[dev->ID].WriteCount = 0;								//д����
	DEV_List[dev->ID].UseFlag = 1;									//���ñ�־			//1�����豸		0û�����豸

	return HVL_NO_ERR;															//ע��ɹ�
}

/****************************************************************************
* ��	�ƣ�HANDLE DeviceOpen(DEV_ID ID,u32 lParam,HVL_ERR_CODE* err)
* ��	�ܣ��豸��
* ��ڲ�����DEV_ID ID						�豸ID��
						u32 lParam					����
						HVL_ERR_CODE* err		��������
* ���ڲ�����HANDLE							�豸���
* ˵	��������Ƿǹ����豸���й�һ�δ򿪺�Ͳ����ٴ���	
****************************************************************************/
HANDLE DeviceOpen(DEV_ID ID,u32 lParam,HVL_ERR_CODE* err)
{
	if(ID > (DEV_MAX - 1))												//��������豸ID��
		return HVL_OPEN_DEV_FAIL;										//��ʧ��
	
	if(ID > CHAR_BLOCK_BOUNDARY)									//���豸��
		ID -= 1;																		//��ȥһ���豸ռλ����
	
	if(DEV_List[ID].UseFlag > 0)									//ע���˸��豸
	{
		if(DEV_List[ID].OpenCount > 0)							//�Ѿ��򿪹��豸
		{
			if((DEV_List[ID].canShared == 0) || (DEV_List[ID].OpenCount == DEV_List[ID].OpenMax))			//�ǹ�������򿪴���
			{
				*err = HVL_OPEN_DEV_FAIL;							//�Ѵ򿪷ǹ����豸���ش���	
				return HANDLENULL;										//���ؿվ��
			}
			
			DEV_List[ID].OpenCount++;								//�豸�򿪴�������
			*err = HVL_NO_ERR;											//û�д���
			return (HANDLE)ID;											//�����û����
		}

		if(((pDevOpen)(DEV_List[ID].devOpen))(lParam) == 0)			//�����豸�Ĵ򿪺���		�豸Open����Ϊ0��ʾ�ɹ�
		{
			DEV_List[ID].OpenCount++;								//�豸�򿪴�������
			*err = HVL_NO_ERR;											//û�д���
			return (HANDLE)ID;											//�����豸���
		}
	}
	*err = HVL_OPEN_DEV_FAIL;										//�豸��ʧ��
	return HANDLENULL;													//���ؿվ��
}

/****************************************************************************
* ��	�ƣ�HVL_ERR_CODE DeviceClose(HANDLE IDH)
* ��	�ܣ��豸�ر�
* ��ڲ�����HANDLE IDH					�豸���
* ���ڲ�����HVL_ERR_CODE* err		��������
* ˵	������			
****************************************************************************/
HVL_ERR_CODE DeviceClose(HANDLE IDH)
{
	DEV_ID ID = (DEV_ID)IDH;
	
	if(ID > (DEV_MAX - 1))												//��������豸ID��
		return HVL_CLOSE_DEV_FAIL;									//�ر�ʧ��
	
	if(DEV_List[ID].UseFlag > 0)									//ע���˸��豸
	{
		if(DEV_List[ID].OpenCount > 0)							//����Ƿ��Ѿ���
		{
			DEV_List[ID].OpenCount--;									//�򿪴�������
			
			if(DEV_List[ID].OpenCount == 0)						//û�д򿪻�ر�������һ�δ�
			{
				if(((pDevClose)(DEV_List[ID].devClose))() > 0)				//�����豸�Ĺرպ���		�豸Close����Ϊ0��ʶ�ɹ�
					return HVL_CLOSE_DEV_FAIL;						//�豸�ر�ʧ��
			}
			return HVL_NO_ERR;												//�رճɹ�
		}
	}
	return HVL_CLOSE_DEV_FAIL;										//�豸�ر�ʧ��
}

/****************************************************************************
* ��	�ƣ�s32 CharDeviceRead(HANDLE IDH,u8* Buffer,u32 len,u32* Reallen,u32 lParam)
* ��	�ܣ��ַ��豸��
* ��ڲ�����HANDLE IDH					�豸���
						u8* Buffer					��ȡ��Ż�����
						u32 len							ϣ����ȡ�����ݸ���
						u32* Reallen				ʵ�ʶ�ȡ�����ݸ���
						u32 lParam					����
* ���ڲ�����s32									�Ƿ�ɹ�		1�ɹ�	0ʧ��
* ˵	������			
****************************************************************************/
s32 CharDeviceRead(HANDLE IDH,u8* Buffer,u32 len)
{
	s32 ReturnVal = 0;
	
	DEV_ID ID = (DEV_ID)IDH;
	
	if(ID > (DEV_MAX - 1))												//��������豸ID��
		return 0;																		//READʧ��
	
	if(ID > CHAR_BLOCK_BOUNDARY)									//���豸��ID
		return 0;																		//READʧ��
	
	if(DEV_List[ID].UseFlag > 0)									//ע���˸��豸
	{
		if(DEV_List[ID].OpenCount > 0)							//�豸�򿪹�
		{
			if(DEV_List[ID].ReadCount >= DEV_List[ID].ReadMax)
				return 0;																//������ʧ��
			
			DEV_List[ID].ReadCount++;									//��������������

			ReturnVal = ((pCDevRead)(DEV_List[ID].devRead))(Buffer,len);		//�豸������
			
			DEV_List[ID].ReadCount--;									//��������������
			
			return ReturnVal;													//�������
		}
	}
	return 0;
}

/****************************************************************************
* ��	�ƣ�s32 CharDeviceWrite(HANDLE IDH,u8* Buffer,u32 len,u32* Reallen,u32 lParam)
* ��	�ܣ��ַ��豸д
* ��ڲ�����HANDLE IDH			�豸���
						u8* Buffer			�豸д����Դ������
						u32 len					ϣ��д��ĳ���
						u32* Reallen		ʵ��д������ݸ���
						u32 lParam			����
* ���ڲ�����s32 						ʵ��д������ݸ���
* ˵	������			
****************************************************************************/
s32 CharDeviceWrite(HANDLE IDH,u8* Buffer,u32 len)
{
	s32 ReturnVal = 0;
	
	DEV_ID ID = (DEV_ID)IDH;
	
	if(ID > (DEV_MAX - 1))												//��������豸ID��
		return 0;																		//READʧ��
	
	if(ID > CHAR_BLOCK_BOUNDARY)									//���豸��ID
		return 0;																		//READʧ��
	
	if(DEV_List[ID].UseFlag > 0)									//ע���˸��豸
	{
		if(DEV_List[ID].OpenCount > 0)							//�豸�򿪹�
		{
			if(DEV_List[ID].WriteCount >= DEV_List[ID].WriteMax)
				return 0;																//д����ʧ��
			
			DEV_List[ID].WriteCount++;								//д������������

			ReturnVal = ((pCDevWrite)(DEV_List[ID].devWrite))(Buffer,len);		//�豸д����
			
			DEV_List[ID].WriteCount--;								//д������������
			
			return ReturnVal;													//�������
		}
	}
	return 0;
}

/****************************************************************************
* ��	�ƣ�s32 BlockDeviceRead(HANDLE IDH,u32 offset,u8* Buffer,u32 len,u32* Reallen,u32 lParam)
* ��	�ܣ����豸��
* ��ڲ�����HANDLE IDH					�豸���
						u32 offset					��ʼ��ַ
						u8* Buffer					��ȡ��Ż�����
						u32 len							ϣ����ȡ�����ݸ���
						u32* Reallen				ʵ�ʶ�ȡ�����ݸ���
						u32 lParam					����
* ���ڲ�����s32									ʵ�ʶ�ȡ�����ݸ���
* ˵	������			
****************************************************************************/
s32 BlockDeviceRead(HANDLE IDH,u32 offset,u8* Buffer,u32 len)
{
	s32 ReturnVal = 0;
	
	DEV_ID ID = (DEV_ID)IDH;
	
	if(ID > (DEV_MAX - 1))												//��������豸ID��
		return 0;																		//READʧ��
	
	if(ID <= (CHAR_BLOCK_BOUNDARY - 1))						//�ַ��豸��ID
		return 0;																		//READʧ��
	
	if(DEV_List[ID].UseFlag > 0)									//ע���˸��豸
	{
		if(DEV_List[ID].OpenCount > 0)							//�豸�򿪹�
		{
			if(DEV_List[ID].ReadCount >= DEV_List[ID].ReadMax)
				return 0;																//������ʧ��
			
			DEV_List[ID].ReadCount++;									//��������������

			ReturnVal = ((pBDevRead)(DEV_List[ID].devRead))(offset,Buffer,len);		//�豸������
			
			DEV_List[ID].ReadCount--;									//��������������
			
			return ReturnVal;													//�������
		}
	}
	return 0;
}

/****************************************************************************
* ��	�ƣ�s32 BlockDeviceWrite(HANDLE IDH,u32 offset,u8* Buffer,u32 len,u32* Reallen,u32 lParam)
* ��	�ܣ����豸д
* ��ڲ����HANDLE IDH						�豸���
						u32 offset					��ʼ��ַ
						u8* Buffer					�豸д����Դ������
						u32 len							ϣ��д������ݳ���
						u32* Reallen				ʵ��д������ݳ���
						u32 lParam					����
* ���ڲ�����s32									ʵ��д������ݳ���
* ˵	������			
****************************************************************************/
s32 BlockDeviceWrite(HANDLE IDH,u32 offset,u8* Buffer,u32 len)
{
	s32 ReturnVal = 0;
	
	DEV_ID ID = (DEV_ID)IDH;
	
	if(ID > (DEV_MAX - 1))												//��������豸ID��
		return 0;																		//READʧ��
	
	if(ID <= (CHAR_BLOCK_BOUNDARY - 1))						//�ַ��豸��ID
		return 0;																		//READʧ��
	
	if(DEV_List[ID].UseFlag > 0)									//ע���˸��豸
	{
		if(DEV_List[ID].OpenCount > 0)							//�豸�򿪹�
		{
			if(DEV_List[ID].WriteCount >= DEV_List[ID].WriteMax)
				return 0;																//д����ʧ��
			
			DEV_List[ID].WriteCount++;								//д������������

			ReturnVal = ((pBDevWrite)(DEV_List[ID].devWrite))(offset,Buffer,len);		//�豸д����
			
			DEV_List[ID].WriteCount--;								//д������������
			
			return ReturnVal;													//�������
		}
	}
	return 0;
}

/****************************************************************************
* ��	�ƣ�s32 DeviceIoctl(HANDLE IDH,u32 cmd,u32 lParam)
* ��	�ܣ��豸����
* ��ڲ�����HANDLE IDH					�豸���
						u32 cmd							����
						u32 lParam					����		��������Ĳ�ͬ����ֵ��ͬ
* ���ڲ�����s32									����ֵ	��������Ĳ�ͬ����ֵ��ͬ
* ˵	������			
****************************************************************************/
s32 DeviceIoctl(HANDLE IDH,u32 cmd,u32 lParam)
{
	DEV_ID ID = (DEV_ID)IDH;
	
	if(ID > (DEV_MAX - 1))												//��������豸ID��
		return 0;																		//READʧ��		
	
	if(DEV_List[ID].UseFlag > 0)																	//ע���˸��豸
		return ((pDevIoctl)(DEV_List[ID].devIoctl))(cmd,lParam);		//�豸����
	return 0;
}

/****************************************************************************
* ��	�ƣ�s8 EXTIIRQHandlerInstall(SDEV_IRQ_REGS *irq)
* ��	�ܣ��ж�ע��
* ��ڲ�����SDEV_IRQ_REGS *irq				�ж�ע����Ϣ
* ���ڲ�����s8 �ж�ע���Ƿ�ɹ�		0�ɹ� ��0ʧ��
* ˵	�����ַ��豸�Ϳ��豸�����ж�ע�����		
****************************************************************************/
s8 EXTIIRQHandlerInstall(SDEV_IRQ_REGS *irq)
{
  if(irq->EXTI_No > 15) 				//0-15
		return -1;                  //������Χ
	
	if(EXTI_List[irq->EXTI_No].EXTI_Flag == 1) 	//�Ѵ���
			return -1;          			
	
	EXTI_List[irq->EXTI_No].EXTI_No = irq->EXTI_No; 				//�жϺ�
	EXTI_List[irq->EXTI_No].EXTI_Flag = irq->EXTI_Flag; 		//��־
	EXTI_List[irq->EXTI_No].IRQHandler = irq->IRQHandler; 	//�жϺ���

	return 0;
}

/****************************************************************************
* ��	�ƣ�SDEV_IRQ_REGS* GetExti_list(void)
* ��	�ܣ���ȡ�ж�ӳ�����Ԫ
* ��ڲ�������
* ���ڲ�����SDEV_IRQ_REGS		�ж�ӳ����ַ
* ˵	������			
****************************************************************************/
SDEV_IRQ_REGS* GetExti_list(void)
{
	return EXTI_List;		//�����ж�ӳ���
}

/****************************************************************************
* ��	�ƣ�void DevList_init(void)
* ��	�ܣ��豸����Ԫ�б��ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵	������			
****************************************************************************/
void DevList_init(void)
{
	u8 i;

	DEV_List = (DEV_SPACE *)Ccm_Malloc((DEV_MAX - 1) * sizeof(DEV_SPACE));
	if(DEV_List == NULL) while(1);
	
	for(i = 0;i < (DEV_MAX - 1);i++)							//�и���־λ
	{
		DEV_List[i].ver					= 0;								//�����汾			�Զ���
		DEV_List[i].pDisc				= (u8*)0;						//��������			�Զ���
		DEV_List[i].canShared		= 0;  							//�Ƿ���			0:���ɹ���ʹ��, 1:�ɹ���ʹ��
		DEV_List[i].OpenMax			= 0; 								//�����豸���򿪴���
		DEV_List[i].OpenCount		= 0; 	 							//�򿪼���
		DEV_List[i].ReadMax			= 0; 								//��������
		DEV_List[i].ReadCount		= 0;								//������
		DEV_List[i].WriteMax		= 0; 								//���д����
		DEV_List[i].WriteCount	= 0;								//д��������
		DEV_List[i].UseFlag			= 0;  							//�豸���ñ�־				0��δ����	1������
		DEV_List[i].devOpen			= (u32*)0;  				//���豸�򿪺���ָ��
		DEV_List[i].devClose		= (u32*)0;  				//���豸�رպ���ָ��
		DEV_List[i].devRead			= (u32*)0;					//���豸������ӳ��ָ��
		DEV_List[i].devWrite		= (u32*)0;					//���豸д����ӳ��ָ��
		DEV_List[i].devIoctl		= (u32*)0;					//���豸���ƺ���ӳ��ָ��
	}
}

/****************************************************************************
* ��	�ƣ�Dev_Info* QueryDev_Info(DEV_ID ID)
* ��	�ܣ��鿴�豸��Ϣ
* ��ڲ�����DEV_ID ID								�豸ID��
* ���ڲ�����Dev_Info*								�豸��Ϣ		û���򷵻�0									
* ˵	�������Լ��Ĳ�ѯ���������޸Ĳ���Ӱ�쵽�豸
****************************************************************************/
Dev_Info* QueryDev_Info(DEV_ID ID)
{
	if(ID > (DEV_MAX - 1))														//��������豸ID��
		return (Dev_Info*)0;
	
 	if(ID > CHAR_BLOCK_BOUNDARY)											//�ڿ��豸��
		ID -= 1;																				//��һ��ռλ����
	
	if(DEV_List[ID].UseFlag > 0)											//�����˸��豸
	{
		QDevInfo.ver = DEV_List[ID].ver;								//�豸�汾��
		QDevInfo.pDisc = DEV_List[ID].pDisc;						//�豸����
		QDevInfo.canShared = DEV_List[ID].canShared;		//�豸�Ƿ���	0:���ɹ���ʹ��, 1:�ɹ���ʹ��
		QDevInfo.OpenMax = DEV_List[ID].OpenMax;				//�豸���򿪴���
		QDevInfo.OpenCount = DEV_List[ID].OpenCount;		//�豸�򿪴���
		QDevInfo.ReadMax = DEV_List[ID].ReadMax;				//�豸��������
		QDevInfo.ReadConut = DEV_List[ID].ReadCount;		//ĿǰRead����
		QDevInfo.WriteMax = DEV_List[ID].WriteMax;			//�豸���д����
		QDevInfo.WriteConut = DEV_List[ID].WriteCount;	//ĿǰWrite����
		
		return &QDevInfo;																//���ز�ѯ����Ϣ
	}

	return (Dev_Info*)0;															//û�ҵ��豸ֵ
}
