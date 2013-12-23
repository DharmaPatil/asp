#include "Ctr_DlgList.h"
#include "Form_Resource.h"


#define GET_LIST_IN_DLG(pListDlg) ((CListSelect*)(((CControl*)(pListDlg->pLPControls))->pContent))



int16 LoadListDlg(CWindow* pDlg)
{
	int16 nSel;
	uint8 err;
	GuiMsgInfo guiMsgInfo;		//GUI��Ϣ�ṹʵ��

	//���ø�����
	pDlg->pParentWindow = g_pCurWindow;	

	//���ͼ�����Ϣ
	PostWindowMsg(pDlg, WM_LOAD, 0, 0);
	
	//��Ϣѭ��
	while(TRUE)
	{
		GuiMsgQueuePend(&guiMsgInfo, &err);		//�ȴ�GUI������Ϣ
		if(err == SYS_NO_ERR) 
		{
			if(guiMsgInfo.ID == WM_RETURN)
			{
				g_pCurWindow = pDlg->pParentWindow;
				PostWindowMsg(g_pCurWindow, WM_SHOW, 0, 0);
				break;
			}
			else
			{
				pDlg->ProcFunc(pDlg, &guiMsgInfo);
			}
		}  
	}	
		
	nSel = GET_LIST_IN_DLG(pDlg)->select;
	return nSel;
}


int16 ListDlgSetCurSel(CWindow* pDlg, int16 nSel)
{
	if(nSel >= GET_LIST_IN_DLG(pDlg)->total)
	{
		nSel = GET_LIST_IN_DLG(pDlg)->total - 1;
	}		

	GET_LIST_IN_DLG(pDlg)->select = nSel;

	return nSel;
}
																


void Dlg_List_Draw(LPWindow pWindow)
{
	LPControl	lpControl;

	//��ֹ��ͼ
	SetRedraw(FALSE);

	//��ֹˢ��
	EnableScreenFlush(FALSE);

	//������ͼ
	SetGdiView(pWindow->nViewPosX, pWindow->nViewPosY, pWindow->nViewSizeX, pWindow->nViewSizeY);
	EnableGdiView(TRUE);

	//����Դ�
	EraseRect(0, 0, pWindow->nViewSizeX, pWindow->nViewSizeY);

	//���ƿؼ�
	lpControl = (LPControl)(pWindow->pLPControls);
	if(lpControl->state & CTRL_VISABLE)
	{
		lpControl->DrawFunc(lpControl);
	}

	//������߿�
	DrawRect(0, 0, pWindow->nViewSizeX - 1, pWindow->nViewSizeY - 1);
	DrawHoriLine(1, pWindow->nViewSizeY - 1, pWindow->nViewSizeX);
	DrawVertLine(pWindow->nViewSizeX - 1, 1, pWindow->nViewSizeY);

	//ʹ��ˢ��
	EnableScreenFlush(TRUE);

	//ˢ��
	FlushScreen();

	//ʹ�ܻ�ͼ
	SetRedraw(TRUE);
}

void Dlg_List_Proc(LPWindow pWindow, LPGuiMsgInfo pGuiMsgInfo)
{
	CControl* pControl;	

	switch(pGuiMsgInfo->ID)
	{
		case WM_LOAD:
			GET_LIST_IN_DLG(pWindow)->focus = 0;
		case WM_SHOW:
			pWindow->DrawFunc(pWindow);
			break;
		case WM_CLOSE:
			if(pWindow->pParentWindow != NULL)
			{
				//���ظ�����
				g_pCurWindow = pWindow->pParentWindow;
				PostWindowMsg(g_pCurWindow, WM_RETURN, 0, 0);
			}
			break;
		case WM_UPDATECTRL:
			pControl = (CControl*)(pGuiMsgInfo->wParam);
			if(pControl != NULL)
			{
				pControl->DrawFunc(pControl);
			} 			
			break;
		case WM_KEYDOWN:
			switch(pGuiMsgInfo->wParam)
			{
				case KEY_UP:
                    if(GET_LIST_IN_DLG(pWindow)->focus > 0)
					{
                        GET_LIST_IN_DLG(pWindow)->focus--;
    					PostWindowMsg(pWindow, WM_UPDATECTRL, (uint32)(pWindow->pLPControls), 0);
                    }
					break;
				case KEY_DOWN:
                    if(GET_LIST_IN_DLG(pWindow)->focus < GET_LIST_IN_DLG(pWindow)->total - 1)
					{
                        GET_LIST_IN_DLG(pWindow)->focus++;
    					PostWindowMsg(pWindow, WM_UPDATECTRL, (uint32)(pWindow->pLPControls), 0);
                    }
					break;
				case KEY_OK:
                    GET_LIST_IN_DLG(pWindow)->select = GET_LIST_IN_DLG(pWindow)->focus;
					PostWindowMsg(pWindow, WM_CLOSE, 0, 0);
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
                    GET_LIST_IN_DLG(pWindow)->focus = pGuiMsgInfo->wParam - KEY_1;
					PostWindowMsg(pWindow, WM_UPDATECTRL, (uint32)(pWindow->pLPControls), 0);
					break;
				case KEY_BACK:
					GET_LIST_IN_DLG(pWindow)->select = -1;
					PostWindowMsg(pWindow, WM_CLOSE, 0, 0);
					break;				   
				default:
					break;
			}
			break;
		
		default:
			break;
	}	
}


