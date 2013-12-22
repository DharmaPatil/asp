#ifndef __WINDOWS_H
#define __WINDOWS_H
#include "includes.h"
#include "GDI.h"
#include "key_init.h"

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

// typedef enum
// {
// 	VK_NONE,					/*��Ч*/
// 	
// 	VK_DELETE,				/*ɾ��*/
// 	VK_UNITS,					/*��λ*/
// 	VK_SEEK,					/*��ѯ*/
// 	VK_PRINT,					//��ӡ		
// 	VK_UP,						/*��*/
// 	VK_DOWN,					/*��*/
// 	VK_LEFT,					/*��*/
// 	VK_RIGHT,					/*��*/
// 	VK_OK,						//ȷ��
// 	VK_MENU,					//�˵�
// 	VK_RETURN,				//����	
// 	VK_WHOLE_CAR,			/*����*/
// 	VK_AXLE_WEIGHT,		/*����*/
// 	VK_DIAGNOSE,			/*���*/
// 	VK_SET_ZERO,			/*����*/
// 	VK_0,
// 	VK_1,
// 	VK_2,
// 	VK_3,
// 	VK_4,
// 	VK_5,
// 	VK_6,
// 	VK_7,
// 	VK_8,
// 	VK_9,
// 	VK_PLUS_MINUS,		/*�Ӽ�*/
// 	VK_POINT,					/*С����*/
// 	VK_F1,						/*��չ��*/
// 	VK_F2,
// 	VK_F3,
// 	VK_F4,
// 	VK_F5,
// 	VK_F6,
// 	VK_F7,
// 	VK_F8,
//} VK_VAL;	 													//��ֵ���ھ���

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

/****************************************************************************
* ��	�ƣ�BOOL CreateWindowTimer(LPWindow lpWindow)
* ��	�ܣ�ע�ᴰ�ڶ�ʱ������1
* ��ڲ�����LPWindow lpWindow-��Ҫע��Ĵ��ڽṹ��
* ���ڲ�����TRUE	-	ע��ɹ�
						FALSE	-	ע��ʧ��
* ˵	������ʱ������ָ�룬��ʱʱ�䳤�Ⱦ��д��ڽṹ�崫��
****************************************************************************/
BOOL CreateWindowTimer(LPWindow lpWindow);

/****************************************************************************
* ��	�ƣ�BOOL CreateWindowTimerEx(LPWindow lpWindow, uint32 delay)
* ��	�ܣ�ע�ᴰ�ڶ�ʱ������2
* ��ڲ�����LPWindow lpWindow-��Ҫע��Ĵ��ڽṹ��
						uint32 delay	�״ν��붨ʱ��������ʱ�䳤��
* ���ڲ�����TRUE	-	ע��ɹ�
						FALSE	-	ע��ʧ��
* ˵	������ʱ������ָ�룬��ʱʱ�䳤�Ⱦ��д��ڽṹ�崫��
****************************************************************************/
BOOL CreateWindowTimerEx(LPWindow lpWindow, uint32 delay);

/****************************************************************************
* ��	�ƣ�BOOL DestoryWindowTimer(LPWindow lpWindow)
* ��	�ܣ�ע�����ڶ�ʱ������
* ��ڲ�����LPWindow lpWindow-��Ҫע��Ĵ��ڽṹ��
* ���ڲ�����TRUE	-	ע���ɹ�
						FALSE	-	ע��ʧ��
* ˵	������ʱ������ָ�룬��ʱʱ�䳤�Ⱦ��д��ڽṹ�崫��
****************************************************************************/
BOOL DestoryWindowTimer(LPWindow lpWindow);

/****************************************************************************
* ��	�ƣ�BOOL StartWindowTimer(LPWindow lpWindow)
* ��	�ܣ��������ڶ�ʱ
* ��ڲ�����LPWindow lpWindow-��Ҫ�����Ķ�ʱ���Ĵ��ڽṹ��
* ���ڲ�����TRUE	-	�����ɹ�
						FALSE	-	����ʧ��
* ˵	����
****************************************************************************/
BOOL StartWindowTimer(LPWindow lpWindow);

/****************************************************************************
* ��	�ƣ�BOOL StopWindowTimer(LPWindow lpWindow)
* ��	�ܣ��رմ��ڶ�ʱ
* ��ڲ�����LPWindow lpWindow-��Ҫ�رյĶ�ʱ���Ĵ��ڽṹ��
* ���ڲ�����TRUE	-	�رճɹ�
						FALSE	-	�ر�ʧ��
* ˵	����
****************************************************************************/
BOOL StopWindowTimer(LPWindow lpWindow);


//------------------TextLabel--------------
typedef struct _CTextLabel
{
	char*	s;
}CTextLabel;

void DrawTextLabel(CControl* pControl);

//�����ı��ؼ�
/****************************************************************************
* ��	�ƣ� DEF_TEXT_LABEL
* ��	�ܣ�	�����ı��ؼ�
* ��ڲ������Զ����ַ�	Name			�ؼ���
						CWindow*		pParent		�ÿؼ�Ҫ��ʾ�����ڴ���
						int16				x					x����ʼλ��
						int16				y					y����ʼλ��
						int16				cx				x����ʾ����
						int16				cy				y����ʾ����
						int16				state			�ؼ�״̬		��CTRL_VISABLE-�ؼ����ӣCTRL_FOCUS-�ؼ��������뽹�㣩
						char*				str				Ҫ��ʾ���ַ�
* ���ڲ�������
* ˵	������			
****************************************************************************/
#define DEF_TEXT_LABEL(Name, pParent, x, y, cx, cy, state, str)							\
CTextLabel CTRL_CONTENT(Name) = {(str)};					 	\
CControl Name = {(pParent), (x), (y), (cx), (cy), (state), (0), (&CTRL_CONTENT(Name)), DrawTextLabel, NULL}

//------------------FloatLabel--------------
typedef struct _CFloatLabel
{
	float*	pF;									//Ҫ��ʾ�ĸ������Ĵ�ŵ�ַ
	uint8	nFixPoint;						//��ʾ��λС��
	float	min;									//�������Сֵ
	float	max;									//��������ֵ
	char	unit[5];							//��λ�ַ���  t
}CFloatLabel;

void DrawSmallFloatLabel(CControl* pControl);
void DrawBigFloatLabel(CControl* pControl);

//���帡�����ؼ�
//С�ֺ�
/****************************************************************************
* ��	�ƣ� DEF_SMALL_FLOAT_LABLE
* ��	�ܣ�	���帡�����ؼ�(С�ֺ�)
* ��ڲ������Զ����ַ�	Name			�ؼ���
						CWindow*		pParent		�ÿؼ�Ҫ��ʾ�����ڴ���
						int16				x					x����ʼλ��
						int16				y					y����ʼλ��
						int16				cx				x����ʾ����
						int16				cy				y����ʾ����
						int16				state			�ؼ�״̬		��CTRL_VISABLE-�ؼ����ӣCTRL_FOCUS-�ؼ��������뽹�㣩
						float*			pFloat		Ҫ��ʾ�ĸ�������ַ
						uint8				nFixPoint	��ʾ��λС��
						float				fMin			�������Сֵ
						float				fMax			��������ֵ
						char				strUnit		��λ�ַ���
						int16				Align			���뷽ʽ										
* ���ڲ�������
* ˵	������			
****************************************************************************/
#define DEF_SMALL_FLOAT_LABLE(Name, pParent, x, y, cx, cy, state, pFloat, nFixPoint, fMin, fMax, strUnit, Align)							\
CFloatLabel CTRL_CONTENT(Name) = {(pFloat), (nFixPoint), (fMin), (fMax), (strUnit)};					 	\
CControl Name = {(pParent), (x), (y), (cx), (cy), (state), (Align), (&CTRL_CONTENT(Name)), DrawSmallFloatLabel, NULL}

//���ֺ�
/****************************************************************************
* ��	�ƣ� DEF_BIG_FLOAT_LABLE
* ��	�ܣ�	���帡�����ؼ�(���ֺ�)
* ��ڲ������Զ����ַ�	Name			�ؼ���
						CWindow*		pParent		�ÿؼ�Ҫ��ʾ�����ڴ���
						int16				x					x����ʼλ��
						int16				y					y����ʼλ��
						int16				cx				x����ʾ����
						int16				cy				y����ʾ����
						int16				state			�ؼ�״̬		��CTRL_VISABLE-�ؼ����ӣCTRL_FOCUS-�ؼ��������뽹�㣩
						float*			pFloat		Ҫ��ʾ�ĸ�������ַ
						uint8				nFixPoint	��ʾ��λС��
						float				fMin			�������Сֵ
						float				fMax			��������ֵ
						char				strUnit		��λ�ַ���
						int16				Align			���뷽ʽ
* ���ڲ�������
* ˵	������			
****************************************************************************/
#define DEF_BIG_FLOAT_LABLE(Name, pParent, x, y, cx, cy, state, pFloat, nFixPoint, fMin, fMax, strUnit, Align)							\
CFloatLabel CTRL_CONTENT(Name) = {(pFloat), (nFixPoint), (fMin), (fMax), (strUnit)};					 	\
CControl Name = {(pParent), (x), (y), (cx), (cy), (state), (Align), (&CTRL_CONTENT(Name)), DrawBigFloatLabel, NULL}


//-------------------Status Image-------------
typedef struct _CStatusImage
{
	uint8*	pNormalImage;
	uint8*	pHotImage;
	BOOL	bHot;
	int16	sx;
	int16	sy;
}CStatusImage;

void DrawStatusImage(CControl* pControl);

//����״̬ͼ�οؼ�
/****************************************************************************
* ��	�ƣ� DEF_STATUS_IMAGE_CTRL
* ��	�ܣ�	����״̬ͼ�οؼ�
* ��ڲ������Զ����ַ�	Name			�ؼ���
						CWindow*		pParent		�ÿؼ�Ҫ��ʾ�����ڴ���
						int16				x					x����ʼλ��
						int16				y					y����ʼλ��
						int16				cx				x����ʾ����
						int16				cy				y����ʾ����
						int16				state						�ؼ�״̬		��CTRL_VISABLE-�ؼ����ӣCTRL_FOCUS-�ؼ��������뽹�㣩
						uint8*			pImageNormal		Ҫ��ʾ��ͼ��1�������׵�ַ
						uint8*			pImageHot				Ҫ��ʾ��ͼ��2�������׵�ַ
						BOOL				Status					ѡ��Ҫ��ʾ��ͼ��TRUE-ͼ��1��FALSE-ͼ��2��
* ���ڲ�������
* ˵	������			
****************************************************************************/
#define DEF_STATUS_IMAGE_CTRL(Name, pParent, x, y, cx, cy, state, pImageNormal, pImageHot, Status)							\
CStatusImage CTRL_CONTENT(Name) = {(pImageNormal), (pImageHot), (Status), (cx), (cy)};					 	\
CControl Name = {(pParent), (x), (y), (cx), (cy), (state), (0), (&CTRL_CONTENT(Name)), DrawStatusImage, NULL}

//-------------------Image Control------------
typedef struct _CImageCtrl
{
	uint8*	pImage;
	int16	sx;
	int16	sy;
}CImageCtrl;

void DrawImageCtrl(CControl* pControl);

//����ͼ�οؼ�
/****************************************************************************
* ��	�ƣ� DEF_IMAGE_CTRL
* ��	�ܣ�	����ͼ�οؼ�
* ��ڲ������Զ����ַ�	Name			�ؼ���
						CWindow*		pParent		�ÿؼ�Ҫ��ʾ�����ڴ���
						int16				x					x����ʼλ��
						int16				y					y����ʼλ��
						int16				cx				x����ʾ����
						int16				cy				y����ʾ����
						int16				state			�ؼ�״̬		
						uint8*			pImage		Ҫ��ʾͼ�����ʼ��ַ(���鶨��Ϊstatic const uint8 ����)
* ���ڲ�������
* ˵	������			
****************************************************************************/
#define DEF_IMAGE_CTRL(Name, pParent, x, y, cx, cy, state, pImage)							\
CImageCtrl CTRL_CONTENT(Name) = {(pImage), (cx), (cy)};					 	\
CControl Name = {(pParent), (x), (y), (cx), (cy), (state), (0), (&CTRL_CONTENT(Name)), DrawImageCtrl, NULL}

//-------------------Custom Text Control--------
typedef struct _CCustomTextCtrl
{
	uint8*	pArr;
	int16	sx;					//ÿ���ֵĿ��(��λ������)
	int16	sy;					//ÿ���ֵĸ߶�(��λ������)
	uint16  nWordSize;			//ÿ���ֵ���ģ���ɵ��ֽ���
	uint16	nCount;				//�Զ����ı��ؼ��������ٸ���
}CCustomTextCtrl;

void DrawCustomTextCtrl(CControl* pControl);

//�����Զ����ı��ؼ�
/****************************************************************************
* ��	�ƣ� DEF_CUSTOM_TEXT_CTRL
* ��	�ܣ�	�����Զ����ı��ؼ�
* ��ڲ������Զ����ַ�	Name			�ؼ���
						CWindow*		pParent		�ÿؼ�Ҫ��ʾ�����ڴ���
						int16				x					x����ʼλ��
						int16				y					y����ʼλ��
						int16				cx				x����ʾ����
						int16				cy				y����ʾ����
						int16				state			�ؼ�״̬		
						uint8*			pMatrix		Ҫ��ʾ���ı���ʼ��ַ(���鶨��Ϊstatic const uint8 ����)
						int16				wcx				ÿ���ֵĿ��(��λ:����)
						int16				wcy				ÿ���ֵĸ߶�(��λ:����)
						uint16			wSize			ÿ���ֵ���ģ���ɵ��ֽ���
						uint16			wCount		�Զ����ı��ؼ��������ٸ���
* ���ڲ�������
* ˵	����	��			
****************************************************************************/
#define DEF_CUSTOM_TEXT_CTRL(Name, pParent, x, y, cx, cy, state, pMatrix, wcx, wcy, wSize, wCount)							\
CCustomTextCtrl CTRL_CONTENT(Name) = {(pMatrix), (wcx), (wcy), (wSize), (wCount)};					 	\
CControl Name = {(pParent), (x), (y), (cx), (cy), (state), (0), (&CTRL_CONTENT(Name)), DrawCustomTextCtrl, NULL}

//-------------------Rect Control--------
typedef struct _CRectCtrl
{
	int16	x;
	int16	y;		
	int16	cx;
	int16	cy;
}CRectCtrl;

void DrawRectCtrl(CControl* pControl);

//������οؼ�
#define DEF_RECT_CTRL(Name, pParent, x, y, cx, cy, state)							\
CRectCtrl CTRL_CONTENT(Name) = {(x), (y), (cx), (cy)};					 	\
CControl Name = {(pParent), (x), (y), (cx), (cy), (state), (0), (&CTRL_CONTENT(Name)), DrawRectCtrl, NULL}

//------------------Cartoon Control-------------
#define CARTOON_STATE_NORMAL	0		//��һ��״̬
#define CARTOON_STATE_HOT		1		//�ڶ���״̬
#define CARTOON_STATE_ALTER		2		//����״̬������

typedef struct _CCartoonCtrl
{
	LPControl			pControl;				   //����ָ��
	uint8*				pNormalImage;
	uint8*				pHotImage;
	uint16				cartoonState;
	int16				sx;
	int16				sy;

	uint16				nFrame;						//����ʱ������һ֡
	uint16				period;						//��ʱ������
	SYS_TMR*				pTimer;
	SYS_TMR_CALLBACK		tmrCallBack;
	
}CCartoonCtrl, *LPCartoonCtrl;

BOOL CreateCartoonCtrl(LPCartoonCtrl lpCartoonCtrl);

BOOL DestoryCartoonCtrl(LPCartoonCtrl lpCartoonCtrl);

BOOL StartCartoonCtrl(LPCartoonCtrl lpCartoonCtrl);

BOOL StopCartoonCtrl(LPCartoonCtrl lpCartoonCtrl);

void CartoonTmrCallBack(void *ptmr, void *parg);

void DrawCartoonCtrl(CControl* pControl);

//���嶯��ͼ�οؼ�
#define DEF_CARTOON_CTRL(Name, pParent, x, y, cx, cy, state, pImageNormal, pImageHot, Status, period)								\
extern CControl Name;\
CCartoonCtrl CTRL_CONTENT(Name) = {(&Name), (pImageNormal), (pImageHot), (Status), (cx), (cy), 0, period, NULL, CartoonTmrCallBack};		\
CControl Name = {(pParent), (x), (y), (cx), (cy), (state), (0), (&CTRL_CONTENT(Name)), DrawCartoonCtrl, NULL}



#define DEF_STATUS_BAR(Name, pParent, x, y, cx, cy, sta_gm, sta_dg, sta_lz, voice, error, unit, time, state)  \
CStatusBar CTRL_CONTENT(Name) = {(sta_gm),(sta_dg),(sta_lz),(voice),(error),(unit),(time)};                     \
CControl Name = {(pParent), (x), (y), (cx), (cy), (state), (0), (&CTRL_CONTENT(Name)), Form_StatusBar_Draw, NULL}



extern CWindow*	g_pCurWindow;	//��ǰ����

//-------------GUI��Ϣ����-------------------
#define GUIMSGSIZE		16		//GUI��Ϣ���д�С

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

} GuiMsgInfo, *LPGuiMsgInfo;

extern GuiMsgInfo			m_GuiMsgInfoArr[GUIMSGSIZE];	   //GUI��Ϣ��Ϣ�洢��

extern void*				m_GuiMsgPointArr[GUIMSGSIZE];	   //GUI��Ϣ��Ϣָ������

extern SYS_EVENT 			*m_pEvtGuiMsg;					   //GUI��Ϣ�����¼�

/****************************************************************************
* ��	�ƣ�void  GuiMsgQueueCreate(void)
* ��	�ܣ�����GUI��Ϣ����
* ��ڲ�������
* ���ڲ�������
* ˵	����
****************************************************************************/
void  GuiMsgQueueCreate(void);

/****************************************************************************
* ��	�ƣuint8 GuiMsgQueuePost(GuiMsgInfo* pGuiMsgInfo)
* ��	�ܣ�����GUI��Ϣ����
* ��ڲ�����GuiMsgInfo* pGuiMsgInfo	Ҫ���͵���Ϣ
* ���ڲ�����SYS_ERR_NONE						û�д���
						SYS_ERR_Q_FULL					��Ϣ��������
						SYS_ERR_EVENT_TYPE			pevent ����ָ����Ϣ�����ָ��
						SYS_ERR_PEVENT_NULL			���pevent��һ����ָ��
* ˵	����
****************************************************************************/
uint8 GuiMsgQueuePost(GuiMsgInfo* pGuiMsgInfo);

/****************************************************************************
* ��	�ƣuvoid* GuiMsgQueuePend(GuiMsgInfo* pGuiMsgInfo, uint8* err)
* ��	�ܣ��ȴ�GUI��Ϣ����
* ��ڲ�����GuiMsgInfo* pGuiMsgInfo	Ҫ�ȴ�����Ϣ
* ���ڲ�����uint8* err							�������͵Ĵ�ŵ�ַ
* ˵	����
****************************************************************************/
void* GuiMsgQueuePend(GuiMsgInfo* pGuiMsgInfo, uint8* err);


/****************************************************************************
* ��	�ƣuuint8 PostWindowMsg(uint16 nID, uint32 wParam, uint32 lParam)
* ��	�ܣ����ʹ�����Ϣ
* ��ڲ����uint16 nID			������һ��Ҫִ�еĲ���
* ���ڲ�����uint32 wParam	��Ϣ���͵Ĳ���֮1
						uint32 lParam	��Ϣ���͵Ĳ���֮2
* ˵	����
****************************************************************************/
uint8 PostWindowMsg(uint16 nID, uint32 wParam, uint32 lParam);	//���ʹ�����Ϣ







#endif
