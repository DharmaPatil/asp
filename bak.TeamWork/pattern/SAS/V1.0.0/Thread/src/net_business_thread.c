#include "includes.h"
#include "LWIP_Impl.h"
#include "LWIP_NetAddr.h"
#include "sys_param.h"
#include "task_msg.h"
#include "app_msg.h"
#include "app_rtc.h"
#include "debug_info.h"
#include "sys_param.h"
#include "task_def.h"
#include "net_business_thread.h"

//UPDATE
#include "system_init.h"
#include "eth_init.h"
#include "stm32f4_crc.h"
#include "crc.h"
#include "RMCP.h"

#ifdef RMCP_SUPPORT_ENCRYPT
#define NET_PROTOCOL_MIN_LEN (CIPHER_DIGEST_SIZE+11)
#define NET_HEAD_INDEX       CIPHER_DIGEST_SIZE
#else
#define NET_PROTOCOL_MIN_LEN 11
#define NET_HEAD_INDEX       0
#endif

static const sTASK_PARAM  TASK_PARAM={LwipMutexPrio,LwipPrio1,LwipPrio2};                 /*�����������ȼ�*/
static const sMAC_ADDR 	MAC_Addr={0x2E,0x21,0x22,0x23,0x24,0x5C};	/*MAC��ַ*/
static const sServerADDR 	default_ServerAddr={192,168,50,125,8080};	/*������IP��ַ�Ͷ˿ں�*/
static const Glocal_IP_ADDR default_Local_IP ={					//����IP submask netgate
		192,168,50,127,	
		255,255,255,0,
		192,168,50,1
};

static s32 net_socket = -1;
static volatile u8 status=NET_INIT;	//��������״̬

//Net Update
//static char NetRecvBuf[NETRECVLEN] = {0};
//static int NetRecvKeepLen = 0;

static volatile BOOL NetUpdateFlag = FALSE;

//===========================================================
//���緢��ά��
void Net_Send_Thread(void *arg)
{
	u8 err;
	TaskMsg msg = {0};

	while(1)
	{
		err = Task_QPend(&NetMsgQ, &msg, sizeof(TaskMsg), 0);		//�ȴ������̵߳���Ϣ
		
		//����ʱ�����Ͳ���
 		if(NetUpdateFlag) 
 		{
 			if(msg.msg_id == Msg_NetSend) goto end;
 		}
				
		if((err==SYS_NO_ERR) && (LwIP_GetStatus() == 0))			//��ȷ���յ��źţ�����������
		{
			if(send(net_socket,(void*)msg.msg_ctx,msg.msg_len,0) != msg.msg_len)	//send error
			{
				status=NET_CLOSE;
			}
		}
		else
		{	
			//����δ������/�ȴ���Ϣʱ����
			status=NET_CLOSE;	
		}
		
end:	
		if(msg.msg_id == Msg_NetSend)
			Wave_Mem_Free((void*)msg.msg_ctx);
		else if(msg.msg_id == Msg_NetUpdate)
			NetUpdateMsg_Mem_Free((void *)msg.msg_ctx);
		else			
			;
	}
}

//===========================================================
//����״̬��ά�����������ά��

//����������־
BOOL Get_NetUpdate_Flag(void)
{
	return NetUpdateFlag;
}

/************************************************************
 *   Function: ��������������־λ
 *	val: TRUE ��־��������,�رղ��η��� FALSE ʹ�ܲ��η���
 ************************************************************/
void Set_NetUpdate_Flag(BOOL val)
{
	NetUpdateFlag = val;
}

//=====================================
//RMCP Э��
#define NETRECVLEN		512		//������ջ�������С

static char NetRecvBuf[NETRECVLEN] = {0};
static int  NetRecvKeepLen = 0;


//�����̼�����flash����
u8 Erase_AppFlash(void)
{
	u8 FlashErr = 0, res = 0;

	//�����̼�flash����,�ȹر������ж�
	ETH_NVIC_Disable();
	SysTimeDly(100);
	while((FLASH_APPBakOne_Erase() == 0))
	{
		SysTimeDly(50);
		FlashErr++;
		if(FlashErr > 2)
		{	
			debug(Debug_Error,"erase flash 1 error!!!\r\n");
			res = 7;
			break;
		}
	}
	
	if(res == 0)
	{
		while((FLASH_APPBakTwo_Erase() == 0))
		{
			SysTimeDly(50);
			FlashErr++;
			if(FlashErr > 2)
			{	
				debug(Debug_Error,"erase flash 2 error!!!\r\n");
				res = 7;
				break;
			}
		}
	}
	ETH_NVIC_Enable();
	SysTimeDly(300);
	return res;
}


//��ȡ��ǰ�����������˷�
BOOL Get_NetStatus(void)
{
	return status == NET_CONNECTED;
}

void Set_NetStatus(u8 val)
{
	status = val;
}

static int  parseFrame(void)
{
	int		rlen = 0, i = 0;
    short   dlen;
    RMCPHeader gHeader = {0};

	//���ȱ���
	if(NetRecvKeepLen < 0) NetRecvKeepLen = 0;
	if(NetRecvKeepLen >= NETRECVLEN) NetRecvKeepLen = NETRECVLEN - 1;

	//������������	
	rlen = recv(net_socket, (unsigned char*)&NetRecvBuf[NetRecvKeepLen], NETRECVLEN-NetRecvKeepLen, 0);
	//���ִ���
	if(rlen <= 0) return rlen;
	 
	NetRecvKeepLen += rlen;

	//�ְ�
	while(NetRecvKeepLen >= NET_PROTOCOL_MIN_LEN) 
	{
        //char findHead = 0;
		//���ұ���ͷ
        for(i = NET_HEAD_INDEX; i < NetRecvKeepLen; i++)
		{
            if((NetRecvBuf[i] == PROTO_HEADER1)  &&
                (NetRecvBuf[i+1] == PROTO_HEADER2) &&
                (NetRecvBuf[i+2] == PROTO_HEADER3) &&
                (NetRecvBuf[i+3] == PROTO_HEADER4) )
			{
                memcpy(&gHeader,NetRecvBuf+i-NET_HEAD_INDEX,NET_PROTOCOL_MIN_LEN);
                convertRCMPHeader(&gHeader);
				break;
			}
		}

		//��������ͷ֮ǰ������
        if( i > NET_HEAD_INDEX)
		{
			NetRecvKeepLen -= i;
			memmove(NetRecvBuf, &NetRecvBuf[i], NetRecvKeepLen);
		}

        if(NetRecvKeepLen < NET_PROTOCOL_MIN_LEN) break;

        //��ȡ���ݳ���
        dlen = gHeader._totalLength-NET_PROTOCOL_MIN_LEN;

		//���Ȳ���,������������
		if(( NetRecvKeepLen-NET_PROTOCOL_MIN_LEN) < dlen) break;
		//����Խ�籣��
		if(dlen > NETRECVLEN) dlen = NetRecvKeepLen - NET_PROTOCOL_MIN_LEN;


		//��������ͷ
		NetRecvKeepLen -= NET_PROTOCOL_MIN_LEN;
		memmove(NetRecvBuf, &NetRecvBuf[NET_PROTOCOL_MIN_LEN], NetRecvKeepLen);

        parseRMCPMessage(&gHeader, (unsigned char *)NetRecvBuf, dlen);

		//��������������
		NetRecvKeepLen -= dlen;
		memmove(NetRecvBuf, &NetRecvBuf[dlen], NetRecvKeepLen);
	}

	return rlen;
}

		
void Net_Business_Thread(void *arg)
{
	char buff[50] = {0};
	struct sockaddr_in server={0};
	sServerADDR ServerAddr={0};
	Glocal_IP_ADDR Local_IP={0};
	int rlen = 0;
	
	//��ȡ��������������
	if(!Param_Read(ParamNet, &Local_IP, sizeof(Local_IP)))
	{
		debug(Debug_Error,"read local IP address from eeprom error!!!\r\n");
	}
	if(!Param_Read(ParamNet+sizeof(Local_IP), &ServerAddr, sizeof(ServerAddr)))
	{
		debug(Debug_Error,"read server IP address from eeprom error!!!\r\n");
	}
	
	//check the net param
	if(ServerAddr.ipaddr.addr1 == 255 || ServerAddr.ipaddr.addr2 == 255)
	{
		ServerAddr = default_ServerAddr;
		Local_IP = default_Local_IP;
		Param_Write(ParamNet, &Local_IP, sizeof(Local_IP));
		Param_Write(ParamNet+sizeof(Local_IP), &ServerAddr, sizeof(ServerAddr));
		debug(Debug_Warning,"The net param read from FRAM is wrong,use the defult param!\r\n");
	}
	//show ip addr...
	debug(Debug_Notify,"local IP:%d.%d.%d.%d,subnet mask:%d.%d.%d.%d,gateway:%d.%d.%d.%d...\r\nServer IP:%d.%d.%d.%d,port:%d...\r\n",
				Local_IP.ipaddr.addr1,Local_IP.ipaddr.addr2,Local_IP.ipaddr.addr3,Local_IP.ipaddr.addr4,
				Local_IP.SubnetMask.addr1,Local_IP.SubnetMask.addr2,Local_IP.SubnetMask.addr3,Local_IP.SubnetMask.addr4,
				Local_IP.GateWay.addr1,Local_IP.GateWay.addr2,Local_IP.GateWay.addr3,Local_IP.GateWay.addr4,
				ServerAddr.ipaddr.addr1,ServerAddr.ipaddr.addr2,ServerAddr.ipaddr.addr3,ServerAddr.ipaddr.addr4,
				ServerAddr.port);

	
	if(LwIP_Init(Local_IP,MAC_Addr,TASK_PARAM)==0)
	{
		debug(Debug_None,"lwip init error...\r\n");	
		SysTaskSuspend(SYS_PRIO_SELF);
	}
		
	server.sin_family=AF_INET;
	server.sin_len=sizeof(struct sockaddr_in);
	server.sin_port=htons(ServerAddr.port);		//htons((u16)port);
	sprintf(buff,"%d.%d.%d.%d", ServerAddr.ipaddr.addr1,
			ServerAddr.ipaddr.addr2, ServerAddr.ipaddr.addr3, ServerAddr.ipaddr.addr4);
	inet_aton(buff, &(server.sin_addr));
	
	while(1) 
	{
		switch(status){
			
		//��ʼ��������socket
		case NET_INIT:
			net_socket = socket(AF_INET,SOCK_STREAM,0);	//�����׽���
			if(net_socket < 0)
			{
				SysTimeDly(300);
				break;
			}
			status=NET_CONNECTING;
			SysTimeDly(50);
			break;
			
		//���ӷ�����	
		case NET_CONNECTING:
			debug(Debug_Notify,"Net wire line connect status:%d !\r\n",LwIP_GetStatus());
			if(connect(net_socket,(struct sockaddr*)&server,sizeof(server))==-1)
			{
				status=NET_CLOSE;	//���ӳ���
				SysTimeDly(500);
				debug(Debug_Notify,"conncet failed...\r\n");
				break;
			}	
			status=NET_CONNECTED;
			debug(Debug_Notify,"conncet success...\r\n");
			break;
			
		//�������Ϸ�����
		case NET_CONNECTED:
			//rlen = Net_Recv_ParseFrame();
			rlen = parseFrame();
			if(rlen < 0) 
			{
				status = NET_INIT;
				SysTimeDly(100);
				close(net_socket);
			} 
			break;
			
		//�ر�socket
		case NET_CLOSE:			
			status=NET_INIT;
			close(net_socket);
			SysTimeDly(500);
			break;
		default:
			status=NET_CLOSE;
			break;
		}	
	}
}


__asm void SystemReset(void)
{
	MOV R0,#1	//;
	MSR FAULTMASK,	R0	//;���FAULTMASK��ֹһ���жϲ���
	LDR R0,=0xE000ED0C		//;
	LDR	R1,=0x05FA0004		//;
	STR R1,[R0]			//;ϵͳ�����λ
	B .					//;��ѭ��ʹ�������в�������Ĵ���
}

