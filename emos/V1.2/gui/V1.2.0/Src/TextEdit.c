#include "guiwindows.h"
#include "TextEdit.h"

#include "string.h"

void LoadTextEdit(CControl* pControl)
{
	int16 wText;
	CTextEdit* pEdit = (CTextEdit*)(pControl->pContent);

	if(pEdit->pTextIn != NULL)
	{
		wText = strlen(pEdit->pTextIn);
	
		pEdit->nCurLen = wText;
		pEdit->nCurPos = wText;
	
		if(wText < pEdit->nMaxAllowed)
		{
			//���Ƶ��༭��
			strcpy(&(pEdit->arrDigit[0]), pEdit->pTextIn);   
		}
		else
		{
			//ֻ������Ч�ķ�Χ
			memcpy(&(pEdit->arrDigit[0]), pEdit->pTextIn, pEdit->nMaxAllowed);
			pEdit->arrDigit[pEdit->nMaxAllowed] = 0;
		}
	}
	else
	{
		pEdit->nCurLen = 0;
		pEdit->nCurPos = 0;
	}
}

/**
  * @brief  ת�������������ݡ�
  * @param  
  * @param  
  * @retval void
  */
void ApplyTextEdit(CControl* pControl)
{

}

/**
  * @brief  �ڽ����յ��ػ���Ϣ�󣬴�������ÿ���ؼ��Լ����ػ溯�����������������
  * 		��Ҫ�������ߺ���˸������λ�ù�ꡣ
  * @param  
  * @param  
  * @retval void
  */
void DrawTextEdit(CControl* pControl)
{
	int16 nStartPosX;
	CTextEdit* pEdit = (CTextEdit*)(pControl->pContent);

	//����ؼ�����
	EnableScreenFlush(FALSE);

	//�����ؼ�����
	EraseRect(pControl->x, pControl->y, pControl->sx, pControl->sy);

	//ȷ���������
	if(pControl->sx >= pEdit->nCurLen * 7)			//�ؼ���ȴ����ı����
	{
		if((pControl->align & 0x000F) == TA_LEFT)
		{
			nStartPosX = pControl->x; 
		}
		else if((pControl->align & 0x000F) == TA_CENTER)
		{
			nStartPosX = pControl->x + (pControl->sx - pEdit->nCurLen*7)/2; 
		}
		else if((pControl->align & 0x000F) == TA_RIGHT)
		{
			nStartPosX = pControl->x + (pControl->sx - pEdit->nCurLen*7);
		}
	}
	else							//�����
	{
		nStartPosX = pControl->x; 
	}

	//��������
	pEdit->arrDigit[pEdit->nCurLen] = 0;
	DrawGbText(pEdit->arrDigit, nStartPosX, pControl->y);
// 	for(i = 0; i < pEdit->nCurLen; i++)
// 	{
// 		DrawSmallChar(pEdit->arrDigit[i], nStartPosX + i * 9, pControl->y);
// 	} 
	 
	DrawRect(pControl->x, pControl->y + 16, pControl->sx, 1);  //��������º���

	//if(((pEdit->nPosStatues & CTRL_VISABLE) != 0) && ((pControl->state & CTRL_FOCUS) != 0))
	//	DrawRect(nStartPosX + pEdit->pPosx, pEdit->pPosy, 1,  15);	//����˸�����ꡣ

	DrawRect(nStartPosX + pEdit->nCurPos*7, pControl->y, 1,  15);	//����˸�����ꡣ

	//ˢ�¿ؼ�����
	EnableScreenFlush(TRUE);
	FlushRect(pControl->x, pControl->y, pControl->sx, pControl->sy);
}

uint16 ProcTextEdit(CControl* pControl, struct _GuiMsgInfo* pGuiMsgInfo)
{
	GuiMsgInfo guiMsgInfo;
	int8 i = 0;
	CTextEdit* pEdit = (CTextEdit*)(pControl->pContent);

	switch(pGuiMsgInfo->ID)
	{
		case WM_LOAD:
			break;
		case WM_KEYDOWN:
			switch(pGuiMsgInfo->wParam)
			{
				case KEY_OK:		//ȷ��
					ApplyTextEdit(pControl);
					break;
				case KEY_UP:	
					break;
				case KEY_DOWN:
					break;
				case KEY_LEFT:
					//���ƹ��
					if(pEdit->nCurPos > 0)
					{
						pEdit->nCurPos--;
						guiMsgInfo.pWindow = g_pCurWindow;
						guiMsgInfo.ID = WM_SHOW;
						GuiMsgQueuePost(&guiMsgInfo);
					}
					break;

				case KEY_RIGHT:
					//���ƹ��
					if(pEdit->nCurPos < pEdit->nCurLen)
					{
						pEdit->nCurPos++;
						guiMsgInfo.pWindow = g_pCurWindow;
						guiMsgInfo.ID = WM_SHOW;
						GuiMsgQueuePost(&guiMsgInfo);
					}
					break;
			    case KEY_0:
				case KEY_1:
				case KEY_2:
				case KEY_3:
				case KEY_4:
				case KEY_5:
				case KEY_6:
				case KEY_7:
				case KEY_8:
				case KEY_9:												//��������
					if(pEdit->nCurLen >= pEdit->nMaxAllowed) return 0;	//���λ������

					for(i = pEdit->nCurLen - 1; i >= pEdit->nCurPos; i--)
					{
						pEdit->arrDigit[i+1] = pEdit->arrDigit[i];
					}
					pEdit->arrDigit[pEdit->nCurPos] =  0x30 + pGuiMsgInfo->wParam - KEY_0;
					pEdit->nCurPos++;
					pEdit->nCurLen++;
					guiMsgInfo.pWindow = g_pCurWindow;
					guiMsgInfo.ID = WM_SHOW;
					GuiMsgQueuePost(&guiMsgInfo);
					break;

				case KEY_DOT:			//���ţ�С����(���������뷨ʱ��Ϊ�����Ź���)
					if(pEdit->nCurLen >= pEdit->nMaxAllowed) return 0;	//���λ������

					for(i = pEdit->nCurLen - 1; i >= pEdit->nCurPos; i--)
					{
						pEdit->arrDigit[i+1] = pEdit->arrDigit[i];
					}
					pEdit->arrDigit[pEdit->nCurPos] =  '.';
					pEdit->nCurPos++;
					pEdit->nCurLen++;
					guiMsgInfo.pWindow = g_pCurWindow;
					guiMsgInfo.ID = WM_SHOW;
					GuiMsgQueuePost(&guiMsgInfo);
					break;

// 				case VK_PLUS_MINUS:			//���ţ�����(���������뷨ʱ��Ϊ���뷨�л�����)
// 					break;

				case KEY_Del: 			//ɾ����괦������
					if((pEdit->nCurLen == 0) || (pEdit->nCurPos == 0))
					{
						return 0;
					}

					if(pEdit->nCurPos > 0)	
					{
						for(i = pEdit->nCurPos; i < pEdit->nCurLen; i++ )
						{
							pEdit->arrDigit[i-1] = pEdit->arrDigit[i];
						}
					}
					pEdit->nCurLen--;
					pEdit->nCurPos--;
					guiMsgInfo.pWindow = g_pCurWindow;
					guiMsgInfo.ID = WM_SHOW;
					GuiMsgQueuePost(&guiMsgInfo);
					break;

				//Ĭ�Ϸ���
				default:
					return 0;
			}
			break;
		default:
			return 0;
	}

	return 0;		
}

void DefaultTextEditKeyProc(LPWindow pWindow, LPGuiMsgInfo pGuiMsgInfo)
{
	if((pWindow->nCurControl >= 0) && (pWindow->nCurControl < pWindow->nNbControls))
	{
		if(pWindow->pLPControls[pWindow->nCurControl]->ProcFunc != NULL)
		{
			if(pWindow->pLPControls[pWindow->nCurControl]->ProcFunc(pWindow->pLPControls[pWindow->nCurControl], pGuiMsgInfo) > 0)
			{
				//break;
			} 
		}
	}
	return;	
}

//========================================���������ֵĸ������༭����========================================
static const char* m_pName = NULL;
DEF_TEXT_EDIT(m_TextEdit, &g_TextEditWindow, 20 + 8, 20 + 32, 184, 28, CTRL_VISABLE, NULL, 16, NULL, TA_CENTER);

LPControl m_arrLPTextEditWindowControl[] = 
{
	&m_TextEdit,
};

void TextEditWindowDraw(LPWindow pWindow);
void TextEditWindowProc(LPWindow pWindow, LPGuiMsgInfo pGuiMsgInfo);
void TextEditWndTmrCallBack(void *ptmr, void *parg);
CWindow g_TextEditWindow = 
{
	m_arrLPTextEditWindowControl,
	sizeof(m_arrLPTextEditWindowControl)/sizeof(LPControl),
	0,
	20,
	20,
	200,
	88,
	WS_VISABLE,
	NULL,
	6, 
	NULL, 
	TextEditWndTmrCallBack,
	TextEditWindowDraw,
	TextEditWindowProc
};

//BOOL OpenTextEditWindow(const char* pName, CWindow* pParentWindow, float* pFloat, uint8 nLeftDigs, uint8 nRightDigs, uint8 nReadOnly, TextEditCallBack CallBack)
//{
//	m_pName = pName;
//	CTRL_CONTENT(m_TextEdit).pF = pFloat;
//	CTRL_CONTENT(m_TextEdit).nLeftDigs = nLeftDigs;
//	CTRL_CONTENT(m_TextEdit).nRightDigs = nRightDigs;
//	CTRL_CONTENT(m_TextEdit).nReadOnly = nReadOnly;
//	CTRL_CONTENT(m_TextEdit).CallBack = CallBack;
//	
//	g_TextEditWindow.pParentWindow = pParentWindow;
//	g_pCurWindow = &g_TextEditWindow;
//
//
//
//	PostWindowMsg(WM_LOAD, 0, 0);
//	return TRUE;
//}

//�򿪱༭�����ô���
BOOL LoadEditDlg(CWindow* pDlg, char* pDlgName, char* pTextIn, char* pTextOut, uint16 nMaxLen)
{
	uint8 err;
	GuiMsgInfo guiMsgInfo;		//GUI��Ϣ�ṹʵ��
	BOOL bReturn = 0;

	//�������
	if(pTextIn == NULL) return 0;
	if(pTextOut == NULL) return 0;

	//�������ݳ�ʼ��
	m_pName = pDlgName;	
	
	//�༭�����ݳ�ʼ��
	CTRL_CONTENT(m_TextEdit).nMaxAllowed = nMaxLen;	 
	CTRL_CONTENT(m_TextEdit).pTextIn = pTextIn;	
	//strcpy(CTRL_CONTENT(m_TextEdit).arrDigit, pTextIn);

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
			
				if((guiMsgInfo.ID == WM_KEYDOWN) && (guiMsgInfo.wParam == KEY_OK))
				{
					strcpy(pTextOut, CTRL_CONTENT(m_TextEdit).arrDigit);
					bReturn = 1;
				}			
			}
		}  
	}
	
	return bReturn;
}

void TextEditWindowDraw(LPWindow pWindow)
{
	uint16 i;
	int16 nStrSize;
	LPControl lpControl;	  

	//��ֹ��ͼ
	SetRedraw(FALSE);
	//��ֹˢ��
	EnableScreenFlush(FALSE);
	//������ͼ
	SetGdiView(pWindow->nViewPosX, pWindow->nViewPosY, pWindow->nViewSizeX, pWindow->nViewSizeY);
	EnableGdiView(TRUE);
	//����Դ�
	EraseRect(pWindow->nViewPosX, pWindow->nViewPosY, pWindow->nViewSizeX, pWindow->nViewSizeY);
	//���ƴ�������
	if (m_pName != NULL) {
		nStrSize = strlen((char*)m_pName);	
		if (nStrSize > 0) {		
			DrawGbText((char*)m_pName, pWindow->nViewPosX + 2, pWindow->nViewPosY + 2);
		}	
	}
	//��ֹ��ͼ
	//SetRedraw(FALSE);
	//��ֹˢ��
	//EnableScreenFlush(FALSE);
	//���ƿؼ�
	for (i = 0; i < pWindow->nNbControls; i++) {
		lpControl = *(pWindow->pLPControls + i);
		if ((lpControl->state & CTRL_VISABLE) != 0) { 
			lpControl->DrawFunc(lpControl);
		}
	}

	//������߿�
	DrawRect(pWindow->nViewPosX, pWindow->nViewPosY, pWindow->nViewSizeX - 1, pWindow->nViewSizeY - 1);
	DrawHoriLine(pWindow->nViewPosX + 1, pWindow->nViewSizeY-1+pWindow->nViewPosY, pWindow->nViewSizeX);
	DrawVertLine(pWindow->nViewSizeX-1+pWindow->nViewPosX, pWindow->nViewPosY + 1, pWindow->nViewSizeY-1);

	//ʹ��ˢ��
	EnableScreenFlush(TRUE);

	//ˢ��
	FlushScreen();

	//ʹ�ܻ�ͼ
	SetRedraw(TRUE);
}



void TextEditWindowProc(LPWindow pWindow, LPGuiMsgInfo pGuiMsgInfo)
{
	CControl* pControl;
	switch(pGuiMsgInfo->ID)
	{
		case WM_LOAD:
			CreateWindowTimer(pWindow);
			StartWindowTimer(pWindow);

			LoadTextEdit(&m_TextEdit);
			m_TextEdit.state |= CTRL_FOCUS;		   //�趨��������ʾ�ĸ��༭������޸�
			pWindow->nCurControl = 0;
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
			DefaultTextEditKeyProc(pWindow,pGuiMsgInfo);	 //�������̵Ĵ���
			switch(pGuiMsgInfo->wParam)
			{
				case KEY_BACK:			//����
					StopWindowTimer(pWindow);
					PostWindowMsg(pWindow, WM_CLOSE, 0, 0);
					break;

				case KEY_OK:				//ȷ��
					StopWindowTimer(pWindow);
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

void TextEditWndTmrCallBack(void *ptmr, void *parg)
{
	static uint8 flag = 0;
	LPWindow lpWindow = (LPWindow)parg;
	if(lpWindow != NULL)
	{
		flag = ~flag;

		//if(flag)	CTRL_CONTENT(m_TextEdit).nPosStatues &= ~CTRL_VISABLE;
		//else		CTRL_CONTENT(m_TextEdit).nPosStatues |= CTRL_VISABLE;

		//PostWindowMsg(WM_SHOW, 0, 0);
		//PostWindowMsg(WM_UPDATECTRL, (uint32)&m_TextEdit);
	}
}
