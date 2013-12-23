#include "includes.h"
#include "Form_ADChannel.h"

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

static float sADChannel1_Value   = 0.0;
static float sADChannel2_Value   = 0.0;
static float sADChannel3_Value   = 0.0;
static float sADChannel4_Value   = 0.0;

 static __align(8) char sADChannel_Title[24] = {0};


//static long *pADValue1,*pADValue2;

static void Form_ADChannel_Timer(void *ptmr, void *parg);
static void Form_ADChannel_Draw(LPWindow pWindow);
static void Form_ADChannel_Proc(LPWindow pWindow, LPGuiMsgInfo pGuiMsgInfo);
 
DEF_SMALL_FLOAT_LABLE(mADChannel1,   &gWD_ADChannel, 70,  44, 	170,  14, CTRL_VISABLE, &sADChannel1_Value, 4,  -5000000, 5000000,    "", TA_LEFT);
DEF_SMALL_FLOAT_LABLE(mADChannel2,   &gWD_ADChannel, 70,  66, 	170,  14, CTRL_VISABLE, &sADChannel2_Value, 4,  -5000000, 5000000,    "", TA_LEFT);
DEF_SMALL_FLOAT_LABLE(mADChannel3,   &gWD_ADChannel, 70,  88, 	170,  14, CTRL_VISABLE, &sADChannel3_Value, 4,  -5000000, 5000000,    "", TA_LEFT);
DEF_SMALL_FLOAT_LABLE(mADChannel4,   &gWD_ADChannel, 70,  110, 	170,  14, CTRL_VISABLE, &sADChannel4_Value, 4,  -5000000, 5000000,    "", TA_LEFT);




static const LPControl marrLPControl[] = 
{
    &gStatusBar,
	&mADChannel1,
    &mADChannel2,
	&mADChannel3,
	&mADChannel4,
};

CWindow  gWD_ADChannel = {
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
    	if(g_pCurWindow == &gWD_ADChannel) 
		{
			SYS_ENTER_CRITICAL();
			sADChannel1_Value = wet_chanle.ScalerAD;
			sADChannel2_Value = wet_chanle.BackAxleAD;		//
			sADChannel3_Value = wet_chanle.PreAxle1AD;
			sADChannel4_Value = wet_chanle.PreAxle2AD;   //
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
    
    DrawGbText("ADͨ��1",5,44);
    DrawGbText("ADͨ��2",5,66);
    DrawGbText("ADͨ��3",5,88);
    DrawGbText("ADͨ��4",5,110);

    snprintf(sADChannel_Title, 23, "ͨ����ADֵ SCS-ZC-%d", (Get_System_Mode() + 1));

	DrawGbText(sADChannel_Title, 5, 21);
    DrawHoriLine(0, 40, 240);
    DrawHoriLine(0, 62, 240);
    DrawHoriLine(0, 84, 240);
    DrawHoriLine(0, 106, 240);
    DrawHoriLine(0, 127, 240);
    DrawVertLine(60, 40, 88);

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
			WeightFregDiv(TRUE, 10);
			SysTimeDly(3);
			CreateWindowTimer(&gWD_ADChannel);
			StartWindowTimer(&gWD_ADChannel);

		case WM_SHOW:
			pWindow->DrawFunc(pWindow);
			break;

		case WM_UPDATECTRL:
			mADChannel1.DrawFunc(&mADChannel1);
			mADChannel2.DrawFunc(&mADChannel2);
			mADChannel3.DrawFunc(&mADChannel3);
			mADChannel4.DrawFunc(&mADChannel4);
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
				    //MsgBoxDlg(&g_MsgBoxDlg, "����ʽ���궨�ɹ�", "����������ʽ����������������");
//				
//					g_pCurWindow = &gWD_CalibrateInput;					//ʱ��
//					g_pCurWindow->pParentWindow = &gWD_CalibrateWeight;
//					PostWindowMsg(WM_LOAD, NULL, NULL);	
//				
					break;

				case KEY_RIGHT: 
					break;

				case KEY_BACK:
					WeightFregDiv(FALSE, 0);
          			StopWindowTimer(&gWD_ADChannel);
//	           		 CalibrateProc();
//	           		 CalibParamSync();
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



