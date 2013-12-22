#include "Form_CalibrateWeightMode1.h"

#include "Ctr_StatusBar.h"
#include "Form_Main.h"

#include "Form_Resource.h"
#include "../Inc/TextEdit.h"
#include "../Inc/Menu.h"

#include "Form_MsgBox.h"
#include "Common.h"

#include "wet_algorim.h"		//����ʽ
#include "sys_config.h"
#include "weight_param.h"

#include "string.h"

extern  CControl  gStatusBar; 		//״̬�

extern  BOOL CalibrateZeroFlag;		//�궨��ɱ�־�

enum  _CalibPosition {
    Calib_Position0=0,
 
    Calib_Position1,
    Calib_Position2,
    Calib_Position3,
    Calib_Position4,
    Calib_Position5,
    Calib_End
};
enum  _CalibStat {
    Calib_Step1=1,
    Calib_Step2,
    Calib_Step3,
    Calib_Step4,
		Calib_Step5,
};
static char gCalibPosition = Calib_Position0;
static char gCalibStat  = Calib_Step1;
static long lWeights_Value;   // ������
static char CalibPosition[10] = "";


static char sScale_Title[10] = "";
static char sCali_Title[30] = "";

static float sAD1_Value   = 0.0;
static float sAD2_Value   = 0.0;
static float sWeightMode1_Value   = 0.0;



static void Form_CalibrateWeightMode1_Timer(void *ptmr, void *parg);
static void Form_CalibrateWeightMode1_Draw(LPWindow pWindow);
static void Form_CalibrateWeightMode1_Proc(LPWindow pWindow, LPGuiMsgInfo pGuiMsgInfo);


DEF_SMALL_FLOAT_LABLE(mAD1_Attr,   &gWD_CalibrateWeightMode1, 45,  44, 	170,  14, CTRL_VISABLE, &sAD1_Value, 2,  -5000000, 5000000,    "", TA_LEFT);
DEF_SMALL_FLOAT_LABLE(mAD2_Attr,   &gWD_CalibrateWeightMode1, 45,  66, 	170,  14, CTRL_VISABLE, &sAD2_Value, 2,  -5000000, 5000000,    "", TA_LEFT);
DEF_SMALL_FLOAT_LABLE(mWeightMode1_Attr,   &gWD_CalibrateWeightMode1, 45,  88, 	170,  14, CTRL_VISABLE, &sWeightMode1_Value, 0,  -5000000, 5000000,    "", TA_LEFT);

DEF_STATUS_IMAGE_CTRL(mCalibMode1SteadyImage, &gWD_CalibrateWeightMode1, 220, 21, 14, 14, CTRL_VISABLE, Img_steady, Img_unsteady, FALSE);

static LPControl marrLPControl[] = 
{
    &gStatusBar,
		&mAD1_Attr,
		&mAD2_Attr,

    &mWeightMode1_Attr,
		&mCalibMode1SteadyImage,


};

CWindow  gWD_CalibrateWeightMode1 = {
    marrLPControl,
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
    Form_CalibrateWeightMode1_Timer,
    Form_CalibrateWeightMode1_Draw,
    Form_CalibrateWeightMode1_Proc
};

static WetStatue wet1 = {0};

void Form_CalibrateWeightMode1_Timer(void *ptmr, void *parg)
{
	LPWindow lpWindow = (LPWindow)parg;
//	u32 tmp = 0;

	Get_Scaler_WetPar(&wet1);

	if(lpWindow != NULL) {
    	if(g_pCurWindow == &gWD_CalibrateWeightMode1) 
		{		
			sAD1_Value = wet1.PreAxle1AD;
			sAD2_Value = wet1.ScalerAD;
			sWeightMode1_Value = wet1.Wet;
			
			if(CTRL_CONTENT(mCalibMode1SteadyImage).bHot != wet1.ScalerStableFlag) 
			{
				CTRL_CONTENT(mCalibMode1SteadyImage).bHot = wet1.ScalerStableFlag;
				PostWindowMsg(lpWindow, WM_UPDATECTRL,(u32)&mCalibMode1SteadyImage,0);
			}

			PostWindowMsg(lpWindow, WM_UPDATECTRL, 0, 0);
    	}

    }
}

void Form_CalibrateWeightMode1_Draw(LPWindow pWindow)
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

	switch (gCalibPosition) {

    case Calib_Position0:
        sprintf(sCali_Title,"%s",sScale_Title);
		DrawGbText("��ȷ������ʼ�궨�������뷵��", 42, 110);
		break;

	case Calib_Position1:
	case Calib_Position2:
	case Calib_Position3:
	case Calib_Position4:
	case Calib_Position5:
	    switch(gCalibStat) {
	        case Calib_Step1:
				DrawGbText("�궨�����У�������궨λ��", 1, 110);
	            sprintf(sCali_Title,"%s������(%dkg)",sScale_Title,lWeights_Value);
	            break;
	            
	        case Calib_Step2:			
				DrawGbText("�밴ȷ�ϼ�����궨λ��", 42, 110);
				sprintf(sCali_Title,"%s������(%dkg)",sScale_Title,lWeights_Value);
	            break;
	            
	        case Calib_Step3:
				DrawGbText("��̨�ȶ����밴ȷ�ϼ��궨�˶�λ��", 1, 110);
				sprintf(sCali_Title,"%s������(%dkg)-λ��(%d)",sScale_Title,lWeights_Value,CalibPosition[0]);
	            break;
	            
	        case Calib_Step4:	
			case Calib_Step5:							
				break;
	            
	        default:
	            break;
	    }
        break;

    case Calib_End:
		DrawGbText("�궨���", 112, 88);
		//DrawGbText("�뷵��", 42, 110);
        break;

    default:
        break;
    }
    
    DrawGbText("AD��1",5,44);
    DrawGbText("AD��2",5,66);
	DrawGbText("����",5,88);

	DrawGbText(sCali_Title, 5, 21);
    DrawHoriLine(0, 40, 240);
    DrawHoriLine(0, 62, 240);
    DrawHoriLine(0, 84, 240);
		DrawHoriLine(0, 108, 240);
    DrawHoriLine(0, 127, 240);
    DrawVertLine(40, 40, 66);
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


void Form_CalibrateWeightMode1_Proc(LPWindow pWindow, LPGuiMsgInfo pGuiMsgInfo)
{
//	CControl* pControl;	
//	GuiMsgInfo guiMsgInfo;
    char   cBuff[20]="";
    char   cTemp[10]="";

	switch(pGuiMsgInfo->ID)
	{
		case WM_LOAD:
//             memset((char *)&mCalib,0,sizeof(SCalibrationData));

//       pADValue1 = &gWeightMeas.AD1;
//       pADValue2 = &gWeightMeas.AD2;

      sprintf(sScale_Title,"�����궨");
            
			ClearScreen();
			SysTimeDly(3);
			CreateWindowTimer(&gWD_CalibrateWeightMode1);
			StartWindowTimer(&gWD_CalibrateWeightMode1);
		case WM_SHOW:
			
			
			pWindow->DrawFunc(pWindow);
			break;

		case WM_UPDATECTRL:
			mAD1_Attr.DrawFunc(&mAD1_Attr);
			mWeightMode1_Attr.DrawFunc(&mWeightMode1_Attr);
			mCalibMode1SteadyImage.DrawFunc(&mCalibMode1SteadyImage);	
		
// 			pControl = (CControl*)(pGuiMsgInfo->wParam);
// 			if(pControl != NULL)
// 			{
//          if (pControl->state & CTRL_VISABLE)
//          pControl->DrawFunc(pControl);
// 			} 			
			break;

		case WM_TIMEUPDATE:
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
					switch (gCalibPosition) {

                    case Calib_Position0:
                        sprintf(cBuff,"�������������� (%s)",GetCurUnitStr());
						if(LoadEditDlg(&g_TextEditWindow, cBuff, cTemp, cTemp, 10)) 
						{
							if (atoi(cTemp)) 
							{
								lWeights_Value = atoi(cTemp);
							
									gCalibPosition = Calib_Position1;
									gCalibStat  = Calib_Step2;
								
							}
							else
							{
								MsgBoxDlg(&g_MsgBoxDlg, "����", "����ֵ�Ƿ�������������");
							}
						}
// }
						break;

                    case Calib_Position1:
                        //if (GetWetSteadySta()) {	 //ȡ�����ȶ��ļ��
                        memset(cTemp,0,sizeof(cTemp));                      
						if(LoadEditDlg(&g_TextEditWindow, "������궨λ��(1-3)", cTemp, cTemp, 10)) 
						{
							if (atoi(cTemp)<4 && atoi(cTemp)>0) 
							{
								CalibPosition[0] = atoi(cTemp);
								
								if(CalibPosition[0] == 1 && CalibrateZeroFlag)
								{
										gCalibPosition = Calib_Position5;
										gCalibStat  = Calib_Step3;
										//SCSZC1_Cal_K(lWeights_Value, PreAxle1Chanle, 1);
								}
								else if(CalibPosition[0] == 2 && CalibrateZeroFlag)
								{
										gCalibPosition = Calib_Position5;
										gCalibStat  = Calib_Step3;
										//SCSZC1_Cal_K(lWeights_Value, ScalerChanle, 2);
								}
								else if(CalibPosition[0] == 3)
								{
										gCalibPosition = Calib_Position5;
										gCalibStat  = Calib_Step3;
										//SCSZC1_Cal_K(lWeights_Value, ScalerChanle, 3);
								}
								else
								{
									MsgBoxDlg(&g_MsgBoxDlg, "������ʾ", "δ�������궨����궨���");
								}
												
							}
							else
							{
								MsgBoxDlg(&g_MsgBoxDlg, "����", "����ֵ�Ƿ�������������");
							}
						}
   // }
						break;

                    case Calib_Position2:
                    case Calib_Position3:
									
                    case Calib_Position4:
                    case Calib_Position5:
						EraseRect(0, 110, 240, 17);
                    	if(CalibPosition[0] == 1)
                    	{                   			
                   			if(!SCSZC1_Cal_K(lWeights_Value, PreAxle1Chanle, 1)) 
							{
								MsgBoxDlg(&g_MsgBoxDlg, "�궨ʧ��", "�����±궨");
								break;
							}

							DrawGbText("�밴���ؼ��궨��һ��λ��", 1, 110);
	                    }
	                    else if(CalibPosition[0] == 2)
						{
							if(!SCSZC1_Cal_K(lWeights_Value, ScalerChanle, 2)) 
							{
								MsgBoxDlg(&g_MsgBoxDlg, "�궨ʧ��", "�����±궨");
								break;
							}

							DrawGbText("�밴���ؼ��궨��һ��λ��", 1, 110);
						} 
						else if(CalibPosition[0] == 3)
						{
							if(!SCSZC1_Cal_K(lWeights_Value, ScalerChanle, 3)) 
							{
								MsgBoxDlg(&g_MsgBoxDlg, "�궨ʧ��", "�����±궨");
								break;
							}

							DrawGbText("�궨��ɣ��뷵��", 42, 110);
						}
											
                    break;

                    default:
                        break;
                  }
					break;

				case KEY_RIGHT: 
					break;

				case KEY_BACK:

					gCalibPosition = Calib_Position0;
					StopWindowTimer(&gWD_CalibrateWeightMode1);

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
