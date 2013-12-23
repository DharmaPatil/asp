#include "ymodem_init.h"
#include "static_init.h"	//ASCII ת long		�̼���ŵ�ַ
#include "String.h"				//memset
#include "Flash_init.h"		//FLASH_DataSpace_Erase
#include "crc_init.h"			//CRC32H_Chack8
#include "ConnectCfg.h"		//ͨѶ�˿�		//ComPort��				//��˵���ʾΪͬһ�˿�
#include "printf_init.h"	//DebugPf

//ymodemЭ���������
#define YmodemUSART_SendByte  			ComSendByte								//YmodemЭ�鷢�ʹ���
#define YmodemUSART_GetByte_Wait  	ComGetByte_Wait						//YmodemЭ����մ��� �ȴ�
#define YmodemUSART_RX_Buffer_Clear	BootLoaderUsart_RX_Clear	//���YmodemЭ����ջ�����

#define YmodemGetFileWait		60					//�ȴ������ļ�	��

/****************************************************************************
* YmodemЭ��˵��
���ʹ�ò�����57600  //115200̫�죬��������

�ɽ��շ���������ͨѶ
���շ�					���ͷ�
						0-C
C->						
SHO-0
						<-SHO
						<-���00 ����FF �ļ���+�ļ�����128 Ч��1 Ч��2
						0-ACK
ACK->
						0-C
C->
SOH/STX/EOT-0
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						1:<-SOH/STX ���1 ����FE 128/1024���� Ч��1 Ч��2		//SOH=128 STX=1024 ���ݲ������0x1A��
						0-ACK
ACK->					
						�ظ� 1 �������ݰ� �����ۼ���0x00��λ��0xFF�����ۼ�
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						2:<-EOT		����
						0-ACK
ACK->
						0-C
C->
						<-���00 ����FF 128��0x00 Ч��0 Ч��0	ȫ0���ݰ�
						0-ACK
ACK->
****************************************************************************/

#define MODEM_SOH 		0x01 		//128�ֽڿ�ʼ
#define MODEM_STX 		0x02		//1028�ֽڿ�ʼ
#define MODEM_EOT 		0x04		//�ļ��������
#define MODEM_ACK 		0x06		//ȷ��Ӧ��
#define MODEM_NAK 		0x15		//���ִ���
#define MODEM_CAN 		0x18 		//ȡ������
#define MODEM_C 			0x43 		//��д��ĸC
#define FileMessageCode	0xFF	//�ļ���Ϣ����
#define FileDataCode	0xFE		//�ļ����ݲ���
#define FileDataSUB		0x1A		//�ļ����ݲ�������

#define YMODEM_RX_CRC(ch, crc) ((XYMODEM_CRC_TAB[((crc >> 8) & 0xff)] ^ (crc << 8) ^ ch) & 0xffff)		//����CRCЧ��
#define YMODEM_TX_CRC(ch, crc) (XYMODEM_CRC_TAB[((crc >> 8) ^ ch) & 0xff] ^ (crc << 8))								//����CRCЧ��

#define WaitDataTime 5600000					//�ȴ�����ʱ��		500MS ��Ӧ9600 - 115200�����ʷ�Χ

const u16 XYMODEM_CRC_TAB[256] =	   //YMODEMЧ�����ݱ�
{
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
    0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
    0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
    0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
    0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
    0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
    0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};

/****************************************************************************
* ��	�ƣ�u16 Ymodem_CRC16(u8* data, const u32 len)
* ��	�ܣ�����YMODEM����
* ��ڲ�����u8* data	����Ч�������
			const u32 len		����Ч������ݸ���
* ���ڲ�����u16 CRC	Ч��ֵ
* ˵	�������շ�ֻ�����ݽ���TXЧ��
****************************************************************************/ 
u16 Ymodem_CRC16(u8* data, const u32 len)
{
	u16 crc = 0x0000;
	u32 i;
	for (i = 0; i < len; i++)
		crc = YMODEM_TX_CRC(data[i], crc);
	return crc;
}

/****************************************************************************
* ��	�ƣ�u8 Ymodem_FileProperty(u8* num_temp,u8* Data_temp,u8* chack_temp,const u8 code_temp)
* ��	�ܣ�����YMODEM����
* ��ڲ�����u8* num_temp	���ݰ����
			u8* Data_temp	���յ������� 128/1024
			u8* chack_temp	2λЧ����
			const u8 code_temp	�ۼ������Ĳ���
* ���ڲ�����u8 	1	�����ļ���Ϣ�ɹ�
				0 	�����ļ���Ϣ����
				2	�û��˳�
* ˵	������
****************************************************************************/
u8 Ymodem_FileProperty(u8* num_temp,u8* ymessage_temp,u8* chack_temp,const u8 code_temp)
{
	u8 get_temp = 0x00;					//���յ�������
	u8 flag = 0;							//���ձ�־
	u8 i = 0;
	
	flag = YmodemUSART_GetByte_Wait(&get_temp,WaitDataTime);	//�������ݣ��ӳ�10ms
	if(flag == 0)
		return 0;				//δ���յ�����

	if(get_temp == 'B' || get_temp == 'b')		//���յ�������Ϊ�û��˳�����
		return 2;				//�������ݴ���

	if(get_temp != MODEM_SOH)		//���յ������ݲ���MODEM_SOH
		return 0;				//�������ݴ���
	
	flag = YmodemUSART_GetByte_Wait(&get_temp,WaitDataTime);		//���
	if(flag == 0)
		return 0;
	*num_temp = get_temp;	

	flag = YmodemUSART_GetByte_Wait(&get_temp,WaitDataTime);	   //����
	if(flag == 0)
		return 0;		  
	if(get_temp != 0xFF)		//0xFF
		return 0;

	while(1)				//128������
	{
		flag = YmodemUSART_GetByte_Wait(&get_temp,WaitDataTime);		//128������
		if(flag == 0)
			return 0;
		ymessage_temp[i] = get_temp;	 
		
		i++;					//�����ۼ�
		if(i >= 128)		
			break;
	}

	flag = YmodemUSART_GetByte_Wait(&get_temp,WaitDataTime);		  //Ч��ֵ1
	if(flag == 0)
		return 0;
	chack_temp[0] = get_temp;	  

	flag = YmodemUSART_GetByte_Wait(&get_temp,WaitDataTime);		  //Ч��ֵ2
	if(flag == 0)
		return 0;

	chack_temp[1] = get_temp;	  

	return 1;				//�������
}

/****************************************************************************
* ��	�ƣ�u8 Ymodem_Com(const u32 S_Time,u8* num_temp,u8* Data_temp,u8* chack_temp,const u8 code_temp)
* ��	�ܣ�����YMODEM����
* ��ڲ�����const u32 S_Time	 �� ���������ȴ����ͷ��������ݵ�ʱ��
			u8* num_temp	���ݰ����
			u8* Data_temp	���յ������� 128/1024
			u8* chack_temp	2λЧ����
			const u8 code_temp	�ۼ������Ĳ���
* ���ڲ�����u8 	1	ͨѶ��ʼ
				0 	ͨѶʧ��
				2	�û��˳�����
* ˵	������
****************************************************************************/
u8 Ymodem_Com(const u32 S_Time,u8* num_temp,u8* ymessage_temp,u8* chack_temp,const u8 code_temp)
{
	u8 flag = 0;															//���ձ�־
			
	u8 Time = 0;															//���ӳٵ�һ�λ�ȡʱ�������
	
	Time = (u8)(GetTimeMSecCnt() / 1000);			//��һ�λ�ȡʱ��
	
	while(1)																	//����ȴ�(SOH)״̬	2��ԼΪ1�� N���ڷ��ͷ����ļ�������
	{
		TimDelayMS(1000);												//�ӳ�
		YmodemUSART_SendByte(MODEM_C);					//��һ����д��ĸC��������
		
		flag = Ymodem_FileProperty(num_temp,ymessage_temp,chack_temp,code_temp);	//����YMODEM����
		if(flag == 1)														//�������ݳɹ�
			return 1;															//ͨѶ�����ɹ��������յ��ļ���Ϣ

		if(flag == 2)														//�û��˳�����
			return 2;															//����һ����ʾ

		YmodemUSART_RX_Buffer_Clear();					//û�н��յ�SOH����ս��ջ����� ��Ҫ�Ƕ��ն˵�ȡ�������������ݽ������

		if((u8)(GetTimeMSecCnt() / 1000) >= Time + S_Time)					//��ǰʱ�䳬���趨��ʱ��rtc��ȷ
			return 0;															//��ʱ�˳�	����һ�������ʾ
	}		
}
/****************************************************************************
* ��	�ƣ�u8 Ymodem_FilePropertyCRC(u8* Data_CRC,u8* chack_temp)
* ��	�ܣ��ļ�����CRC����Ч��
* ��ڲ�����u8* Data_CRC	��ҪЧ�������
			u8* chack_temp		��ȡ������CRCЧ��ֵ
* ���ڲ�����u8 	1	Ч����ȷ
				0 	Ч�����
* ˵	������δʹ��Ч�� ������ȷ		
****************************************************************************/ 		
u8 Ymodem_FilePropertyCRC(u8* Data_CRC,u8* chack_temp)
{
	u16 CRC_temp = 0;		//���������Ч��ֵ
	CRC_temp = Ymodem_CRC16(Data_CRC,128);	  //���շ�ֻ�����ݽ���TXЧ��

	if((((u8)(CRC_temp >> 8)) != chack_temp[0]) && (((u8)CRC_temp) != chack_temp[1]))	
		return 0;	//Ч��ʧ��
	else			//��֤�ɹ����ͺ���ͨ���ź�
		return 1;	//Ч��ɹ�
}

/****************************************************************************
* ��	�ƣ�u8 Ymodem_Data(u8* num_temp,u8* Data_temp,u8* chack_temp,const u8 code_temp)
* ��	�ܣ��ļ����ݽ���
* ��ڲ�����u8* num_temp			���ݿ����
			u8* Data_temp			����
			u8* chack_temp			Ч��ֵ
			const u8 code_temp			�ۼ�����	
* ���ڲ�����u8 	0 	����ʧ��
				1	128���ݽ������
				2	1024���ݽ������
				3	�ļ��������
* ˵	����xp�ĳ����ն˷���ʱ���ļ�С��128�ֽڣ�����128���ͣ��ļ�����128�ֽڣ����Զ���1024����
****************************************************************************/
u8 Ymodem_Data(u8* num_temp,u8* Data_temp,u8* chack_temp,const u8 code_temp)
{
	u8 flag = 0;					//���ձ�־
	u8 get_temp = 0;			//���յ�������
	u32 i = 0;						//�ۼ���(���1024)

	flag = YmodemUSART_GetByte_Wait(&get_temp,WaitDataTime);	//�������ݣ��ӳ�10ms
	if(flag == 0)			//���ݽ��ճɹ�,���ҽ��յ�������ΪMODEM_SOH
		return 0;

	switch(get_temp)
	{
		case MODEM_SOH:		//128λ���ݴ���
		{
			flag = YmodemUSART_GetByte_Wait(&get_temp,WaitDataTime);	   //���
			if(flag == 0)			
				return 0;
			*num_temp = get_temp;	

			flag = YmodemUSART_GetByte_Wait(&get_temp,WaitDataTime);	   //����
			if(flag == 0)			
				return 0;
			if(get_temp != code_temp)	//���ݲ������
				return 0;

			i = 0;
			while(1)			//128������
			{
				flag = YmodemUSART_GetByte_Wait(&get_temp,WaitDataTime);	//128������
				if(flag == 0)
					return 0;
				Data_temp[i] = get_temp;	 
				
				i++;					//�����ۼ�
				if(i >= 128)		
					break;
			}

			flag = YmodemUSART_GetByte_Wait(&get_temp,WaitDataTime);	 //Ч��ֵ1
			if(flag == 0)
				return 0;
			chack_temp[0] = get_temp;	  

			flag = YmodemUSART_GetByte_Wait(&get_temp,WaitDataTime);	  //Ч��ֵ2
			if(flag == 0)
				return 0;
			chack_temp[1] = get_temp;	  

			return 1;				//128 �������
		}
		case MODEM_STX:		//1024λ���ݴ���
		{
			flag = YmodemUSART_GetByte_Wait(&get_temp,WaitDataTime);	   //���
			if(flag == 0)			
				return 0;
			*num_temp = get_temp;	

			flag = YmodemUSART_GetByte_Wait(&get_temp,WaitDataTime);	   //����
			if(flag == 0)			
				return 0;
			if(get_temp != code_temp)	//���ݴ���
				return 0;

			i = 0;
			while(1)			//1024������
			{
				flag = YmodemUSART_GetByte_Wait(&get_temp,WaitDataTime);	 //1024������
				if(flag == 0)
					return 0;
				Data_temp[i] = get_temp;	 
				
				i++;					//�����ۼ�
				if(i >= 1024)		
					break;
			}

			flag = YmodemUSART_GetByte_Wait(&get_temp,WaitDataTime);	   //Ч��ֵ1
			if(flag == 0)
				return 0;
			chack_temp[0] = get_temp;	  

			flag = YmodemUSART_GetByte_Wait(&get_temp,WaitDataTime);	   //Ч��ֵ2
			if(flag == 0)
				return 0;
			chack_temp[1] = get_temp;	  

			return 2;				//1024 �������
		} 
		case MODEM_EOT:		//�ļ��������
		{
			YmodemUSART_SendByte(MODEM_ACK);		//������� ����ȷ���ź�
			YmodemUSART_SendByte(MODEM_C);			//�ٷ���һ��C����һ�����ݽ���  (����ȫ0���ݰ�)
			return 3;				//�ļ��������
		}
		default :	//�����򷵻ش���
			return 0; 
	}
}

/****************************************************************************
* ��	�ƣ�u8 Ymodem_FileDataCRC(const u8 Byenumflag,u8* Data_CRC,u8* chack_temp)
* ��	�ܣ��ļ�����CRC����Ч��
* ��ڲ�����const u8 Byenumflag	λ��־ 1=128 2=1024
			u8* Data_CRC	��ҪЧ�������
			u8* chack_temp	��ȡ������CRCЧ��ֵ
* ���ڲ�����u8 	1	Ч����ȷ
				0 	Ч�����
* ˵	������δʹ��Ч�� ������ȷ		
****************************************************************************/
u8 Ymodem_FileDataCRC(const u8 Byenumflag,u8* Data_CRC,u8* chack_temp)
{
	u16 CRC_temp=0;		//���������Ч��ֵ
	
	if(Byenumflag == 1)	//128λ����Ч��
		CRC_temp = Ymodem_CRC16(Data_CRC,128);	  //���շ�ֻ�����ݽ���TXЧ��
	else				//1024λ����Ч��
		CRC_temp = Ymodem_CRC16(Data_CRC,1024);	  //���շ�ֻ�����ݽ���TXЧ��

	if((((u8)(CRC_temp >> 8)) != chack_temp[0]) && (((u8)CRC_temp) != chack_temp[1]))
		return 0;	//Ч��ʧ��
	else			//��֤�ɹ����ͺ���ͨ���ź�
		return 1;	//Ч��ɹ�
}


//�Ե�һ���ļ���Ϣ���ݽ��н������ţ������Ժ���ʾ��
static u8 FileName[128] = {0};	  	//����
static u8 FileNameSize = 0;		  		//���ֳ���
static u32 FileSize = 0;						//ת�����ļ���С
/****************************************************************************
* ��	�ƣ�u8* GetFlieNameDataAddr(void)
* ��	�ܣ���ȡ�ļ�����ŵ�ַ
* ��ڲ�������
* ���ڲ�����u8* �ļ�����ŵ�ַ
* ˵	������
****************************************************************************/
u8* GetFlieNameDataAddr(void)
{
	return FileName;
}

/****************************************************************************
* ��	�ƣ�u8 GetFlieNameLen(void)
* ��	�ܣ���ȡ�ļ�������
* ��ڲ�������
* ���ڲ�����u8 �ļ�������
* ˵	������
****************************************************************************/
u8 GetFlieNameLen(void)
{
	return FileNameSize;
}

/****************************************************************************
* ��	�ƣ�u32 GetFlieSize(void)
* ��	�ܣ���ȡ�ļ���С
* ��ڲ�������
* ���ڲ�����u32 �ļ���С
* ˵	������
****************************************************************************/
u32 GetFlieSize(void)
{
	return FileSize;
}

/****************************************************************************
* ��	�ƣ�u8 Ymodem_FileDecode(u8* Message_temp,u8* Data_temp,u8* FileName_temp,u8* FileNameLen_temp,u32* FileSizeLen)
* ��	�ܣ��ļ����ݽ���
* ��ڲ�����u8* Message_temp	��ȡ���ļ���Ϣԭʼ����	�����Ϊ�ļ���Ϣ
			u8* FileName_temp	�����������ļ���
			u8 FileNameLen_temp	�����������ļ�������
			u32* FileSizeLen	�����������ļ���С����
* ���ڲ�����u8 	1	�����ɹ�
				0 	��������
* ˵	������		
****************************************************************************/
u8 Ymodem_FileDecode(u8* Message_temp,u8* FileName_temp,u8* FileNameLen_temp,u32* FileSizeLen)
{
	u32 i = 0;	//�ۼ���
	u8 FileSize_temp[10] = {0};	//�ļ���С ASCII��	   10λ=1G
	u32 FileSizeLen_temp = 0;		//�ļ���С ASCII�����

	while(1)	//���ļ���
	{
		if(Message_temp[i] == 0x00)	 		//�ļ��������NUL
		{
			*FileNameLen_temp = i;		  	//��ȡ�ļ�������
			break;
		}
		i++;
	}
	memcpy(FileName_temp,Message_temp,*FileNameLen_temp);	 //����ļ���

	i = 1;		//�ۼ�����λ
	while(1)	//���ļ���С
	{
		if(Message_temp[i + *FileNameLen_temp] == 0x00)	 		//�ļ���С�����NUL
		{
			FileSizeLen_temp = i - 1;		  	//��ȡ�ļ���С����
			break;
		}
		i++;
	}
	memcpy(FileSize_temp,Message_temp + *FileNameLen_temp + 1,FileSizeLen_temp);	 //����ļ���С
	*FileSizeLen = ASCIItoNumber(FileSize_temp,FileSizeLen_temp);				 //�ļ���СASCIIת������

	return 1;	//Ч��ɹ�
}

/****************************************************************************
* ��	�ƣ�u8 Ymodem_START(const u8 Option)
* ��	�ܣ�YMODEM ��������		���տռ���С128�ֽ� ���1024
* ��ڲ�����const u8 Option		1/2/3ѡ��
* ���ڲ�����u8 �����Ƿ�ɹ�	0ʧ��	1�ɹ�
* ˵	��������ѡ��1/2/3�Ĳ�ͬ������ĵ�ַҲ��ͬ
****************************************************************************/
static u32 FileCRC = 0;							//�����̼���CRCЧ��	ÿ����ȡʱ���Զ԰����е��Լ��
static s32 ExcessSize = 0;					//����ʣ���ļ�����
static u32 num = 1;									//���������
static u8 FileMessage[128] = {0};		//�ļ���Ϣ�ռ� 	   //С��128�İ�128�ռ�		//�û������ļ������ļ������ȣ��ļ�������
static u8 FileData[1024] = {0};  		//���ݽ��տռ�		128���պ�1024���վ��ô˻�����
static u32 CtrlAddress = 0;					//������ַ	���ݽ����ͬ��ָ��ͬ�ĵ�ַ��(App/Bak/Flag)
u8 Ymodem_START(const u8 Option)
{
	u8 serial_number = 0;							//���
	u8 chack[2] = {0};								//Ч��ֵ
	u8 flag = 0;											//��־
	u8 code = 0xFF;										//���룬ÿ����һ������-1
	u32 newbootflag = UPBOOTFlag;			//���±�־
	u32 FileUpSrc = PCBOOT;						//�ļ���Դ(Ymodem���վ�ΪPCBOOT��Դ)
	
	switch(Option)
	{
		case 1:		
		{
			CtrlAddress = AppStartAddressBak;																								//����Ӧ�ó��򵽱�����
			DebugPf(0,"Ӧ�ó��򱸷ݿռ������...\r\n");
			if(FLASH_DataSpace_Erase(AppStartAddressBak,AppBakSpaceSize) == 0) 					//����������
			{
				DebugPf(0,"Ӧ�ó��򱸷ݿռ����ʧ��!\r\n");
				return 0;
			}
			else
				DebugPf(0,"Ӧ�ó��򱸷ݿռ�������!\r\n");
			break;
		}
		case 2:
		{
			CtrlAddress = GBFontStartAddress;																								//����GB�ֿ⵽GB�ֿ���
			DebugPf(0,"GB�ֿ�ռ������...\r\n");
			if(FLASH_DataSpace_Erase(GBFontStartAddress,GBFontSpaceSize) == 0) 					//����GB�ֿ���
			{
				DebugPf(0,"GB�ֿ�ռ����ʧ��!\r\n");
				return 0;
			}
			else
				DebugPf(0,"GB�ֿ�ռ�������!\r\n");
			break;
		}
		case 3:
		{
			CtrlAddress = ASCIIFontStartAddress;																						//����ASCII�ֿ⵽ASCII�ֿ���
			DebugPf(0,"ASCII�ֿ�ռ������...\r\n");
			if(FLASH_DataSpace_Erase(ASCIIFontStartAddress,ASCIIFontSpaceSize) == 0) 		//����ASCII�ֿ���
			{
				DebugPf(0,"ASCII�ֿ�ռ����ʧ��!\r\n");
				return 0;
			}
			else
				DebugPf(0,"ASCII�ֿ�ռ�������!\r\n");
			break;
		}
		case 4:
		{
			CtrlAddress = AuxiliaryAppStartAddress;																						//���ظ���Ӧ�ó��򵽸���Ӧ�ó�����
			DebugPf(0,"����Ӧ�ó���ռ������...\r\n");
			if(FLASH_DataSpace_Erase(AuxiliaryAppStartAddress,AuxiliaryAppSpaceSize) == 0) 		//��������Ӧ�ó�����
			{
				DebugPf(0,"����Ӧ�ó���ռ����ʧ��!\r\n");
				return 0;
			}
			else
				DebugPf(0,"����Ӧ�ó���������!\r\n");
			break;
		}
		default:
			return 0;																//����ѡ��
	}

	flag = Ymodem_Com((u32)YmodemGetFileWait,&serial_number,FileData,chack,code);	  //����ͨѶ��ͨѶ�ȴ�Ϊ15��	   //�ļ���Ϣ���	//Data_t   ��ͷ��ʼ�������	
	if(flag == 0)		 									//15����δ���յ����ͷ�����Ӧ��ʧ��
	{
		DebugPf(0,"\r\n****************************\r\n");
		DebugPf(0,"�ļ����ճ�ʱ�˳�!\r\n");		   
		DebugPf(0,"****************************\r\n\r\n");
		YmodemUSART_GetByte_Wait(&flag,168000000);	   //��ʾͣ�� Լ15��
		return 0;		//��ʱʧ��
	}

	if(flag == 2)
	{
		DebugPf(0,"\r\n****************************\r\n");
		DebugPf(0,"�˳�Ymodem!\r\n");
		DebugPf(0,"****************************\r\n\r\n");
		YmodemUSART_GetByte_Wait(&flag,14000000);	   //��ʾͣ��
		return 0;		//�û��˳�
	}

	if(Ymodem_FilePropertyCRC(FileData,chack) == 0)		//�ļ���Ϣ����Ч��
	{
		DebugPf(0,"****************************\r\n");
		DebugPf(0,"�ļ���CRCЧ�����!\r\n");
		DebugPf(0,"****************************\r\n\r\n");
		YmodemUSART_GetByte_Wait(&flag,14000000);	   //��ʾͣ��
		return 0;
	}

	memcpy(FileMessage,FileData,128);		//128�ļ���Ϣת��	   0-128
	memset(FileName,0,sizeof(FileName));		//�����ʾ������
	FileNameSize = 0;	//�ļ�����С���
	FileSize = 0;	//�ļ���С���

	flag = Ymodem_FileDecode(FileMessage,FileName,&FileNameSize,&FileSize);	  //�������ݵõ��ļ������ļ�����
	if(flag == 1)	  //�ļ���Ϣ�����ɹ�
	{
		ExcessSize = FileSize; 						//ʣ���ļ����ȳ�ʼ��
	  YmodemUSART_SendByte(MODEM_ACK);	//������� ����ȷ���ź� 
		YmodemUSART_SendByte(MODEM_C);		//����ʼ�źţ���ʽ�������ݴ���
	}

	while(1)
	{
		if(code == 0x00)
			code = 0xFF;		//�����ۼ����λ
		else
			code = code - 1;	//ÿ�ɹ�����һ�����ݲ���-1
		
		flag = Ymodem_Data(&serial_number,FileData,chack,code);		//һ���ļ����ݰ���ȡ
		switch(flag)
		{
			case 1:				//�������ݰ�128������
			{
				if(Ymodem_FileDataCRC(1,FileData,chack) == 0)
					return 0;

				if(ExcessSize >= 128)
				{
					Flash_DataWrite(CtrlAddress + (FileSize - ExcessSize),128,FileData);					//���һ����дһ����	ƫ���ϴ�ʣ���ַ
					FileCRC = CRC32H_Chack8(0,FileData,128);		//��õ��ļ�У��ֵ
				}
				else
				{
					Flash_DataWrite(CtrlAddress + (FileSize - ExcessSize),ExcessSize,FileData);		//���һ����дһ����	ƫ���ϴ�ʣ���ַ
					FileCRC = CRC32H_Chack8(0,FileData,ExcessSize);	//��õ��ļ�У��ֵ
				}
				ExcessSize -= 128;							//����ʣ���С
				YmodemUSART_SendByte(MODEM_ACK);			//������� ����ȷ���ź�
				 
				num = num + 1;		//���ݰ�����
				break;
			}
			case 2:				//�������ݰ�1024������
			{
				if(Ymodem_FileDataCRC(2,FileData,chack)==0)
					return 0;
				
				if(ExcessSize >= 1024)
				{
					Flash_DataWrite(CtrlAddress + (FileSize - ExcessSize),1024,FileData);					//���һ����дһ����	ƫ���ϴ�ʣ���ַ
					FileCRC = CRC32H_Chack8(0,FileData,1024);		//��õ��ļ�У��ֵ
				}
				else
				{
					Flash_DataWrite(CtrlAddress + (FileSize - ExcessSize),ExcessSize,FileData);		//���һ����дһ����	ƫ���ϴ�ʣ���ַ
					FileCRC = CRC32H_Chack8(0,FileData,ExcessSize);	//��õ��ļ�У��ֵ
				}
				ExcessSize -= 1024;							//����ʣ���С
				YmodemUSART_SendByte(MODEM_ACK);			//������� ����ȷ���ź�
				 
				num = num + 1;		//���ݰ�����
				break;
			}
			case 3:				//�����ļ����
			{
				flag = Ymodem_FileProperty(&serial_number,FileData,chack,0xFF);		//����һ�����ļ���Ϣ��ȫ0����
				if(flag == 0)		
					return 0;

				YmodemUSART_SendByte(MODEM_ACK);		//ͨ�Ž���		��
				DebugPf(0,"\r\nFile Receive Achieve!\r\n");	//�ն���ʾ�������
				
				switch(Option)
				{
					case 1:	//������ص��ǹ̼�������̼���Ӧ��־
					{
						Flash_DataWrite(AppCRCAddress,4,(u8*)&FileCRC);							//�̼��ļ�У��ֵ
						Flash_DataWrite(AppSizeAddress,4,(u8*)&FileSize);						//�̼��ļ���С
						Flash_DataWrite(AppUpSrcAddress,4,(u8*)&FileUpSrc);					//�̼��ļ���Դ
						
						if(FLASH_DataSpace_Erase(AppFlagAddress,AppFlagSpaceSize) == 0)
						{
							DebugPf(0,"Ӧ�ó��򱸷ݱ�־�ռ����ʧ��!\r\n");				//Ӧ�ó����־�ռ����ʧ��
							return 0;
						}
						Flash_DataWrite(AppUPAddress,4,(u8*)&newbootflag);					//���±�־
						break;
					}
					case 2://������ص���GB�ֿ⣬����GB�ֿ��Ӧ��־
					{
						Flash_DataWrite(GBFontCRCAddress,4,(u8*)&FileCRC);					//GB�ֿ�У��ֵ
						Flash_DataWrite(GBFontSizeAddress,4,(u8*)&FileSize);				//GB�ֿ��С
						Flash_DataWrite(GBFontUpSrcAddress,4,(u8*)&FileUpSrc);			//GB�ֿ���Դ
						break;
					}
					case 3://������ص���ASCII�ֿ⣬����ASCII�ֿ��Ӧ��־
					{
						Flash_DataWrite(ASCIIFontCRCAddress,4,(u8*)&FileCRC);					//ASCII�ֿ�У��ֵ
						Flash_DataWrite(ASCIIFontSizeAddress,4,(u8*)&FileSize);				//ASCII�ֿ��С
						Flash_DataWrite(ASCIIFontUpSrcAddress,4,(u8*)&FileUpSrc);			//ASCII�ֿ���Դ
						break;
					}
					case 4://������ص���ASCII�ֿ⣬����ASCII�ֿ��Ӧ��־
					{
						Flash_DataWrite(AuxiliaryAppCRCAddress,4,(u8*)&FileCRC);					//��������У��ֵ
						Flash_DataWrite(AuxiliaryAppSizeAddress,4,(u8*)&FileSize);				//���������С
						Flash_DataWrite(AuxiliaryAppUpSrcAddress,4,(u8*)&FileUpSrc);			//����������Դ
						break;
					}
					default:
						break;
				}
				CRC_ResetDR();	  //�������ʱ���CRC
				return 1;
			}
			default:
				return 0;	
		}
	}		 	
}

/*
	//���������;������Ҫԭ����2�֣�
	//1�������ʹ��죬������115200��̫�����57600ʵ��û���ִ��󣬽������ʹ��57600�������ļ�
	//2�����ݴ�ſռ䲻�����½�������ʧ��

	u8 flag = 0;						//״̬��־	
	u8 ydata[4096] = {0};  	//���ݴ�ſռ� 	   //С��128�İ�128�ռ䣬����128�İ�1024�ı����ռ� ����2100��С���ļ���Ҫ��3072�Ŀռ������
	u8 yname[128] = {0};	  //����
	u8 ynamelen = 0;		  	//���ֳ���
	u32 ysizelen = 0;		  	//�ļ���С
	u32 ydatalen = 0;		  	//���ݳ���

	flag = Ymodem_START(ydata);					//�����ļ�
	if(flag == 1)		 //���ճɹ�
	{
		flag = Ymodem_FileDecode(ydata,yname,&ynamelen,&ysizelen,&ydatalen);	  //��������
		if(flag == 1)		 //�����ɹ�
		{
			printf("File Name : ");
			USART1_SendString(yname,ynamelen);		 //�ļ�����
			printf("\r\n");
		
			printf("File Size : %d\r\n",ysizelen);	 //�ļ���С
		
			printf("File Data : ");
			USART1_SendString(ydata+128,ydatalen);	 //�ļ�����	ƫ��128
			printf("\r\n");
		}
	}
*/
