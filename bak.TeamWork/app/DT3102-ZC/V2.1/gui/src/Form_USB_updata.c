#include "Form_USB_update.h"
#include "Form_Main.h"
#include "Form_MainMenu.h"
#include "Form_MsgBox.h"
#include "includes.h"
#include "Form_Main.h"
#include "Form_Resource.h"
#include "includes.h"
#include "system_init.h"

#include "driver.h"
#include "task_def.h"
#include "task_timer.h"
#include "key_thread.h"
#include "timer2.h"
#include "debug_info.h"


//��������
static int8* errnoArr[]={
	"��ʼ������",			// 0
	"���ػ��ȡ�ļ�����",	// 1 		//usb device errno information
	"���ļ�����,�����ļ�",// 2
	"CRC�ļ�����",			// 3
	"��ȡ�����ļ�����ȫ",	// 4
	"CRCУ�����",			// 5
	"�����ļ�����",			// 6
	"������洢FLASH����",	// 7
	"��ȷ�ϼ���ʼ",			// 8		//gui information
	"��ʼ���U��,�����ĵȴ�",			// 9
	"����������,������...",	// 10
	"δ��⵽U��,�����²���U��",	// 11
	"�����ɹ�,��ϵͳ��������Ч",	// 12
	"����ʧ��",						// 13
	"������ʱ,��γ�U�̺����¿�ʼ"	// 14
};

volatile static u8 count=0;	//��ʱ������
static u8 OverTimeFlag=0;	//0-δ��ʱ��1-�ѳ�ʱ
static HANDLE Tim2_dev=HANDLENULL;
extern  CControl  gStatusBar; 		//״̬��

static void Form_USB_Updata_Draw(LPWindow pWindow);
static void Form_USB_Updata_Proc(LPWindow pWindow, LPGuiMsgInfo pGuiMsgInfo);


DEF_TEXT_LABEL(mInformation, &gWD_USB_Updata, 10, 60, 90, 20, CTRL_VISABLE,NULL);
DEF_TEXT_LABEL(mTips, &gWD_USB_Updata, 10, 90, 90, 20,0,"���6���û����ʾ,��γ�u��!");//"�ɰ�ȷ�ϼ����¿�ʼ");


static const LPControl marrLPControl[] = 
{
    &gStatusBar,
	&mInformation,
	&mTips
};

CWindow  gWD_USB_Updata = {
    (LPControl *)marrLPControl,
	sizeof(marrLPControl)/sizeof(LPControl),
    0,
    0,
    0,
    240,
    128,
    WS_VISABLE,
	NULL,//(pParentWindow)
    1,//1 tick=10ms
    NULL,
    NULL,			//Form_USB_Updata_Timer,
    Form_USB_Updata_Draw,
    Form_USB_Updata_Proc
};


//system timer to check updata time out
static void USB_timeout(void)
{
//	GuiMsgInfo guiMsgInfo={WM_SHOW,0xf0f0,14};
	if(++count==60)		//10s
	{
		Key_Enable(TRUE);	//open key	
//		OverTimeFlag=1;		
//		GuiMsgQueuePost(&guiMsgInfo);
	}
}
static void Form_USB_Updata_Draw(LPWindow pWindow)
{
	uint16 i;
	LPControl	lpControl;
	
	EraseBuffer();//����Դ�		
	SetRedraw(FALSE);//��ֹ��ͼ
	EnableScreenFlush(FALSE);//��ֹˢ��
	//������ͼ
	SetGdiView(pWindow->nViewPosX, pWindow->nViewPosY, pWindow->nViewSizeX, pWindow->nViewSizeY);
	EnableGdiView(TRUE);

    DrawGbText("һ��ʽU������", 0, 25);
    DrawHoriLine(0, 25+16, 240);
    DrawHoriLine(0, 25+18, 240);
	for(i = 0; i < pWindow->nNbControls; i++)//���ƿؼ�
	{
		lpControl = *(pWindow->pLPControls + i);
		if(lpControl->state & CTRL_VISABLE)
		{
			lpControl->DrawFunc(lpControl);
		}
	}	
	EnableScreenFlush(TRUE);//ʹ��ˢ��	
	FlushScreen();//ˢ��
	SetRedraw(TRUE);//ʹ�ܻ�ͼ
}

static void Form_USB_Updata_Proc(LPWindow pWindow, LPGuiMsgInfo pGuiMsgInfo)
{
	CControl* pControl;	
	GuiMsgInfo guiMsgInfo;
	u8 err;
	switch(pGuiMsgInfo->ID)	{
	case WM_LOAD:
		ClearScreen();
		mInformationCC.s="��ȷ�ϼ���ʼ";
		SysTimeDly(15);
		OverTimeFlag=0;
		if(Tim2_dev==HANDLENULL && TIM2_Config(USB_timeout,200,Mode_500US)==1)
		{
			Tim2_dev=DeviceOpen(CHAR_TIM2,0,&err);		
			if(err!=HVL_NO_ERR)
			{
				debug(Debug_Error,"create hard timer error!!!\r\n");
			}
		}
	case WM_SHOW:
		//mInformationCC.s=errnoArr[8];	//default;
		mTips.state=0;
		if(pGuiMsgInfo->wParam==0xf0f0)	//post by usb thread or OK key
		{
			mInformationCC.s = (char*)errnoArr[pGuiMsgInfo->lParam];
			if(pGuiMsgInfo->lParam==9)		//��ʼ���,��OK������
			{
				Key_Enable(FALSE);			//close key
				SysTaskResume(USBPrio);		//resume usb therad			
			}
			else if(pGuiMsgInfo->lParam==10) //��⵽U��,��ʼ��������ʱ
			{
				mTips.state=CTRL_VISABLE;
				if(DeviceIoctl(Tim2_dev,TIM2_CMDTimOpen,0)==0)	//������ʱ��
				{
					debug(Debug_Error,"start timer 6 error!!!\r\n");
				}
// 				debug(Debug_Notify,"start tick:%d\r\n",Get_Sys_Tick());
			}
			else
			{
				//hard tiemr
				if(DeviceIoctl(Tim2_dev,TIM2_CMDTimClose,0)==0)
				{
					debug(Debug_Error,"stop timer 6 error!!!\r\n");;
				}
				DeviceIoctl(Tim2_dev,TIM2_CMDClearCnt,0);
				count=0;
				if(pGuiMsgInfo->lParam!=14)
				{
//					mTips.state=CTRL_VISABLE;
					Key_Enable(TRUE);	//open key
				}
				if(pGuiMsgInfo->lParam<8)
					OverTimeFlag=0;
// 				debug(Debug_Notify,"end tick:%d\r\n",Get_Sys_Tick());
			}
		}			
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
		switch(pGuiMsgInfo->wParam)	{
		case KEY_OK:
			if(OverTimeFlag)
				break;
			guiMsgInfo.ID=WM_SHOW;
			guiMsgInfo.wParam=0xf0f0;
			guiMsgInfo.lParam=9;
			guiMsgInfo.pWindow = pWindow;
			GuiMsgQueuePost(&guiMsgInfo);
			break;
		case KEY_BACK:
			if(pWindow->pParentWindow != NULL)
			{
				g_pCurWindow =pWindow->pParentWindow;	//&gWD_TimeDate;
			}
			else
			{
				g_pCurWindow = &gWD_Main;
			}
			PostWindowMsg(g_pCurWindow, WM_LOAD,0,0);
			guiMsgInfo.pWindow = g_pCurWindow;			
			guiMsgInfo.ID = WM_LOAD;
			GuiMsgQueuePost(&guiMsgInfo);
			break;
		default:
			break;
		}
	}
}



