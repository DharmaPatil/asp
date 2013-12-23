#include "includes.h"
#include "Form_Warning.h"
#include "form_query.h"
#include "Form_Main.h"
#include "sys_param.h"
#include "Ctr_Attr.h"
#include "Ctr_DlgList.h"
#include "../Inc/TextEdit.h"
#include "device_info.h"
#include "io.h"

#include "adjob.h"

extern  CControl  gStatusBar; 		//״̬��
				  
static void Form_Warning_Timer(void *ptmr, void *parg);
static void Form_Warning_Draw(LPWindow pWindow);
static void Form_Warning_Proc(LPWindow pWindow, LPGuiMsgInfo pGuiMsgInfo);

#define AttritemLen 9+2

static char attritemstr[AttritemLen][16];
static int gSysStatus;

//��������ҳ
static const CAttrItem  _attritem[] = {
   {"ǰ�ظ�״̬",attritemstr[0],1},
   {"��ظ�״̬",attritemstr[1],2},
   {"�ֳ���Ļ״̬",attritemstr[2],3},
   {"������1״̬",attritemstr[3],4},
   {"������2״̬",attritemstr[4],5},
   {"������3״̬",attritemstr[5],6},
   {"������4״̬",attritemstr[6],7},
   {"ADоƬ״̬",attritemstr[7],8},
   {"��̥ʶ����״̬",attritemstr[8],9},
		{"����1����",attritemstr[9],10},
		{"SDK���ڲ���",attritemstr[10],11},
};

DEF_ATTR_CTRL(mWarningAttr, &gWD_Warning, 0, 20, 240, 108, "�豸״̬",(CAttrItem *)(_attritem), sizeof(_attritem)/sizeof(CAttrItem), 0, CTRL_VISABLE);

static const LPControl marrLPControl[] = 
{
	&gStatusBar,
	&mWarningAttr,
};

CWindow  gWD_Warning = {		 //����
    (LPControl *)marrLPControl,
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
    Form_Warning_Timer,
    Form_Warning_Draw,
    Form_Warning_Proc
};

//��ȡ��Ļ���ظС���̥ʶ��������������ADת��оƬ�Ƿ���ϵ�״̬
void Device_status(void)
{
	int ADChipErr = 0;
	char buf[16] = {0};
	
	if(Get_GC_ErrStatus(1))//�жϵظ�״̬��˫��ֻ̨����ǰ�ظ�
	{
		sprintf(attritemstr[0],"����");
	}
	else 
	{
		sprintf(attritemstr[0],"����");
	}

	if(Get_GC_ErrStatus(2))//�жϵظ�״̬��˫��ֻ̨����ǰ�ظ�
	{
		sprintf(attritemstr[1],"����");
	}
	else 
	{
		sprintf(attritemstr[1],"����");
	}

	if(Get_LC_ErrStatus(2))   //�жϹ�Ļ״̬,˫��ֻ̨���ú��Ļ
	{
		sprintf(attritemstr[2],"����");
	}
	else 
	{
		sprintf(attritemstr[2],"����");
	}

	if(Get_AD_ErrStatus(1))
	{
		sprintf(attritemstr[3],"�쳣");
	}
	else 
	{
		sprintf(attritemstr[3],"����");
	}	

	if(Get_AD_ErrStatus(2))
	{
		sprintf(attritemstr[4],"�쳣");
	}
	else 
	{
		sprintf(attritemstr[4],"����");
	}	

	if(Get_AD_ErrStatus(3))
	{
		sprintf(attritemstr[5],"�쳣");
	}
	else 
	{
		sprintf(attritemstr[5],"����");
	}	

	if(Get_AD_ErrStatus(4))
	{
		sprintf(attritemstr[6],"�쳣");
	}
	else 
	{
		sprintf(attritemstr[6],"����");
	}	

	ADChipErr = Get_ADChip_ErrStatus();
	if(ADChipErr > 0)
	{
 		if(ADChipErr & ERROR_AD1) snprintf(buf, 15, "1 ");
 		if(ADChipErr & ERROR_AD2) snprintf(buf+2,15-2,"2 ");
 		if(ADChipErr & ERROR_AD3) snprintf(buf+2,15-4,"3 ");
 		if(ADChipErr & ERROR_AD4) snprintf(buf+2,15-6,"4 ");
 		snprintf(attritemstr[7], 15, "%s·�쳣", buf);
	}
	else if(ADChipErr == 0)
	{
		sprintf(attritemstr[7],"����");
	}	
	else
	{
		;
	}
	
	if(Get_Tire_ErrStatus())//�ж���̥ʶ����״̬
	{
		sprintf(attritemstr[8],"����");
	}
	else 
	{
		sprintf(attritemstr[8],"�쳣");
	}	
}

void Form_Warning_Timer(void *ptmr, void *parg)
{
	LPWindow lpWindow = (LPWindow)parg;
    static u32 Error_Status_Temp=0;
	if(lpWindow != NULL)
	{
		gSysStatus = Get_Device_Status();
		
		if(Error_Status_Temp != gSysStatus)	   //���������仯
		{	
			Error_Status_Temp = gSysStatus;	   
		}
		
		Device_status();

		PostWindowMsg(lpWindow, WM_UPDATECTRL, (uint32)&mWarningAttr, 0);
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

//add_by_StreakingMCU
char m_USART6_test_flag = 0;//SDK���ڲ��Ա�־
#include "serial_protocol.h"
#include "overtime_queue.h"
void Form_Warning_Proc(LPWindow pWindow, LPGuiMsgInfo pGuiMsgInfo)
{
	CControl* pControl;	
	TaskMsg msg = {0};

	switch(pGuiMsgInfo->ID)
	{
		case WM_LOAD:
			ClearScreen();							//����
			SysTimeDly(3);

			//����1
			sprintf(attritemstr[9],"�̽�TR��ȷ��");	
			//SDK����
			sprintf(attritemstr[10],"�̽�TR��ȷ��");

			CTRL_CONTENT(mWarningAttr).focus = 0;	//���ͣ��λ��
			CreateWindowTimer(&gWD_Warning);
			StartWindowTimer(&gWD_Warning);

		case WM_SHOW:
			//���ù�������
			Device_status();								
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
//			PostWindowMsg(WM_UPDATECTRL, (uint32)&gStatusBar, 0);
			gStatusBar.DrawFunc(&gStatusBar);
			break;

		case WM_KEYDOWN:
			switch(pGuiMsgInfo->wParam)
			{
				case KEY_UP:
                    if (CTRL_CONTENT(mWarningAttr).focus > 0) {
                        CTRL_CONTENT(mWarningAttr).focus--;
            			PostWindowMsg(pWindow, WM_UPDATECTRL, (uint32)&mWarningAttr, 0);
                    }
					break;
				case KEY_DOWN:
                    if (CTRL_CONTENT(mWarningAttr).focus + 1 < CTRL_CONTENT(mWarningAttr).total) {
                        CTRL_CONTENT(mWarningAttr).focus++;
            			PostWindowMsg(pWindow, WM_UPDATECTRL, (uint32)&mWarningAttr, 0);
                    }
					break;

				case KEY_LEFT:
					break;

				case KEY_OK:
					//SDK���ڲ���
					if(0x0A == CTRL_CONTENT(mWarningAttr).focus)
					{
						m_USART6_test_flag = 0;
						//����SDK������Ϣ
						msg.msg_id = Msg_USARTSDKTest;
						Task_QPost(&DataManagerMsgQ,  &msg);
						
						//��ʱ200ms
						SysTimeDly(20);
						if(1 == m_USART6_test_flag)	//����յ����͵�����
						{
							sprintf(attritemstr[10],"����        ");
						}
						else
						{
							sprintf(attritemstr[10],"�쳣        ");
						}
							
					}
					//����1����
					if(0x09 == CTRL_CONTENT(mWarningAttr).focus)
					{
						
					}
					
					PostWindowMsg(pWindow, WM_UPDATECTRL, (uint32)&mWarningAttr, 0);
					break;

				case KEY_RIGHT: 
					break;

				case KEY_BACK:
					StopWindowTimer(&gWD_Warning);
// 					DestoryWindowTimer(&gWD_Warning);
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



