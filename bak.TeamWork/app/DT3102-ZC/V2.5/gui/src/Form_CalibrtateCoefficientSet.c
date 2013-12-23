#include "includes.h"
#include "Form_NetSet.h"
#include "Form_Main.h"
#include "Form_MsgBox.h"
#include "Ctr_Attr.h"
#include "../Inc/TextEdit.h"
#include "sys_param.h"
#include "lwip_netaddr.h"

extern  CControl  gStatusBar; 		//״̬��

static void Form_CalibrtateCoefficientSet_Timer(void *ptmr, void *parg);
static void Form_CalibrtateCoefficientSet_Draw(LPWindow pWindow);
static void Form_CalibrtateCoefficientSet_Proc(LPWindow pWindow, LPGuiMsgInfo pGuiMsgInfo);

extern SIPADDR gIpAddr;
extern SIPADDR gSubnetMask;
extern SIPADDR gGateWay;
extern u32     gServerPort;
extern SIPADDR gServerAddr;

static char  gsIpAddr[16]="";
static char  gsSubnetMask[16]="";
static char  gsGateWay[16]="";
static char  gsServerPort[6]="";
static char  gsServerAddr[16]="";

static const CAttrItem  _attritem[] = {
   {"IP��ַ",gsIpAddr,1},
   {"��������",gsSubnetMask,2},
   {"Ĭ������",gsGateWay,3},
   {"�������˿�",gsServerPort,4},
   {"��������ַ",gsServerAddr,5},
};

DEF_ATTR_CTRL(mNetParaAttr, &gWD_NetSet, 0, 20, 240, 108, "��������",(CAttrItem *)(_attritem), sizeof(_attritem)/sizeof(CAttrItem), 0, CTRL_VISABLE);

static const LPControl marrLPControl[] = 
{
    &gStatusBar,
    &mNetParaAttr,
};

CWindow  gWD_NetSet = {
    (LPControl *)marrLPControl,
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

SIPADDR *IPFormatDetection(char *ipaddr)
{
    static SIPADDR ip;
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
    SIPADDR *setip;
    s32 port;
    char   cTemp[20]="";
    static u8 reset = 0;

	switch(pGuiMsgInfo->ID)
	{
		case WM_LOAD:
			ClearScreen();
			SysTimeDly(15);
			//CreateWindowTimerEx(pWindow, 1);
		case WM_SHOW:
            sprintf(gsIpAddr,"%d.%d.%d.%d",gIpAddr.addr1,gIpAddr.addr2,gIpAddr.addr3,gIpAddr.addr4);
            sprintf(gsSubnetMask,"%d.%d.%d.%d",gSubnetMask.addr1,gSubnetMask.addr2,gSubnetMask.addr3,gSubnetMask.addr4);
            sprintf(gsGateWay,"%d.%d.%d.%d",gGateWay.addr1,gGateWay.addr2,gGateWay.addr3,gGateWay.addr4);
            sprintf(gsServerPort,"%d",gServerPort);
            sprintf(gsServerAddr,"%d.%d.%d.%d",gServerAddr.addr1,gServerAddr.addr2,gServerAddr.addr3,gServerAddr.addr4);
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
			PostWindowMsg(WM_UPDATECTRL, (uint32)&gStatusBar, 0);
			break;

		case WM_KEYDOWN:
			switch(pGuiMsgInfo->wParam)
			{
				case KEY_UP:
                    if (CTRL_CONTENT(mNetParaAttr).focus > 0) {
                        CTRL_CONTENT(mNetParaAttr).focus--;
            			PostWindowMsg(WM_UPDATECTRL, (uint32)&mNetParaAttr, 0);
                    }
					break;
                    
				case KEY_DOWN:
                    if (CTRL_CONTENT(mNetParaAttr).focus + 1 < CTRL_CONTENT(mNetParaAttr).total) {
                        CTRL_CONTENT(mNetParaAttr).focus++;
            			PostWindowMsg(WM_UPDATECTRL, (uint32)&mNetParaAttr, 0);
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
                                memcpy(&gIpAddr,setip,sizeof(SIPADDR));
                                strcpy(gsIpAddr,cTemp);
                                reset = 1;
                                //IP4_ADDR(&ipaddr, setip->addr1, setip->addr2, setip->addr3, setip->addr4);
                                //tcpclient_close();
                                //netif_set_ipaddr(&xnetif,&ipaddr);
                                NetParamSave();
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
                                memcpy(&gSubnetMask,setip,sizeof(SIPADDR));
                                strcpy(gsSubnetMask,cTemp);
                                reset = 1;
                                //IP4_ADDR(&netmask, setip->addr1, setip->addr2, setip->addr3, setip->addr4);
                                //tcpclient_close();
                                //netif_set_netmask(&xnetif,&netmask);
                                NetParamSave();
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
                                memcpy(&gGateWay,setip,sizeof(SIPADDR));
                                strcpy(gsGateWay,cTemp);
                                reset = 1;
                                //IP4_ADDR(&gw, setip->addr1, setip->addr2, setip->addr3, setip->addr4);
                                //tcpclient_close();
                                //netif_set_gw(&xnetif,&gw);
                                NetParamSave();
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
                                gServerPort = port;
                                reset = 1;
                                //tcpclient_close();
                                NetParamSave();
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
                                memcpy(&gServerAddr,setip,sizeof(SIPADDR));
                                strcpy(gsServerAddr,cTemp);
                                reset = 1;
                                //tcpclient_close();
                                NetParamSave();
                            } else {
                                MsgBoxDlg(&g_MsgBoxDlg, "��Ϣ��ʾ", "IP��ַ���Ϸ�������������");
                            }
    					}
                        break;
                        
                    default:
                        break;
                    }
        			PostWindowMsg(WM_SHOW, NULL, NULL);	
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
                    PostWindowMsg(WM_LOAD,0,0);
					break;

				default:
					break;
			}
			break;
		
		default:
			break;
	}	
}



