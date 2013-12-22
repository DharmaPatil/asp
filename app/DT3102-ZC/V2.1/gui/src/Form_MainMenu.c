#include "Form_MainMenu.h"
#include "Form_MeasureSet.h"
#include "Ctr_StatusBar.h"
#include "Keyboard.h"
#include "../Inc/Menu.h"
#include "Form_Main.h"
#include "Form_Query.h"
#include "Form_TimeDate.h"
#include "Form_SystemSet.h"

#include "Form_CalibrateZero.h"
#include "Form_CalibrateSet.h"
#include "Form_CalibrateWeight.h"
#include "Form_CalibrateWeightMode1.h"
#include "Form_CalibrateAxleWeight.h"
#include "Form_ADChannel.h"
#include "Form_Version.h"

#include "Form_PassWord.h"

#include "Form_MsgBox.h"
#include "Form_NetSet.h"

#include "Form_SpeedMod.h"
#include "Form_OverLoadSet.h"
#include "Form_USB_update.h"
#include "form_warning.h"
#include "form_device_signal.h"

#include "Form_MeasureSet.h"
#include "Form_InPortTest.h"
#include "Form_OutPortTest.h"
#include "sys_config.h"
#include "device_info.h"

extern  CControl  gStatusBar;

extern  BOOL CalibrateZeroFlag;		//�궨��ɱ�־

// #define	ReadDemarcateIO()		(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_10))

/*
 *  �궨�˵�
 */
DEF_MENU_ITEM(mnCalibraA,       "���궨",     0, NULL);
DEF_MENU_ITEM(mnCalibraB,       "�����궨",     1, NULL);
DEF_MENU_ITEM(mnCalibraAxle,     "��궨",      2, NULL);

DEF_MENU_ITEM(mnCalibraSet,     "�궨ϵ��",      3, NULL);
DEF_MENU_ITEM(mnADChannel,     	"AD ͨ�� ",      4, NULL);

static LPMenuItem	m_CalibMenuItems[] = 
{
	&mnCalibraA,
	&mnCalibraB,	
	&mnCalibraAxle,
	&mnCalibraSet,
	&mnADChannel,
};
void FormCalibMenuProc(struct _CWindow* pParentWindow, uint32 nCmd);

DEF_MENU(mCalibMenu, "�궨", m_CalibMenuItems, NULL, FormCalibMenuProc);	 
DEF_MENU_WND(gWD_CalibMenu, &mCalibMenu, NULL);


void FormCalibMenuProc(struct _CWindow* pParentWindow, uint32 nCmd)
{	
	switch(nCmd)
	{
		case 0:
            if(Get_AD_ErrStatus(1)) 
            {
                MsgBoxDlg(&g_MsgBoxDlg, "��Ϣ��ʾ", "���Ӵ������쳣,���ܱ궨");
            } 
            else 
            {
                g_pCurWindow = &gWD_CalibrateZero;
    			g_pCurWindow->pParentWindow = &gWD_CalibMenu;
    			PostWindowMsg(g_pCurWindow, WM_LOAD, 1, NULL);	
            }
			break;
        
		case 1:
             if(Get_AD_ErrStatus(1)) 
             {
                 MsgBoxDlg(&g_MsgBoxDlg, "��Ϣ��ʾ", "���Ӵ������쳣,���ܱ궨");
             } 
             else 
             {
				if(SCS_ZC_1 == Get_System_Mode())
				{
					g_pCurWindow = &gWD_CalibrateWeightMode1;
					g_pCurWindow->pParentWindow = &gWD_CalibMenu;
					PostWindowMsg(g_pCurWindow, WM_LOAD, 2, NULL);	
				}
				else
				{		
					if(CalibrateZeroFlag)
					{
						g_pCurWindow = &gWD_CalibrateWeight;
						g_pCurWindow->pParentWindow = &gWD_CalibMenu;
						PostWindowMsg(g_pCurWindow, WM_LOAD, 2, NULL);	
					}
					else 
					{
						MsgBoxDlg(&g_MsgBoxDlg, "������ʾ", "δ�������궨����궨���");
					}
				}
            }
			break;

		case 2:
             if(0)//Get_AD_ErrStatus(3) || Get_AD_ErrStatus(4)) 
             {
                 MsgBoxDlg(&g_MsgBoxDlg, "��Ϣ��ʾ", "�ᴫ�����쳣,���ܱ궨");
             } 
             else 
             {	
				if(CalibrateZeroFlag)
				{
					g_pCurWindow = &gWD_CalibrateAxleWeight;
					g_pCurWindow->pParentWindow = &gWD_CalibMenu;
					PostWindowMsg(g_pCurWindow, WM_LOAD, 2, NULL);	
				}
				else 
				{
					MsgBoxDlg(&g_MsgBoxDlg, "������ʾ", "δ�������궨����궨���");
				}
            }
			break;
        
		case 3:
            g_pCurWindow = &gWD_CalibrateSet;
			g_pCurWindow->pParentWindow = &gWD_CalibMenu;
			PostWindowMsg(g_pCurWindow, WM_LOAD, 2, NULL);	
			break;
		
		case 4:
            g_pCurWindow = &gWD_ADChannel;
			g_pCurWindow->pParentWindow = &gWD_CalibMenu;
			PostWindowMsg(g_pCurWindow, WM_LOAD, 2, NULL);	
			break;
            
		default:
			break;
	}
}

/*
 *  ���˵�
 */
DEF_MENU_ITEM(mnQuery,	    	"��ѯ",     	0, 	NULL);
DEF_MENU_ITEM(mnErrorQuery,		"���ϲ�ѯ", 	1, NULL);
DEF_MENU_ITEM(mnMeasureSet,		"��������", 	2, 	NULL);  
DEF_MENU_ITEM(mnTimeSet,			"ʱ������", 	3, 	NULL);

DEF_MENU_ITEM(mnNetSet, 			"��������", 	4, 	NULL);
DEF_MENU_ITEM(mnSoftVer,    	"�汾��ѯ", 	5, 	NULL);

DEF_MENU_ITEM(mnSpeedCorr,  	"�ٶ�����", 	6, 	NULL);
DEF_MENU_ITEM(mnWeightSet,		"ϵͳ����", 	7, 	NULL);
DEF_MENU_ITEM(mnOverLoad,   	"��������", 	8, 	NULL);


DEF_MENU_ITEM(mnCalibrate,		"�궨",     	9, 	NULL);

DEF_MENU_ITEM(mnPassWord,  		"��������", 	10, 	NULL);
DEF_MENU_ITEM(mnInPortTest,   "�������", 	11, NULL);
DEF_MENU_ITEM(mnOutPortTest,  "�������", 	12, NULL);
DEF_MENU_ITEM(mnUsbTest,   "U������", 13, NULL);
DEF_MENU_ITEM(mnDeviceSignal, "�����ƽ", 14, NULL);


static LPMenuItem	m_MainMenuItems[] = 
{
    //&gStatusBar,
	&mnQuery,
	&mnErrorQuery,
	&mnMeasureSet,	  	
	&mnTimeSet,
	&mnNetSet,
	&mnSoftVer,	  	
	&mnSpeedCorr,	  	
	&mnWeightSet,	  	
	&mnOverLoad,	  	
	&mnCalibrate,	  
	&mnPassWord,
	&mnInPortTest,
	&mnOutPortTest,	
	&mnUsbTest,
	&mnDeviceSignal
};

void FormMainMenuProc(struct _CWindow* pParentWindow, uint32 nCmd);

DEF_MENU(mMainMenu, "���˵�", m_MainMenuItems, NULL, FormMainMenuProc);	 
DEF_MENU_WND(gWD_MainMenu, &mMainMenu, &gWD_Main);

static volatile BOOL SuperUserFlag = FALSE;

BOOL Get_SuperUser_Use(void)
{
	return SuperUserFlag;
}


void Set_SuperUser_Use(BOOL flag)
{
	SuperUserFlag = flag;
}


void FormMainMenuProc(struct _CWindow* pParentWindow, uint32 nCmd)
{
	int pwd = 0;
	
	switch(nCmd)
	{
		case 0:
			g_pCurWindow = &gWD_DataMenu;//gWD_Query;						//��ѯ
			g_pCurWindow->pParentWindow = &gWD_MainMenu;
			PostWindowMsg(g_pCurWindow, WM_LOAD, NULL, NULL);	
			break;
			
		case 1:												//����
			g_pCurWindow = &gWD_Warning;
			g_pCurWindow->pParentWindow = &gWD_MainMenu;
			PostWindowMsg(g_pCurWindow, WM_LOAD, NULL, NULL);	
			break;
		
        
		case 2:												//��������---���ز�������
			pwd = CheckPassWord();
			if(pwd == 0) {
				MsgBoxDlg(&g_MsgBoxDlg, "����", "�������");
				break; 
			} else if(pwd == 2) {
				SuperUserFlag = TRUE;
			}
			
			g_pCurWindow = &gWD_MeasureSet;
			g_pCurWindow->pParentWindow = &gWD_MainMenu;
			PostWindowMsg(g_pCurWindow, WM_LOAD, NULL, NULL);	
			break;

		case 3:
			g_pCurWindow = &gWD_TimeDate;					//ʱ��
			g_pCurWindow->pParentWindow = &gWD_MainMenu;
			PostWindowMsg(g_pCurWindow, WM_LOAD, NULL, NULL);	
			break;


		case 4:												//����
            g_pCurWindow = &gWD_NetSet;
			g_pCurWindow->pParentWindow = &gWD_MainMenu;
			PostWindowMsg(g_pCurWindow, WM_LOAD, NULL, NULL);	
			break;
			    
		case 5:												//�汾
			g_pCurWindow = &gWD_Version;
			g_pCurWindow->pParentWindow = &gWD_MainMenu;
			PostWindowMsg(g_pCurWindow, WM_LOAD, NULL, NULL);	
			break;
			    
		    
		case 6:												//�ٶ�
		   if(FALSE == CheckPassWord()) {
				MsgBoxDlg(&g_MsgBoxDlg, "����", "�������");
				break; 
			}
			g_pCurWindow = &gWD_SpeedMod;
			g_pCurWindow->pParentWindow = &gWD_MainMenu;
			PostWindowMsg(g_pCurWindow, WM_LOAD, NULL, NULL);	
			break;
            
		case 7:												//ϵͳ����
			if(FALSE == CheckPassWord()) {
				MsgBoxDlg(&g_MsgBoxDlg, "����", "�������");
				break; 
			}
			g_pCurWindow = &gWD_SystemSet;
			g_pCurWindow->pParentWindow = &gWD_MainMenu;
			PostWindowMsg(g_pCurWindow, WM_LOAD, NULL, NULL);	
			break;
            
		case 8:												//����
			if(FALSE == CheckPassWord()) {
						MsgBoxDlg(&g_MsgBoxDlg, "����", "�������");
						break; 
			}
			g_pCurWindow = &gWD_OverLoadSet;
			g_pCurWindow->pParentWindow = &gWD_MainMenu;
			PostWindowMsg(g_pCurWindow, WM_LOAD, NULL, NULL);	
			break;
            

						
		case 9:										    //�궨
			if(IsBbKeyOpen() == FALSE) 
			{
				MsgBoxDlg(&g_MsgBoxDlg, "����", "��򿪱궨����");
				break; 
			}           
			g_pCurWindow = &gWD_CalibMenu;
			g_pCurWindow->pParentWindow = &gWD_MainMenu;
			PostWindowMsg(g_pCurWindow, WM_LOAD, NULL, NULL);	
		    break;
			
		case 10:										    //����
            g_pCurWindow = &gWD_PassWord;
			g_pCurWindow->pParentWindow = &gWD_MainMenu;
			PostWindowMsg(g_pCurWindow, WM_LOAD, NULL, NULL);	
			break;
            
		case 11:										    //�������

			g_pCurWindow = &gWD_InPortTest;
			g_pCurWindow->pParentWindow = &gWD_MainMenu;
			PostWindowMsg(g_pCurWindow, WM_LOAD, NULL, NULL);	
			break;
		
		case 12:										    //�������
			g_pCurWindow = &gWD_OutPortTest;
			g_pCurWindow->pParentWindow = &gWD_MainMenu;
			PostWindowMsg(g_pCurWindow, WM_LOAD, NULL, NULL);	
			break;
           
		case 13:
			g_pCurWindow = &gWD_USB_Updata;
			g_pCurWindow->pParentWindow = &gWD_MainMenu;
			PostWindowMsg(g_pCurWindow, WM_LOAD, NULL, NULL);	
			break;

		case 14:
			if(FALSE == CheckPassWord()) {
				MsgBoxDlg(&g_MsgBoxDlg, "����", "�������");
				break; 
			}
			g_pCurWindow = &gWD_DeviceSignal;
			g_pCurWindow->pParentWindow = &gWD_MainMenu;
			PostWindowMsg(g_pCurWindow, WM_LOAD, NULL, NULL);
			break;
           
		default:
			break;
	}
}



