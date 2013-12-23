#include "Form_Main_Axle.h"
#include "Form_Main_Veh.h"
#include "Form_Resource.h"
#include "Ctr_StatusBar.h"
#include "Form_Main.h"
#include "Common.h"

#include "includes.h"
#include "overtime_queue.h"

static s8  AxleGroupShow = 0;			//����

static void Form_Main_Axle_Timer(void *ptmr, void *parg);
static void Form_Main_Axle_Draw(LPWindow pWindow);
static void Form_Main_Axle_Proc(LPWindow pWindow, LPGuiMsgInfo pGuiMsgInfo);

static LPControl marrLPControl[] = 
{
    &gStatusBar,
};

CWindow  gWD_Main_Axle = {
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
    Form_Main_Axle_Timer,
    Form_Main_Axle_Draw,
    Form_Main_Axle_Proc
};

void Form_Main_Axle_Timer(void *ptmr, void *parg)
{
	LPWindow lpWindow = (LPWindow)parg;
	if(lpWindow != NULL)
	{
	}
}

u8 GetAxleByAxleGroupType(char Type)
{
    u8 ret = 0;
    
    switch(Type) {

    case 1:// ���ᵥ��
        ret = 1;
        break;

    case 2:// ����˫��
        ret = 1;
        break;

    case 3:// ˫���ᵥ��
        ret = 2;
        break;

    case 4:// ˫���ᵥ˫��
        ret = 2;
        break;

    case 5:// ˫����˫��
        ret = 2;
        break;

    case 6:// �����ᵥ��
        ret = 3;
        break;

    case 7:// ������˫��
        ret = 3;
        break;

    case 8:// ��������
        //ret = 4;
        break;
        
    default:
        break;
    }

    return ret;
}

void Form_Main_Axle_Draw(LPWindow pWindow)
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

  if(AxleGroupShow == 0) 
	{
		sprintf(cBuff,"����: %d/%d",AxleGroupShow,gMainCarInfo.AxleGroupNum);
	    DrawGbText(cBuff, 5, 22);
	    DrawGbText("����: ", 5, 22);
			DrawGbText("��", 18, 44);
	    DrawGbText("����", 71, 44);
	    DrawGbText("����", 131, 44);
	    DrawGbText("���", 191, 44);  
  } 
	else
	{
    sprintf(cBuff,"����: %d/%d",(AxleGroupShow),gMainCarInfo.AxleGroupNum);
    DrawGbText(cBuff, 5, 22);
			
		sprintf(cBuff,"%d",AxleGroupShow);				//��
    DrawGbText(cBuff, 18, 66);
			
		sprintf(cBuff,"%d",gMainCarInfo.AxleWet[AxleGroupShow-1]);				//����
    DrawGbText(cBuff, 71, 66);
			
		if((AxleGroupShow) == 1)															//�ж�����
		{
			DrawGbText("����", 131, 66);													//��һ����Ϊ����
		}
		else
			DrawGbText("˫��", 131, 66);													//��������Ĭ��Ϊ˫��
			
	sprintf(cBuff,"%d m",(int)(gMainCarInfo.AxleLen[AxleGroupShow-1] * 0.01f));				//���
	DrawGbText(cBuff, 191, 66);
    if(1 == gMainCarInfo.OverWetFlag) 																//�Ƿ���
	{
      DrawGbText("����", 210, 22);
    }
    DrawGbText("��", 18, 44);
    DrawGbText("����", 71, 44);
    DrawGbText("����", 131, 44);
    DrawGbText("���", 191, 44);
    
   }
	//���ߣ�ˮƽ��������ֱ������
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
void Form_Main_Axle_Proc(LPWindow pWindow, LPGuiMsgInfo pGuiMsgInfo)
{
	CControl* pControl;	
	GuiMsgInfo guiMsgInfo;
//	char cBuff[36];

	switch(pGuiMsgInfo->ID)
	{
		case WM_LOAD:
			ClearScreen();
			SysTimeDly(2);
			if(gMainCarInfo.AxleGroupNum) 
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
					if(AxleGroupShow  < gMainCarInfo.AxleGroupNum)
					{
						AxleGroupShow++;
					}
					else if(gMainCarInfo.AxleGroupNum) 
					{
						AxleGroupShow = gMainCarInfo.AxleGroupNum;
					} 
					else 
					{
					 AxleGroupShow = 0;
					}
							
					PostWindowMsg(pWindow, WM_SHOW, 0, 0);
					break;

				case KEY_LEFT:
					if(AxleGroupShow  <= gMainCarInfo.AxleGroupNum)
					{
    					if(AxleGroupShow>1)
    					{
    						AxleGroupShow--;
    					}
					}
					else if(gMainCarInfo.AxleGroupNum) 
					{
					  AxleGroupShow = gMainCarInfo.AxleGroupNum;
					}
					else
					{
					  AxleGroupShow = 0;
					}
					PostWindowMsg(pWindow, WM_SHOW, 0, 0);
					break;
                    
				case KEY_CAR:
					g_pCurWindow = &gWD_Main_Veh;
					g_pCurWindow->pParentWindow = &gWD_Main;
					guiMsgInfo.pWindow = g_pCurWindow;
					guiMsgInfo.ID = WM_LOAD;
					GuiMsgQueuePost(&guiMsgInfo);
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
// 				case VK_6:
				case KEY_7:
				case KEY_8:
				case KEY_9:
					break;
				default:
					break;
			}
			break;
        case WM_CARIN:
//             if(gVehWeight.ucAxleGroupCount) {
//                 AxleGroupShow = gVehWeight.ucAxleGroupCount-1;
//             } else {
//                 AxleGroupShow = -1;
//             }
            PostWindowMsg(pWindow, WM_SHOW,0,0);
            break;
        case WM_CARBACK:
            
            break;
    	case WM_AXELADD:
            
            break;
    	case WM_AXELSUB:
            
            break;
    	case WM_VEHWET:
           
            break;
		
		default:
			break;
	}	
}



