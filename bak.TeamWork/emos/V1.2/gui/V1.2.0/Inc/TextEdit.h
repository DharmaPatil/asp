#ifndef __TEXT_EDIT_H_
#define __TEXT_EDIT_H_
#include "includes.h"
#include "GDI.h"
#include "guiwindows.h"



//�򿪱༭�����ô���
/****************************************************************************
* ��	�ƣ�BOOL LoadEditDlg(CWindow* pDlg, char* pDlgName, char* pTextIn, char* pTextOut, uint16 nMaxLen)
* ��	�ܣ��򿪱༭�����ô���
* ��ڲ�����CWindow*	pDlg				������ָ��
						char*			pDlgName		Ҫ��ʾ�ı༭����
						char*			pTextIn			Ҫ��ʾ�Ŀɱ༭�ı�
						char*			pTextOut		�޸ĺ���ı��������ַ
						uint16		mMaxLen			�ı�����󳤶�	
* ���ڲ�����
* ˵	������
****************************************************************************/
BOOL LoadEditDlg(CWindow* pDlg, char* pDlgName, char* pTextIn, char* pTextOut, uint16 nMaxLen);
extern CWindow g_TextEditWindow;


//�ڲ�����===========================================================================================
#define TEXT_EDIT_MAX_CHAR		36

//------------------TextEdit--------------
typedef struct _CTextEdit
{
	int16	nMaxAllowed;					   	//���������ַ��༭��
	int16	nCurLen;							//��ǰд����ַ���Ŀ
	int16	nCurPos;							//��ǰ���༭���ַ�����
	char*	pMaskBuff;							//���뻺����
	char*	pTextIn;							//��ʼ���ַ�����ָ��
	char	arrDigit[TEXT_EDIT_MAX_CHAR];		//�ַ���
}CTextEdit;

void LoadTextEdit(CControl* pControl);
void ApplyTextEdit(CControl* pControl);
void DrawTextEdit(CControl* pControl);
uint16 ProcTextEdit(CControl* pControl, struct _GuiMsgInfo* pGuiMsgInfo);

//���帡�����ؼ�
//С�ֺ�
#define DEF_TEXT_EDIT(Name, pParent, x, y, cx, cy, state, pTextIn, nMaxAllowed, pMaskBuff, Align)	 		\
CTextEdit CTRL_CONTENT(Name) = {(nMaxAllowed), 0, 0, pMaskBuff, pTextIn};					 	 \
CControl Name = {(pParent), (x), (y), (cx), (cy), (state), (Align), (&CTRL_CONTENT(Name)), DrawTextEdit, ProcTextEdit} \

extern void DefaultTextEditKeyProc(LPWindow pWindow, LPGuiMsgInfo pGuiMsgInfo);


#endif 
