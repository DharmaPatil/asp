#ifndef RMCP_INCLUDE
#define RMCP_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif



#if defined(__GNUC__) || defined(WIN32)
#define PACK __packed
#else
#define PACK
#endif



#define CIPHER_DIGEST_SIZE 16

#pragma pack(push) //�������״̬
#pragma  pack(1)  

typedef struct{
#ifdef RMCP_SUPPORT_ENCRYPT
    unsigned char   _cipherDigest[CIPHER_DIGEST_SIZE];
#endif
	char 			_protoName[4];
	unsigned char 	_protoVer;
	unsigned int    _totalLength;
	unsigned short  _msgType;
 }RMCPHeader;

#pragma  pack(pop)

#define RMCP_HEAD_LEN (sizeof(RMCPHeader))
#define RMCP_VER 0x20   //V2.0

#define PROTO_HEADER_LEN 42
#define PROTO_HEADER1 'R'
#define PROTO_HEADER2 'M'
#define PROTO_HEADER3 'C'
#define PROTO_HEADER4 'P'

typedef enum{
    CMD_REQ_AUTH=1,
    CMD_RESP_AUTH_OK,
    CMD_RESP_AUTH_FAILD,
	CMD_HEART_REQ=0x11,
	CMD_HEART_RESP,
	CMD_BUSINESS_REQ=0x21,
    CMD_BUSINESS_RESP,

    //rmcp����
	CMD_UPDATE_SERVER_REQUEST = 0x51,
	CMD_UPDATE_3102_REFUSE,
	CMD_UPDATE_3102_ACCEPT,
	CMD_UPDATE_DATA_SEG,
	CMD_UPDATE_3102_RSEG,
	CMD_UPDATE_CANCEL,
	CMD_UPDATE_CANCEL_ACK,
	CMD_UPDATE_3102_NONSUPPORT,
	CMD_UPDATE_DATA_VERIFY,
	CMD_UPDATE_3102_RESULT,
	CMD_UPDATE_DATA_SEG_ACK,
	CMD_UPDATE_DATA_CHECK_ERR,
	CMD_UPDATE_DATA_NUM_ERR,

	CMD_WAVE_DATA = 0xf5,	//��������
	CMD_DEBUG_DATA, 	//������Ϣ
	CMD_PARAM_DATA, 	//3102����
	CMD_APP_UPDATE, 	//���������������
	CMD_SYS_CTRL		//ϵͳ��������
}CmdRMCP;

enum {
	//ϵͳ����	
	CMD_CTRL_RST = 0x10,	
	//��ӡϵͳ��������
	CMD_CTRL_RSTCNT,	
	//���õ�����Ϣ��ӡ�ȼ�
	CMD_CTRL_SET_DLEVLE, 
	//��ӡϵͳ����ʱ��
	CMD_CTRL_SYS_RUNTIME,
	//��̨����
	CMD_CTRL_SCALER_SETZERO,
	//ģ�⳵��
	CMD_CTRL_SIMULATE_CAR,	
	//��ӡ����汾
	CMD_CTRL_APP_VER,
	//��ӡ�����豸״̬
	CMD_CTRL_DEVICE_STATUS,
	//ģ�⵹��
	CMD_CTRL_SIMULATE_BACKCAR,
	//��������
	CMD_CTRL_FOLLOWCAR_MOD,
	//����kϵ������
	CMD_CTRL_SET_SCALERK,
	//�ϳӶ�2��������kϵ������
	CMD_CTRL_SET_AD3K,
	//�Ի����˳�
	CMD_CTRL_EXIT,
};

//����������β����
void Net_Update_Complete(void);

//RMCP����ͷ���
int Build_Rcmp_HeadPack(CmdRMCP msgType, unsigned char* buf, int context_size);

void parseRMCPMessage(RMCPHeader* pHeader, unsigned char* context ,int len);

void convertRCMPHeader(RMCPHeader* head);

int sendRmcpMessage(CmdRMCP msgType, unsigned char* context, int context_size);

unsigned int rmcp_htonl(unsigned int n);
unsigned short rmcp_htons(unsigned short n);

#ifdef __cplusplus
}
#endif



#endif
