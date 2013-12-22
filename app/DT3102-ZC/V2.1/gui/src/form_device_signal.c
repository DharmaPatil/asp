#include "form_device_signal.h"
#include "Form_Main.h"
#include "Form_MsgBox.h"

#include "Ctr_Attr.h"
#include "Ctr_DlgList.h"
#include "../Inc/TextEdit.h"
#include "ioctrl.h"
#include "dt3102_io.h"

extern  CControl  gStatusBar; 		//״̬��
				  
static void Form_Device_Draw(LPWindow pWindow);
static void Form_Device_Proc(LPWindow pWindow, LPGuiMsgInfo pGuiMsgInfo);


static char device_att_str[11][8] = {{0}};
static DeviceSignal sDeviceSignal = {0};

//��������ҳ
static CAttrItem  _attritem[] = {
   {"��Ļ1�źŵ�ƽ",device_att_str[0],1},
   {"��Ļ1���ϵ�ƽ",device_att_str[1],2},
   {"��Ļ2�źŵ�ƽ",device_att_str[2],3},
   {"��Ļ2���ϵ�ƽ",device_att_str[3],4},
   {"�ظ�1�źŵ�ƽ",device_att_str[4],5},
   {"�ظ�1���ϵ�ƽ",device_att_str[5],6},
   {"�ظ�2�źŵ�ƽ",device_att_str[6],7},
   {"�ظ�2���ϵ�ƽ",device_att_str[7],8},
   {"�����źŵ�ƽ",device_att_str[8],9},
   {"������ϵ�ƽ",device_att_str[9],10},
   {"����˫�ֵ�ƽ",device_att_str[10],11},
};

DEF_ATTR_CTRL(mDeviceSignalAttr, &gWD_DeviceSignal, 0, 20, 240, 108, "�����ƽ����",(_attritem), sizeof(_attritem)/sizeof(CAttrItem), 0, CTRL_VISABLE);

static LPControl marrLPControl[] = 
{
	&gStatusBar,
	&mDeviceSignalAttr,
};

CWindow  gWD_DeviceSignal = {		 //����
    marrLPControl,
	sizeof(marrLPControl)/sizeof(LPControl),
    0,
    0,
    0,
    240,
    128,
    WS_VISABLE,
	NULL,//(pParentWindow)
    0,
    NULL,
    NULL,
    Form_Device_Draw,
    Form_Device_Proc
};

static void Show_Device_Signal(void)
{
	//��Ļ1
	if(sDeviceSignal.Signal.LC1EventSignal) {
		strcpy(device_att_str[0],"�ߵ�ƽ");
	} else {
		strcpy(device_att_str[0],"�͵�ƽ");
	}
	
	if(sDeviceSignal.Signal.LC1ErrSignal) {
		strcpy(device_att_str[1],"�ߵ�ƽ");
	} else {
		strcpy(device_att_str[1],"�͵�ƽ");
	}

	//��Ļ2
	if(sDeviceSignal.Signal.LC2EventSignal) {
		strcpy(device_att_str[2],"�ߵ�ƽ");
	} else {
		strcpy(device_att_str[2],"�͵�ƽ");
	}

	if(sDeviceSignal.Signal.LC2ErrSignal) {
		strcpy(device_att_str[3],"�ߵ�ƽ");
	} else {
		strcpy(device_att_str[3],"�͵�ƽ");
	}

	//ǰ�ظ�
	if(sDeviceSignal.Signal.GC1EventSignal) {
		strcpy(device_att_str[4],"�ߵ�ƽ");
	} else {
		strcpy(device_att_str[4],"�͵�ƽ");
	}

	
	if(sDeviceSignal.Signal.GC1ErrSignal) {
		strcpy(device_att_str[5],"�ߵ�ƽ");
	} else {
		strcpy(device_att_str[5],"�͵�ƽ");
	}

	//��ظ�
	if(sDeviceSignal.Signal.GC2EventSignal) {
		strcpy(device_att_str[6],"�ߵ�ƽ");
	} else {
		strcpy(device_att_str[6],"�͵�ƽ");
	}

	
	if(sDeviceSignal.Signal.GC2ErrSignal) {
		strcpy(device_att_str[7],"�ߵ�ƽ");
	} else {
		strcpy(device_att_str[7],"�͵�ƽ");
	}

	//����ʶ����
	if(sDeviceSignal.Signal.TyreEventSignal) {
		strcpy(device_att_str[8],"�ߵ�ƽ");
	} else {
		strcpy(device_att_str[8],"�͵�ƽ");
	}

	if(sDeviceSignal.Signal.TyreErrSignal) {
		strcpy(device_att_str[9],"�ߵ�ƽ");
	} else {
		strcpy(device_att_str[9],"�͵�ƽ");
	}

	
	if(sDeviceSignal.Signal.TyreDoubleSignal) {
		strcpy(device_att_str[10],"�ߵ�ƽ");
	} else {
		strcpy(device_att_str[10],"�͵�ƽ");
	}
}

void Form_Device_Draw(LPWindow pWindow)
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

void Form_Device_Proc(LPWindow pWindow, LPGuiMsgInfo pGuiMsgInfo)
{
	CControl* pControl;	

	switch(pGuiMsgInfo->ID)
	{
		case WM_LOAD:
			ClearScreen();							//����
			SysTimeDly(3);
			CTRL_CONTENT(mDeviceSignalAttr).focus = 0;	//���ͣ��λ��

		case WM_SHOW:
			//��ȡĬ�ϵ��豸��ƽֵ
			sDeviceSignal.value = Get_Device_Signal();
			//��ʾ�豸��ƽ
			Show_Device_Signal();
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
                    if (CTRL_CONTENT(mDeviceSignalAttr).focus > 0) {
                        CTRL_CONTENT(mDeviceSignalAttr).focus--;
            			PostWindowMsg(pWindow, WM_UPDATECTRL, (uint32)&mDeviceSignalAttr, 0);
                    }
					break;
				case KEY_DOWN:
                    if (CTRL_CONTENT(mDeviceSignalAttr).focus + 1 < CTRL_CONTENT(mDeviceSignalAttr).total) {
                        CTRL_CONTENT(mDeviceSignalAttr).focus++;
            			PostWindowMsg(pWindow, WM_UPDATECTRL, (uint32)&mDeviceSignalAttr, 0);
                    }
					break;

				case KEY_LEFT:
					sDeviceSignal.value |= (1 << CTRL_CONTENT(mDeviceSignalAttr).focus);
					Show_Device_Signal();
					PostWindowMsg(pWindow, WM_UPDATECTRL, (uint32)&mDeviceSignalAttr, 0);
					break;

				case KEY_OK:
					if(Set_Device_Signal(sDeviceSignal.value))
					{
						MsgBoxDlg(&g_MsgBoxDlg, "�ɹ���ʾ", "�������ñ���ɹ�!");
					}
					else
					{
						MsgBoxDlg(&g_MsgBoxDlg, "������ʾ", "�������ñ���ʧ��,����������");
					}
					break;

				case KEY_RIGHT: 
					sDeviceSignal.value &= (~(1 << CTRL_CONTENT(mDeviceSignalAttr).focus));
					Show_Device_Signal();
					PostWindowMsg(pWindow, WM_UPDATECTRL, (uint32)&mDeviceSignalAttr, 0);
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





