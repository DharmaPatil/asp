#include "Form_Version.h"
#include "Form_Main.h"
#include "sys_param.h"
#include "sys_config.h"

#include "system_init.h"


extern  CControl  gStatusBar; 		//״̬��

static void Form_Version_Timer(void *ptmr, void *parg);
static void Form_Version_Draw(LPWindow pWindow);
static void Form_Version_Proc(LPWindow pWindow, LPGuiMsgInfo pGuiMsgInfo);

__align(8) static char gCpuID[16]={0};
__align(8) static char mUpDate_XHval[8]={0};		  	//2���ַ�	ASCII
__align(8) static char mUpDate_VersionVal[24]={0};	  	//10���ַ�
__align(8) static char mUpDate_PlatformVal[24]={0};	  	//10���ַ�
__align(8) static char mUpDate_NumVal[32]={0};	  		//6*4(24�ֽ�)���ַ�	ASCII
__align(8) static char mUpDate_Mode[16] = {0};

DEF_TEXT_LABEL(mUpDate_XH_Title, &gWD_Version, 2, 25, 150, 18, CTRL_VISABLE, "�ն��ͺţ�");
DEF_TEXT_LABEL(mUpDate_XHVal_Title, &gWD_Version, 75, 25, 150, 18, CTRL_VISABLE, mUpDate_XHval);
DEF_TEXT_LABEL(mUpDate_Platform_Tile, &gWD_Version, 2, 50, 150, 18, CTRL_VISABLE, "ƽ̨�汾�ţ�");
DEF_TEXT_LABEL(mUpDate_PlatformVal_Tile, &gWD_Version, 83, 50, 150, 18, CTRL_VISABLE, mUpDate_PlatformVal);

DEF_TEXT_LABEL(mUpDate_Version_Title, &gWD_Version, 2, 70, 150, 18, CTRL_VISABLE, "����汾�ţ�");
DEF_TEXT_LABEL(mUpDate_VersionVal_Title, &gWD_Version, 83, 70, 150, 18, CTRL_VISABLE, mUpDate_VersionVal);
DEF_TEXT_LABEL(mUpDate_Num_Title, &gWD_Version, 2, 90, 150, 18, CTRL_VISABLE, "�����룺");
DEF_TEXT_LABEL(mUpDate_NumVal_Title, &gWD_Version, 55, 90, 150, 18, CTRL_VISABLE,mUpDate_NumVal);
DEF_TEXT_LABEL(mUpDate_Mode_Title, &gWD_Version, 2, 110, 150, 18, CTRL_VISABLE, "����ģʽ:");
DEF_TEXT_LABEL(mUpDate_ModeVal_Title, &gWD_Version, 68, 110, 150, 18, CTRL_VISABLE,mUpDate_Mode);


static const LPControl marrLPControl[] = 
{
    &gStatusBar,
	&mUpDate_XH_Title,
	&mUpDate_XHVal_Title,
	&mUpDate_Platform_Tile,
	&mUpDate_PlatformVal_Tile,
	&mUpDate_Version_Title,
	&mUpDate_VersionVal_Title,
	&mUpDate_Num_Title,
	&mUpDate_NumVal_Title,
	&mUpDate_Mode_Title,
	&mUpDate_ModeVal_Title
};

CWindow  gWD_Version = {
    (LPControl *)marrLPControl,
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
    Form_Version_Timer,
    Form_Version_Draw,
    Form_Version_Proc
};


void Form_Version_Timer(void *ptmr, void *parg)
{
	LPWindow lpWindow = (LPWindow)parg;
	if(lpWindow != NULL)
	{
	}
}

void Form_Version_Draw(LPWindow pWindow)
{
	uint16 i;
	LPControl	lpControl;

	//����Դ�
	EraseBuffer();

	//��ֹ��ͼ
	SetRedraw(FALSE);

	//��ֹˢ��
	EnableScreenFlush(FALSE);

	//������ͼ
	SetGdiView(pWindow->nViewPosX, pWindow->nViewPosY, pWindow->nViewSizeX, pWindow->nViewSizeY);
	EnableGdiView(TRUE);
    DrawHoriLine(0, 25+16, 240);
    DrawHoriLine(0, 25+18, 240);

	//���ƿؼ�
	for(i = 0; i < pWindow->nNbControls; i++)
	{
		lpControl = *(pWindow->pLPControls + i);
		if(lpControl->state & CTRL_VISABLE)
		{
			lpControl->DrawFunc(lpControl);
		}
	}
	//ʹ��ˢ��
	EnableScreenFlush(TRUE);

	//ˢ��
	FlushScreen();

	//ʹ�ܻ�ͼ
	SetRedraw(TRUE);
}

/****************************************************************************
* ��	�ƣ�char HexToAscii(u8 nHex)
* ��	�ܣ�����16����תASCII��		   
* ��ڲ�����nHex	0x0A-0x0F.0x00-0x09
* ���ڲ�����ת�����ASCII��
* ˵	����16����0x0A-0x0F.0x00-0x09תASCII��A-F.0-9
****************************************************************************/
char HexToAscii(u8 nHex)
{
	if(nHex <= 9)	//����0~9
	{
		return (nHex + 0x30);
	}
	else
	{
		if(nHex >= 0x0A && nHex <= 0x0F)	//��ĸA~F
		{
			return (nHex + 0x37);
		}
		else
		{
			return 0xFF;
		}
	}
}

/****************************************************************************
* ��	�ƣ�void BufferHexToAscii(u8 *pHexBuf,u8 *pAaciiBuf , u16 nHexLen)
* ��	�ܣ����16����תASCII��	   
* ��ڲ�����*pAaciiBuf	ASCII��������	   ���
			*pHexBuf	16������������	   Դ
			nHexLen	ת������
* ���ڲ�������
* ˵	����u8 x[2]={0xAB,0XCD};
			u8 z[4]={0};		ת������{65,66,67,68}		 2λת4λ
****************************************************************************/
void BufferHexToAscii(u8 *pHexBuf,u8 *pAaciiBuf , u16 nHexLen)
{
	u16 i;
	if(pAaciiBuf == 0)
		return;
	if(pHexBuf == 0)
		return;

	for(i=0; i<nHexLen; i++)
	{
		pAaciiBuf[i<<1] = HexToAscii(pHexBuf[i]>>4);
		if(pAaciiBuf[i] == 0xFF)
			return;

		
		pAaciiBuf[(i<<1)+1] = HexToAscii(pHexBuf[i] & 0x0F);
		if(pAaciiBuf[(i<<1)+1] == 0xFF)
			return;
	}
}

void Form_Version_Proc(LPWindow pWindow, LPGuiMsgInfo pGuiMsgInfo)
{

	CControl* pControl;


	switch(pGuiMsgInfo->ID)
	{
		case WM_LOAD:
			ClearScreen();
			SysTimeDly(15);
    		
		case WM_SHOW:		
			GetCpuIdHex((u8*)gCpuID);
			//�ͺ�
			strcpy(mUpDate_XHval,"DT3101");
		
			Get_EMOS_Versions((u8*)mUpDate_PlatformVal);//���ƽ̨�ķ����汾��
			//�汾
			snprintf(mUpDate_VersionVal, 23, "DT3101-V%d.%d.%d", 
				AppVersion1, AppVersion2, AppVersion3);
			//������
			BufferHexToAscii((u8*)gCpuID,(u8*)&mUpDate_NumVal,12);	 //3*4
			//����ģʽ
			snprintf(mUpDate_Mode, 15, "SCS-ZC-%d", (Get_System_Mode() + 1));
			
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

				case KEY_LEFT:
					break;

				case KEY_OK:
					break;

				case KEY_RIGHT: 
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

				default:
					break;
			}
			break;
		
		default:
			break;
	}	
}



