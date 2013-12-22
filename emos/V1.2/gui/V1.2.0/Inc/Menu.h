#ifndef __MENU_H
#define __MENU_H
#include "includes.h"
#include "GDI.h"
#include "guiwindows.h"

#define MAX_SHOW_MENU_ITEM		8		//�����ʾ��Ŀ��
#define MAX_MENU_NAME_CHARACTER	10		//���˵�������
#define MAX_LINE_CHARACTER		20		//һ������ʾ������ַ���

struct _CMenu;

//�˵�������
typedef void (*MenuProcTypeDef)(struct _CWindow* pParentWindow, uint32 nCmd);

//����˵���
typedef struct _CMenuItem
{
	char* 					strName;				 		//�˵�������
	uint16 					nbs;					 			//�˵������Ƶ��ַ�����
	uint16 					nCmd;				 				//�˵�������
	struct _CMenu* 	pSubMenu;						//�Ӳ˵�
} CMenuItem, *LPMenuItem;

//����˵�
typedef struct _CMenu
{
	char*					strName;					//�˵���
	LPMenuItem*		pLPMenuItems;			//�˵���ָ������
	uint16				nNbItems;					//�˵������
	uint16				nCurItem;					//��ǰ��ѡ�еĲ˵�
	uint16				nCurPos;					//��ǰѡ�в˵��ڴ����е�λ��
	uint16				nFirstItem;				//�����е�һ����ʾ���Ĳ˵���

	struct _CMenu*		pParentMenu;		//���˵�
	MenuProcTypeDef		ProcFunc;
	
} CMenu, *LPMenu;

/****************************************************************************
* ��	�ƣ�DEF_MENU_ITEM(Name, strName, nCmd, pSubMenu)
* ��	�ܣ�����˵�
* ��ڲ�����CMenuItem				Name			�˵��ṹ��
						char*						strName		�˵���
						uint16					nCmd			�˵�������
						struct _CMenu*	pSubMenu	�Ӳ˵�
* ���ڲ�����
* ˵	������
****************************************************************************/
#define DEF_MENU_ITEM(Name, strName, nCmd, pSubMenu)	\
CMenuItem Name = {(strName), 0, nCmd, pSubMenu}					 


#define DEF_MENU(Name, strName, Items, pParentMenu, ProcFunc)	\
CMenu Name = {strName, (Items), sizeof(Items)/sizeof(LPMenu), 0, 0, 0, (pParentMenu), (ProcFunc)}					 

#define DEF_MENU_WND(Name, pMenu, pParentWindow)	\
CWindow Name = {(LPControl*)(pMenu), 0, 0, 0, 0, 240, 128, WS_VISABLE, (pParentWindow), 0, NULL, NULL, DefaultMenuWindowDraw, DefMenuWindowProc}	

//���ں���
void DefaultMenuWindowDraw(LPWindow pWindow);
void DefMenuWindowProc(LPWindow pWindow, LPGuiMsgInfo pGuiMsgInfo);

#endif



