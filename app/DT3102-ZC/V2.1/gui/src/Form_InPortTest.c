#include "Form_InPortTest.h"
#include "Form_Main.h"

#include "Ctr_Attr.h"
#include "Ctr_DlgList.h"
#include "../Inc/TextEdit.h"
#include "ioctrl.h"

extern  CControl  gStatusBar; 		//״̬��

extern INTEST InTest;
extern OUTTEST OutTest;
				  
static void Form_Warning_Timer(void *ptmr, void *parg);
static void Form_Warning_Draw(LPWindow pWindow);
static void Form_Warning_Proc(LPWindow pWindow, LPGuiMsgInfo pGuiMsgInfo);
void InPortState(void);


static char attritemstr[10][20];

//��������ҳ
static CAttrItem  _attritem[] = {
   {"����״̬1",attritemstr[0],1},
   {"����״̬2",attritemstr[1],2},
   {"����״̬3",attritemstr[2],3},
   {"����״̬4",attritemstr[3],4},
   {"����״̬5",attritemstr[4],5},
   {"����״̬6",attritemstr[5],6},
   {"����״̬7",attritemstr[6],7},
   {"����״̬8",attritemstr[7],8},
   {"����״̬9",attritemstr[8],9},
   {"����״̬10",attritemstr[9],10},

};

DEF_ATTR_CTRL(mInPortAttr, &gWD_InPortTest, 0, 20, 240, 108, "�������״̬",(_attritem), sizeof(_attritem)/sizeof(CAttrItem), 0, CTRL_VISABLE);

static LPControl marrLPControl[] = 
{
  &gStatusBar,
	&mInPortAttr,
};

CWindow  gWD_InPortTest = {		 //����
    marrLPControl,
	sizeof(marrLPControl)/sizeof(LPControl),
    0,
    0,
    0,
    240,
    128,
    WS_VISABLE,
	NULL,//(pParentWindow)
    3,//5
    NULL,
    Form_Warning_Timer,
    Form_Warning_Draw,
    Form_Warning_Proc
};

void Form_Warning_Timer(void *ptmr, void *parg)
{
	LPWindow lpWindow = (LPWindow)parg;

	if(lpWindow != NULL)
	{

			InPortTest();
		if((InTest.In0 | InTest.In1 | InTest.In2 | InTest.In3 | InTest.In4 | InTest.In5 | InTest.In6 | InTest.In7 | InTest.In8 | InTest.In9))
		{
//			PostWindowMsg(WM_SHOW, 0, 0);		   //�ٽ����ػ洰��
// 			if(InTest.In0) {
//                 sprintf(attritemstr[0],"��");
// 			} else {
//                 sprintf(attritemstr[0],"��");
// 			}
// 			
// 			if(InTest.In1) {
//                 sprintf(attritemstr[1],"��");
// 			} else {
//                 sprintf(attritemstr[1],"��");
// 			}
// 			
// 			if(InTest.In2) {
//                 sprintf(attritemstr[2],"��");
// 			} else {
//                 sprintf(attritemstr[2],"��");
// 			}
// 			
// 			if(InTest.In3) {
//                 sprintf(attritemstr[3],"��");
// 			} else {
//                 sprintf(attritemstr[3],"��");
// 			}
// 			
// 			if(InTest.In4) {
//                 sprintf(attritemstr[4],"��");
// 			} else {
//                 sprintf(attritemstr[4],"��");
// 			}
// 			
// 			if(InTest.In5) {
//                 sprintf(attritemstr[5],"��");
// 			} else {
//                 sprintf(attritemstr[5],"��");
// 			}
// 			
// 			if(InTest.In6) {
//                 sprintf(attritemstr[6],"��");
// 			} else {
//                 sprintf(attritemstr[6],"��");
// 			}
// 			
// 			if(InTest.In7) {
//                 sprintf(attritemstr[7],"��");
// 			} else {
//                 sprintf(attritemstr[7],"��");
// 			}
// 			
// 			if(InTest.In8) {
//                 sprintf(attritemstr[8],"��");
// 			} else {
//                 sprintf(attritemstr[8],"��");
// 			}
// 			
// 			if(InTest.In9) {
//                 sprintf(attritemstr[9],"��");
// 			} else {
//                 sprintf(attritemstr[9],"��");
// 			}

			InPortState();
			PostWindowMsg(lpWindow, WM_UPDATECTRL, (uint32)&mInPortAttr, 0);
		}
		
	}
}

void Form_Warning_Draw(LPWindow pWindow)
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

void Form_Warning_Proc(LPWindow pWindow, LPGuiMsgInfo pGuiMsgInfo)
{
	CControl* pControl;	
//     char buf0[2]="";
//     char buf1[2]="";
//     char buf2[2]="";
//     char buf3[2]="";
//     char buf4[2]="";
//     char buf5[2]="";
//     char buf6[2]="";
//     char buf7[2]="";
//     char buf8[2]="";
//     char buf9[2]="";


	switch(pGuiMsgInfo->ID)
	{
		case WM_LOAD:
			ClearScreen();							//����
			SysTimeDly(3);
			CTRL_CONTENT(mInPortAttr).focus = 0;	//���ͣ��λ��
			CreateWindowTimer(&gWD_InPortTest);
			StartWindowTimer(&gWD_InPortTest);

		case WM_SHOW:
			//���ù�������
/*			if(gSysStatus & SYSSTA_INFCUR) {
                sprintf(attritemstr[0],"����");
			} else {
                sprintf(attritemstr[0],"����");
			}
			if(gSysStatus & SYSSTA_COIL) {
                sprintf(attritemstr[1],"����");
			} else {
                sprintf(attritemstr[1],"����");
			}
			if(gSysStatus & (SYSSTA_SE_A1 | SYSSTA_SE_A2 | SYSSTA_SE_B1 | SYSSTA_SE_B2)) {
                strcpy(buf0,"");
    			if(gSysStatus & SYSSTA_SE_A1) {
                    sprintf(buf1,"%sA1",buf0);
                    strcpy(buf0,",");
                }
    			if(gSysStatus & SYSSTA_SE_A2) {
                    sprintf(buf2,"%sA2",buf0);
                    strcpy(buf0,",");
                }
    			if(gSysStatus & SYSSTA_SE_B1) {
                    sprintf(buf3,"%sB1",buf0);
                    strcpy(buf0,",");
                }
    			if(gSysStatus & SYSSTA_SE_B2) {
                    sprintf(buf4,"%sB2",buf0);
                }
                sprintf(attritemstr[2],"%s%s%s%s����",buf1,buf2,buf3,buf4);
			} else {
                sprintf(attritemstr[2],"����");
			}
			if(gSysStatus & (SYSSTA_AD_A1 | SYSSTA_AD_A2 | SYSSTA_AD_B1 | SYSSTA_AD_B2)) {
                strcpy(buf0,"");
    			if(gSysStatus & SYSSTA_AD_A1) {
                    sprintf(buf1,"%sA1",buf0);
                    strcpy(buf0,",");
                }
    			if(gSysStatus & SYSSTA_AD_A2) {
                    sprintf(buf2,"%sA2",buf0);
                    strcpy(buf0,",");
                }
    			if(gSysStatus & SYSSTA_AD_B1) {
                    sprintf(buf3,"%sB1",buf0);
                    strcpy(buf0,",");
                }
    			if(gSysStatus & SYSSTA_AD_B2) {
                    sprintf(buf4,"%sB2",buf0);
                }
                sprintf(attritemstr[3],"%s%s%s%s����",buf1,buf2,buf3,buf4);
			} else {
                sprintf(attritemstr[3],"����");
			}
			if(gSysStatus & SYSSTA_WHEEL) {
                sprintf(attritemstr[4],"����");
			} else {
                sprintf(attritemstr[4],"����");
			}
			
			
*/
			InPortState();
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
                    if (CTRL_CONTENT(mInPortAttr).focus > 0) {
                        CTRL_CONTENT(mInPortAttr).focus--;
            			PostWindowMsg(pWindow, WM_UPDATECTRL, (uint32)&mInPortAttr, 0);
                    }
					break;
				case KEY_DOWN:
                    if (CTRL_CONTENT(mInPortAttr).focus + 1 < CTRL_CONTENT(mInPortAttr).total) {
                        CTRL_CONTENT(mInPortAttr).focus++;
            			PostWindowMsg(pWindow, WM_UPDATECTRL, (uint32)&mInPortAttr, 0);
                    }
					break;

				case KEY_LEFT:
					break;

				case KEY_OK:
					break;

				case KEY_RIGHT: 
					break;

				case KEY_BACK:
					StopWindowTimer(&gWD_InPortTest);
					DestoryWindowTimer(&gWD_InPortTest);
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

void InPortState(void)
{
			if(InTest.In0) {
                sprintf(attritemstr[0],"��");
			} else {
                sprintf(attritemstr[0],"��");
			}
			
			if(InTest.In1) {
                sprintf(attritemstr[1],"��");
			} else {
                sprintf(attritemstr[1],"��");
			}
			
			if(InTest.In2) {
                sprintf(attritemstr[2],"��");
			} else {
                sprintf(attritemstr[2],"��");
			}
			
			if(InTest.In3) {
                sprintf(attritemstr[3],"��");
			} else {
                sprintf(attritemstr[3],"��");
			}
			
			if(InTest.In4) {
                sprintf(attritemstr[4],"��");
			} else {
                sprintf(attritemstr[4],"��");
			}
			
			if(InTest.In5) {
                sprintf(attritemstr[5],"��");
			} else {
                sprintf(attritemstr[5],"��");
			}
			
			if(InTest.In6) {
                sprintf(attritemstr[6],"��");
			} else {
                sprintf(attritemstr[6],"��");
			}
			
			if(InTest.In7) {
                sprintf(attritemstr[7],"��");
			} else {
                sprintf(attritemstr[7],"��");
			}
			
			if(InTest.In8) {
                sprintf(attritemstr[8],"��");
			} else {
                sprintf(attritemstr[8],"��");
			}
			
			if(InTest.In9) {
                sprintf(attritemstr[9],"��");
			} else {
                sprintf(attritemstr[9],"��");
			}	
}



