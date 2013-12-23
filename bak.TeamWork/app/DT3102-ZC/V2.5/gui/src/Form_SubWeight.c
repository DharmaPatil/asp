#include "includes.h"
#include "Form_SubWeight.h"

#include "Ctr_StatusBar.h"
#include "Form_Main.h"

#include "Form_Resource.h"
#include "TextEdit.h"
#include "Menu.h"
#include "gdi.h"

#include "wet_algorim.h"		//����ʽ
#include "sys_config.h"
#include "weight_param.h"
#include "task_timer.h"
#include "StaticProc.h"

#include "Form_MsgBox.h"

#include "string.h"

/*//�������ݽӿ�
 *adͨ���ɳ��ؼ���ģʽ����
 *SCS_ZC_1:
	PreAxle1AD ScalerAD��Ч; PreAxle1AD��ʾ��̨��һ·AD, ScalerAD��ʾ��̨�ڶ�·AD
 *SCS_ZC_2:
	PreAxle1AD ScalerAD��Ч; PreAxleAD��ʾǰ��ʶ����AD, ScalerAD��ʾ��̨AD 
 *SCS_ZC_3:
	PreAxle1AD ScalerAD BackAxle��Ч; PreAxleAD��ʾǰ��ʶ����AD, ScalerAD��ʾ��̨AD,BackAxle��ʾ����ʶ����AD
 *SCS_ZC_5,SCS_ZC_6:
	PreAxle1AD PreAxle2AD ScalerAD BackAxle��Ч; PreAxle1AD,PreAxle2AD��ʾС��̨AD, ScalerAD��ʾ��̨AD,BackAxle��ʾ����ʶ����AD
	
typedef struct {
	s32 Wet;				//���ӷֶ�����
	s32 Axle1_Wet;			//ǰ������
  s32 Axle2_Wet;			//��������
	u32 PreAxle1AD;			//adֵ
	u32 PreAxle2AD;
  u32 ScalerAD;
	u32 BackAxleAD;
	u8  ScalerStableFlag;				//��̨�ȶ���־
	u8	ScalerZeroFlag;				//��̨��λ��־
}WetStatue; 

*/



extern  CControl  gStatusBar; 		//״̬��

static float fSmallWet_Value   = 0.0;
static float fBigWet_Value   = 0.0;
static float fTotalWet_Value   = 0.0;

static __align(8) char sADChannel_Title[24] = {0};


static void Form_ADChannel_Timer(void *ptmr, void *parg);
static void Form_ADChannel_Draw(LPWindow pWindow);
static void Form_ADChannel_Proc(LPWindow pWindow, LPGuiMsgInfo pGuiMsgInfo);
 
DEF_SMALL_FLOAT_LABLE(mSmallScaler,   &gWD_SubWeight, 70,  44, 	170,  14, CTRL_VISABLE, &fSmallWet_Value, 4,  -5000000, 5000000,    "", TA_LEFT);
DEF_SMALL_FLOAT_LABLE(mBigScaler,   &gWD_SubWeight, 70,  66, 	170,  14, CTRL_VISABLE, &fBigWet_Value, 4,  -5000000, 5000000,    "", TA_LEFT);
DEF_SMALL_FLOAT_LABLE(mTotalScaler,   &gWD_SubWeight, 70,  88, 	170,  14, CTRL_VISABLE, &fTotalWet_Value, 4,  -5000000, 5000000,    "", TA_LEFT);




static const LPControl marrLPControl[] = 
{
    &gStatusBar,
	&mSmallScaler,
    &mBigScaler,
	&mTotalScaler,
};

CWindow  gWD_SubWeight = {
    (LPControl *)marrLPControl,
	sizeof(marrLPControl)/sizeof(LPControl),
    0,
    0,
    0,
    240,
    128,
    WS_VISABLE,
	NULL,//(pParentWindow)
    5,//5
    NULL,
    Form_ADChannel_Timer,
    Form_ADChannel_Draw,
    Form_ADChannel_Proc
};

static WetStatue wet_chanle = {0};

void Form_ADChannel_Timer(void *ptmr, void *parg)
{
	LPWindow lpWindow = (LPWindow)parg;
	
#if SYS_CRITICAL_METHOD == 3  	 
    SYS_CPU_SR  cpu_sr = 0;
#endif

	Get_Scaler_WetPar(&wet_chanle);
	if(lpWindow != NULL) {
    	if(g_pCurWindow == &gWD_SubWeight) 
		{
			SYS_ENTER_CRITICAL();
			fSmallWet_Value = GetSmallWet();
			fBigWet_Value = GetBigWet();
			fTotalWet_Value = wet_chanle.Wet;
			SYS_EXIT_CRITICAL();

 			PostWindowMsg(lpWindow, WM_UPDATECTRL, 0, 0);		

    	}
    }
}

void Form_ADChannel_Draw(LPWindow pWindow)
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
    
    DrawGbText("������",5,44);
    DrawGbText("��������",5,66);
    DrawGbText("������",5,88);

    snprintf(sADChannel_Title, 23, "�����鿴 SCS-ZC-%d", (Get_System_Mode() + 1));

	DrawGbText(sADChannel_Title, 5, 21);
    DrawHoriLine(0, 40, 240);
    DrawHoriLine(0, 62, 240);
    DrawHoriLine(0, 84, 240);
    DrawHoriLine(0, 106, 240);
//     DrawHoriLine(0, 127, 240);
    DrawVertLine(60, 40, 66);
//     DrawVertLine(140, 40, 44);

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

void Form_ADChannel_Proc(LPWindow pWindow, LPGuiMsgInfo pGuiMsgInfo)
{
	switch(pGuiMsgInfo->ID)
	{
		case WM_LOAD:
			ClearScreen();
			WeightFregDiv(TRUE, 50);
			SysTimeDly(3);
			CreateWindowTimer(&gWD_SubWeight);
			StartWindowTimer(&gWD_SubWeight);

		case WM_SHOW:
			pWindow->DrawFunc(pWindow);
			break;

		case WM_UPDATECTRL:
			mSmallScaler.DrawFunc(&mSmallScaler);
			mBigScaler.DrawFunc(&mBigScaler);
			mTotalScaler.DrawFunc(&mTotalScaler);

			break;

		case WM_TIMEUPDATE:
//			gWD_ADChannel.DrawFunc(&gWD_ADChannel);
			gStatusBar.DrawFunc(&gStatusBar);
//			PostWindowMsg(WM_UPDATECTRL, (uint32)&gStatusBar, 0);
			break;

		case WM_KEYDOWN:
			switch(pGuiMsgInfo->wParam)
			{
				case KEY_UP:
					break;
				case KEY_DOWN:
					break;

				case KEY_LEFT:
					break;

				case KEY_OK:		
					break;

				case KEY_RIGHT: 
					break;
				case KEY_ZERO:
          Scaler_Set_Zero(FALSE);
					break;

				case KEY_BACK:
					WeightFregDiv(FALSE, 0);
          StopWindowTimer(&gWD_SubWeight);
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

				case KEY_1:
				case KEY_2:
				case KEY_3:
				case KEY_4:
				case KEY_5:
				case KEY_6:
				case KEY_7:
				case KEY_8:
				case KEY_9:
					break;
				default:
					break;
			}
			break;
		
		default:
			break;
	}	
}



