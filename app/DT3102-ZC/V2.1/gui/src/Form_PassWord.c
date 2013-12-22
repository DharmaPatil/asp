/* 
 * History:
 * 1. 2013-1-8 ��С���������ʹ����;
 */
#include "Form_PassWord.h"
#include "Ctr_Attr.h"
#include "Form_Resource.h"
#include "Ctr_StatusBar.h"
#include "../Inc/TextEdit.h"
#include "Form_Main.h"
#include "form_mainmenu.h"
#include "Form_MsgBox.h"
#include "Common.h"

#include "sys_param.h"

extern  CControl  gStatusBar;
extern  SSYSSET gSysSet; // 0:kg  1:t

//��������
static const char* sSuperPwd = "666888";

static void Form_PassWord_Timer(void *ptmr, void *parg);
static void Form_PassWord_Draw(LPWindow pWindow);
static void Form_PassWord_Proc(LPWindow pWindow, LPGuiMsgInfo pGuiMsgInfo);

static char oldpw[20]  = "";
static char newpw1[20] = "";
static char newpw2[20] = "";
static char pwinfo[10] = "";
static CAttrItem  _attritem[] = {
   {"ԭ����",oldpw,1},
   {"������",newpw1,2},
   {"����ȷ��",newpw2,3},
   {"��������",pwinfo,4},
};

DEF_ATTR_CTRL(mPassWord, &gWD_PassWord, 0, 20, 240, 108, "��������",(_attritem), sizeof(_attritem)/sizeof(CAttrItem), 0, CTRL_VISABLE);

static LPControl marrLPControl[] = 
{
    &gStatusBar,
	&mPassWord,
};

CWindow  gWD_PassWord = {
    marrLPControl,
	sizeof(marrLPControl)/sizeof(LPControl),
    0,
    0,
    0,
    240,
    128,
    WS_VISABLE,
		NULL,//(pParentWindow)
    10,//5
    NULL,
    Form_PassWord_Timer,
    Form_PassWord_Draw,
    Form_PassWord_Proc
};

void Form_PassWord_Timer(void *ptmr, void *parg)
{
	LPWindow lpWindow = (LPWindow)parg;
	if(lpWindow != NULL)
	{
	}
}

void Form_PassWord_Draw(LPWindow pWindow)
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

static void Check_User_Pwd(const char *psw1, const char *psw2)
{
	if(strcmp(psw1,psw2))  {
        MsgBoxDlg(&g_MsgBoxDlg,"������ʾ","��������������벻һ��");
    }else {
        strcpy(pwinfo,"�ѱ���");
        strcpy((char *)gSysSet.password,psw1);
        SysSetParamSave();
        MsgBoxDlg(&g_MsgBoxDlg,"��ʾ��Ϣ","�����ѱ���");
    }
}

void Form_PassWord_Proc(LPWindow pWindow, LPGuiMsgInfo pGuiMsgInfo)
{
	CControl* pControl;	
	GuiMsgInfo guiMsgInfo;
    static char   cPw0[20]="";
    static char   cPw1[20]="";
    static char   cPw2[20]="";
    char   len;

	switch(pGuiMsgInfo->ID)
	{
		case WM_LOAD:
			ClearScreen();
			SysTimeDly(15);
			CTRL_CONTENT(mPassWord).focus = 0;
            memset(oldpw,0,sizeof(oldpw));
            memset(newpw1,0,sizeof(newpw1));
            memset(newpw2,0,sizeof(newpw2));
            strcpy(pwinfo,"δ����");
            memset(cPw0,0,sizeof(cPw0));
            memset(cPw1,0,sizeof(cPw1));
            memset(cPw2,0,sizeof(cPw2));

		case WM_SHOW:
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
                    if (CTRL_CONTENT(mPassWord).focus > 0) {
                        CTRL_CONTENT(mPassWord).focus--;
            			PostWindowMsg(pWindow, WM_UPDATECTRL, (uint32)&mPassWord, 0);
                    }
					break;
				case KEY_DOWN:
                    if (CTRL_CONTENT(mPassWord).focus + 1 < CTRL_CONTENT(mPassWord).total) {
                        CTRL_CONTENT(mPassWord).focus++;
            			PostWindowMsg(pWindow, WM_UPDATECTRL, (uint32)&mPassWord, 0);
                    }
					break;

				case KEY_LEFT:
					break;

				case KEY_OK:
					switch (CTRL_CONTENT(mPassWord).focus) {

					case 0:
					        memset(cPw0,0,sizeof(cPw0));
					        memset(oldpw,0,sizeof(oldpw));
							if( LoadEditDlg(&g_TextEditWindow, "������ԭ����", cPw0, cPw0, 8)) {
					            len = strlen(cPw0);                            	
					            for (;len;len--) {
					                oldpw[len-1]='*';
								}
							}
					        break;
					        
					    case 1:
					        memset(cPw1,0,sizeof(cPw1));
					        memset(newpw1,0,sizeof(newpw1));
							if( LoadEditDlg(&g_TextEditWindow, "������������", cPw1, cPw1, 8)) {
					            len = strlen(cPw1);
					            for (;len;len--) {
					                newpw1[len-1]='*';
										}
					            strcpy(pwinfo,"δ����");
							}
					        break;

					    case 2:
					        memset(cPw2,0,sizeof(cPw2));
					        memset(newpw2,0,sizeof(newpw2));
							if( LoadEditDlg(&g_TextEditWindow, "����������������", cPw2, cPw2, 8)) {
					            len = strlen(cPw2);
					            for (;len;len--) {
					                newpw2[len-1]='*';
										}
					            strcpy(pwinfo,"δ����");
							}
					        break;
					        
					    case 3:                  	
							if(strcmp(cPw0, sSuperPwd) == 0) {
								Check_User_Pwd(cPw1, cPw2);
					            break;
					        }
					        				
					        if(strcmp(cPw0,(char *)gSysSet.password)) {
					            MsgBoxDlg(&g_MsgBoxDlg,"������ʾ","ԭ�������");
					        } else {
								Check_User_Pwd(cPw1, cPw2);
							}
					        break;

					    default:
					        break;
					}
					guiMsgInfo.pWindow = pWindow;
					guiMsgInfo.ID = WM_SHOW;
					GuiMsgQueuePost(&guiMsgInfo);					
					break;

				case KEY_RIGHT: 
					break;

				case KEY_BACK:
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


int CheckPassWord(void)
{
    char cbuf[10]="";
    int ret = 0;

	if( LoadEditDlg(&g_TextEditWindow, "����������", cbuf, cbuf, 8)) 
	{
        if(0 == strcmp(cbuf,(char *)gSysSet.password) ) 
        {
        	ret = 1;
        } 

        if( 0 == strcmp(cbuf,sSuperPwd)) 
        {
            ret = 2;
        }
	}
		
  	return ret;
}

BOOL IsBbKeyOpen(void)           //�궨����
{
	BOOL ret = TRUE;
	
    if(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_10) == Bit_SET) 
    {      
    	ret = FALSE;

    	if(Get_SuperUser_Use()) 
    	{
    		ret = TRUE;
    		Set_SuperUser_Use(FALSE);
    	}
    } 

	return ret;
}



