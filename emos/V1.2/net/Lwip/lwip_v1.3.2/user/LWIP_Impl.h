#ifndef _LWIP_Impl_H_
#define _LWIP_Impl_H_
#ifdef __cplusplus		   		//�����CPP����C���� //��ʼ����
extern "C" {
#endif

#include "lwip/sockets.h"
#include "includes.h"
	
    /*IP��ַ*/
    typedef struct{
        char addr1;
        char addr2;
        char addr3;
        char addr4;
    } sIP_ADDR;

    /*����MAC��ַ*/
    typedef struct{
        char addr1;
        char addr2;
        char addr3;
        char addr4;
        char addr5;
        char addr6;
    } sMAC_ADDR;
    
    /*����IP��ַ*/
    typedef struct{
        sIP_ADDR ipaddr;                                    /*����IP��ַ*/
        sIP_ADDR SubnetMask;                                /*���������ַ*/
        sIP_ADDR GateWay;                                   /*�������ص�ַ*/
    } Glocal_IP_ADDR;

    
    /*������IP��ַ���Ͷ˿ں�*/
    typedef struct{
        sIP_ADDR ipaddr;                                    /*������IP��ַ*/
        int port;                                           /*�������˿�*/
    } sServerADDR;



////////////////////////////////////////////////////////////
    
/*���û�������LWIP�߳����ȼ�*/
typedef struct{
    char nMutexPrio;                                    /*���������ȼ�,�����ȼ����������ȼ���Ӧ�����*/
    char nLwipBPrio;                                    /*LWIPҵ�����߳����ȼ�,�����ȼ�������û����ȼ���һ������*/
    char nLwipRPrio;                                    /*LWIP�����߳����ȼ��������ȼ������LWIPҵ���߳����ȼ���һ������*/
}sTASK_PARAM;



///@brief
///�����ӿڶ���
/****************************************************************************
* ��	�ܣ������豸��ʼ��
* ��ڲ�����Glocal_IP_ADDR:����IP��ַ�Ͷ˿�
          ��sMAC_ADDR����MAC��ַ
          ��sTASK_PARAM LWIP���õ������ȼ� ��������������������ȼ�
* ���ڲ������ɹ�ʱ����1��ʧ��ʱ����0�� 
* ����˵����
****************************************************************************/
u8 LwIP_Init(Glocal_IP_ADDR local_IP,sMAC_ADDR macAddr,sTASK_PARAM taskparam);


/****************************************************************************
* ��	�ܣ�����IP��ַ���޸�
* ��ڲ�����Glocal_IP_ADDR:����IP��ַ�Ͷ˿�
* ���ڲ������ɹ�ʱ����0��ʧ��ʱ����-1�� 
* ����˵����
****************************************************************************/
int LwIP_Set(Glocal_IP_ADDR local_IP);


/****************************************************************************
* ��	�ܣ�����Ӳ����ʼ��
* ��ڲ�����domain:˵����������������ڵ��������õ�ͨѶЭ��(AF_UNIX��AF_INET��)��
          ��type���õ�ͨѶЭ��(SOCK_STREAM,SOCK_DGRAM��) 
          �� SOCK_STREAM���������õ���TCP Э��,�������ṩ��˳���,
          ���ɿ�,˫��,�������ӵı�������SOCK_DGRAM ���������õ���UDPЭ��,
          ������ֻ���ṩ������,���ɿ�,�����ӵ�ͨ�š�
          ��protocol:��������ָ����type,��������ط�����һ��ֻҪ��0������Ϳ����ˡ�
* ���ڲ������ɹ�ʱ�����ļ���������ʧ��ʱ����-1�� 
* ����˵����Int socket(int domain, int type, int protocol)
****************************************************************************/
#define socket(a,b,c)         lwip_socket(a,b,c)



/****************************************************************************
* ����˵�������ӷ����� 
* ��ڲ�����sockfd:socket���ص��ļ���������
          ��serv_addr�����˷������˵�������Ϣ������sin_add�Ƿ���˵ĵ�ַ��
          ��addrlen serv_addr�ĳ��ȡ�
* ���ڲ�����connect�����ǿͻ�������ͬ��������ӵ�.�ɹ�ʱ����0��ʧ��ʱ����-1��
* ����˵����int connect(int sockfd, struct sockaddr * serv_addr,int addrlen) 
****************************************************************************/
#define connect(a,b,c)        lwip_connect(a,b,c)



/****************************************************************************
* ����˵�������ݽ��� 
* ��ڲ�����sockfd:socket���ص��ļ���������
          ��buf �Ǵ�Ž������ݵĻ�������
          ��len�ǻ���ĳ��ȡ�
          ��FlagsҲ����Ϊ0������Ϊ1��ʾ��ʱ10MS��100��ʾ��ʱ1S��
* ���ڲ���������ʵ���Ͻ��յ��ֽ����������ִ���ʱ������-1��
* ����˵���� int recv(int sockfd,void *buf,int len,unsigned int flags);
****************************************************************************/
#define recv(a,b,c,d)         lwip_recv(a,b,c,d)



/****************************************************************************
* ����˵���� 
* ��ڲ�����sockfd:socket���ص��ļ���������
          ��buf�Ǹ�ָ��Ҫ�������ݵ�ָ�롣
          ��len�����ֽ�Ϊ��λ�����ݵĳ��ȡ�
          ��flagsһ���������Ϊ0��
* ���ڲ�������������ʵ���Ϸ��ͳ����ֽ��������ܻ�������ϣ�����͵����ݣ�
*           �ڳ�����Ӧ�ý�send()�ķ���ֵ�������͵��ֽ������бȽϡ�
* ����˵����int send(int sockfd, const void *buf, int len, int flags) 
****************************************************************************/
#define send(a,b,c,d)         lwip_send(a,b,c,d)



/****************************************************************************
* ����˵���� 
* ��ڲ�����sockfd���ص��ļ���������
* ���ڲ����������ɹ�ʱ����0���ڳ��ִ���ʱ����-1��
* ����˵���� 
****************************************************************************/
#define close(s)              lwip_close(s)


/****************************************************************************
* ����˵���� 
* ��ڲ�����sockfd:����socket���÷��ص��ļ���������
          ��my_addr:��һ��ָ��sockaddr��ָ�롣
          ��addrlen:��sockaddr�ṹ�ĳ��ȡ�
* ���ڲ�����bind�����صĶ˿�ͬsocket���ص��ļ�������������һ�𣬳ɹ��Ƿ���0��ʧ��ʱ����-1��
* ����˵���� int bind(int sockfd, struct sockaddr *my_addr, int addrlen)
****************************************************************************/
#define bind(a,b,c)           lwip_bind(a,b,c)


/****************************************************************************
* ����˵���� 
* ��ڲ�����sockfd:����bind���÷��ص��ļ���������
          ��backlog:���������Ŷӵ���󳤶ȡ�
* ���ڲ�����listen������bind���ļ���������Ϊ�����׽��֣��ɹ��Ƿ���0��ʧ��ʱ����-1��
* ����˵���� int listen(int sockfd,int backlog) 
****************************************************************************/
#define listen(a,b)           lwip_listen(a,b)


/****************************************************************************
* ����˵���� 
* ��ڲ�����sockfd�Ǳ�������socket��������
          ��addrͨ���Ǹ�ָ��sockaddr������ָ�룬�ñ���������������������������������Ϣ��
          ��addrlenͨ��Ϊһ��ָ��ֵΪsizeof(struct sockaddr)������ָ�������

* ���ڲ��������ؿͻ����µ�socket�����������׽��֣�ʧ��ʱ����-1��
* ����˵���� int accept(int sockfd, void *addr, int *addrlen)
****************************************************************************/
#define accept(a,b,c)         lwip_accept(a,b,c)


/****************************************************************************
* ����˵���� UDP �������ݽӿ�
* ��ڲ�����sockfd:socket���ص��ļ���������
          ��buf�Ǹ�ָ��Ҫ�������ݵ�ָ�롣
          ��len�����ֽ�Ϊ��λ�����ݵĳ��ȡ�
          ��flagsһ���������Ϊ0��
          ��from�ñ�������Դ����IP��ַ���˿ںš�
          ��fromlen����Ϊsizeof (struct sockaddr)��
* ���ڲ��������ؽ��յ����ֽ����򵱳��ִ���ʱ����-1��
* ����˵����Int recvfrom(int sockfd, void *buf, int len, int flags,   struct sockaddr *from, socklen_t *fromlen)
****************************************************************************/
#define recvfrom(a,b,c,d,e,f) lwip_recvfrom(a,b,c,d,e,f)


/****************************************************************************
* ����˵���� UDP�������ݽӿ�
* ��ڲ�����sockfd:socket���ص��ļ���������
          ��buf�Ǹ�ָ��Ҫ�������ݵ�ָ�롣
          ��len�����ֽ�Ϊ��λ�����ݵĳ��ȡ�
          ��flagsһ���������Ϊ0��
          ��to��ʾĿ�ػ���IP��ַ�Ͷ˿ں���Ϣ��
          ��tolen��������ֵΪsizeof (struct sockaddr)��
* ���ڲ���������ʵ�ʷ��͵������ֽڳ��Ȼ��ڳ��ַ��ʹ���ʱ����-1��
* ����˵����int sendto(int sockfd, const void *buf,int len,unsigned int flags,const struct sockaddr *to, int tolen)
****************************************************************************/
#define sendto(a,b,c,d,e,f)   lwip_sendto(a,b,c,d,e,f)


/****************************************************************************
* ����˵���� 
* ��ڲ�����maxfdp1��ֵ����Ҫ���ĺ�����ߵ��ĵ���������1��
          ��readfds��select()��صĶ����������ϡ�
          ��writefds��select()��ص�д�ĵ�����������
          ��exceptfds��select()��ص��쳣������ĵ�����������
          ��Timeout�����Ǹ�ָ��struct timeval���͵�ָ�룬�ܹ�ʹselect()
          ���ڵȴ�timeout��ʱ���û���ĵ�������׼���ü����ء�
* ���ڲ�������ʱ����0��ʧ��ʱ����-1���������ش���0��
* ����˵����int select(int maxfdp1,fd_set *readfds,fd_set *writefds��fd_set *exceptfds,struct timeval *timeout); 
****************************************************************************/
#define select(a,b,c,d,e)     lwip_select(a,b,c,d,e)

/****************************************************************************
* ����˵������������״̬�ж� 
* ��ڲ������ޡ�
* ���ڲ��������߲��뷵��0���������ط�0ֵ��
* ����˵�����ú���ֻ�ܹ��������ʼ���ɹ�֮����� 
****************************************************************************/
int  LwIP_GetStatus(void);

#ifdef __cplusplus		   		//�����CPP����C���� //��������
}
#endif

#endif

/*
#include "LwipJob.h"
#include "includes.h"
#include "LWIP_Impl.h"
#include "string.h"
#include "usart1_init.h"
#include "usart_cfg.h"
#include "printf_init.h"

typedef enum 
{
	NetInit = 0,
	NetConecting,
	NetConected,
	NetClose
}ENETSTATAS;

#define  NETBUFLEN      0x200
static ENETSTATAS NetSta = NetInit;
static u8   gcNetRecvBuf[NETBUFLEN]="";

//���֧�ֿͻ�������
#define CLIENTNUM 10

//�ͻ���ģ��
void lwipClient(void)
{
	struct sockaddr_in sServer;
	int iSSocket = -1;
	int ret;
	char cbuf[20];
	sServerADDR ServerAddr={192,168,50,110,10000};      //������IP��ַ�Ͷ˿ں�

	printf("Lwip �ͻ��˲��Կ�ʼ\r\n");
	while(1)
	{   
		switch(NetSta) 
		{
			case NetInit:
			{
				iSSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_IP);
				if(iSSocket >= 0) 
				{
					NetSta = NetConecting;
					printf("socket �����ɹ�\r\n");
					SysTimeDly(50);
				} 
				else 
				{
					printf("socket ����ʧ��\r\n");
					SysTimeDly(400);
				}
				break;
			}
						
			case NetConecting:
			{
				sServer.sin_len = sizeof(struct sockaddr_in);
				sServer.sin_family = AF_INET;
				sServer.sin_port =  htons(ServerAddr.port);
				sprintf(cbuf,"%d.%d.%d.%d", ServerAddr.ipaddr.addr1, ServerAddr.ipaddr.addr2, ServerAddr.ipaddr.addr3, ServerAddr.ipaddr.addr4);
				printf("��ʼ���ӷ���������ip��ַ�ǣ�%s\r\n",cbuf);
				inet_aton(cbuf, &(sServer.sin_addr));
				ret = connect(iSSocket,(struct sockaddr*)(&sServer),sizeof(sServer));		//���ӷ�����
				if (ret < 0) 
				{
					printf("���ӷ���������ip��ַʧ��\r\n");
					NetSta = NetInit;
					SysTimeDly(10);
					close(iSSocket);
					SysTimeDly(400);
				} 
				else 
				{
					printf("���ӷ���������ip��ַ�ɹ�\r\n");
					SysTimeDly(10);
					NetSta = NetConected;
				}
			}
			case NetConected:
			{
                #if 0
				memset(gcNetRecvBuf, 0, NETBUFLEN);
				ret = recv(iSSocket,gcNetRecvBuf,NETBUFLEN,100);
				if(ret < 0) 
				{
					if(NetSta == NetConected) 
					{
						NetSta = NetInit;
						SysTimeDly(100);
						close(iSSocket);
					}
					SysTimeDly(200);
				}
				else if(0 == ret)
				{
					printf("%d ���ճ�ʱ\r\n",SysTimeGet());
				}
				else 
				{
					send(iSSocket,gcNetRecvBuf,ret,0);		//��������
				}
                #else
                memset(gcNetRecvBuf, 'A', NETBUFLEN);
                if(send(iSSocket,gcNetRecvBuf,NETBUFLEN,0) <0 )
                {
                    NetSta = NetClose;
                }
                #endif
                SysTimeDly(1);
				break;
			}
			case NetClose:
			{
				NetSta = NetInit;
                close(iSSocket);
				SysTimeDly(100);
				break;
			}
			default:
			{

				NetSta = NetInit;
				SysTimeDly(100);
                close(iSSocket);
				break;
			}
		}
	}
}

//������ģ��
void lwipServer(void)
{
  //�����ӿͻ��˵��ļ�����������
  static int connect_host[CLIENTNUM];
  static int connect_number =  0;
  int sock, newconn, size,revLen,i;
	//int maxfd = -1;                                         //��������ļ�������
	//fd_set scanfd;	                                        //��������������
	//struct   timeval   timeout; 	                        //��ʱ
  struct sockaddr_in address, remotehost;

    
	//timeout.tv_sec     =   0;                               //����1���ʱ����    
	//timeout.tv_usec   =   100;    
	memset(connect_host, -1, (sizeof(int)*CLIENTNUM));


	printf("%d create a TCP socket\r\n",SysTimeGet());
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{
			printf("can not create socket");
			return;
	}

	// bind to port 9999 at any interface
	address.sin_family = AF_INET;
	address.sin_port = htons(9999);
	address.sin_addr.s_addr = INADDR_ANY;
	printf("%d bind to port 9999 at any interface\r\n",SysTimeGet());
	if (bind(sock, (struct sockaddr *)&address, sizeof (address)) < 0)
	{
			printf("can not bind socket");
			return;
	}

	//listen for incoming connections (TCP listen backlog = 5)
	printf("%d listen for incoming connections (TCP listen backlog = 5)\r\n",SysTimeGet());
	listen(sock, 5);

	size = sizeof(remotehost);

	while (1) 
	{
		printf("%d waitting accept a new\r\n",SysTimeGet());
		newconn = accept(sock, (struct sockaddr *)&remotehost, (socklen_t *)&size);

		printf("a client connect, from:%s port = %d\n",inet_ntoa(remotehost.sin_addr),remotehost.sin_port);
		while(1)                                            //������ѯ�ȴ��ͻ�������
		{
			for(i=0;i<CLIENTNUM;i++)                        //���Һ���λ�ã����ͻ��˵��ļ�����������				
			{
				if(connect_host[i] == -1)                   //�ҵ�
				{
					connect_host[i]= newconn;               //����
					connect_number ++;                      //�ͻ��˼�������1
					break;						
				}	
			}
					
			#if 0
			maxfd = -1;                                     //����ļ�������ֵ��ʼ��Ϊ-1		
			FD_ZERO(&scanfd);                               //�����ļ�����������
			for(i=0;i<CLIENTNUM;i++)                        //���ļ����������뼯��
			{
				if(connect_host[i] != -1)                   //�Ϸ����ļ�������
				{
					FD_SET(connect_host[i], &scanfd);       //���뼯��
					if(maxfd < 	connect_host[i])            //��������ļ�������ֵ
					{
						maxfd = connect_host[i];
					}
				}
			}
				
			err = select(maxfd + 1, &scanfd, NULL, NULL, &timeout) ;//select�ȴ�		
			switch(err)
			{
				case 0:                                     //��ʱ
									printf(" �������ݳ�ʱ\r\n");
					break;
				case -1:                                    //������
					break;
				default:                                    //�пɶ��׽����ļ�������
				if(connect_number<=0)
				{
					break;
				}
				for(i = 0;i<CLIENTNUM;i++)
				{
					
					if(connect_host[i] != -1)               //���Ҽ�����ļ�������
					if(FD_ISSET(connect_host[i],&scanfd))   
					{  
						memset(gcNetRecvBuf,0,NETBUFLEN);
						revLen = recv(connect_host[i], gcNetRecvBuf, NETBUFLEN,0); 
						if(revLen >0)
						{
							printf("%s",gcNetRecvBuf);
							send(connect_host[i], gcNetRecvBuf, revLen,0);
						}
						else
						{
							close(connect_host[0]);
							break;
						}
					} 
				}
				break; 	
			}	
			#else
			revLen = recv(connect_host[0], gcNetRecvBuf, NETBUFLEN,0); 
			if(revLen >0)
			{
				//printf("%s",gcNetRecvBuf);
				send(connect_host[0], gcNetRecvBuf, revLen,0);
			}
			else
			{
				close(connect_host[0]);
				break;
			}
			#endif
		}
	}
}

//�����߳�
void LwipJobProcess(void)		//��������߳�
{
  Glocal_IP_ADDR local_IP =		                            //����IP
	{
		192,168,50,111,
		255,255,255,0,
		192,168,50,1
	};
    sMAC_ADDR MAC_ADDR={0x2E,0x21,0x22,0x23,0x24,0x5C};     //MAC��ַ
    sTASK_PARAM sTASK_PARAM={15,17,18};                     //�����������ȼ���ע�⻥�������ȼ�Ҫ�ȴ����̵߳����ȼ���


	USART_STRUCT usart1cfg = 
	{
		57600,																	//������
		USART_WordLength_8b,										//����λ
		USART_StopBits_1,												//ֹͣλ
		USART_Parity_No													//Ч��λ
	};
	if(USART1_Config(&usart1cfg) == 0)				//���ڳ�ʼ��
		while(1);
	Set_PrintfPort(1);						//���ô�ӡ����
	
	printf("%d ��ʼ��LWIP\r\n",SysTimeGet());
	if(LwIP_Init(local_IP,MAC_ADDR,sTASK_PARAM) == 0)
		while(1);
	
	SysTimeDly(10);
	
	printf("�޸�IP��ַ\r\n");
	LwIP_Set(local_IP);
	
	#if 0
	while(1)
	{
		printf("%d ",LwIP_GetStatus());//��ȡ��������״̬0��ʾ�����Ѿ������� -1��ʾ�Ͽ����ú���ֻ���������ʼ����ɺ����
		SysTimeDly(10);
	}
	#endif

	#if 1
	lwipClient();
	#else
	lwipServer();
	#endif
	
	while(1)
	{
		SysTimeDly(1);
	}
}
*/
