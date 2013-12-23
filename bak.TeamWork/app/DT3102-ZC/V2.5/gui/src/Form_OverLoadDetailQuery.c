#include "Form_OverLoadDetailQuery.h"
#include "Form_Main_Axle.h"
#include "Ctr_ListSelect.h"
#include "Form_Resource.h"
#include "Ctr_StatusBar.h"
#include "Form_Main.h"

#include "Form_OverLoadQuery.h"
#include "Common.h"

#include "overtime_queue.h"
#include "data_store_module.h"
#include "sys_param.h"



static OvertimeData  OverLoadDetailVehShow;
static s8  AxleGroupShow = 0;

static void Form_OverLoadDetailQuery_Timer(void *ptmr, void *parg);
static void Form_OverLoadDetailQuery_Draw(LPWindow pWindow);
static void Form_OverLoadDetailQuery_Proc(LPWindow pWindow, LPGuiMsgInfo pGuiMsgInfo);


static LPControl marrLPControl[] = 
{
    &gStatusBar,
};

CWindow  gWD_OverLoadDetailQuery = {
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
    Form_OverLoadDetailQuery_Timer,
    Form_OverLoadDetailQuery_Draw,
    Form_OverLoadDetailQuery_Proc
};

void Form_OverLoadDetailQuery_Timer(void *ptmr, void *parg)
{
	LPWindow lpWindow = (LPWindow)parg;
	if(lpWindow != NULL)
	{
	}
}

void Form_OverLoadDetailQuery_Draw(LPWindow pWindow)
{
	uint16 i;
	LPControl	lpControl;
	char cBuff[36];
//    u8 ucTemp;

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

    if(AxleGroupShow < 0) {
    	DrawGbText("����: ", 5, 22);
//     	DrawGbText("������: ", 100, 22);
    	DrawGbText("��", 18, 44);
    	DrawGbText("����", 71, 44);
    	DrawGbText("����", 131, 44);
    	DrawGbText("���", 191, 44);
    } else {
		sprintf(cBuff,"����: %d/%d",(AxleGroupShow),OverLoadDetailVehShow.AxleGroupNum);
		DrawGbText(cBuff, 5, 22);
			
		sprintf(cBuff,"%d",AxleGroupShow);				//��
		DrawGbText(cBuff, 18, 66);
			
		sprintf(cBuff,"%d",OverLoadDetailVehShow.AxleWet[AxleGroupShow-1]);				//����
		DrawGbText(cBuff, 71, 66);
			
		if((AxleGroupShow) == 1)															//�ж�����
		{
			DrawGbText("����", 131, 66);													//��һ����Ϊ����
		}
		else
			DrawGbText("˫��", 131, 66);													//��������Ĭ��Ϊ˫��
			
		sprintf(cBuff,"%d m",(int)(OverLoadDetailVehShow.AxleLen[AxleGroupShow-1]*0.01f));				//���
			DrawGbText(cBuff, 191, 66);
		
		DrawGbText("��", 18, 44);
		DrawGbText("����", 71, 44);
		DrawGbText("����", 131, 44);
		DrawGbText("���", 191, 44);			 
    }
    
    DrawHoriLine(0, 40, 240);
    DrawHoriLine(0, 62, 240);
    DrawHoriLine(0, 84, 240);

    DrawVertLine(50, 40, 44);
    DrawVertLine(120, 40, 44);
    DrawVertLine(170, 40, 44);


	//ʹ��ˢ��
	EnableScreenFlush(TRUE);

	//ˢ��
	FlushScreen();

	//ʹ�ܻ�ͼ
	SetRedraw(TRUE);
}
void Form_OverLoadDetailQuery_Proc(LPWindow pWindow, LPGuiMsgInfo pGuiMsgInfo)
{
	CControl* pControl;	

	switch(pGuiMsgInfo->ID)
	{
		case WM_LOAD:
			ClearScreen();
			SysTimeDly(15);
			Get_OverLoadVehShow(&OverLoadDetailVehShow);
			if(OverLoadDetailVehShow.AxleGroupNum) 
			{
				AxleGroupShow = 1;
			} 
			else
			{
				AxleGroupShow = 0;
			}

		case WM_SHOW:
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
					break;
				case KEY_DOWN:
					break; 

				case KEY_OK:
					break;

				case KEY_RIGHT: 
					if(AxleGroupShow < OverLoadDetailVehShow.AxleNum )
					{
						AxleGroupShow++;
					}		
					PostWindowMsg(pWindow, WM_SHOW, 0, 0);
					break;

				case KEY_LEFT:
					if(AxleGroupShow>1)
					{
						AxleGroupShow--;
					}		
					PostWindowMsg(pWindow, WM_SHOW, 0, 0);
					break;
                    
				case KEY_BACK:
					if(pWindow->pParentWindow != NULL)
					{
						g_pCurWindow = pWindow->pParentWindow;
                        PostWindowMsg(g_pCurWindow, WM_SHOW,0,0);
					}
					else
					{
						g_pCurWindow = &gWD_Main;
                        PostWindowMsg(g_pCurWindow, WM_LOAD,0,0);
					}
					break;
                    
				case KEY_PRINT:
         #if 0
					//��ӡ����
					printerQueryInfo.PrinterManagerID = PM_SEARCH;		 //�ֶ���ӡ
					if(pPrinterManagerEvent != NULL)
						OSQPost(pPrinterManagerEvent, (void*)&printerQueryInfo);	//���ʹ�ӡ��
          #endif
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



