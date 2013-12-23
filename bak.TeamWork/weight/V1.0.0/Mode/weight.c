#include "weight.h"
#include "drvproc.h"

#define _WEIGHT_DEBUG		1

#define	DTOS_MAJOR_VER	1
#define	DTOS_MAIN_VER		0
#define	DTOS_MINOR_VER	2
#define	DTOS_DEBUG_VER	2

/*! \brief ÿ����̨�����֧�ֵ�ADͨ���� */
#define MAX_AD_CHANLE 6  /*!< ���ADͨ���� */

/*! \brief ��������뵽�ĳ�̨���� */
#define MAX_SCALER 2 /*!< ���Ƹ��� */


/*!
\brief 
ADC�ɼ�ͨ������ʾһ��ADCת��ͨ��
*/
typedef struct _sAdcProcChannel
{
	/*! \brief AD�ɼ�*/
	void*		pAdcGet_UserData;				/*!<AD�ɼ��û����� */
	pInitFunc	pAdcGetInit;					/*!<AD�ɼ���ʼ�� */
	pAdcGetFun	pAdcGet;						/*!<AD�ɼ����� */
	long		nOrgAd;							/*!<����,��ǰԭʼADֵ */

	/*! \brief ���ϼ��*/
	void*				pAvc_UserData;			/*!<�쳣������*/
	pInitFunc			pAdcValidCheckInit;	    /*!<�쳣����ʼ��*/
	pAdcValidCheckFunc	pAdcValidCheck;	        /*!<�쳣��⺯��*/
	int					nValidState;			/*!<���ϼ������ֵ*/

	/*! \brief ��Ƶ*/
	int					bDivEnable;			/*!<�Ƿ�ʹ�ܷ�Ƶ*/	
	unsigned short	nDivIndex;			/*!<��Ƶ����*/	
	unsigned short	nDivMax;			/*!<��Ƶϵ��*/	
	float				fDivSum;			/*!<��Ƶ�ۼƺ�*/		
	float				fDivValue;			/*!<��Ƶ���*/	

	/*! \brief �����˲�*/
	void*		pFcb_UserData;					/*!<�˲�����*/
	pInitFunc	pFilterInit;					/*!<�˲���ʼ������*/
	pFilterFunc pFilter;						/*!<�˲�����*/
	float		fFilterValue;					/*!<�˲����*/

	/*! \brief ��׼��*/
	void*				pScb_UserData;			/*!<��׼������*/
	pInitFunc			pAdcStandlizeInit;		/*!<��׼����ʼ������*/
	pAdcStandlizeFunc	pAdcStandlize;			/*!<��׼������*/
	float				fStandValue;			/*!<��׼��������*/

	/*! \brief ��һ��*/
	void*				pNcb_UserData;			/*!<��һ������*/
	pInitFunc			pAdcNormalizeInit;		/*!<��һ����ʼ������*/
	pAdcNormalizeFunc	pAdcNormalize;			/*!<��һ������*/
	float				fNormalizeValue;		/*!<��һ�����������̬����*/

	/*! \brief ��̬�����е���λ���� */
	void*			pZcb_UserData;				/*!<��λ�������*/
	pInitFunc		pZeroTraceInit;				/*!<��λ�����ʼ������*/
	pZeroTraceFunc	pZeroTrace;					/*!<��λ������*/
	pSetZeroFunc pSetZero;					/*!<���㴦����*/
	float			fZerolizeValue;				/*!<��λ�������������̬����*/
}sAdcProcChannel;


/*!
\brief ��̬��	
*/
typedef  struct _sStaticScalerImpl
{

	/*! \brief ���룬��̬���������*/
	float InArr[MAX_AD_CHANLE];
	int nInCount;		/*!<InArr�е����ݸ���*/

	/*! \brief ��̬��ֵ*/   
	void*			 pTcb_UserData;		/*!<��Ҫע��ľ�̬��������*/
	pInitFunc		 pStaticTotalInit;		/*!<��Ҫע��ľ�̬������*/
	pStaticTotalFunc pStaticTotal;			/*!<��̬��ֵ����*/
	float			 fTotalValue;          /*!<��̬��ֵ���*/

	/*! \brief ��̬����*/
	void*			pDscb_UserData;		/*!<��̬��������*/
	pInitFunc		pSampleInit;		/*!<��Ҫע��ľ�̬������ʼ������*/
	pSampleFunc		pSample;			/*!<��Ҫע��ľ�̬��������*/
	float			fSampleValue;       /*!<��̬��ֵ���*/

	/*! \brief ��̬�˲�*/
	void*			pFcb_UserData;				/*!<�˲�����*/
	pInitFunc		pFilterInit;	            /*!<�˲���ʼ������*/
	pFilterFunc		pFilter;			     /*!< �˲�����*/
	float			fFilterValue;                 /*!< �˲����*/

	/*! \brief ��λ����*/

	void*			pZtcb_UserData;				/*!<��λ���ٲ���*/
	pInitFunc		pZeroTraceInit;					/*!<��λ���ٳ�ʼ������*/
	pZeroTraceFunc	pZeroTrace;		             /*!<��λ���ٺ���*/
	pSetZeroFunc pSetZero;					/*!<���㴦����*/		
	float			fZeroTraceValue;						 /*!<��λ����������*/

	/*! \brief ״̬�ж�*/

	void*			pSccb_UserData;								 /*!<״̬�ж�����*/
	pInitFunc		pStateCheckInit;					/*!<״̬�ж���ʼ������*/
	pStateCheckFunc pStateCheck;		         /*!<״̬�ж�����*/
	int				nState;						             /*!<״̬�ж�������*/

	/*! \brief �ֶ�ֵ����*/

	float			fStaticDrv;						    /*!<��̬���طֶ�ֵ*/
	float			fStaticDrvValue;						    /*!<��̬�ֶ�ֵ������*/

} sStaticScalerImpl;

/*!
\brief 
��̬��
*/
typedef  struct _sDyncScalerImpl
{
	//���룬��̬���������
	float InArr[MAX_AD_CHANLE];
	int nInCount;

	//��̬��ֵ
	void* pDecb;
	pDyncProcInitFunc pDyncProcInit;            /*!<��̬�����ʼ��*/
	pDyncProcFunc pDyncProc;                    /*!<��̬������*/
	float fDyncValue;                           /*!<��̬������   */
	float fDyncDrv;								/*!<��̬���طֶ�ֵ*/
	float fDyncDrvValue;						/*!<��̬�ֶ�ֵ������*/
} sDyncScalerImpl;


/*! \brief �ƶ������� */
typedef  struct _sScaler
{
	/*!< �Ƿ�ʹ�� */
	short bUsed;

	/*!< �Ƿ����� */
	short bRun;	

	/*!< ���룬ADͨ��������*/
	sAdcProcChannel ArrAdcProcChannel[MAX_AD_CHANLE];
	int nAdcCount;

	/*!< ��̬���� */
	sStaticScalerImpl StaticScalerImpl;

	/*!< ��̬���� */
	sDyncScalerImpl DyncScalerImpl;
} sScaler;

//ȡ��̨����
#define SCALER(hs)	((sScaler*)(hs))


/*! \brief ���Ծ���Ƿ���Ч */
#ifdef _WEIGHT_DEBUG
#define CHECK_HANDLE_VALID(hs) do \
{  \
	if(hs == HSCALER_INVALID) return ERR_INVALID_VAR; \
} while (0)
#else
#define CHECK_HANDLE_VALID(hs)
#endif

/*! \brief ����ͨ���;���Ƿ���Ч */
#ifdef _WEIGHT_DEBUG
#define CHECK_CHANLE_VALID(hs, ch) do \
{  \
	if(hs == HSCALER_INVALID) return ERR_INVALID_VAR; \
	if(ch >= SCALER(hs)->nAdcCount) return ERR_INVALID_VAR; \
} while (0)
#else
#define CHECK_CHANLE_VALID(hs, ch)
#endif


//�ڲ���������
static void InitAdcChanle(sAdcProcChannel* pAdc);
static void InitStaticScalerParam(sStaticScalerImpl* ssi,int nChanle);
static void InitDyncScalerParam(sDyncScalerImpl* sdi,int nChanle);

//��̨����
static  sScaler gScalerArray[MAX_SCALER];

static pLockFunc  pScaleLock = NULL;

static pUnlockFunc  pScaleUnlock = NULL;

void	ScalerLock(void)
{
	if(pScaleLock != NULL) pScaleLock();
}

void	ScalerUnlock(void)
{
	if(pScaleUnlock != NULL) pScaleUnlock();
}

//������̨
HScaler ScalerCreate(int nAdChanle, ERR_WEIGHT* err)
{
	int i = 0;
	HScaler hs = HSCALER_INVALID;

	*err = ERR_SUCCESS;

	ScalerLock();

	//���ͨ����Ŀ
	if((nAdChanle <= 0) || (nAdChanle > MAX_AD_CHANLE)) 
	{
		*err = ERR_INVALID_VAR;

		ScalerUnlock();

		return HSCALER_INVALID;
	}

	//��������ѡ��δʹ�õĳ�̨
	for(i = 0; i < MAX_SCALER; i++)
	{
		if(gScalerArray[i].bUsed == 0)
		{
			gScalerArray[i].bUsed = 1;
			gScalerArray[i].bRun   = 0;
			hs = (unsigned int)(&(gScalerArray[i]));
			break;
		}
	}

	//������Ч��̨
	if(hs == HSCALER_INVALID)
	{
		*err = ERR_NO_SCALER;

		ScalerUnlock();

		return HSCALER_INVALID;
	}

	//���ó�̨ͨ����
	SCALER(hs)->nAdcCount = nAdChanle;

	//��ʼ��AD�ɼ�ͨ��
	for(i=0; i < nAdChanle; i++)
	{	  
		InitAdcChanle(&(SCALER(hs)->ArrAdcProcChannel[i]));
	}

	//��ʼ����̬�Ӳ���
	InitStaticScalerParam(&(SCALER(hs)->StaticScalerImpl), nAdChanle);

	//��ʼ����̬�Ӳ���
	InitDyncScalerParam(&(SCALER(hs)->DyncScalerImpl), nAdChanle);

	ScalerUnlock();

	return hs;   
}

//���ٳ�̨
void ScalerDestory(HScaler hs)
{
	int i = 0;

	ScalerLock();

	//��������ѡ��δʹ�õĳ�̨
	for(i = 0; i < MAX_SCALER; i++)
	{
		if(hs == (unsigned int)(&(gScalerArray[i])))
		{
			gScalerArray[i].bUsed = 0;
			gScalerArray[i].bRun = 0;
		}
	}

	ScalerUnlock();

}

//ע���ٽ�����������

int RegisterScalerLockProc( pLockFunc  pLock, pUnlockFunc  pUnlock)
{
	pScaleLock 	= pLock;
	pScaleUnlock = pUnlock;	

	return 0;
}

//ע��ADCԭʼAD��ȡ����
int RegisterAdcGetProc(HScaler hs, int chanle, pInitFunc pAdcGetInit, pAdcGetFun pAdcGet, void* UserData)
{    
	CHECK_CHANLE_VALID(hs, chanle);

	SCALER(hs)->ArrAdcProcChannel[chanle].pAdcGetInit		= pAdcGetInit;
	SCALER(hs)->ArrAdcProcChannel[chanle].pAdcGet			= pAdcGet;
	SCALER(hs)->ArrAdcProcChannel[chanle].pAdcGet_UserData	= UserData;

	return 0;
}

//ע��ADCԭʼAD�쳣��麯��
int RegisterAdcCheckProc(HScaler hs, int chanle, pInitFunc pAdcValidCheckInit, pAdcValidCheckFunc pAdcValidCheck, void* UserData)
{
	CHECK_CHANLE_VALID(hs, chanle);

	SCALER(hs)->ArrAdcProcChannel[chanle].pAdcValidCheckInit = pAdcValidCheckInit;
	SCALER(hs)->ArrAdcProcChannel[chanle].pAdcValidCheck		= pAdcValidCheck;
	SCALER(hs)->ArrAdcProcChannel[chanle].pAvc_UserData		= UserData;

	return 0;
}

//ע��ADCͨ���˲�����
int RegisterAdcFilterProc(HScaler hs, int chanle, pInitFunc pFilterInit, pFilterFunc pFilter, void* UserData)
{
	CHECK_CHANLE_VALID(hs, chanle);

	SCALER(hs)->ArrAdcProcChannel[chanle].pFilterInit	= pFilterInit;
	SCALER(hs)->ArrAdcProcChannel[chanle].pFilter		= pFilter;
	SCALER(hs)->ArrAdcProcChannel[chanle].pFcb_UserData	= UserData;

	return 0;
}

//ע��ADC��׼������
int RegisterAdcStandlizeProc(HScaler hs, int chanle, pInitFunc pAdcStandlizeInit, pAdcStandlizeFunc pAdcStandlize, void* UserData)
{
	CHECK_CHANLE_VALID(hs, chanle);

	SCALER(hs)->ArrAdcProcChannel[chanle].pAdcStandlizeInit	= pAdcStandlizeInit;
	SCALER(hs)->ArrAdcProcChannel[chanle].pAdcStandlize		= pAdcStandlize;
	SCALER(hs)->ArrAdcProcChannel[chanle].pScb_UserData		= UserData;

	return 0;
}

//ע��ADCͨ����һ������
int RegisterAdcNormalizeProc(HScaler hs, int chanle, pInitFunc pAdcNormalizeInit, pAdcNormalizeFunc pAdcNormalize, void* UserData)
{
	CHECK_CHANLE_VALID(hs, chanle);

	SCALER(hs)->ArrAdcProcChannel[chanle].pAdcNormalizeInit	= pAdcNormalizeInit;
	SCALER(hs)->ArrAdcProcChannel[chanle].pAdcNormalize		= pAdcNormalize;
	SCALER(hs)->ArrAdcProcChannel[chanle].pNcb_UserData		= UserData;

	return 0;
}

//ע��ADCͨ���ж�̬��λ���ٺ���
int RegisterAdcZeroTraceProc(HScaler hs, int chanle, pInitFunc pZeroTraceInit, pZeroTraceFunc pZeroTrace, pSetZeroFunc pSetZero, void* UserData)
{
	CHECK_CHANLE_VALID(hs, chanle);

	SCALER(hs)->ArrAdcProcChannel[chanle].pZeroTraceInit = pZeroTraceInit;
	SCALER(hs)->ArrAdcProcChannel[chanle].pZeroTrace = pZeroTrace;
	SCALER(hs)->ArrAdcProcChannel[chanle].pSetZero = pSetZero;
	SCALER(hs)->ArrAdcProcChannel[chanle].pZcb_UserData = UserData;

	return 0;
}

//ע�ᾲ̬������
int RegisterScalerStaticTotalProc(HScaler hs, pInitFunc pStaticTotalInit, pStaticTotalFunc pStaticTotal, void* UserData)
{
	CHECK_HANDLE_VALID(hs);

	SCALER(hs)->StaticScalerImpl.pStaticTotalInit	= pStaticTotalInit;
	SCALER(hs)->StaticScalerImpl.pStaticTotal		= pStaticTotal;
	SCALER(hs)->StaticScalerImpl.pTcb_UserData	= UserData;

	return 0;
}

//��ɾ�̬������ú���
int RegisterScalerStaticSampleProc(HScaler hs, pInitFunc pSampleInit, pSampleFunc pSample, void* UserData)
{
	CHECK_HANDLE_VALID(hs);

	SCALER(hs)->StaticScalerImpl.pSampleInit = pSampleInit;
	SCALER(hs)->StaticScalerImpl.pSample	 = pSample;
	SCALER(hs)->StaticScalerImpl.pDscb_UserData = UserData;

	return 0;
}

//ע�ᾲ̬�����˲�����
int RegisterScalerStaticFilterProc(HScaler hs, pInitFunc pFilterInit, pFilterFunc pFilter, void* UserData)
{
	CHECK_HANDLE_VALID(hs);

	SCALER(hs)->StaticScalerImpl.pFilterInit   =  pFilterInit;
	SCALER(hs)->StaticScalerImpl.pFilter		=  pFilter;
	SCALER(hs)->StaticScalerImpl.pFcb_UserData	=  UserData;

	return 0;
}

//ע�ᾲ̬������λ���ٺ���
int RegisterScalerStaticZeroTraceProc(HScaler hs, pInitFunc pZeroTraceInit, pZeroTraceFunc pZeroTrace, pSetZeroFunc pSetZero, void* UserData)
{
	CHECK_HANDLE_VALID(hs);
	SCALER(hs)->StaticScalerImpl.pZeroTraceInit	= pZeroTraceInit;
	SCALER(hs)->StaticScalerImpl.pZeroTrace		= pZeroTrace;
	SCALER(hs)->StaticScalerImpl.pSetZero		= pSetZero;
	SCALER(hs)->StaticScalerImpl.pZtcb_UserData	= UserData;

	return 0;
}

//ע�ᾲ̬����״̬��麯��
int RegisterScalerStaticStateCheckProc(HScaler hs, pInitFunc pStateCheckInit, pStateCheckFunc pStateCheck, void* UserData)
{
	CHECK_HANDLE_VALID(hs);

	SCALER(hs)->StaticScalerImpl.pStateCheckInit	= pStateCheckInit;
	SCALER(hs)->StaticScalerImpl.pStateCheck		=  pStateCheck;
	SCALER(hs)->StaticScalerImpl.pSccb_UserData	= UserData;

	return 0;
}

//ע�ᶯ̬������
int RegisterScalerDyncProc(HScaler hs, pDyncProcInitFunc pDyncProcInit, pDyncProcFunc pDyncProc, void* UserData)
{
	CHECK_HANDLE_VALID(hs);

	SCALER(hs)->DyncScalerImpl.pDyncProcInit	= pDyncProcInit;
	SCALER(hs)->DyncScalerImpl.pDyncProc		= pDyncProc;
	SCALER(hs)->DyncScalerImpl.pDecb			= UserData;

	return 0;
}

//��ȡADCͨ��ԭʼADֵ
int GetAdcChanleOrgAd(HScaler hs, int chanel) 
{
	sAdcProcChannel* pApc;
	CHECK_CHANLE_VALID(hs,chanel);

	pApc = &(SCALER(hs)->ArrAdcProcChannel[chanel]);

	return pApc->nOrgAd;
}

//��ȡADCͨ�����״̬
int GetAdcChanleValidState(HScaler hs, int chanel) 
{
	sAdcProcChannel* pApc;
	CHECK_CHANLE_VALID(hs,chanel);

	pApc = &(SCALER(hs)->ArrAdcProcChannel[chanel]);

	return pApc->nValidState ;
}

//��ȡADCͨ���˲�������
float GetAdcChanleFilterValue(HScaler hs, int chanel) 
{
	sAdcProcChannel* pApc;
	CHECK_CHANLE_VALID(hs,chanel);

	pApc = &(SCALER(hs)->ArrAdcProcChannel[chanel]);

	return pApc->fFilterValue ;
}

//��ȡADCͨ����׼��������
float GetAdcChanleStandValue(HScaler hs, int chanel) 
{
	sAdcProcChannel* pApc;
	CHECK_CHANLE_VALID(hs,chanel);

	pApc = &(SCALER(hs)->ArrAdcProcChannel[chanel]);

	return pApc->fStandValue;
}

//��ȡADCͨ����һ������������
float GetAdcChanleNormalizeValue(HScaler hs, int chanel) 
{
	sAdcProcChannel* pApc;
	CHECK_CHANLE_VALID(hs,chanel);

	pApc = &(SCALER(hs)->ArrAdcProcChannel[chanel]);

	return pApc->fNormalizeValue;
}

//��ȡADCͨ����λ������
float GetAdcChanleZerolizeValue(HScaler hs, int chanel) 
{
	sAdcProcChannel* pApc;
	CHECK_CHANLE_VALID(hs,chanel);

	pApc = &(SCALER(hs)->ArrAdcProcChannel[chanel]);

	return pApc->fZerolizeValue ;
}

//��ȡ��̬��ֵ���
float GetScalerStaticTotalValue(HScaler hs)
{
	sStaticScalerImpl* ssi;
	CHECK_HANDLE_VALID(hs);

	ssi = &(SCALER(hs)->StaticScalerImpl);

	return ssi->fTotalValue;
}

//��ȡ��̬�������
float GetScalerStaticSampleValue(HScaler hs)
{
	sStaticScalerImpl* ssi;
	CHECK_HANDLE_VALID(hs);

	ssi = &(SCALER(hs)->StaticScalerImpl);

	return ssi->fSampleValue;
}

//��ȡ��̬�˲����
float GetScalerStaticFilterValue(HScaler hs)
{
	sStaticScalerImpl* ssi;
	CHECK_HANDLE_VALID(hs);

	ssi = &(SCALER(hs)->StaticScalerImpl);

	return ssi->fFilterValue;
}

//��ȡ��̬��λ���ٽ��
float  GetScalerStaticZeroTraceValue(HScaler hs)
{
	sStaticScalerImpl* ssi;
	CHECK_HANDLE_VALID(hs);

	ssi = &(SCALER(hs)->StaticScalerImpl);

	return ssi->fZeroTraceValue;
}

//��ȡ��̬״̬���
int GetScalerStaticState(HScaler hs)
{
	sStaticScalerImpl* ssi;
	CHECK_HANDLE_VALID(hs);

	ssi = &(SCALER(hs)->StaticScalerImpl);

	return ssi->nState;
}

//��ȡ��̬�趨�ֶ�ֵ
float GetScalerStaticDrv(HScaler hs)
{
	sStaticScalerImpl* ssi;
	CHECK_HANDLE_VALID(hs);

	ssi = &(SCALER(hs)->StaticScalerImpl);

	return ssi->fStaticDrv;
}

//��ȡ��̬�ֶ�ֵ�����Ľ��
float GetScalerStaticDrvValue(HScaler hs)
{
	sStaticScalerImpl* ssi;
	CHECK_HANDLE_VALID(hs);

	ssi = &(SCALER(hs)->StaticScalerImpl);

	return ssi->fStaticDrvValue;
}

//��ȡ��̬����ֵ
float GetScalerDyncValue(HScaler hs )
{
	sDyncScalerImpl* dsi;
	CHECK_HANDLE_VALID(hs);
	dsi= &(SCALER(hs)->DyncScalerImpl);

	return dsi->fDyncValue;
}

//��ȡ��̬�����ķֶ�ֵ
float GetScalerDyncDrvValue(HScaler hs )
{
	sDyncScalerImpl* dsi;
	CHECK_HANDLE_VALID(hs);

	dsi= &(SCALER(hs)->DyncScalerImpl);

	return dsi->fDyncDrvValue;
}

//��ȡ��̬�趨�ֶ�ֵ
float GetScalerDyncDrv(HScaler hs )
{
	sDyncScalerImpl* dsi;
	CHECK_HANDLE_VALID(hs);

	dsi= &(SCALER(hs)->DyncScalerImpl);
	return dsi->fDyncDrv;
}

#define STATE_ZERO      0x01
#define STATE_STEADY    0x10

//��ȡ��̨�ȶ�״̬
static char GetSteadyState(int State)
{
	if(State & STATE_STEADY) 
	{
		return 1;
	}

	return 0;
}

//��ȡ��̨��λ״̬
static char GetZeroState(int State)
{
	if(State & STATE_ZERO) 
	{
		return 1;
	}

	return 0;
}

//��ȡ�㷨�汾��
long	GetScalerVer(void)
{
	return ((DTOS_MAJOR_VER << 24) + (DTOS_MAIN_VER << 16 ) + ( DTOS_MINOR_VER<<8) + DTOS_DEBUG_VER);
}

/*
�ж�ָ����̨�Ƿ��ȶ�
*/
int IsScalerStaticSteady(HScaler hs)
{
	CHECK_HANDLE_VALID(hs);

	if(GetSteadyState(SCALER(hs)->StaticScalerImpl.nState))
		return 1;
	else
		return 0;
}

/*
�жϳ�̨�Ƿ������
*/
int IsScalerStaticAtZero(HScaler hs)
{
	CHECK_HANDLE_VALID(hs);

	if(GetZeroState(SCALER(hs)->StaticScalerImpl.nState))
		return 1;
	else
		return 0;
}

/*
�жϳ�̨ĳһ·AD�Ƿ����
*/
int IsAdcError(HScaler hs, int channel)
{
	CHECK_CHANLE_VALID(hs,channel);

	return SCALER(hs)->ArrAdcProcChannel[channel].nValidState?1:0;

}
/*! 
*	�������Ͷ���
*
*/
static char* errStrings[] = 
{
	"�ɹ�",			/*!< ��Ч����. */
	"��Ч����",
	"�޿��ó�̨",
	"��Ч��ADCͨ����Ŵ�����",	
	"û�ж���AD�ɼ�����",	 
	"û�ж���AD��Ч�Լ�麯��",	
	"û�ж���AD�����˲�����",	
	"û�ж���AD��׼������",	
	"û�ж���AD��һ������",	
	"û�ж���AD�����ٺ���",	
	"û�ж��徲̬�����ʼ������",	
	"û�ж��徲̬������",	
	"û�ж��徲̬����������",	 
	"û�ж��徲̬�˲�����",	 
	"û�ж��徲̬�����ٴ�����",	
	"û�ж��徲̬״̬������",	
	"û�ж��嶯̬�����ʼ������",	
	"û�ж��嶯̬������",	 
};
//��ȡ����������ʾ
const char* GetError(ERR_WEIGHT err)
{
	if(err >= ERR_MAX) return NULL;

	return errStrings[err];
}

//���þ�̬�ֶ�ֵ
int SetScalerStaticDrv(HScaler hs, float nDrv)
{
	CHECK_HANDLE_VALID(hs);

	SCALER(hs)->StaticScalerImpl.fStaticDrv = nDrv;

	return 0;
}

//���ö�̬�ֶ�ֵ
int SetScalerDyncDrv(HScaler hs, float nDrv)
{
	CHECK_HANDLE_VALID(hs);

	SCALER(hs)->DyncScalerImpl.fDyncDrv = nDrv;

	return 0;
}

//���ó�̨��Ƶ
void EnableScalerFreqDiv(HScaler hs, int bEnable, unsigned short nDiv)
{
	int chanle = 0;
	sAdcProcChannel *pAdcChanle = NULL;

	for(chanle = 0 ; chanle < SCALER(hs)->nAdcCount; chanle++)
	{
		pAdcChanle = &(SCALER(hs)->ArrAdcProcChannel[chanle]);

		pAdcChanle->bDivEnable = bEnable;
		pAdcChanle->nDivIndex = 0;
		pAdcChanle->nDivMax = nDiv;
		pAdcChanle->fDivSum = 0.0f;		
	}				
}

//����ADC��Ƶ
void EnableAdcFreqDiv(HScaler hs, int nChannel, int bEnable, unsigned short nDiv)
{
	sAdcProcChannel *pAdcChanle = NULL;

	if(hs == HSCALER_INVALID) return;
	if(nChannel < 0) return;
	if(nChannel >= SCALER(hs)->nAdcCount) return;

	pAdcChanle = &(SCALER(hs)->ArrAdcProcChannel[nChannel]);

	pAdcChanle->bDivEnable = bEnable;
	pAdcChanle->nDivIndex = 0;
	pAdcChanle->nDivMax = nDiv;
	pAdcChanle->fDivSum = 0.0f;		
}

//ADͨ����ʼ��
static void InitAdcChanle(sAdcProcChannel* pAdc)
{
	if(pAdc == NULL) return;

	//��ȡԭʼADֵ
	pAdc->pAdcGetInit	= NULL;
	pAdc->pAdcGet		= NULL;
	pAdc->nOrgAd		= 0;

	//AD����Ч�Լ��
	pAdc->pAvc_UserData			= NULL;
	pAdc->pAdcValidCheckInit	= NULL;
	pAdc->pAdcValidCheck		= NULL;
	pAdc->nValidState			= 0;

	//��Ƶ
	pAdc->bDivEnable = 0;
	pAdc->nDivIndex = 0;
	pAdc->nDivMax = 1;
	pAdc->fDivSum = 0.0f;
	pAdc->fDivValue = 0.0f;

	//�˲�
	pAdc->pFcb_UserData			= NULL;
	pAdc->pFilterInit			= NULL;
	pAdc->pFilter				= NULL;
	pAdc->fFilterValue			= 0.0f;

	//��׼��
	pAdc->pScb_UserData			= NULL;
	pAdc->pAdcStandlizeInit		= NULL;
	pAdc->pAdcStandlize			= NULL;
	pAdc->fStandValue			= 0.0f;

	//��һ��
	pAdc->pNcb_UserData			= NULL;
	pAdc->pAdcNormalizeInit		= NULL;
	pAdc->pAdcNormalize			= NULL;
	pAdc->fNormalizeValue		= 0.0f;

	//��̬��λ����
	pAdc->pZcb_UserData			= 0;
	pAdc->pZeroTraceInit		= NULL;
	pAdc->pZeroTrace			= NULL;
	pAdc->fZerolizeValue		= 0.0f;
}

//��̬�����ʼ��
static void InitStaticScalerParam(sStaticScalerImpl* ssi,int nChanle)
{
	int i;

	if(ssi == NULL) return;

	ssi->nInCount			= nChanle;
	if(ssi->nInCount < 0)	ssi->nInCount = 0;
	if(ssi->nInCount > MAX_AD_CHANLE) ssi->nInCount = MAX_AD_CHANLE;

	for(i = 0; i < ssi->nInCount; i++)
	{
		ssi->InArr[i] = 0.0f;
	}

	ssi->pStaticTotalInit	= NULL;
	ssi->pStaticTotal		= NULL;
	ssi->fTotalValue		= 0.0f;


	ssi->pSampleInit		= NULL;
	ssi->pSample			= NULL;
	ssi->fSampleValue		= 0.0f;

	ssi->pFilterInit		= NULL;
	ssi->pFilter			= NULL;
	ssi->fFilterValue		= 0.0f;

	ssi->pZeroTraceInit		= NULL;
	ssi->pZeroTrace			= NULL;
	ssi->fZeroTraceValue	= 0.0f;


	ssi->pStateCheckInit	= NULL;
	ssi->pStateCheck		= NULL;
	ssi->nState				= 0;

	ssi->fStaticDrv			= 1.0f;
	ssi->fStaticDrvValue	= 0.0f;
}

//��̬�����ʼ��
static void InitDyncScalerParam(sDyncScalerImpl* sdi,int nChanle)
{
	int i;

	if(sdi == NULL) return;

	sdi->nInCount			= nChanle;
	if(sdi->nInCount < 0)	sdi->nInCount = 0;
	if(sdi->nInCount > MAX_AD_CHANLE) sdi->nInCount = MAX_AD_CHANLE;

	for(i = 0; i < sdi->nInCount; i++)
	{
		sdi->InArr[i] = 0.0f;
	}

	sdi->pDyncProcInit	= NULL;
	sdi->pDyncProc		= NULL;
	sdi->fDyncDrv		= 1.0f;
	sdi->fDyncValue		= 0.0f;
	sdi->fDyncDrvValue	= 0.0f;
}

//ADCͨ��������
static ERR_WEIGHT AdcChanleProc(HScaler hs)
{
	int chanle = 0;
	sAdcProcChannel *pAdcChanle = NULL;

	for(chanle = 0 ; chanle < SCALER(hs)->nAdcCount; chanle++)
	{
		pAdcChanle = &(SCALER(hs)->ArrAdcProcChannel[chanle]);

		//��ȡԭʼADֵ
		pAdcChanle->nOrgAd = pAdcChanle->pAdcGet(chanle);

		//���ù��ϼ�⺯�������¹���״̬
		pAdcChanle->nValidState = pAdcChanle->pAdcValidCheck(pAdcChanle->nOrgAd);
		if(pAdcChanle->nValidState != 0)
		{
			continue;
		}

		//��Ƶ����
		if(pAdcChanle->bDivEnable > 0)
		{
			pAdcChanle->fDivSum += (float)pAdcChanle->nOrgAd;
			pAdcChanle->nDivIndex++;
			if(pAdcChanle->nDivIndex < pAdcChanle->nDivMax) continue;

			pAdcChanle->fDivValue = pAdcChanle->fDivSum / pAdcChanle->nDivIndex;
			pAdcChanle->fDivSum = 0.0f;
			pAdcChanle->nDivIndex = 0;
		}
		else
		{
			pAdcChanle->fDivValue = (float)pAdcChanle->nOrgAd;
		}

		//���ó����˲����������˲��������˲����ADֵ
		pAdcChanle->fFilterValue = pAdcChanle->pFilter(pAdcChanle->pFcb_UserData, pAdcChanle->fDivValue);

		//��׼��
		pAdcChanle->fStandValue       = pAdcChanle->pAdcStandlize(pAdcChanle->pScb_UserData, pAdcChanle->fFilterValue);

		//��һ��
		pAdcChanle->fNormalizeValue   = pAdcChanle->pAdcNormalize(pAdcChanle->pNcb_UserData, pAdcChanle->fStandValue);


		//��̬��λ
		pAdcChanle->fZerolizeValue    = pAdcChanle->pZeroTrace(pAdcChanle->pZcb_UserData, pAdcChanle->fNormalizeValue);

		//��̬�Ӳ�����ֵ, ��ֵ���ھ�̬��������
		SCALER(hs)->StaticScalerImpl.InArr[chanle] = pAdcChanle->fNormalizeValue;

		//��̬�Ӳ�����ֵ����ֵ����������
		SCALER(hs)->DyncScalerImpl.InArr[chanle] = pAdcChanle->fZerolizeValue;
	}

	return ERR_SUCCESS;
}


//��̬����
ERR_WEIGHT StaticScalerProcs(sStaticScalerImpl *pSSI)
{
	char ret=0;

	if(pSSI == NULL) return ERR_INVALID_VAR;

	//��̬��ֵ
	pSSI->fTotalValue      = pSSI->pStaticTotal(pSSI->pTcb_UserData, pSSI->InArr, pSSI->nInCount);

	//��̬����
	ret                     = pSSI->pSample(pSSI->pDscb_UserData, pSSI->fTotalValue, &(pSSI->fSampleValue));
	if(!ret) return  ERR_SUCCESS;

	//��̬���˲�
	pSSI->fFilterValue      = pSSI->pFilter(pSSI->pFcb_UserData, pSSI->fSampleValue);

	//��λ����
	pSSI->fZeroTraceValue   = pSSI->pZeroTrace(pSSI->pZtcb_UserData, pSSI->fFilterValue);

	//״̬�ж�
	pSSI->nState            = pSSI->pStateCheck(pSSI->pSccb_UserData, pSSI->fZeroTraceValue);

	//��̬�ֶ�ֵ����
	pSSI->fStaticDrvValue   = WetDrvProc(pSSI->fZeroTraceValue, pSSI->fStaticDrv);

	return ERR_SUCCESS;
}


//��̬����
ERR_WEIGHT DyncScalerProcs(sDyncScalerImpl *pDSI)
{
	if(pDSI == NULL) return ERR_INVALID_VAR;

	//��̬��ֵ
	pDSI->fDyncValue      = pDSI->pDyncProc(pDSI->pDecb, pDSI->InArr, pDSI->nInCount);

	//��̬�ֶ�ֵ����
	pDSI->fDyncDrvValue   = WetDrvProc(pDSI->fDyncValue, pDSI->fDyncDrv);

	return ERR_SUCCESS;
}


/*!
\brief ����̨�Ƿ���Ч
*/
static ERR_WEIGHT ScalerCheckVaild(sScaler *pScaler)
{
	int i;
	sStaticScalerImpl*	pSSI = NULL;
	sDyncScalerImpl*	pDSI = NULL;
	sAdcProcChannel		*pAdcChanle = NULL;

	//����̨����
	if(pScaler == NULL) return ERR_INVALID_VAR;

	//��������˶���Ч��̨
	for(i = 0; i < MAX_SCALER; i++)
	{
		if(pScaler == &(gScalerArray[i]))
		{
			break;
		}
	}
	if(i == MAX_SCALER) return ERR_INVALID_VAR;

	if((pScaler->nAdcCount < 0) || (pScaler->nAdcCount > MAX_AD_CHANLE)) return ERR_INVALID_VAR;

	//���ÿ��ͨ���Ĳ���
	for(i = 0; i < pScaler->nAdcCount; i++)
	{
		pAdcChanle = &(pScaler->ArrAdcProcChannel[i]);

		if(pAdcChanle->pAdcGet == NULL)
		{
			///��AD�ɼ�ͨ��û��AD�ɼ�������ʧ�ܷ���
			return ERR_NO_ADC_PROC;
		}
		if(pAdcChanle->pAdcValidCheck == NULL)
		{
			///AD��⺯�������� ,��������������Ϊ�п��ܲ���Ҫ���AD
			return ERR_NO_ADC_CHECK_PROC;
		}
		if(pAdcChanle->pFilter == NULL)
		{
			///AD�˲�����������
			return ERR_NO_ADC_FILTER_PROC;
		}
		if(pAdcChanle->pAdcStandlize == NULL)
		{
			///��׼������������
			return ERR_NO_ADC_STAND_PROC;
		}
		if(pAdcChanle->pAdcNormalize == NULL)
		{
			///��һ������������
			return ERR_NO_ADC_NORMALIZE_PROC;
		}

		if(pAdcChanle->pZeroTrace == NULL)
		{
			///�����ٺ���������
			return ERR_NO_ADC_ZEROTRACE_PROC;
		}

		if(pAdcChanle->pSetZero== NULL)
		{
			///�����ٺ���������
			return ERR_NO_ADC_ZEROTRACE_PROC;
		}
	}

	//��̬����������
	pSSI = &(pScaler->StaticScalerImpl);	
	if(pSSI->pStaticTotal == NULL)		return ERR_NO_SS_STATIC_PROC;
	if(pSSI->pSample == NULL)		return ERR_NO_SS_DIGSAMPLE_PROC;
	if(pSSI->pFilter == NULL)			return ERR_NO_SS_FILTER_PROC;
	if(pSSI->pZeroTrace == NULL)		return ERR_NO_SS_ZEROTRACE_PROC;
	if(pSSI->pSetZero == NULL)		return ERR_NO_SS_ZEROTRACE_PROC;
	if(pSSI->pStateCheck == NULL)		return ERR_NO_SS_STATECHECK_PROC;

	//��̬����������
	pDSI = &(pScaler->DyncScalerImpl);
	if(pDSI->pDyncProc == NULL)			return ERR_NO_DS_DYNC_PROC;

	return ERR_SUCCESS;
}

//�������ش���
ERR_WEIGHT ScalerStart(HScaler hs)
{
	int i = 0;
	sScaler *pScaler = NULL;
	ERR_WEIGHT retCode = ERR_SUCCESS;

	ScalerLock();

	//�������
	CHECK_HANDLE_VALID(hs);

	pScaler = SCALER(hs);	
	if(pScaler->bUsed == 0)
	{
		ScalerUnlock();
		return ERR_INVALID_VAR;
	}	

	if((pScaler->nAdcCount > MAX_AD_CHANLE )  || (pScaler->nAdcCount <= 0) )
	{
		ScalerUnlock();
		return ERR_INVALID_VAR;
	}

	//�������
	retCode = ScalerCheckVaild(pScaler);	
	if(ERR_SUCCESS != retCode)
	{
		ScalerUnlock();
		return retCode;
	}	

	//����ͨ����ʼ������
	for (i = 0; i < pScaler->nAdcCount; i++)
	{	
		//AD�ɼ�ͨ����ʼ��
		if(pScaler->ArrAdcProcChannel[i].pAdcGetInit != NULL)
		{
			pScaler->ArrAdcProcChannel[i].pAdcGetInit(pScaler->ArrAdcProcChannel[i].pAdcGet_UserData);
		}

		//ADͨ���쳣�����ʼ��
		if(pScaler->ArrAdcProcChannel[i].pAdcValidCheckInit != NULL)
		{
			pScaler->ArrAdcProcChannel[i].pAdcValidCheckInit(pScaler->ArrAdcProcChannel[i].pAvc_UserData);
		}

		//ADͨ�������˲���ʼ��
		if(pScaler->ArrAdcProcChannel[i].pFilterInit != NULL)
		{
			pScaler->ArrAdcProcChannel[i].pFilterInit(pScaler->ArrAdcProcChannel[i].pFcb_UserData);
		}

		//ADͨ����׼����ʼ��
		if(pScaler->ArrAdcProcChannel[i].pAdcStandlizeInit != NULL)
		{
			pScaler->ArrAdcProcChannel[i].pAdcStandlizeInit(pScaler->ArrAdcProcChannel[i].pScb_UserData);
		}

		//ADͨ����һ����ʼ��
		if(pScaler->ArrAdcProcChannel[i].pAdcNormalizeInit != NULL)
		{
			pScaler->ArrAdcProcChannel[i].pAdcNormalizeInit(pScaler->ArrAdcProcChannel[i].pNcb_UserData);
		}

		//ADͨ�������ٳ�ʼ��
		if(pScaler->ArrAdcProcChannel[i].pZeroTraceInit != NULL)
		{
			pScaler->ArrAdcProcChannel[i].pZeroTraceInit(pScaler->ArrAdcProcChannel[i].pZcb_UserData);
		}	
	}

	//��̬�Ӵ����ʼ��
	if(pScaler->StaticScalerImpl.pStaticTotalInit != NULL)
	{
		pScaler->StaticScalerImpl.pStaticTotalInit(pScaler->StaticScalerImpl.pTcb_UserData);
	}

	//����������ʼ��
	if(pScaler->StaticScalerImpl.pSampleInit != NULL)
	{
		pScaler->StaticScalerImpl.pSampleInit(pScaler->StaticScalerImpl.pDscb_UserData);
	}

	//��̬�˲���ʼ��
	if(pScaler->StaticScalerImpl.pFilterInit != NULL)
	{
		pScaler->StaticScalerImpl.pFilterInit(pScaler->StaticScalerImpl.pFcb_UserData);
	}

	//�����ٳ�ʼ�� 
	if(pScaler->StaticScalerImpl.pZeroTraceInit != NULL)
	{
		pScaler->StaticScalerImpl.pZeroTraceInit(pScaler->StaticScalerImpl.pZtcb_UserData);
	}

	//״̬�����ʼ�� 
	if(pScaler->StaticScalerImpl.pStateCheckInit != NULL)
	{
		pScaler->StaticScalerImpl.pStateCheckInit(pScaler->StaticScalerImpl.pSccb_UserData);
	}

	//��̬��ʼ��
	if(pScaler->DyncScalerImpl.pDyncProcInit != NULL)
	{
		pScaler->DyncScalerImpl.pDyncProcInit(pScaler->DyncScalerImpl.pDecb);
	}

	//��������
	pScaler->bRun = 1;

	ScalerUnlock();

	return ERR_SUCCESS;
}

//���г��ش���
ERR_WEIGHT ScalerRun(HScaler hs)
{
	ERR_WEIGHT nRet = ERR_SUCCESS;

	if(SCALER(hs)->bUsed == 0)  return ERR_NO_SCALER;	
	if(SCALER(hs)->bRun   == 0)  return ERR_INVALID_VAR;

	//AD�ɼ�ͨ������
	nRet = AdcChanleProc(hs);	
	if (ERR_SUCCESS != nRet)
	{
		return nRet;
	}

	//��̬�Ӵ���
	nRet = StaticScalerProcs(&(SCALER(hs)->StaticScalerImpl));
	if (ERR_SUCCESS != nRet)
	{
		return nRet;
	}


	//��̬����
	nRet = DyncScalerProcs(&SCALER(hs)->DyncScalerImpl);
	if (ERR_SUCCESS != nRet)
	{
		return nRet;
	}

	return nRet;
}

//ֹͣ���ش���
ERR_WEIGHT ScalerStop(HScaler hs)
{
	ScalerLock();

	//�������
	CHECK_HANDLE_VALID(hs);

	SCALER(hs)->bRun = 0;

	ScalerUnlock();

	return ERR_SUCCESS;
}

//�ָ����ش���
ERR_WEIGHT ScalerResume(HScaler hs)
{
	ScalerLock();

	//�������
	CHECK_HANDLE_VALID(hs);

	SCALER(hs)->bRun = 1;

	ScalerUnlock();

	return ERR_SUCCESS;
}

//��̬���㴦��
ERR_WEIGHT ScalerStaticSetZero(HScaler hs, signed char bSet)
{
	ScalerLock();

	//�������
	CHECK_HANDLE_VALID(hs);

	if(SCALER(hs)->bRun == 0) 
	{
		ScalerUnlock();
		return ERR_INVALID_VAR;
	}
	if(SCALER(hs)->StaticScalerImpl.pSetZero == NULL) 
	{
		ScalerUnlock();
		return ERR_INVALID_VAR;
	}	

	SCALER(hs)->StaticScalerImpl.pSetZero(SCALER(hs)->StaticScalerImpl.pZtcb_UserData, bSet);

	ScalerUnlock();

	return ERR_SUCCESS;
}

//��̬���㴦��
ERR_WEIGHT ScalerDyncSetZero(HScaler hs, signed char bSet)
{
	int i = 0;
	sAdcProcChannel		*pAdcChanle = NULL;

	//�������
	CHECK_HANDLE_VALID(hs);

	if(SCALER(hs)->bRun == 0) 
	{
		return ERR_INVALID_VAR;
	}

	ScalerLock();

	//����ÿ��ͨ�������㺯��
	for(i = 0; i < SCALER(hs)->nAdcCount; i++)
	{
		pAdcChanle = &(SCALER(hs)->ArrAdcProcChannel[i]);

		if(pAdcChanle->pSetZero== NULL)
		{
			///��AD�ɼ�ͨ��û��AD�ɼ�������ʧ�ܷ���
			ScalerUnlock();
			return ERR_NO_SS_ZEROTRACE_PROC;
		}
		pAdcChanle->pSetZero(pAdcChanle->pZcb_UserData, bSet);

	}	
	ScalerUnlock();
	return ERR_SUCCESS;
}



