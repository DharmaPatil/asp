#include "Form_NetSet.h"
#include "Form_Main.h"
//#include "Form_MainMenu.h"
#include "Form_MsgBox.h"
#include "Ctr_Attr.h"
//#include "..\Bsp\bsp.h"
#include "../Inc/TextEdit.h"


#include "sys_param.h"
#include "LWIP_Impl.h"

extern  CControl  gStatusBar; 		//״̬��

static void Form_NetSet_Timer(void *ptmr, void *parg);
static void Form_NetSet_Draw(LPWindow pWindow);
static void Form_NetSet_Proc(LPWindow pWindow, LPGuiMsgInfo pGuiMsgInfo);

static sServerADDR 	ServerAddr = {0};	/*������IP��ַ�Ͷ˿ں�*/
static Glocal_IP_ADDR Local_IP = {0};

static char  gsIpAddr[16]="";
static char  gsSubnetMask[16]="";
static char  gsGateWay[16]="";
static char  gsServerPort[6]="";
static char  gsServerAddr[16]="";

static CAttrItem  _attritem[] = {
   {"IP��ַ",gsIpAddr,1},
   {"��������",gsSubnetMask,2},
   {"Ĭ������",gsGateWay,3},
   {"�������˿�",gsServerPort,4},
   {"��������ַ",gsServerAddr,5},
};

enum {
	NETSlave = 0,
	NETMaster
};

static void Net_Param_Save(uint8 type) 
{
	if(type == NETSlave) {
		if(!Param_Write(ParamNet, &Local_IP, sizeof(Local_IP)))
		{
			debug(Debug_Error,"Write local IP address from eeprom error!!!\r\n");
		}
	} else {
		if(!Param_Write(ParamNet+sizeof(Local_IP), &ServerAddr, sizeof(ServerAddr)))
		{
			debug(Debug_Error,"Write server IP address from eeprom error!!!\r\n");
		}
	}
}


DEF_ATTR_CTRL(mNetParaAttr, &gWD_NetSet, 0, 20, 240, 108, "��������",(_attritem), sizeof(_attritem)/sizeof(CAttrItem), 0, CTRL_VISABLE);

static LPControl marrLPControl[] = 
{
    &gStatusBar,
    &mNetParaAttr,
};

CWindow  gWD_NetSet = {
    marrLPControl,
	sizeof(marrLPControl)/sizeof(LPControl),
    0,
    0,
    0,
    240,
    128,
    WS_VISABLE,
	NULL,//(pParentWindow)
    2,//5
    NULL,
    Form_NetSet_Timer,
    Form_NetSet_Draw,
    Form_NetSet_Proc
};


void Form_NetSet_Timer(void *ptmr, void *parg)
{
	LPWindow lpWindow = (LPWindow)parg;
	if(lpWindow != NULL)
	{
	}
}

void Form_NetSet_Draw(LPWindow pWindow)
{
	uint16 i;
	LPControl	lpControl;

	//����Դ�
	EraseBuffer();

	//��ֹ��ͼ
	SetRedraw(FALSE);

	//��ֹˢ��
	EnableScreenFlush(FALSE);

	//������ͼ
	SetGdiView(pWindow->nViewPosX, pWindow->nViewPosY, pWindow->nViewSizeX, pWindow->nViewSizeY);
	EnableGdiView(TRUE);

	//���ƿؼ�
	for(i = 0; i < pWindow->nNbControls; i++)
	{
		lpControl = *(pWindow->pLPControls + i);
		if(lpControl->state & CTRL_VISABLE)
		{
			lpControl->DrawFunc(lpControl);
		}
	}
	//ʹ��ˢ��
	EnableScreenFlush(TRUE);

	//ˢ��
	FlushScreen();

	//ʹ�ܻ�ͼ
	SetRedraw(TRUE);
}

sIP_ADDR *IPFormatDetection(char *ipaddr)
{
    static sIP_ADDR ip;
    uint16 usTemp[4];
    uint8 i;
    char * pStr;
    pStr = ipaddr;
    
    for(i = 0; i < 4 ; i++) {
        if(strlen(pStr)) {
            if((*pStr < '0') || (*pStr > '9')) {
                break;
            }
        } else {
            break;
        }
        usTemp[i]=atoi(pStr);
        if(usTemp[i] > 255) {
            break;
        }
        if(i < 3) {
            pStr = (char*)strstr(pStr, ".");
            if(pStr) {
                pStr++;
            } else {
                break;
            }
        }
    }
    if(i == 4) {
        ip.addr1 = usTemp[0];
        ip.addr2 = usTemp[1];
        ip.addr3 = usTemp[2];
        ip.addr4 = usTemp[3];
        return &ip;
    }
    else
        return NULL;
}

extern struct netif xnetif;
void Form_NetSet_Proc(LPWindow pWindow, LPGuiMsgInfo pGuiMsgInfo)
{
	CControl* pControl;
    sIP_ADDR *setip;
    s32 port;
    char   cTemp[20]="";
    static u8 reset = 0;

	switch(pGuiMsgInfo->ID)
	{
		case WM_LOAD:
			ClearScreen();
			SysTimeDly(15);
		if(!Param_Read(ParamNet, &Local_IP, sizeof(Local_IP)))
		{
			debug(Debug_Error,"read local IP address from eeprom error!!!\r\n");
		}
		if(!Param_Read(ParamNet+sizeof(Local_IP), &ServerAddr, sizeof(ServerAddr)))
		{
			debug(Debug_Error,"read server IP address from eeprom error!!!\r\n");
		}
			//CreateWindowTimerEx(pWindow, 1);
		case WM_SHOW:
	
			sprintf(gsIpAddr,"%d.%d.%d.%d",Local_IP.ipaddr.addr1,Local_IP.ipaddr.addr2,Local_IP.ipaddr.addr3,Local_IP.ipaddr.addr4);
      sprintf(gsSubnetMask,"%d.%d.%d.%d",Local_IP.SubnetMask.addr1,Local_IP.SubnetMask.addr2,Local_IP.SubnetMask.addr3,Local_IP.SubnetMask.addr4);
      sprintf(gsGateWay,"%d.%d.%d.%d",Local_IP.GateWay.addr1,Local_IP.GateWay.addr2,Local_IP.GateWay.addr3,Local_IP.GateWay.addr4);
      sprintf(gsServerPort,"%d",ServerAddr.port);
      sprintf(gsServerAddr,"%d.%d.%d.%d",ServerAddr.ipaddr.addr1,ServerAddr.ipaddr.addr2,ServerAddr.ipaddr.addr3,ServerAddr.ipaddr.addr4);
		pWindow->DrawFunc(pWindow);
			break;

		case WM_UPDATECTRL:
			pControl = (CControl*)(pGuiMsgInfo->wParam);
			if(pControl != NULL)
			{
				pControl->DrawFunc(pControl);
			} 
			break;

		case WM_TIMEUPDATE:
			PostWindowMsg(pWindow, WM_UPDATECTRL, (uint32)&gStatusBar, 0);
			break;

		case WM_KEYDOWN:
			switch(pGuiMsgInfo->wParam)
			{
				case KEY_UP:
                    if (CTRL_CONTENT(mNetParaAttr).focus > 0) {
                        CTRL_CONTENT(mNetParaAttr).focus--;
            			PostWindowMsg(pWindow, WM_UPDATECTRL, (uint32)&mNetParaAttr, 0);
                    }
					break;
                    
				case KEY_DOWN:
                    if (CTRL_CONTENT(mNetParaAttr).focus + 1 < CTRL_CONTENT(mNetParaAttr).total) {
                        CTRL_CONTENT(mNetParaAttr).focus++;
            			PostWindowMsg(pWindow, WM_UPDATECTRL, (uint32)&mNetParaAttr, 0);
                    }
					break;
                    
				case KEY_OK:
                    memset(cTemp,0,20);
                    switch (CTRL_CONTENT(mNetParaAttr).focus) {

                    case 0:
						//
    					if( LoadEditDlg(&g_TextEditWindow, "�������豸IP��ַ", gsIpAddr, cTemp, 15)) {
                            setip = IPFormatDetection(cTemp);
                            if(setip) {
                                memcpy(&Local_IP.ipaddr,setip,sizeof(sIP_ADDR));
                                strcpy(gsIpAddr,cTemp);
                                reset = 1;
                                //IP4_ADDR(&ipaddr, setip->addr1, setip->addr2, setip->addr3, setip->addr4);
                                //tcpclient_close();
                                //netif_set_ipaddr(&xnetif,&ipaddr);
                                Net_Param_Save(NETSlave);
                            } else {
                                MsgBoxDlg(&g_MsgBoxDlg, "��Ϣ��ʾ", "IP��ַ���Ϸ�������������");
                            }
    					}
                        break;

                    case 1:
						//
    					if( LoadEditDlg(&g_TextEditWindow, "�������豸��������", gsSubnetMask, cTemp, 15)) {
                            setip = IPFormatDetection(cTemp);
                            if(setip) {
                                memcpy(&Local_IP.SubnetMask,setip,sizeof(sIP_ADDR));
                                strcpy(gsSubnetMask,cTemp);
                                reset = 1;
                                //IP4_ADDR(&netmask, setip->addr1, setip->addr2, setip->addr3, setip->addr4);
                                //tcpclient_close();
                                //netif_set_netmask(&xnetif,&netmask);
                                Net_Param_Save(NETSlave);
                            } else {
                                MsgBoxDlg(&g_MsgBoxDlg, "��Ϣ��ʾ", "�������벻�Ϸ�������������");
                            }
    					}
                        break;

                    case 2:
						//
    					if( LoadEditDlg(&g_TextEditWindow, "�������豸Ĭ������", gsGateWay, cTemp, 15)) {
                            setip = IPFormatDetection(cTemp);
                            if(setip) {
                                memcpy(&Local_IP.GateWay,setip,sizeof(sIP_ADDR));
                                strcpy(gsGateWay,cTemp);
                                reset = 1;
                                //IP4_ADDR(&gw, setip->addr1, setip->addr2, setip->addr3, setip->addr4);
                                //tcpclient_close();
                                //netif_set_gw(&xnetif,&gw);
                                Net_Param_Save(NETSlave);
                            } else {
                                MsgBoxDlg(&g_MsgBoxDlg, "��Ϣ��ʾ", "���ص�ַ���Ϸ�������������");
                            }
    					}
                        break;

                    case 3:
						//
    					if( LoadEditDlg(&g_TextEditWindow, "������������˿�", gsServerPort, cTemp, 5)) {
                            port = atoi(cTemp);
                            if((port > 0) && (port < 65535)) {
                                strcpy(gsServerPort,cTemp);
                                ServerAddr.port= port;
                                reset = 1;
                                //tcpclient_close();
                                Net_Param_Save(NETMaster);
                            } else {
                                MsgBoxDlg(&g_MsgBoxDlg, "��Ϣ��ʾ", "�˿ںŲ��Ϸ�������������");
                            }
    					}
                        break;

                    case 4:
						//
    					if( LoadEditDlg(&g_TextEditWindow, "�����������IP��ַ", gsServerAddr, cTemp, 15)) {
                            setip = IPFormatDetection(cTemp);
                            if(setip) {
                                memcpy(&ServerAddr.ipaddr,setip,sizeof(sIP_ADDR));
                                strcpy(gsServerAddr,cTemp);
                                reset = 1;
                                //tcpclient_close();
                                Net_Param_Save(NETMaster);
                            } else {
                                MsgBoxDlg(&g_MsgBoxDlg, "��Ϣ��ʾ", "IP��ַ���Ϸ�������������");
                            }
    					}
                        break;
                        
                    default:
                        break;
                    }
        			PostWindowMsg(pWindow, WM_SHOW, NULL, NULL);	
					break;

				case KEY_BACK:
                    if(reset) {
                        reset = 0;
                        MsgBoxDlg(&g_MsgBoxDlg, "��Ϣ��ʾ", "�µ����ý���ϵͳ��������Ч");
                    }
					if(pWindow->pParentWindow != NULL)
					{
						g_pCurWindow = pWindow->pParentWindow;
					}
					else
					{
						g_pCurWindow = &gWD_Main;
					}
                    PostWindowMsg(g_pCurWindow, WM_LOAD,0,0);
					break;

				default:
					break;
			}
			break;
		
		default:
			break;
	}	
}



