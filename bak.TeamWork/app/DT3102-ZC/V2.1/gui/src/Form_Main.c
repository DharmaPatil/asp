#include "Form_Main.h"
#include "Form_Resource.h"
#include "Ctr_StatusBar.h"
#include "../Inc/TextEdit.h"

#include "Form_Main_Veh.h"
#include "Form_Main_Axle.h"
#include "Form_MainMenu.h"
#include "Form_Query.h"
#include "form_warning.h"

// #include "Ctr_DlgList.h"
#include "Form_MsgBox.h"
#include "Common.h"

#include "overtime_queue.h"
#include "car_business.h"
#include "wet_algorim.h"
#include "weight_param.h"

#include "Form_OutPortTest.h"



static float GUI_fAxleWet  = 0;//����
static float GUI_fTotalWet = 0;//��ǰ������������
static float GUI_fWetAll   = 0;//��ǰ��̨����


static float GUI_fSpeed   = 0;//����
OvertimeData gMainCarInfo = {{0}};
// OvertimeData gMainCarInfo = {{0}};



static uint8 caring = 0; 			//��ǰ��̨�ϵĳ�������
static BOOL backcarflag = FALSE;
//static uint8 overload = 0; 		//�жϳ����Ƿ���
static uint8 axle_show = 0;   //��������
static uint8 axle_type = 0;   //0: �޳�  1: ����  2: ˫��

static void Form_Main_Timer(void *ptmr, void *parg);
static void Form_Main_Draw(LPWindow pWindow);
static void Form_Main_Proc(LPWindow pWindow, LPGuiMsgInfo pGuiMsgInfo);

//DEF_SMALL_FLOAT_LABLE(mAxleVal, &gWD_Main, 5,   46,  60,  14, CTRL_VISABLE, &GUI_fAxleWet,  0, -5, 100000, "",   TA_CENTER);

DEF_STATUS_IMAGE_CTRL(m_Zero,   &gWD_Main, 225, 5, 14,  14, CTRL_VISABLE,  Img_zero, Img_nonezero, TRUE);
DEF_STATUS_IMAGE_CTRL(m_Steady, &gWD_Main, 225, 22, 14,  14, CTRL_VISABLE, Img_steady, Img_unsteady, TRUE);

DEF_TEXT_LABEL(mGUI_fTotalWet,              &gWD_Main, 30, 65, 30,  14, CTRL_VISABLE, "����");
DEF_SMALL_FLOAT_LABLE(mGUI_fTotalWetValue,  &gWD_Main, 10, 85, 50,  14, CTRL_VISABLE, &GUI_fTotalWet,  0, -1000, 1000000, "kg",   TA_RIGHT);


DEF_TEXT_LABEL(m_labelSpeed,    &gWD_Main,  170,  65, 30,   14, CTRL_VISABLE, "����");
DEF_SMALL_FLOAT_LABLE(mSpeed,   &gWD_Main,  170,  85, 50,   14, CTRL_VISABLE, &GUI_fSpeed, 1,  -100, 100, "km/h", TA_CENTER);
//DEF_TEXT_LABEL(mGUI_fWetAll,         &gWD_Main, 85,  10, 60,  14, CTRL_VISABLE, "��������:");

DEF_BIG_FLOAT_LABLE(mGUI_fWetAllValue,  &gWD_Main, 0, 10, 200,  30, CTRL_VISABLE, &GUI_fWetAll,  120, -100000, 200000, "",   TA_RIGHT);

DEF_TEXT_LABEL(m_labelAlex,      &gWD_Main, 100,   65, 30,   14, CTRL_VISABLE, "����");
DEF_SMALL_FLOAT_LABLE(m_labelAlexNum,  &gWD_Main, 100, 85, 50,  14, CTRL_VISABLE, &GUI_fTotalWet,  0, -1000, 1000000, "",   TA_CENTER);


DEF_TEXT_LABEL(m_labelSysCfg,   &gWD_Main,  10,   112,  48,   14, CTRL_VISABLE, "ϵͳ");
DEF_TEXT_LABEL(m_labelCalib,    &gWD_Main,  58,  112,  48,   14, CTRL_VISABLE, "�궨");
DEF_TEXT_LABEL(m_labelWetCfg,   &gWD_Main,  102,  112,  48,   14, CTRL_VISABLE, "����");
DEF_TEXT_LABEL(m_labelMenu,     &gWD_Main,  150, 112,  48,   14, CTRL_VISABLE, "�˵�");
DEF_TEXT_LABEL(m_labelZD,       &gWD_Main,  198,  112, 48,   14, CTRL_VISABLE, "���");

static const LPControl		marrLPControl[] = 
{
    //&gStatusBar,
    //&mAxleVal,
    &m_Zero,
    &m_Steady,
    &mGUI_fTotalWet,
    &mGUI_fTotalWetValue,
		&m_labelSpeed,
    &mSpeed,
    &mGUI_fWetAllValue,
		&m_labelAlex,
	//&m_labelAlexNum,
	&m_labelSysCfg,
	&m_labelCalib,
	&m_labelWetCfg,
	&m_labelMenu,
	&m_labelZD
    //&mGUI_fWetAll ,

};

CWindow  gWD_Main = {
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
    Form_Main_Timer,
    Form_Main_Draw,
    Form_Main_Proc
};

static void Draw_ScalerCar_Status(void)
{
	caring = CarQueue_Get_OnScaler_Count();//��ȡ��̨�ϳ�������������Ҫ�жϳ�̨�������Ƿ����200kg
	
	GUI_fWetAll = Get_Scaler_DrvWeight(); 
#if 0
	//ʵʱ�жϳ�̨���Ƿ��г�����ʱ�õ��ĳ�̨������λΪKG
	if(caring > 0)	
	{
		if(!backcarflag) 
		{
			if((GUI_fWetAll > 500.0f)) 
				DrawGbText("�г�", 85, 22);	
		}
	} 
	else 
	{
		DrawGbText("�޳�", 85, 22);
	}
#endif
	
}

//static WetStatue wet_main = {0};
static WetStatue wet = {0};
void Form_Main_Timer(void *ptmr, void *parg)
{
 	LPWindow lpWindow = (LPWindow)parg;

	Get_Scaler_WetPar(&wet);
	if(lpWindow != NULL)
	{
    	if(g_pCurWindow == &gWD_Main)
    	{
           if(GUI_fWetAll != Kg2CurVal(GUI_fWetAll))    //��λת������  kg��tת��
                GUI_fWetAll = Kg2CurVal(GUI_fWetAll);
         
           
		   PostWindowMsg(lpWindow, WM_UPDATECTRL,(u32)&mGUI_fWetAllValue,0);
//�˴��ж�״̬������Ƿ��ȶ�
			if(CTRL_CONTENT(m_Zero).bHot != wet.ScalerZeroFlag) {
				CTRL_CONTENT(m_Zero).bHot = wet.ScalerZeroFlag;
				PostWindowMsg(lpWindow, WM_UPDATECTRL,(u32)&m_Zero,0);
			}
			if(CTRL_CONTENT(m_Steady).bHot != wet.ScalerStableFlag) {
				CTRL_CONTENT(m_Steady).bHot = wet.ScalerStableFlag;
				PostWindowMsg(lpWindow, WM_UPDATECTRL,(u32)&m_Steady,0);
			}
 		}
	}
 }
 
static void UpDateVehInfo(OvertimeData *pcar, LPWindow pWindow)
{
    char buf[16] = {0};
#if 0
    if(axle_show) {
        if(GUI_fAxleWet!=Kg2CurVal(pcar->AxleWet[axle_show - 1])) {
            GUI_fAxleWet=Kg2CurVal(pcar->AxleWet[axle_show - 1]);
            PostWindowMsg(pWindow, WM_UPDATECTRL,(u32)&mAxleVal,0);
        }
         axle_type = pcar->AxleType[axle_show - 1];
			
    } else {
        if(GUI_fAxleWet!=0) {
            GUI_fAxleWet=0;
            PostWindowMsg(pWindow, WM_UPDATECTRL,(u32)&mAxleVal,0);
        }
        axle_type = 0;
    }

	if( caring > 0 )
	{
        DrawGbText("�г�", 85, 22);
    } else {
        DrawGbText("�޳�", 85, 22);
    }

	if(1 == pcar->OverWetFlag) {
        DrawGbText("����", 150, 22);
    } else {
        DrawGbText("    ", 150, 22);
    }

    snprintf(buf,15,"��%2d/%2d��",axle_show,pcar->AxleNum);
    DrawGbText(buf, 5, 22);	
    switch(axle_type) {
	    case 0:
	        DrawGbText("    ", 26, 70);
	        break;
	    case 1:
	        DrawGbText("����", 26, 70);
	        break;
	    case 2:
	        DrawGbText("˫��", 26, 70);
	        break;
	    default:
	        DrawGbText("    ", 26, 70);
	        break;
    }
#endif
	snprintf(buf,15,"%2d ��",pcar->AxleNum);
	DrawGbText(buf, 100, 85);	
		
	GUI_fSpeed = 0.1f * pcar->speed;
    
    if(GUI_fTotalWet != Kg2CurVal(pcar->TotalWet)) {
        GUI_fTotalWet = Kg2CurVal(pcar->TotalWet);
    }
		EraseRect(10, 85, 50,     14); //total weight
	  EraseRect(170,  85, 50,   14); //speed
	  PostWindowMsg(pWindow, WM_UPDATECTRL,(u32)&mGUI_fTotalWetValue,0);
	  PostWindowMsg(pWindow, WM_UPDATECTRL,(u32)&mSpeed,0);

}

void Form_Main_Draw(LPWindow pWindow)
{
	uint16 i;
	LPControl	lpControl;

	//����Դ�
	EraseBuffer();

	//��ֹˢ��
	EnableScreenFlush(FALSE);

	//��ֹ��ͼ
	SetRedraw(FALSE);

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
    UpDateVehInfo(&gMainCarInfo, pWindow);
//    DrawImage(Img_Left,  1, 85, 12, 12);
//    DrawImage(Img_Right, 67, 85, 12, 12);
    
	//����������
  	DrawHoriLine(0, 60, 240);
    DrawHoriLine(0, 110 , 240);
	
    DrawVertLine(48, 110 , 128);
		DrawVertLine(92, 110 , 128);
		DrawVertLine(140, 110 , 128);
		DrawVertLine(188, 110 , 128);
//    DrawVertLine(160, 100 , 28);
	
	//ʹ��ˢ��
	EnableScreenFlush(TRUE);

	//ˢ��
	FlushScreen();

	//ʹ�ܻ�ͼ
	SetRedraw(TRUE);
}

void Form_Main_Proc(LPWindow pWindow, LPGuiMsgInfo pGuiMsgInfo)
{
	CControl* pControl;	
	GuiMsgInfo guiMsgInfo;
	char UnitBuff[20]={0};
	
	switch(pGuiMsgInfo->ID)
	{
		case WM_LOAD:
			ClearScreen();
			WeightFregDiv(FALSE, 0);
			SysTimeDly(15);
			CreateWindowTimer(&gWD_Main);
			StartWindowTimer(&gWD_Main);
      		axle_show = gMainCarInfo.AxleNum;
      		
		case WM_SHOW:
//			strcpy(CTRL_CONTENT(mAxleVal).unit,GetCurUnitStr());
            if(GetCurUnit()) {
                CTRL_CONTENT(mGUI_fWetAllValue).nFixPoint       = 2;

                CTRL_CONTENT(mGUI_fTotalWetValue).nFixPoint      = 2;
                //CTRL_CONTENT(mAxleVal).nFixPoint     = 2;
            } else {
                CTRL_CONTENT(mGUI_fWetAllValue).nFixPoint       = 0;

                CTRL_CONTENT(mGUI_fTotalWetValue).nFixPoint      = 0;
                //CTRL_CONTENT(mAxleVal).nFixPoint     = 0;
            }
			pWindow->DrawFunc(pWindow);
			break;
		case WM_UPDATECTRL:
			Draw_ScalerCar_Status();
			SysTimeDly(2);
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
					break;
				case KEY_DOWN:
					break;
				case KEY_LEFT:
					break;
				case KEY_RIGHT:
         
					break;
 				case KEY_OK:
					break;
				case KEY_AXIS:
          
					break;
				case KEY_CAR:
              
           break;
				case KEY_DIAGNOSIS:
					StopWindowTimer(&gWD_Main);
					g_pCurWindow = &gWD_Warning;
					g_pCurWindow->pParentWindow = &gWD_Main;
					guiMsgInfo.pWindow = g_pCurWindow;
					guiMsgInfo.ID = WM_LOAD;
					GuiMsgQueuePost(&guiMsgInfo);
					break;
				case KEY_QUERY:
                 
					break;
				case KEY_UNIT:
					StopWindowTimer(&gWD_Main);
					SwitchUnit();	//��λת��
					snprintf(UnitBuff,19,"��ǰʹ�õ�λ:%s",GetCurUnitStr());
					MsgBoxDlg(&g_MsgBoxDlg, "��λת��", UnitBuff);
					StartWindowTimer(&gWD_Main);

					break;
				case KEY_MENU:
					StopWindowTimer(&gWD_Main);
					g_pCurWindow = &gWD_MainMenu;
					PostWindowMsg(g_pCurWindow, WM_LOAD,1,1);
					break;		
                    
				case KEY_ZERO:
          Scaler_Set_Zero(FALSE);
					break;
				default:
					break;
			}
			break;
     case WM_CARIN:
				memcpy(&gMainCarInfo, (void *)pGuiMsgInfo->wParam, sizeof(OvertimeData));
				caring = CarQueue_Get_OnScaler_Count();
				axle_show = 1;
				backcarflag = FALSE;
				UpDateVehInfo(&gMainCarInfo, pWindow);
			break;
     case WM_CARBACK:
						backcarflag = TRUE;
						//DrawGbText("����", 85, 22);
            break;
    	case WM_AXELADD:

            break;
    	case WM_AXELSUB:

            break;
    	case WM_VEHWET:

		
		default:
			break;
	}	
}

