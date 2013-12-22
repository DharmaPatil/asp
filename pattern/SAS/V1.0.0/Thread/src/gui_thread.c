#include "includes.h"
#include "Form_Logo.h"
#include "Form_Main.h"
#include "Keyboard.h"
#include "guiwindows.h"

void GUI_Thread(void *arg)
{

	uint8 err;
	GuiMsgInfo guiMsgInfo;		//GUI��Ϣ�ṹʵ��
	
	//��ʼ��GUI
	BeginPaint();
	
	//����Ĭ����ɫ
	SetColor(0);
	
	//����
	EraseScreen();	 

	//��ʾ��������
    guiMsgInfo.ID = WM_LOAD;
    gWD_Logo.ProcFunc(&gWD_Logo, &guiMsgInfo);
    SysTimeDly(300);		  					
	
	//����GUI��Ϣ���н���
	GuiMsgQueueCreate();

	//����������
	g_pCurWindow = &gWD_Main;// &gWD_Main_Axle;				//&g_FlowWindow;	
	
	//������ǰ���ڷ��ͼ�����Ϣ
	guiMsgInfo.pWindow = g_pCurWindow;
	guiMsgInfo.ID = WM_LOAD;
	GuiMsgQueuePost(&guiMsgInfo);

	//��Ϣѭ��
	while(TRUE)
	{
		GuiMsgQueuePend(&guiMsgInfo, &err);		//�ȴ�GUI������Ϣ
		
		if(err == SYS_NO_ERR) 
		{
			if((g_pCurWindow != NULL) && (g_pCurWindow->ProcFunc != NULL))
			{
				g_pCurWindow->ProcFunc(g_pCurWindow, &guiMsgInfo);
			}
		}  
	}
}
