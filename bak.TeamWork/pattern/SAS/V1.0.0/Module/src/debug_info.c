#include <stdarg.h>
#include <ctype.h>
#include "includes.h"
#include "sys_param.h"
#include "sys_config.h"
#include "task_msg.h"
#include "debug_info.h"
#include "stm32f4_crc.h"
#include "overtime_queue.h"
#include "net_business_thread.h"
#include "app.h"

#include "sdk_serial.h"
#include "app_msg.h"
#include "dt3102_io.h"
#include "crc.h"

#include "rmcp.h"
#include "task_def.h"
#include "ccm_mem.h"

//����1
#include "usart1.h"
#include "printf_init.h"
#include "kfifo.h"
#include "task_timer.h"
#include "ymodem_update.h"

//#define DEBUG_BUF_LEN 	 8
#define DEBUG_BUF_SIZE 1024 	//ע��,���ȱ�����2����

static volatile int8 DebugLevel = Debug_None;

static unsigned char *pdebugbuff = NULL; //[DEBUG_BUF_SIZE] = {0};
static struct kfifo debugfifo;

static HANDLE debug_handle;

void Set_Debug_Level(uint8 level,BOOL bSave)
{
	if(level <= Debug_Constant)
	{
		DebugLevel = level;
		if(bSave)
		{
			Param_Write(GET_U8_OFFSET(ParamDebugLevel), &level, sizeof(uint8));
		}
		debug(Debug_Notify, "Set debuglevel\r\n");
	}
}

int8 Get_Debug_Level(void)
{
	return DebugLevel;
}

int debug_get_info(char *buf)
{
	uint32 len;
#if SYS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
		SYS_CPU_SR	cpu_sr = 0;
#endif

	SYS_ENTER_CRITICAL();
	len = kfifo_len(&debugfifo);
	SYS_EXIT_CRITICAL();

	if(len == 0) return 0;	
	if(len > 120) len = 120;

	//Build_NetData_Pack(buf, len+1, Net_Data_Debug);
	Build_Rcmp_HeadPack(CMD_DEBUG_DATA, (unsigned char*)buf, len);

	SYS_ENTER_CRITICAL();
	kfifo_get(&debugfifo, (unsigned char *)(buf+RMCP_HEAD_LEN), len);
	SYS_EXIT_CRITICAL();

	return (len+RMCP_HEAD_LEN);
}

static u8 debug_buff[128] = {0};
void debug(int8 level, const char *fmt, ...)
{
	uint32 len = 0;	
#if SYS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
	SYS_CPU_SR	cpu_sr = 0;
#endif

#if 1	
	if(level <= DebugLevel) {
		va_list ap;

		SYS_ENTER_CRITICAL();		
		va_start(ap,fmt);
		vsnprintf((char *)debug_buff,127,fmt,ap);
		len = strlen((char *)debug_buff);
		kfifo_put(&debugfifo, debug_buff, len);
		
		va_end(ap);
		
		SYS_EXIT_CRITICAL();
	}
#else		
	TaskMsg msg = {0};
	uint8 err;
	
	if(level <= DebugLevel) {
		va_list ap;
	
		va_start(ap,fmt);
		vsnprintf(debug_buff,127,fmt,ap);
		len = strlen(debug_buff);

		Usart1_WriteData( debug_buff, len);
	}
#endif
}

uint8 Debug_Mem_Free(void *pblk)
{
	return 1;
}

//===========================================
//���Դ������, ע��ú������ɶ���̵߳���
int Usart1_WriteData(const void *pbuf, u32 len)
{
	if(pbuf == NULL) return -1;
	
	return CharDeviceWrite(debug_handle, (u8 *)pbuf ,len);
}

int Usart1_ReadData(char *pbuf, u32 len)
{
	if(pbuf == NULL) return -1;

	return CharDeviceRead(debug_handle, (u8 *)pbuf, len);
}

int Usart1_Get_Size(u32 len)
{
	return DeviceIoctl(debug_handle, CMD_GET_INPUT_BUF_SIZE, len);
}

void Usart1_Clear_Buffer(void)
{
	DeviceIoctl(debug_handle, CMD_FLUSH_INPUT, 0);
}

void Usart1_Set_OvertimeTick(uint32 tick)
{
	DeviceIoctl(debug_handle, CMD_SET_RX_TIMEOUT, tick);
}

/**********************************************************************
 * ���� shell ����
 **********************************************************************/
#define MAX_DEBUG_CMD 64
#define MAX_DEBUGBUF_LEN 64
 
struct _DebugCmd {
	char *name;					//cmd name
	DebugFun Debug_Prase;		//�����������
	char *help;					//�������
	BOOL bParam;				//�Ƿ������
};


static struct _DebugCmd DebugCmd[MAX_DEBUG_CMD] = {0};
static uint8 nDebugCmdLen = 0;
static BOOL DebugFlag = FALSE;

static char tmpbuf[24] = {0}; 

//������е�����
static int Do_Help(char *arg, DebugSend send)
{
	static char *prompt = "-----all cmd list-----\r\n";
	int i = 0, j, k;

	if(send == NULL) return 1;

	send(prompt, strlen(prompt));
	
	for(i = 0; i < nDebugCmdLen; i++)
	{
		snprintf(tmpbuf, 5, "%02d.", i+1); 
		send(tmpbuf, strlen(tmpbuf));
		k = strlen(DebugCmd[i].name);
		send( DebugCmd[i].name, k);
		if(k < 16)
		{
			//���ո�, ʹ�����б����
			for(j = 0; j < 16 - k; j++)
				send(" ", 1);
		}
		send( "--", 2);
		send( DebugCmd[i].help, strlen(DebugCmd[i].help));
		send( "\r\n", 2);
		SysTimeDly(5);
	}
	
	return 0;
}

//���õ�����Ϣ��ӡ�ȼ�
static int Do_Set_DLevel(char *arg, DebugSend send)
{
	int level = -1;
	char buf[25] = {0};
	
	if((arg == NULL) || (send == NULL)) return -1;

	level = *arg - 0x30;
	
	snprintf(buf, 24, "OrigL:%d,RevL:%d\r\n", DebugLevel, level);
	send(buf, strlen(buf));
	
	if((level < 0) || (level > Debug_Constant)) return -1;
	
	Set_Debug_Level(level, TRUE);

	return 0;
}

//��ӡϵͳ��������
static int Do_Get_ResetCnt(char *arg, DebugSend send)
{
	if(send == NULL) return 1;
	
	snprintf(tmpbuf, 16, "RstCnt=%d\r\n", Get_AppReset_Count());
	send(tmpbuf, strlen(tmpbuf));
	
	return 0;
}

//��λϵͳ
static int Do_Reset(char *arg, DebugSend send)
{
	SystemReset();

	return 0;
}

//�˳�shell
static int Do_Exit(char *arg, DebugSend send)
{
	DebugFlag = FALSE;

	return 0;
}

//��ȡϵͳ��������ʱ��
static int Do_Get_AppTime(char *arg, DebugSend send)
{
	unsigned long tick = Get_Sys_Tick();
	int hour = 0, sec = 0, min = 0;

	if(send == NULL) return 1;
	
	sec = tick / 1000;		
	min = sec / 60;		
	
	hour = min / 60;
	sec = sec - min * 60;	
	min = min - hour * 60;  

	snprintf(tmpbuf, 23, " %dʱ%02d��%02d��\r\n", hour, min, sec);
	send(tmpbuf, strlen(tmpbuf));
	
	return 0;
}

//=========================================================
//ע�ᴮ�ڽ�������
//shell prompt
static char *prompt = "DT#";
static char *prompt1 = "û��ƥ�������!\r\n";
static char *prompt2 = "�ַ����������~^~!\r\n";
static char *prompt3 = "\r\nWelcome to DT shell!\r\n";
static char *prompt4 = ">>�����Իس�����,֧��CTRL+Cȡ����������\r\n";
static char *prompt5 = "����ִ��ʧ��\r\n";
static char *prompt6 = "����ִ�гɹ�\r\n";
static char *prompt_help = ">>����help���Բ鿴֧�ֵ���������\r\n";

int Register_Debug_Cmd(char *name, DebugFun fun, char *help, BOOL bParam)
{
	if((name == NULL) || (fun == NULL) || (help == NULL))	
	{
		return -1;
	}
	
	if(nDebugCmdLen >= MAX_DEBUG_CMD) return -2;
	
	DebugCmd[nDebugCmdLen].name = name;
	DebugCmd[nDebugCmdLen].Debug_Prase = fun ;
	DebugCmd[nDebugCmdLen].help = help;
	DebugCmd[nDebugCmdLen].bParam = bParam;

	nDebugCmdLen++;

	return 0;
}

static void Do_RmcpCtrlCmd(const char *name, char *buf, DebugSend send)
{
	int i = 0, len = 0;

	if((name == NULL) || (buf == NULL) || (send == NULL)) return;
	
	len = strlen(name);
	for(i = 0; i < nDebugCmdLen; i++)
	{
		//��������
		if(strncmp(name, DebugCmd[i].name, len) == 0)
		{
			if(!DebugCmd[i].bParam)
			{
				if(len == strlen(name)) break;
			}
			else
			{
				break;
			}
		}
	}
	
	//û���ҵ�����
	if(i >= nDebugCmdLen)
	{
		send( prompt1, strlen(prompt1));
		return;
	}
	
	//ִ������
	if(DebugCmd[i].Debug_Prase != NULL)
	{
		if(((DebugCmd[i].Debug_Prase)(buf, send)) != 0)
		{
			send( prompt5, strlen(prompt5));
		}
		else
		{
			send( prompt6, strlen(prompt6));
		}
	}
}

//=============================================
//�����������
void Net_RmcpCtrlCmd_Parse(char *buf, DebugSend send)
{
	uint8 type = 0;
	
	if((buf == NULL) || (send == NULL)) return;

	type = *buf;
	buf++;

	switch(type)
	{
		case CMD_CTRL_RST:
			Do_RmcpCtrlCmd("reset", buf, send);
			break;
			
		//��ӡϵͳ��������
		case CMD_CTRL_RSTCNT:
			Do_RmcpCtrlCmd("resetcnt", buf, send);
			break;
			
		//���õ�����Ϣ��ӡ�ȼ�
		case CMD_CTRL_SET_DLEVLE:
			Do_RmcpCtrlCmd("setlevel", buf, send);
			break;
			
		//��ӡϵͳ����ʱ��
		case CMD_CTRL_SYS_RUNTIME:
			Do_RmcpCtrlCmd("apptime", buf, send);
			break;
			
		//��̨����
		case CMD_CTRL_SCALER_SETZERO:
			Do_RmcpCtrlCmd("setzero", buf, send);
			break;

		//ģ�⳵��
		case CMD_CTRL_SIMULATE_CAR:
			Do_RmcpCtrlCmd("car", buf, send);
			break;

		//��ӡ����汾
		case CMD_CTRL_APP_VER:
			Do_RmcpCtrlCmd("version", buf, send);
			break;
			
		//��ӡ�����豸״̬
		case CMD_CTRL_DEVICE_STATUS:
			Do_RmcpCtrlCmd("print", buf, send);
			break;

		case CMD_CTRL_EXIT:
			//ʹ�ܲ��η���
			Set_NetUpdate_Flag(FALSE);
			break;

		//ģ�⵹��
		case CMD_CTRL_SIMULATE_BACKCAR:
			Do_RmcpCtrlCmd("backcar", buf, send);
			break;

		//��������
		case CMD_CTRL_FOLLOWCAR_MOD:
			Do_RmcpCtrlCmd("setfmode", buf, send);
			break;

		//����kϵ������
		case CMD_CTRL_SET_SCALERK:
			Do_RmcpCtrlCmd("setscalerk", buf, send);
			break;
			
		//�ϳӶ�2��������kϵ������
		case CMD_CTRL_SET_AD3K:
			Do_RmcpCtrlCmd("setad3k", buf, send);
			break;
			
		default:
			break;
	}
}

//shell������һ���������󳤶�
#define BeforeCmdMaxLen 15

//����һ���ַ� 
static char *erase_one_char = "\b \b";

//�ӳ����ն˶�ȡ������
static int Readline_Into_Buffer (char *prompt, char *buffer)
{
	static char before_cmd[BeforeCmdMaxLen + 1] = {0};
	BOOL upflag = FALSE;
	char *pOrig = buffer;
	char *pBuf = buffer;
	int rlen = 0, sum = 0;
	char c;

	//���prompt
	if(prompt != NULL)
	{
		Usart1_WriteData( prompt, strlen(prompt));
	}

	while(1)
	{
		rlen = CharDeviceRead(debug_handle, (u8 *)&c, 1);
		if(rlen > 0)
		{
			switch(c)
			{
				//����س�����
				case '\r':
				case '\n':
					*pBuf = 0;
					Usart1_WriteData( "\r\n", 2);

					//���浱ǰ����
					memset(before_cmd, 0, BeforeCmdMaxLen + 1);
					rlen = strlen(pOrig);
					if(rlen > BeforeCmdMaxLen) rlen = BeforeCmdMaxLen;
					strncpy(before_cmd, pOrig, rlen);
					
					return (pBuf - pOrig);

				//ctrl+c ȡ����������
				case 0x03:
					*pOrig = 0;
					Usart1_WriteData( "\r\n", 2);
					return 0;

				//backspace, delete
				case 0x08:
				case 0x7f:
					if((sum > 0) && ((pBuf - pOrig) > 0))
					{
						sum--;
						pBuf--;
						Usart1_WriteData(erase_one_char, strlen(erase_one_char));
					}
					break;

				//��UP���ظ���һ������
				case 0x1b:
					//ɾ���Ѿ����������
					while((sum > 0) && ((pBuf - pOrig) > 0))                                                 
					{                    
						sum--;            
						pBuf--;
						Usart1_WriteData(erase_one_char, strlen(erase_one_char));
					}
					upflag = TRUE;

					//�����һ������
					strcpy(pOrig, before_cmd);
					sum = strlen(before_cmd);
					pBuf += sum;
					if(sum > 0) Usart1_WriteData(before_cmd, sum);
					break;

				default:
					//��������ĸ�����ֵļ�ֵ
					if(isalnum(c) || (c == ' ') || (c == '.'))
					{		
						//���Ե�����UP���󸽴���'A'�ַ�
						if(upflag)
						{
							upflag = FALSE;
							if(c == 'A') break;
						}
						*pBuf++ = c;
						++sum;
						Usart1_WriteData(&c, 1);				
						//�ַ������ȳ���������
						if(sum >= MAX_DEBUGBUF_LEN - 1) 
						{
							Usart1_WriteData( "\r\n", 2);
							SysTimeDly(12);
							return -1;
						}
					}
					break;
			}//end switch(c)
		}
	}//end while(1)


}

static char DebugBuffer[MAX_DEBUGBUF_LEN] = {0};

void Debug_Thread(void *arg)				//���ڵ�������
{
	int len = 0, i = 0;
	char *str = NULL;

	//shellѭ��
	while(1)
	{
		len = Readline_Into_Buffer(prompt, DebugBuffer);
		
		//�ַ������ȳ���������,�쳣
		if(len < 0) 
		{
			Usart1_WriteData( prompt2, strlen(prompt2));
			goto end;
		}

		//û���յ���Ч���ַ�
		if(DebugBuffer[0] == '\0')
		{
			goto end;
		}
		
		//�յ�dtshell, ��ջ�����
		if(!DebugFlag)
		{
			if(strstr(DebugBuffer, "dtshell") == NULL)
			{
				goto end;
			}
			Usart1_WriteData(prompt3, strlen(prompt3));
			Usart1_WriteData( prompt4, strlen(prompt4));
			Usart1_WriteData( prompt_help, strlen(prompt_help));
			DebugFlag = TRUE;
			continue;
		}
		
		if(DebugFlag)
		{
			//��������
			for(i = 0; i < nDebugCmdLen; i++)
			{
				str = strstr(DebugBuffer, DebugCmd[i].name);
				len = strlen(DebugCmd[i].name);
				//��������
				if((str != NULL) && 
					(strncmp(str, DebugCmd[i].name, len) == 0) &&
					(str == &DebugBuffer[0]))
				{
					if(!DebugCmd[i].bParam)
					{
						if(len == strlen(str)) break;
					}
					else
					{
						break;
					}
				}
			}

			//û���ҵ�����
			if(i >= nDebugCmdLen)
			{
				Usart1_WriteData( prompt1, strlen(prompt1));
				continue;
			}

			//ִ������
			if(DebugCmd[i].Debug_Prase != NULL)
			{
				if((DebugCmd[i].Debug_Prase)(str + strlen(DebugCmd[i].name),Usart1_WriteData) != 0)
				{
					Usart1_WriteData( prompt5, strlen(prompt5));
				}
				else
				{
					Usart1_WriteData( prompt6, strlen(prompt6));
				}
			}
		}//end if(DebugFlag)		
end:
		//��ջ�����
		memset(DebugBuffer, 0, MAX_DEBUGBUF_LEN);
		DeviceIoctl(debug_handle, CMD_FLUSH_INPUT, 0);

		//��ʱ120ms
		SysTimeDly(12);
	}	//end while(1)
}

//������ȡ��ӡ�ȼ�
void Print_Debug_Init(void)
{
	Param_Read(GET_U8_OFFSET(ParamDebugLevel), (void *)&DebugLevel, sizeof(int8));
	if((unsigned int)DebugLevel > (unsigned int)Debug_Notify)
		DebugLevel = Debug_None;
}

void Debug_Init(void)
{	
	USART_InitTypeDef USART1_User = 		
	{
		57600,											//������
		USART_WordLength_8b,
		USART_StopBits_1,
		USART_Parity_No,
		USART_Mode_Rx | USART_Mode_Tx,
		USART_HardwareFlowControl_None
	};//���ڽṹ��ָ��
	u8 err;
	
	usart1_init();					//���Դ��ڳ�ʼ��
  	debug_handle = DeviceOpen(CHAR_USART1,(u32)&USART1_User,&err);							//�豸��
	if(err != HVL_NO_ERR)
	{
			 
  	}
	
	Set_PrintfPort(1);				//printf

	pdebugbuff = (unsigned char *)Ccm_Malloc(DEBUG_BUF_SIZE * sizeof(char));
	
	//���ô���1����ģʽΪ��������ģʽ
	DeviceIoctl(debug_handle, CMD_SET_RX_MODE, RX_LENGTH_MODE);
	kfifo_init(&debugfifo, pdebugbuff, DEBUG_BUF_SIZE); 
	Usart1_Set_OvertimeTick(1000);

	Init_YmodemUpdate_Buffer();

	//ע��ͨ�õ�shell����
	Register_Debug_Cmd("help",	 Do_Help,          "�г���������", FALSE);
	Register_Debug_Cmd("exit",	 Do_Exit,          "�˳�shell", FALSE);
	Register_Debug_Cmd("reset",	 Do_Reset,         "����", FALSE);
	Register_Debug_Cmd("update", Do_Serial_Update, "��������Ӧ�ó���", FALSE);
	Register_Debug_Cmd("apptime",  Do_Get_AppTime,  "��ȡϵͳ����ʱ��", FALSE);
	Register_Debug_Cmd("resetcnt", Do_Get_ResetCnt, "��ȡϵͳ��������", FALSE);
	Register_Debug_Cmd("setlevel", Do_Set_DLevel,   "���õ�����Ϣ��ӡ�ȼ�:\r\n\t\t0:����� 1:���� 2:ҵ�� 3:���� 4:֪ͨ 5:����", TRUE);
}

