#include "Form_MsgBox.h"


//========================================���������ֵĸ������༭����========================================
static const char* m_pTitle = NULL;
DEF_TEXT_LABEL(mMsgText, &g_MsgBoxDlg, 5, 40, 100, 14, CTRL_VISABLE, "");

static LPControl m_MsgBoxDlgControl[] = 
{
	&mMsgText,
};

void MsgBoxDlgDraw(LPWindow pWindow);
void MsgBoxDlgProc(LPWindow pWindow, LPGuiMsgInfo pGuiMsgInfo);

CWindow g_MsgBoxDlg = 
{
	m_MsgBoxDlgControl,
	sizeof(m_MsgBoxDlgControl)/sizeof(LPControl),
	0,
	20,
	20,
	200,
	88,
	WS_VISABLE,
	NULL,
	6, 
	NULL, 
	NULL,
	MsgBoxDlgDraw,
	MsgBoxDlgProc
};

//�򿪱༭�����ô���
void MsgBoxDlg(CWindow* pDlg, char* pTitle, char* pMsg)
{
	uint8 err;
	GuiMsgInfo guiMsgInfo;		//GUI��Ϣ�ṹʵ��

	//�������
	if(pMsg == NULL) return;

	m_pTitle = pTitle;	
	CTRL_CONTENT(mMsgText).s = pMsg;	 

	//���ø�����
	pDlg->pParentWindow = g_pCurWindow;	

    g_pCurWindow = pDlg;
	//���ͼ�����Ϣ
	PostWindowMsg(g_pCurWindow, WM_LOAD, NULL, 0);
	
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
}

void MsgBoxDlgDraw(LPWindow pWindow)
{
	uint16 i;
	int16 nStrSize;
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

	//���ƴ�������
	if(m_pTitle != NULL)
	{
		nStrSize = strlen((char*)m_pTitle);	
		if(nStrSize > 0)
		{		
			DrawGbText((char*)m_pTitle, 2, 2);
		}	
	}
	
	//��ֹ��ͼ
	//SetRedraw(FALSE);

	//��ֹˢ��
	//EnableScreenFlush(FALSE);

	//���ƿؼ�
	for(i = 0; i < pWindow->nNbControls; i++)
	{
		lpControl = *(pWindow->pLPControls + i);

		if((lpControl->state & CTRL_VISABLE) != 0)
		{ 
			lpControl->DrawFunc(lpControl);
		}
	}

	//������߿�
	DrawRect(0, 0, pWindow->nViewSizeX - 1, pWindow->nViewSizeY - 1);
	DrawHoriLine(1, pWindow->nViewSizeY - 1, pWindow->nViewSizeX);
	DrawVertLine(pWindow->nViewSizeX - 1, 1, pWindow->nViewSizeY);

	//���ָ���
	DrawHoriLine(1, 18, pWindow->nViewSizeX); 

	//ʹ��ˢ��
	EnableScreenFlush(TRUE);

	//ˢ��
	FlushScreen();

	//ʹ�ܻ�ͼ
	SetRedraw(TRUE);
}



void MsgBoxDlgProc(LPWindow pWindow, LPGuiMsgInfo pGuiMsgInfo)
{
	CControl* pControl;
	switch(pGuiMsgInfo->ID)
	{
		case WM_LOAD:
			pWindow->DrawFunc(pWindow);
			break;

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
				case KEY_BACK:			//����
					PostWindowMsg(pWindow, WM_CLOSE, 0, 0);
					break;

				case KEY_OK:				//ȷ��
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

