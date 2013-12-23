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

//��������������־λ
void Set_NetUpdate_Flag(BOOL val);

//�����ļ�У�鸽����Կ
static u8 NET_Bin_KEY[] = "EMOS_V001_001_001_001";

/************************************************************
 * Function: �����̼�����flash����
 * ����flashʱ��ر������ж�
 ************************************************************/
u8 Erase_AppFlash(void);


#define NET_INIT		0
#define NET_CONNECTING	1
#define NET_CONNECTED	2
#define NET_CLOSE		3
//��������״̬
BOOL Get_NetStatus(void);
void Set_NetStatus(u8 val);


/****************************************************************************
* ��	�ƣ�_asm void SystemReset(void)
* ��	�ܣ���������
* ��ڲ�������
* ���ڲ�������
* ˵	����
****************************************************************************/
void SystemReset(void);



#endif

