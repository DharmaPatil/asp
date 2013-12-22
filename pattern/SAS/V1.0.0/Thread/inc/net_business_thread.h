#ifndef NET_BUSINESS_THREAD
#define NET_BUSINESS_THREAD

//���紫����������
enum {
	Net_Data_Wet = 1,		//��������
	Net_Data_Debug,		//��������
	Net_Data_Parm,			//ϵͳ�������ݰ�
	Net_Update_Cmd,		//������������
	Net_Update_Data,		//�������ݰ�
	Net_Update_Crc,		//crc
	
	Net_App_Restart = 0x75,		//����ϵͳ����
};

void Build_NetData_Pack(char *buf, uint16 len, uint8 type);
BOOL Get_NetUpdate_Flag(void);

//�����ļ�У�鸽����Կ
static u8 NET_Bin_KEY[] = "EMOS_V001_001_001_001";

//�����̼�����flash����
u8 Erase_AppFlash(void);

/****************************************************************************
* ��	�ƣ�_asm void SystemReset(void)
* ��	�ܣ���������
* ��ڲ�������
* ���ڲ�������
* ˵	����
****************************************************************************/
void SystemReset(void);



#endif

