#include "includes.h"
#include "Form_SystemSet.h"
#include "Form_Main.h"
#include "Ctr_Attr.h"
#include "Ctr_DlgList.h"
#include "../Inc/TextEdit.h"
#include "Ctr_StatusBar.h"
#include "Common.h"
#include "Form_MsgBox.h"

#include "sys_config.h"
#include "weight_param.h"
#include "wet_algorim.h"
#include "sys_param.h"
#include "app_beep.h"
#include "Speed_check.h"

extern  CControl  gStatusBar; 		//״̬��

extern SSYSSET   gSysSet;

static void Form_WetSet_Draw(LPWindow pWindow);
static void Form_WetSet_Proc(LPWindow pWindow, LPGuiMsgInfo pGuiMsgInfo);

//ϵͳģʽ
static char cSysMod[9] = {0};					//����ģʽ
static char* cSysModItem[] = 
{
    "SCS-ZC-1",
    "SCS-ZC-2",
	"SCS-ZC-3",
    "SCS-ZC-4",
	"SCS-ZC-5",
	"SCS-ZC-6",
	"SCS-ZC-7"
};
DEF_LIST_DLG(gWD_SysModSet, 20, 20, 200, 88, "ϵͳģʽ", cSysModItem, sizeof(cSysModItem)/sizeof(char *), 1, 0);


static char keyvoice_set[5]={0};
static char* cVoiceSetItem[] = 
{
    "�ر�",
    "��",
};
DEF_LIST_DLG(gWD_KeyVoiceSet, 20, 20, 200, 88, "������", cVoiceSetItem, sizeof(cVoiceSetItem)/sizeof(char *), 1, 0);

static char cLongCarMode[9] = {0};					//����ģʽ
static char* cLongCarModeItem[] = 
{
    "�Զ�����",
    "�ֶγ���",
};
DEF_LIST_DLG(gWD_LongCarModeSet, 20, 20, 200, 88, "����ģʽѡ��", cLongCarModeItem, sizeof(cLongCarModeItem)/sizeof(char *), 1, 0);

// BGFlag
static char cBGFlag[4] = {0};					//��բ��־  �л���
static char* cBGFlagItem[] = 
{
	    "��",
		"��"
};
DEF_LIST_DLG(gWD_BGFlagSet, 20, 20, 200, 88, "��բ����", cBGFlagItem, sizeof(cBGFlagItem)/sizeof(char *), 0, 1);

//TrafficSignalFlag
static char cTrafficSignalFlag[4] = {0};					//���̵Ʊ�־  �л���
static char* cTrafficSignalFlagItem[] = 
{
	"��",
    "��",
};
DEF_LIST_DLG(gWD_TrafficSignalFlagSet, 20, 20, 200, 88, "���̵�����", cTrafficSignalFlagItem, sizeof(cTrafficSignalFlagItem)/sizeof(char *), 0, 1);

static char cDebugLevel[9] = {0};					//���Դ�ӡ�ȼ�
static char* cDebugLevelItem[] = 
{
    "�����",
    "������Ϣ",
	"ҵ����Ϣ",
    "������Ϣ",
	"֪ͨ��Ϣ",
	"������Ϣ"
};
DEF_LIST_DLG(gWD_DebugLevelSet, 20, 20, 200, 88, "���ڵ�ǰ�ȼ�����Ϣ�����", cDebugLevelItem, sizeof(cDebugLevelItem)/sizeof(char *), 1, 0);


static char cStaticFiltering[4] = {0};
static char cDynaFiltering[4] = {0};
static char cMaxSpeed[4]={0};
static char cLC2GCLength[8] = {0};
static char cAxleLength[8] = {0};

static char cParamUpWet[8] = {0};					//�����ϳ���ֵ
static char cParamDownWet[8] = {0};					//�����³���ֵ
static char cParamAxle1UpWet[8] = {0};				//�ϳӶ�������ֵ
static char cParamAxle1DownWet[8] = {0};			//�ϳӶ�������ֵ
static char cParamAxle2Upwet[8] = {0};				//�³Ӷ�������ֵ
static char cParamAxle2DownWet[8] = {0};			//�³Ӷ�������ֵ
static char cParamAxle3Upwet[8] = {0};				//���������ֵ
static char cParamAxle3DownWet[8] = {0};			//���������ֵ

static CAttrItem  _attritem[] = {
	{"����ģʽ����",cSysMod,1},
	{"��̬�˲��ȼ�",cStaticFiltering,2},
	{"��̬�˲��ȼ�",cDynaFiltering,3},
	{"���ٱ����ٶ�",cMaxSpeed,4},
	{"��Ļ�ظо���",cLC2GCLength, 5},
	{"ǰ��ʶ�������",cAxleLength, 6},
	{"��������",keyvoice_set,7},
	{"�����ϳ���ֵ",cParamUpWet,8},
	{"�����³���ֵ",cParamDownWet,9},
	{"�ϳƶ�������ֵ",cParamAxle1UpWet,10},
	{"�ϳƶ�������ֵ",cParamAxle1DownWet,11},
	{"�³ƶ�������ֵ",cParamAxle2Upwet,12},   
	{"�³ƶ�������ֵ",cParamAxle2DownWet,13},
	{"�������ֵ",cParamAxle3Upwet,14},   
	{"�������ֵ",cParamAxle3DownWet,15},
	{"����ģʽ����",cLongCarMode,16},
	{"��բ����",cBGFlag,17},
	{"���̵�����",cTrafficSignalFlag,18},
	{"���Դ�ӡ�ȼ�",cDebugLevel,19},
};

DEF_ATTR_CTRL(mSystemAttr, &gWD_SystemSet, 0, 20, 240, 108, "ϵͳ��������",(_attritem), sizeof(_attritem)/sizeof(CAttrItem), 0, CTRL_VISABLE);

static LPControl marrLPControl[] = 
{
    &gStatusBar,
	&mSystemAttr,
};

CWindow  gWD_SystemSet = {
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
    NULL,
    Form_WetSet_Draw,
    Form_WetSet_Proc
};

void Form_WetSet_Draw(LPWindow pWindow)
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

static void Set_AxleThreshold(uint8 flag, uint16 ulTemp, uint16 twet, uint16 *swet, uint32 addr)
{
	if((ulTemp < twet) && flag) {
		MsgBoxDlg(&g_MsgBoxDlg,"������ʾ","����ֵ�����������ֵ");
	} else if((ulTemp > twet) && !flag) {
		MsgBoxDlg(&g_MsgBoxDlg,"������ʾ","����ֵ����С������ֵ");
	} else if(ulTemp > 2000 || ulTemp < 20) {
		MsgBoxDlg(&g_MsgBoxDlg,"������ʾ","������20--2000��ֵ");
	} else {
		if(!Param_Write(GET_U16_OFFSET(addr), &ulTemp, sizeof(u16))) {
			MsgBoxDlg(&g_MsgBoxDlg, "������ʾ", "��������ʧ��,����������");
			return;
		}
		*swet = ulTemp;
		Init_Wet_Par();
	}
}

static void WetSet_Business(void)
{
    char   cTemp[12]="";
	u16    ulTemp = 0;
	int16  flag=0;
	
	switch (CTRL_CONTENT(mSystemAttr).focus) {
		case 0:
			if (CTRL_CONTENT(mSystemAttr).focus == 0) {
				CTRL_CONTENT(gWD_SysModSet).focus  = Get_System_Mode();
				CTRL_CONTENT(gWD_SysModSet).select = Get_System_Mode();
				flag =	LoadListDlg(&gWD_SysModSet);
				if((flag >= SCS_ZC_1) && (flag != SCS_ZC_4) && (flag < SCS_CNT)) {	
					Set_System_Mode((SysMod)flag);	//����ϵͳģʽ
					MsgBoxDlg(&g_MsgBoxDlg,"������ʾ", "ģʽ�޸ĳɹ�, ������!");
				} else if(flag == 3) {
					MsgBoxDlg(&g_MsgBoxDlg,"������ʾ","��ģʽ��ȡ��,������ѡ��");
				}
			}else {
				;
			}
			flag = 0;
			break;
				
		case 1:
			//��̬�˲��ȼ�
			memset(cTemp,0,sizeof(cTemp));
			if( LoadEditDlg(&g_TextEditWindow, "�����뾲̬�˲��ȼ�(1-10)", cTemp, cTemp, 2)) 
			{
				if(strlen(cStaticFiltering)) 
				{
					ulTemp = atoi(cTemp);
						
					if(ulTemp > 10 | ulTemp < 1)
					{
						MsgBoxDlg(&g_MsgBoxDlg,"������ʾ","û�д˵ȼ�,����������");
					}
					else
					{
						if(!Param_Write(GET_U8_OFFSET(ParamStaticFilter), &ulTemp, sizeof(u8)))
						{
							MsgBoxDlg(&g_MsgBoxDlg, "������ʾ", "��������ʧ��,����������");
							break;
						}
						gWetPar.StaticFilterLevel = ulTemp;
					}
					
				 }
			} 
			break;
							
		case 2:
			//��̬�˲��ȼ�cDynaFiltering
			memset(cTemp,0,sizeof(cTemp));
			if( LoadEditDlg(&g_TextEditWindow, "�����붯̬�˲��ȼ�(1-10)", cTemp, cTemp, 2)) 
			{
				if(strlen(cDynaFiltering)) 
				{
					ulTemp = atoi(cTemp);

					if(ulTemp > 10 | ulTemp < 1)
					{
						MsgBoxDlg(&g_MsgBoxDlg,"������ʾ","û�д˵ȼ�,����������");
					}
					else
					{
						if(!Param_Write(GET_U8_OFFSET(ParamDynamicFilter), &ulTemp, sizeof(u8)))
						{
							MsgBoxDlg(&g_MsgBoxDlg, "������ʾ", "��������ʧ�ܣ�����������");
							break;
						}
						gWetPar.DynamicFilterLevel = ulTemp;
					}
									
				}
			} 
			break;
				
		case 3://���ٱ����ٶ�
			memset(cTemp,0,sizeof(cTemp));
			if( LoadEditDlg(&g_TextEditWindow, "�����볬�ٱ����ٶ�(km/h)", cTemp, cTemp, 2)) 
			{
				if(strlen(cMaxSpeed)) 
				{
					ulTemp = atoi(cTemp);

					if(ulTemp > 40 | ulTemp < 5)
					{
						MsgBoxDlg(&g_MsgBoxDlg,"������ʾ","������5--40��ֵ");
					}
					else
					{
						if(!Param_Write(GET_U8_OFFSET(ParamSuperSpeedLevel), &ulTemp, sizeof(u8)))
						{
							MsgBoxDlg(&g_MsgBoxDlg, "������ʾ", "��������ʧ��,����������");
							break;
						}	
						gWetPar.SuperSpeedLevel = ulTemp;
					}
					
				}
			}
			break;
	
		case 4://��Ļ�ظо���
			memset(cTemp,0,sizeof(cTemp));
			if( LoadEditDlg(&g_TextEditWindow, "�������Ļ�ظо���(cm)", cTemp, cTemp, 3)) 
			{
				if(strlen(cLC2GCLength)) 
				{
					ulTemp = atoi(cTemp);
					if(ulTemp > 600 | ulTemp < 1)
					{
						MsgBoxDlg(&g_MsgBoxDlg,"������ʾ","������1~600��ֵ");
					}
					else
					{
						//ulTemp = ulTemp * 10;
						if(!Param_Write(GET_U16_OFFSET(ParamSpeedLength), &ulTemp, sizeof(u16)))
						{
							MsgBoxDlg(&g_MsgBoxDlg, "������ʾ", "��������ʧ��,����������");
							break;
						}	
						gWetPar.Length= ulTemp;
						Set_Speed_Length(gWetPar.Length * 10);
					}
				}
			}
			break;

		case 5: //ǰ����
			memset(cTemp,0,sizeof(cTemp));
			if( LoadEditDlg(&g_TextEditWindow, "������ǰ����(cm)", cTemp, cTemp, 3)) 
			{
				if(strlen(cAxleLength)) 
				{
					ulTemp = atoi(cTemp);
					
					if(ulTemp > 600 | ulTemp < 1)
					{
							MsgBoxDlg(&g_MsgBoxDlg,"������ʾ","������1~600��ֵ");
					}
					else
					{
						//ulTemp = ulTemp * 10;
						if(!Param_Write(GET_U16_OFFSET(ParamAxleLength), &ulTemp, sizeof(u16)))
						{
							MsgBoxDlg(&g_MsgBoxDlg, "������ʾ", "��������ʧ��,����������");
							break;
						}	
						gWetPar.AxleLength= ulTemp;
						Init_Wet_Par();
					}
				}
			}
			break;
							
		case 6:  //��������������			 
			if (CTRL_CONTENT(mSystemAttr).focus == 6) {
				CTRL_CONTENT(gWD_KeyVoiceSet).focus  = gSysSet.Beep;
				CTRL_CONTENT(gWD_KeyVoiceSet).select = gSysSet.Beep;
				flag =	LoadListDlg(&gWD_KeyVoiceSet);

				if(flag >= 0)
				{
					gSysSet.Beep = flag;
					if(flag)
						Set_Beep_Flag(1);
					else
						Set_Beep_Flag(0);	   //�ر�	   
				}
				EnableVoiceIcon(gSysSet.Beep);
			}else {
				;
			}
			break;
													
		case 7://�����ϳ���ֵ
			memset(cTemp,0,sizeof(cTemp));
			if( LoadEditDlg(&g_TextEditWindow, "�����������ϳ���ֵ20-2000", cTemp, cTemp, 4)) 
			{
				ulTemp = atoi(cTemp);
				
				if(ulTemp > 2000 || ulTemp < 20)
				{
					MsgBoxDlg(&g_MsgBoxDlg,"������ʾ","������20--2000��ֵ");
				}
				else
				{
					if(!Param_Write(GET_U16_OFFSET(ParamUpWet), &ulTemp, sizeof(u16)))
					{
						MsgBoxDlg(&g_MsgBoxDlg, "������ʾ", "��������ʧ��,����������");
						break;
					}
					gWetPar.TotalUpWet = ulTemp;
				}
			}				
			break;
	
		case 8://�����³���ֵ
			memset(cTemp,0,sizeof(cTemp));
			if( LoadEditDlg(&g_TextEditWindow, "�����������³���ֵ20-2000", cTemp, cTemp, 4)) 
			{
				ulTemp = atoi(cTemp);
				
				if(ulTemp > 2000 || ulTemp < 20)
				{
						MsgBoxDlg(&g_MsgBoxDlg,"������ʾ","������20--2000��ֵ");
				}
				else
				{
					if(!Param_Write(GET_U16_OFFSET(ParamDownWet), &ulTemp, sizeof(u16)))
					{
						MsgBoxDlg(&g_MsgBoxDlg, "������ʾ", "��������ʧ��,����������");
						break;
					}
					gWetPar.TotalDownWet = ulTemp;
				}
			}	
			break;
	
		case 9://�ϳӶ�������ֵ
			memset(cTemp,0,sizeof(cTemp));
			if( LoadEditDlg(&g_TextEditWindow, "�������ϳӶ�������ֵ20-2000", cTemp, cTemp, 4)) 
			{
				ulTemp = atoi(cTemp);
				Set_AxleThreshold(1, ulTemp, gWetPar.PreAxleDownWet, &gWetPar.PreAxleUpWet, ParamPreAxleUpWet);
			}

			break;
		
		case 10://�ϳӶ�������ֵ
			memset(cTemp,0,sizeof(cTemp));
			if( LoadEditDlg(&g_TextEditWindow, "�������ϳӶ�������ֵ20-2000", cTemp, cTemp, 4)) 
			{
				ulTemp = atoi(cTemp);
				Set_AxleThreshold(0, ulTemp, gWetPar.PreAxleUpWet, &gWetPar.PreAxleDownWet, ParamPreAxleDownWet);
			}
			break;
	
		case 11://�³Ӷ�������ֵ		
			memset(cTemp,0,sizeof(cTemp));
			if( LoadEditDlg(&g_TextEditWindow, "�������³Ӷ�������ֵ20-2000", cTemp, cTemp, 4)) 
			{
				ulTemp = atoi(cTemp);
				Set_AxleThreshold(1, ulTemp, gWetPar.BackAxleDownWet, &gWetPar.BackAxleUpWet, ParamBackAxleUpWet);
			}	
			break;
			
		case 12://�³Ӷ�������ֵ
			memset(cTemp,0,sizeof(cTemp));
			if( LoadEditDlg(&g_TextEditWindow, "�������³Ӷ�������ֵ20-2000", cTemp, cTemp, 4)) 
			{
				ulTemp = atoi(cTemp);
				Set_AxleThreshold(0, ulTemp, gWetPar.BackAxleUpWet, &gWetPar.BackAxleDownWet, ParamBackAxleDownWet);
			}
			break;

		case 13://�������ֵ
			memset(cTemp,0,sizeof(cTemp));
			if( LoadEditDlg(&g_TextEditWindow, "�������������ֵ20-2000", cTemp, cTemp, 4)) 
			{
				ulTemp = atoi(cTemp);
				Set_AxleThreshold(1, ulTemp, gWetPar.WbAxleDownWet, &gWetPar.WbAxleUpWet, ParamWbAxleUpWet);
			}
			break;
		
		case 14://�������ֵ
			memset(cTemp,0,sizeof(cTemp));
			if( LoadEditDlg(&g_TextEditWindow, "�������������ֵ20-2000", cTemp, cTemp, 4)) 
			{
				ulTemp = atoi(cTemp);
				Set_AxleThreshold(0, ulTemp, gWetPar.WbAxleUpWet, &gWetPar.WbAxleDownWet, ParamWbAxleDownWet);
			}
			break;
								
		case 15://����ģʽ
			if (CTRL_CONTENT(mSystemAttr).focus == 15) {
				CTRL_CONTENT(gWD_LongCarModeSet).focus	= Get_LongCar_Mode();
				CTRL_CONTENT(gWD_LongCarModeSet).select = Get_LongCar_Mode();
				flag =	LoadListDlg(&gWD_LongCarModeSet);
				if(flag >= 0)
				{							
					Set_LongCar_Mode((LongCarMod)flag);  //���ó���ģʽ
				}
				}else {
					;
				}
				flag = 0;

				break;
								
		case 16://��բ��־����
			if (CTRL_CONTENT(mSystemAttr).focus == 16) {
				CTRL_CONTENT(gWD_BGFlagSet).focus  = Get_BGFlag();
				CTRL_CONTENT(gWD_BGFlagSet).select = Get_BGFlag();
				flag =	LoadListDlg(&gWD_BGFlagSet);
				if(flag >= 0)
				{
					Set_BGFlag(flag);  //���õ�բ��־
				}
			}else {
			}
			flag = 0;

			break;
									
		case 17://���̵�����
			if (CTRL_CONTENT(mSystemAttr).focus == 17) {
				CTRL_CONTENT(gWD_TrafficSignalFlagSet).focus  = Get_TrafficSignal_Flag();
				CTRL_CONTENT(gWD_TrafficSignalFlagSet).select = Get_TrafficSignal_Flag();
				flag =	LoadListDlg(&gWD_TrafficSignalFlagSet);
				if(flag >= 0)
				{	
					Set_TrafficSignal_Flag(flag);  //���ú��̵�ģʽ
				}
			}else {
				;
			}
			flag = 0;

			break;
									
		case 18://���Դ�ӡ�ȼ�
			if (CTRL_CONTENT(mSystemAttr).focus == 18) 
			{
				CTRL_CONTENT(gWD_DebugLevelSet).focus  = Get_Debug_Level();
				CTRL_CONTENT(gWD_DebugLevelSet).select = Get_Debug_Level();
				flag =	LoadListDlg(&gWD_DebugLevelSet);
				Set_Debug_Level(flag);	//���õ��Դ�ӡ�ȼ�
			}
			else 
			{
			}
			flag = 0;

			break;
	
		default:
			break;
	}
}

void Form_WetSet_Proc(LPWindow pWindow, LPGuiMsgInfo pGuiMsgInfo)
{
	CControl* pControl;	
	GuiMsgInfo guiMsgInfo;

	switch(pGuiMsgInfo->ID)
	{
		case WM_LOAD:
			ClearScreen();
			SysTimeDly(2);
			CTRL_CONTENT(mSystemAttr).focus = 0;

		case WM_SHOW:
			gSysSet.Beep = Get_Beep_Flag();
			strncpy(cSysMod, cSysModItem[Get_System_Mode()], 8);	//����ģʽѡ��
			snprintf(cStaticFiltering, 3,"%d", gWetPar.StaticFilterLevel);   //��̬�˲��ȼ�
			snprintf(cDynaFiltering, 3,"%d", gWetPar.DynamicFilterLevel);		//��̬�˲��ȼ�
			snprintf(cMaxSpeed, 3,"%dkm/h", gWetPar.SuperSpeedLevel);				//�ٶ���ֵ
			snprintf(cLC2GCLength, 7, "%dcm", gWetPar.Length);						//��Ļ�ظо���
			snprintf(cAxleLength, 7, "%dcm", gWetPar.AxleLength);						//�����
			strncpy(keyvoice_set, cVoiceSetItem[gSysSet.Beep], 4);			//������
			snprintf(cParamUpWet, 7,"%d", gWetPar.TotalUpWet);   						//�����ϳ���ֵ
			snprintf(cParamDownWet, 7,"%d", gWetPar.TotalDownWet);   				//�����³���ֵ
			snprintf(cParamAxle1UpWet,   7,"%d", gWetPar.PreAxleUpWet);   				//�ϳӶ�������ֵ
			snprintf(cParamAxle1DownWet, 7,"%d", gWetPar.PreAxleDownWet);   		//�ϳӶ�������ֵ
			snprintf(cParamAxle2Upwet,   7,"%d", gWetPar.BackAxleUpWet);   				//�³Ӷ�������ֵ
			snprintf(cParamAxle2DownWet, 7,"%d", gWetPar.BackAxleDownWet);   		//�³Ӷ�������ֵ
			snprintf(cParamAxle3Upwet,   7,"%d", gWetPar.WbAxleUpWet);   				//�³Ӷ�������ֵ
			snprintf(cParamAxle3DownWet, 7,"%d", gWetPar.WbAxleDownWet);   		//�³Ӷ�������ֵ
			strncpy(cLongCarMode, cLongCarModeItem[Get_LongCar_Mode()], 8);			//����ģʽ
			strncpy(cBGFlag,cBGFlagItem[Get_BGFlag()], 2);											//��բ����
			strncpy(cTrafficSignalFlag,cTrafficSignalFlagItem[Get_TrafficSignal_Flag()], 2);			//���̵�����
			strncpy(cDebugLevel, cDebugLevelItem[Get_Debug_Level()], 8);					//���Դ�ӡ�ȼ�

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
                    if (CTRL_CONTENT(mSystemAttr).focus > 0) {
                        CTRL_CONTENT(mSystemAttr).focus--;
            			PostWindowMsg(pWindow, WM_UPDATECTRL, (uint32)&mSystemAttr, 0);
                    }
					break;
				case KEY_DOWN:
                    if (CTRL_CONTENT(mSystemAttr).focus + 1 < CTRL_CONTENT(mSystemAttr).total) {
                        CTRL_CONTENT(mSystemAttr).focus++;
            			PostWindowMsg(pWindow, WM_UPDATECTRL, (uint32)&mSystemAttr, 0);
                    }
					break;

				case KEY_LEFT:
					break;

				case KEY_OK:
					WetSet_Business();
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

