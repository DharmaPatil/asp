#include "Form_CalibrateWeight.h"

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
	Calib_Position6,
    Calib_End
};
enum  _CalibStat {
		Calib_Step0=0,
    Calib_Step1=1,
    Calib_Step2,
    Calib_Step3,
    Calib_Step4,
		Calib_Step5,
};
static char gCalibPosition = Calib_Position0;
static char gCalibStat  = Calib_Step1;
static long lWeights_Value;   // ������
//static char CalibPosition[10] = "";


__align(8) static char sScale_Title[16] = "";
__align(8) static char sCali_Title[32] = "";


static float sADWeight_Value   = 0.0;
static float sWeight_Value   = 0.0;


static void Form_CalibrateWeight_Timer(void *ptmr, void *parg);
static void Form_CalibrateWeight_Draw(LPWindow pWindow);
static void Form_CalibrateWeight_Proc(LPWindow pWindow, LPGuiMsgInfo pGuiMsgInfo);


DEF_SMALL_FLOAT_LABLE(mADWeight_Attr,   &gWD_CalibrateWeight, 45,  44, 	170,  14, CTRL_VISABLE, &sADWeight_Value, 2,  -5000000, 5000000,    "", TA_LEFT);

DEF_SMALL_FLOAT_LABLE(mWeight_Attr,   &gWD_CalibrateWeight, 45,  66, 	170,  14, CTRL_VISABLE, &sWeight_Value, 0,  -5000000, 5000000,    "", TA_LEFT);

DEF_STATUS_IMAGE_CTRL(mCalibWeightSteadyImage, &gWD_CalibrateWeight, 220, 21, 14, 14, CTRL_VISABLE, Img_steady, Img_unsteady, FALSE);



static const LPControl marrLPControl[] = 
{
    &gStatusBar,
	&mADWeight_Attr,

    &mWeight_Attr,
	&mCalibWeightSteadyImage,


};

CWindow  gWD_CalibrateWeight = {
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
    Form_CalibrateWeight_Timer,
    Form_CalibrateWeight_Draw,
    Form_CalibrateWeight_Proc
};

static WetStatue wet1 = {0};

void Form_CalibrateWeight_Timer(void *ptmr, void *parg)
{
	LPWindow lpWindow = (LPWindow)parg;
//	uint32 tmp = 0;

	Get_Scaler_WetPar(&wet1);

	if(lpWindow != NULL) {
    	if(g_pCurWindow == &gWD_CalibrateWeight) 
		{
			sWeight_Value = wet1.Wet;
			sADWeight_Value = wet1.ScalerAD;

             if(CTRL_CONTENT(mCalibWeightSteadyImage).bHot != wet1.ScalerStableFlag) 
 		     {
                 CTRL_CONTENT(mCalibWeightSteadyImage).bHot = wet1.ScalerStableFlag;
                 PostWindowMsg(lpWindow, WM_UPDATECTRL,(u32)&mCalibWeightSteadyImage,0);
             }

			 PostWindowMsg(lpWindow, WM_UPDATECTRL, 0, 0);
    	}

    }
}

void Form_CalibrateWeight_Draw(LPWindow pWindow)
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
			DrawGbText("�����궨", 112, 88);
			DrawGbText("��ȷ������ʼ�궨�������뷵��", 42, 110);
			break;

		case Calib_Position1:
		case Calib_Position2:
		case Calib_Position3:
		case Calib_Position4:
		case Calib_Position5:
		case Calib_Position6:	
		    switch(gCalibStat) {
				case Calib_Step0:
					if(SCS_ZC_1 == Get_System_Mode())//�жϼ���ģʽ
					{
						DrawGbText("�궨������", 112, 88);
						DrawGbText("�밴ȷ�ϼ�����궨λ��", 42, 110);
						sprintf(sCali_Title,"%s������(%dkg)",sScale_Title,lWeights_Value);
					}
					else
					{
						DrawGbText("�궨������", 112, 88);
						DrawGbText("��̨�ȶ���ȷ�ϼ����б궨", 42, 110);
						sprintf(sCali_Title,"%s������(%dkg)",sScale_Title,lWeights_Value);
					}
					
					break;
#if 0					
		        case Calib_Step1:
		            break;
            
		        case Calib_Step2:
					DrawGbText("�궨������", 112, 88);
					DrawGbText("�밴ȷ�ϼ�����궨λ��", 42, 110);
					sprintf(sCali_Title,"%s������(%dkg)",sScale_Title,lWeights_Value);
		            break;
		            
		        case Calib_Step3:
					DrawGbText("�궨������", 112, 88);
					DrawGbText("��̨�ȶ����밴ȷ�ϼ�����궨λ��", 42, 110);
					sprintf(sCali_Title,"%s������(%dkg)-λ��(%d)",sScale_Title,lWeights_Value,CalibPosition[0]);
		            break;
		            
		        case Calib_Step4:
					DrawGbText("�궨������", 112, 88);
					DrawGbText("��̨�ȶ����밴ȷ�ϼ�����궨λ��", 42, 110);
					sprintf(sCali_Title,"%s������(%dkg)-λ��(%d)",sScale_Title,lWeights_Value,CalibPosition[0]);
		            break;
		            
				case Calib_Step5:
					DrawGbText("�궨���", 112, 88);
					DrawGbText("��ȷ���������궨�������뷵��", 42, 110);
					sprintf(sCali_Title,"%s������(%dkg)-λ��(%d)",sScale_Title,lWeights_Value,CalibPosition[0]);
					gCalibPosition = Calib_Position0;
					break;
#endif		            
		        default:
		            break;
		    }
	        break;

		case Calib_End:
			DrawGbText("�궨���", 112, 88);
	        break;

	    default:
	        break;
    }
    
    DrawGbText("AD��",5,44);
    DrawGbText("����",5,66);

	DrawGbText(sCali_Title, 5, 21);
    DrawHoriLine(0, 40, 240);
    DrawHoriLine(0, 62, 240);
    DrawHoriLine(0, 84, 240);
    DrawHoriLine(0, 128, 240);
    DrawVertLine(40, 40, 44);

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


void Form_CalibrateWeight_Proc(LPWindow pWindow, LPGuiMsgInfo pGuiMsgInfo)
{
    char   cBuff[20]="";
    char   cTemp[10]="";
    char *kerr[] = {"Kֵ̫С,�����±궨", "Kֵ̫��,�����±궨", "Kֵ����ʧ��,�����±궨", "δ֪adͨ��"};
    uint8  ret = KERR_NONE;

	switch(pGuiMsgInfo->ID)
	{
		case WM_LOAD:
	      	sprintf(sScale_Title,"�����궨");
	            
			ClearScreen();
			WeightFregDiv(TRUE, 10);
			SysTimeDly(3);
			CreateWindowTimer(&gWD_CalibrateWeight);
			StartWindowTimer(&gWD_CalibrateWeight);

		case WM_SHOW:	
			pWindow->DrawFunc(pWindow);
			break;

		case WM_UPDATECTRL:            
			mADWeight_Attr.DrawFunc(&mADWeight_Attr);
			mWeight_Attr.DrawFunc(&mWeight_Attr);
			mCalibWeightSteadyImage.DrawFunc(&mCalibWeightSteadyImage);
			break;

		case WM_TIMEUPDATE:
			gStatusBar.DrawFunc(&gStatusBar);
			break;

		case WM_KEYDOWN:
			switch(pGuiMsgInfo->wParam)
			{
				case KEY_UP:
				case KEY_DOWN:
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

									//if(CalibrateZeroFlag)
									{
										gCalibPosition = Calib_Position5;
										gCalibStat  = Calib_Step0;
									}
// 									else 
// 									{
// 											MsgBoxDlg(&g_MsgBoxDlg, "������ʾ", "δ�������궨����궨���");
// 									}
								}
								else
								{
									MsgBoxDlg(&g_MsgBoxDlg, "����", "����ֵ�Ƿ�������������");
								}
							}
							break;

                    case Calib_Position1:
                    case Calib_Position2:
                    case Calib_Position3:
                    case Calib_Position4:
						break;
						
					case Calib_Position5:
						EraseRect(1, 230, 90, 18);
						EraseRect(111, 88, 90, 18);
						gCalibPosition = Calib_Position0;

						ret = Weight_Cal_K(lWeights_Value, ScalerChanle);
						if(ret != KERR_NONE) 
						{
						    if(ret <= KERR_ADERR)
							    MsgBoxDlg(&g_MsgBoxDlg, "�궨ʧ��", kerr[ret-1]);
							break;
						}

						DrawGbText("�궨���", 112, 88);
						DrawGbText("��ȷ�ϼ����±궨�����򰴷��ؼ�����", 1, 110);
						sprintf(sCali_Title,"%s������(%dkg)",sScale_Title,lWeights_Value);
													
                        break;
                        
					case Calib_Position6:
						break;

                    default:
                        break;
                    }
					break;

				case KEY_RIGHT: 
					break;

				case KEY_BACK:
					WeightFregDiv(FALSE, 0);
					CalibrateZeroFlag = FALSE;
					gCalibPosition = Calib_Position0;
					StopWindowTimer(&gWD_CalibrateWeight);

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



