#ifndef __GUI_WINDOWS_H
#define __GUI_WINDOWS_H
#include "includes.h"



struct _CControl;
struct _CWindow;
struct _GuiMsgInfo;

typedef void (*GuiDrawTypeDef)(struct _CControl* pCControl);
typedef uint16 (*ControlProcTypeDef)(struct _CControl* pCControl, struct _GuiMsgInfo* pGuiMsgInfo);

typedef void (*WindowDrawTypeDef)(struct _CWindow* pWindow);
typedef void (*WindowProcTypeDef)(struct _CWindow* pWindow, struct _GuiMsgInfo* pGuiMsgInfo);

//���ڷ��
//window styles
#define WS_VISABLE		 0x00000001L		//���ڿ���
#define WS_CHILD		 0x00000002L		//�����Ƿ�Ϊ�Ӵ���
#define WS_POPUP	   	 0x00000004L		//��������
#define WS_DISABLED     0x00000008L		//�����Ƿ񱻽�ֹ


//�ؼ�״̬
//control state
#define CTRL_VISABLE	0x0001		//�ؼ�����
#define CTRL_FOCUS		0x0002		//�ؼ��������뽹��


//���뷽ʽ
//test allign
#define TA_LEFT			0x0001
#define TA_CENTER		0x0002
#define TA_RIGHT		0x0004
#define TA_TOP			0x0010
#define TA_VCENTER	0x0020
#define TA_BOTTOM		0x0040

//��ȡ�ؼ�����
#define CTRL_CONTENT(Name)	 Name##CC
#define CTRL_NAME(Name)		 Name##CTRL

typedef struct _CControl
{
	//����
	struct _CWindow*	pParent;			//������
	int16	x;												//��ʾ��x����ʼλ��
	int16	y;												//��ʾ��y����ʼλ��
	int16	sx;												//��ʾ��x�᳤��
	int16	sy;												//��ʾ��y�᳤��
	int16	state;										//��ʾ״̬
	int16	align;										//���뷽ʽ
	void*	pContent;					   			//�ؼ�����

	//����
	GuiDrawTypeDef			DrawFunc;		//�ؼ���ͼ����
	ControlProcTypeDef	ProcFunc;		//�ؼ�������
}CControl, *LPControl;

#define SYS_TMR	int
#define SYS_TMR_CALLBACK int

typedef struct _CWindow
{
	LPControl*			pLPControls;//�ؼ���ַ
	int16				nNbControls;		//�ؼ�����
	int16				nCurControl;
	int16				nViewPosX;			//��ͼˮƽλ��(����������ڣ���λΪ����)
	int16				nViewPosY;			//��ͼ��ֱλ��(����������ڣ���λΪ����)
	int16				nViewSizeX;			//��ͼˮƽ�ߴ�(��λΪ����)
	int16				nViewSizeY;			//��ͼ��ֱ�ߴ�(��λΪ����)
	uint32				nStyle;				//���ڷ��
	struct _CWindow*	pParentWindow;		//������

	uint32				period;				//��ʱ������
	SYS_TMR*				pTimer;
	SYS_TMR_CALLBACK		tmrCallBack;	//��ʱ���ص�����

	WindowDrawTypeDef	DrawFunc;			//���ڻ�ͼ����
	WindowProcTypeDef	ProcFunc;			//���ڴ�����
}CWindow, *LPWindow;

typedef enum
{
	WM_LOAD,
	WM_SHOW,
	WM_CLOSE,	   
	WM_RETURN,
	WM_KEYDOWN,
	WM_KEYUP,
	WM_TIMER,
	WM_TIMEUPDATE,		//ʱ�����
	WM_UPDATECTRL,		//ˢ�¿ؼ�
	WM_SUBMENU,			
	WM_ICREAD,			//ˢ����Ϣ
	WM_INPUTUSERID,		//�����û�������Ϣ
	WM_CARIN,           //��������
	WM_CARBACK,         //�����˳�
	WM_AXELADD,         //����������
	WM_AXELSUB,         //��С������
	WM_VEHWET,          //�����������
	WM_VEHOVERSPEED,    //����
	WM_VEHSTA,           //����״̬
	
	
	WM_LCDTest,					//��Ļ����
} GuiMsgID;				//������ϢID��


typedef struct _GuiMsgInfo		//GUI��Ϣ��Ϣ
{

	uint16		ID;				//GUI��Ϣ��
	uint32		wParam;
	uint32		lParam;
	LPWindow	pWindow;
} GuiMsgInfo, *LPGuiMsgInfo;





uint8 GuiMsgQueuePost(GuiMsgInfo* pGuiMsgInfo);


extern CWindow*	g_pCurWindow;	//��ǰ����


#endif
