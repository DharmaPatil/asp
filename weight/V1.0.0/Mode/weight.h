
/**
 * \file weight.c
 * \brief ����ģ��ӿ�
 */

/*! \addtogroup group1 ����ģ���ⲿ�ӿ�
 *  @{
 */

#ifndef __WEIGHT_H
#define __WEIGHT_H

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif


#ifndef NULL
#define NULL			0
#endif

typedef unsigned char  BOOLEAN;

#ifndef BOOL
#define	BOOL			BOOLEAN
#endif


#ifndef FALSE
#define FALSE 0
#define TRUE 1
#endif






//������================================================================================================
/*!
	\brief ����ģ����󷵻�����
	\enum  ����ģ������ʱ���صĴ���
*/
typedef enum{
	ERR_SUCCESS=0,					/*!<�޴��󣬳ɹ�. */ 
	ERR_INVALID_VAR,				/*!<��Ч���� */ 
	ERR_NO_SCALER,					/*!<�޿��ó�̨ */
	ERR_NO_ADC_CHANLE,				/*!<��Ч��ADCͨ����Ŵ����� */
	ERR_NO_ADC_PROC,				/*!<û�ж���AD�ɼ�����*/ 
	ERR_NO_ADC_CHECK_PROC,			/*!<û�ж���AD��Ч�Լ�麯��*/ 
	ERR_NO_ADC_FILTER_PROC,			/*!<û�ж���AD�����˲�����*/ 
	ERR_NO_ADC_STAND_PROC,			/*!<û�ж���AD��׼������*/ 
	ERR_NO_ADC_NORMALIZE_PROC,		/*!<û�ж���AD��һ������*/ 
	ERR_NO_ADC_ZEROTRACE_PROC,		/*!<û�ж���AD�����ٺ���*/ 
	ERR_NO_SS_STATIC_INIT_PROC,		/*!<û�ж��徲̬�����ʼ������*/ 
	ERR_NO_SS_STATIC_PROC,			/*!<û�ж��徲̬������*/ 
	ERR_NO_SS_DIGSAMPLE_PROC,		/*!<û�ж��徲̬����������*/ 
	ERR_NO_SS_FILTER_PROC,			/*!<û�ж��徲̬�˲�����*/ 
	ERR_NO_SS_ZEROTRACE_PROC,		/*!<û�ж��徲̬�����ٴ�����*/ 
	ERR_NO_SS_STATECHECK_PROC,		/*!<û�ж��徲̬״̬������*/ 
	ERR_NO_DS_DYNC_INIT_PROC,		/*!<û�ж��嶯̬�����ʼ������*/ 
	ERR_NO_DS_DYNC_PROC,			/*!<û�ж��嶯̬������*/ 
	ERR_MAX,						/*!<������ţ�����*/ 
}ERR_WEIGHT;




//�ص���������================================================================================================

//�ٽ��������ӿں���ԭ�Ͷ���
typedef void (*pLockFunc)(void );

typedef void (*pUnlockFunc)(void );

/*! 
\brief �����˲���ʼ��������ԭ�Ͷ���
\param pUserParam �û�������Զ���ֵ
\return ���س�ʼ���Ƿ�ɹ�
\retval 1 �ɹ�
\retval 0 ʧ��
*/
typedef char (*pInitFunc)(void* pUserParam);


/*! 
\brief ע���AD�ɼ�����ԭ�Ͷ���. 
\param[in]  chanle ͨ�����
\param[out] value  �ɼ�����ADֵ	
\return ���زɼ��Ƿ�ɹ�
\retval 1 �ɹ�
\retval 0 ʧ��
*/
typedef long (*pAdcGetFun)(int chanle);

/*! 
\brief AD�����⺯��ԭ�Ͷ���
\param nAD ��Ҫ�����Ч�Ե�ADֵ
\return ����ADֵ�Ƿ���Ч
\retval 1 ��Ч
\retval 0 ��Ч

*/
typedef int (*pAdcValidCheckFunc)(long nAd);


/*! 
\brief �����˲�������ԭ�Ͷ���
\param pFCB �û�������Զ���ֵ
\param nIn  ��Ҫ�˲���ADֵ
\return �����˲����ֵ
*/
typedef float (*pFilterFunc)(void* pFCB, float nIn);

/*! 
\brief ��׼������ԭ�Ͷ���
\param pSCB �û�������Զ���ֵ
\param nIn  ��Ҫ��׼����ADֵ
\return		���ر�׼�����ֵ
*/
typedef float (*pAdcStandlizeFunc)(void* pSCB, float fIn);

/*! 
\brief ��һ������ԭ�Ͷ���
\param pSCB �û�������Զ���ֵ
\param nIn  ��Ҫ��һ����ֵ
\return		���ع�һ�����ֵ
*/
typedef float (*pAdcNormalizeFunc)(void *pNCB, float fIn);

/*! 
\brief ��λ���ٺ���ԭ�Ͷ���

*/
typedef float (*pZeroTraceFunc)(void *pZTCB, float fIn);

/*! 
\brief  ���㺯��ԭ�Ͷ���

*/
typedef void (*pSetZeroFunc)(void *pZTCB);


/*! \brief ״̬�ж�����ԭ�Ͷ���*/
typedef int (*pStateCheckFunc)(void *pSCCB, float fIn);


/*! \brief �ֶ�ֵ������ԭ�Ͷ���*/
typedef float (*pDrvFunc)(void *pDCB, float fIn);


/*!
\brief	��̬��ֵ����ԭ��
*/
typedef float (*pStaticTotalFunc)(void* pSecb, float * pInArr, int nInCount);


/*!
\brief	��̬��������ԭ��
*/
typedef int (*pSampleFunc)(void *pDscb, float in, float* pOut);


/*! \brief ��̬�����ʼ����������*/
typedef char (*pDyncProcInitFunc)(void* pDecb);


/*! \brief ��̬����������*/
typedef float (*pDyncProcFunc)(void* pDecb, float * pInArr, int nInCount);

/**@}*/






//��̨����==================================================================================================
/*! \brief ��̨������*/
typedef unsigned int HScaler; /*!> ��̨������*/


/*! \brief ��Ч�ĳ�̨������ */
#define HSCALER_INVALID  0  /*!< ��Ч�ĳ�̨������*/


/*!
\brief ����һ����̨����.
\param nAdcChanle �ó�̨ӵ�е�AD�ɼ�ͨ����.
\return ���ط���ɹ��ĳ�̨��� .
\retval HSCALER_INVALID ��̨����ʧ��
\retval ����0			�ɹ�
*/
HScaler ScalerCreate(int nAdcChanle, ERR_WEIGHT* err);



/*!
\brief ����һ����̨����.
\param HScaler Ҫ���ٵĳ�̨.
*/
void ScalerDestory(HScaler hs);




//ADCͨ������ע��===========================================================================================
/*!
\brief 
    ע��AD�ɼ����������ⲿģ��ʵ��AD�ɼ��������ڲ�����ģ�鸺�����
    Ϊĳ��ָ����̨��ĳ��AD�ɼ�ͨ���� ָ��AD�ɼ�����
\param hs		
	��̨���
\param init_proc
	ע��AD�ɼ���ʼ������
\param adc_proc 
	ע���AD�ɼ�����
\return 
	����ע��Ľ��
\retval  0  �ɹ�
\retval -1  ʧ��
\remark �ýӿ�Ϊָ����̨�ڵ�����AD�ɼ�ͨ��ע��ͬһ��AD�ɼ�����
*/
int RegisterAdcGetProc(HScaler hs, int chanle, pInitFunc pAdcGetInit, pAdcGetFun pAdcGet, void* UserData);


/*!
\brief
        Ϊ��ָ̨��AD�ɼ�ͨ��ע��AD��Ч�Լ�⺯��,���AD�ɼ�ֵ�Ƿ���ȷ
\param hs			��̨���
\param chanle		ָ���ɼ�ͨ��
\param check_proc   ע���ADת�������⺯��
\see ::RegisterAdcCheckProc
\return ����ע��Ľ��
        
\retval 0  �ɹ�
\retval -1 ʧ��
*/
int RegisterAdcCheckProc(HScaler hs, int chanle, pInitFunc pAdcValidCheckInit, pAdcValidCheckFunc pAdcValidCheck, void* UserData);


/*!
\brief
        Ϊ��ָ̨��ADCͨ��ע��AD�����˲���ʼ������
\param hs									��̨���
\param chanle							ͨ�����
\param init_filter_proc   ע���ADC�����˲���ʼ������
\param filter_proc				ע���ADC�����˲�����
\param userData						�û��Զ�������
\return ����ע��Ľ��
        
\retval 0  �ɹ�
\retval -1 ʧ��
*/		
int RegisterAdcFilterProc(HScaler hs, int chanle, pInitFunc pFilterInit, pFilterFunc pFilter, void* UserData);


/*!
\brief
        Ϊ��ָ̨��AD�ɼ�ͨ��ע���׼��������
\param hs			��̨���
\param chanle		ָ���ɼ�ͨ��
\param stand_proc   ע���ADת�������⺯��
\param userData						�û��Զ�������
\return ����ע��Ľ��
        
\retval 0  �ɹ�
\retval -1 ʧ��
*/
int RegisterAdcStandlizeProc(HScaler hs, int chanle, pInitFunc pAdcStandlizeInit, pAdcStandlizeFunc pAdcStandlize, void* UserData);


/*!
\brief
        Ϊ��ָ̨��ͨ��ע��AD��һ��������
\param hs									��̨���
\param chanle							ͨ�����
\param normalize_proc   	ע��Ĺ�һ��������
\param userData						�û��Զ�������
\return ����ע��Ľ��
        
\retval 0  �ɹ�
\retval -1 ʧ��
*/								
int RegisterAdcNormalizeProc(HScaler hs, int chanle, pInitFunc pAdcNormalizeInit, pAdcNormalizeFunc pAdcNormalize, void* UserData);


/*!
\brief
        Ϊ��̨ע��ͳһ��ADC�����ٴ�����
\param hs									��̨���
\param chanel							ͨ�����
\param zerotrack_proc   	ע��Ĺ�һ��������
\param userData						�û��Զ�������
\return ����ע��Ľ��
        
\retval 0  �ɹ�
\retval -1 ʧ��
*/								
int RegisterAdcZeroTraceProc(HScaler hs, int chanle, pInitFunc pZeroTraceInit, pZeroTraceFunc pZeroTrace, pSetZeroFunc pSetZero, void* UserData);





//��̬����ע��=====================================================================================================
/*!
\brief	ע�ᾲ̬��������������ʼ���Ͷ�̬������

  \param	hs		��̨����
  \param	init_proc	ע�ᾲ̬�����ʼ������
  \param	dync_proc	ע�ᾲ̬������ 
  \param	userData	��̬����ĸ������� 
  \return			���سɹ����
  \retval	0		�ɹ�
  \retval  -1		ʧ��
*/
int RegisterScalerStaticTotalProc(HScaler hs, pInitFunc pStaticTotalInit, pStaticTotalFunc pStaticTotal, void* UserData);

/*!
\brief	Ϊ��̨ע�ᾲ̬�ӳ�������

  \param	hs		��̨����
  \param	sample_proc	ע�������ʼ������
  \param	sample_proc	ע���������
  \param	userData	��̬���˲�����ĸ������� 
  \return			���سɹ����
  \retval	0		�ɹ�
  \retval  -1		ʧ��
*/
int RegisterScalerStaticSampleProc(HScaler hs, pInitFunc pSampleInit, pSampleFunc pSample, void* UserData);


/*!
\brief	Ϊ��̨ע�ᾲ̬���˲�������������ʼ���Ͷ�̬������

  \param	hs		��̨����
  \param	init_proc	ע�ᾲ̬���˲���ʼ������
  \param	dync_proc	ע�ᾲ̬���˲������� 
  \param	userData	��̬���˲�����ĸ������� 
  \return			���سɹ����
  \retval	0		�ɹ�
  \retval  -1		ʧ��
*/
int RegisterScalerStaticFilterProc(HScaler hs, pInitFunc pFilterInit, pFilterFunc pFilter, void* UserData);


/*!
\brief	Ϊ��̨ע�ᾲ̬�������ٺ���

  \param	hs		��̨����
  \param	zt_proc	ע���������
  \param	userData	��̬���˲�����ĸ������� 
  \return			���سɹ����
  \retval	0		�ɹ�
  \retval  -1		ʧ��
*/
int RegisterScalerStaticZeroTraceProc(HScaler hs, pInitFunc pZeroTraceInit, pZeroTraceFunc pZeroTrace, pSetZeroFunc pSetZero, void* UserData);



/*!
\brief	Ϊ��̨ע�ᾲ̬��״̬��⺯��

  \param	hs		��̨����
  \param	sta_check_proc	ע���������
  \param	userData	��̬���˲�����ĸ������� 
  \return			���سɹ����
  \retval	0		�ɹ�
  \retval  -1		ʧ��
*/
int RegisterScalerStaticStateCheckProc(HScaler hs, pInitFunc pStateCheckInit, pStateCheckFunc pStateCheck, void* UserData);


//���þ�̬�ֶ�ֵ
int SetScalerStaticDrv(HScaler hs, float nDrv);










//��̬����ע��========================================================================================================
/*!
\brief	ע�ᶯ̬��������������ʼ���Ͷ�̬������

  \param	hs		��̨����
  \param	init_proc	ע�ᶯ̬�����ʼ������
  \param	dync_proc	ע�ᶯ̬������ 
  \param	userData	��̬����ĸ������� 
  \return			���سɹ����
  \retval	0		�ɹ�
  \retval  -1		ʧ��
*/
int RegisterScalerDyncProc(HScaler hs, pDyncProcInitFunc pDyncProcInit, pDyncProcFunc pDyncProc, void* UserData);


//���ö�̬�ֶ�ֵ
int SetScalerDyncDrv(HScaler hs, float nDrv);









//ADCͨ��������===========================================================================
//��ȡADCͨ��ԭʼADֵ
int GetAdcChanleOrgAd(HScaler hs,int chanel);

//��ȡADCͨ�����״̬
int GetAdcChanleValidState(HScaler hs,int chanel) ;

//��ȡADCͨ���˲�������
float GetAdcChanleFilterValue(HScaler hs,int chanel);

//��ȡADCͨ����׼��������
float GetAdcChanleStandValue(HScaler hs,int chanel);

//��ȡADCͨ����һ������������
float GetAdcChanleNormalizeValue(HScaler hs,int chanel);

//��ȡADCͨ����λ������
float GetAdcChanleZerolizeValue(HScaler hs,int chanel);

//�жϳ�̨ĳһ·AD�Ƿ����
//0 --- ����
// != 0 �쳣
int IsAdcError(HScaler hs,int channel);








//��̬������================================================================================
//��ȡ��̬��ֵ���
float GetScalerStaticTotalValue(HScaler hs);

//��ȡ��̬�������
float GetScalerStaticSampleValue(HScaler hs);

//��ȡ��̬�˲����
float GetScalerStaticFilterValue(HScaler hs);

//��ȡ��̬��λ���ٽ��
float  GetScalerStaticZeroTraceValue(HScaler hs);

//��ȡ��̬״̬���
int GetScalerStaticState(HScaler hs);

//��ȡ��̬�趨�ֶ�ֵ
float GetScalerStaticDrv(HScaler hs);

//��ȡ��̬�ֶ�ֵ�����Ľ��
float GetScalerStaticDrvValue(HScaler hs);

//�ж�ָ����̨�Ƿ�̬�ȶ�
//0 ���ȶ� !=0 �ȶ� 
int IsScalerStaticSteady(HScaler hs);

//�жϳ�̨�Ƿ��ھ�̬���
//0 ���ȶ� !=0 �ȶ� 
int IsScalerStaticAtZero(HScaler hs); 








//��̬������==================================================================================
//��ȡ��̬����ֵ
float GetScalerDyncValue(HScaler hs );


//��ȡ��̬�����ķֶ�ֵ
float GetScalerDyncDrvValue(HScaler hs );


//��ȡ��̬�趨�ֶ�ֵ
float GetScalerDyncDrv(HScaler hs );



//��̨����=====================================================================================
/*!
\brief ����������������Ҫ�ٵ�����ע�ắ���󱻵��á���ʼָ���ĳ��ض���.
\param hs ��̨����
\return ���������Ľ��
\retval ERR_SUCCESS  �ɹ�
\retval ���� ʧ��
*/
ERR_WEIGHT ScalerStart(HScaler hs);

/*!
\brief ����ģ���ܵ������к������ú�����Ҫ��ע��͵�������������ʱ���á�
\param   hs�� ����Ǹ���̨�������г��ش���
\return  ���еĽ��   
\retval  ERR_SUCCESS  �ɹ�
\retval  ���� 				ʧ��
*/
ERR_WEIGHT ScalerRun(HScaler hs);


//ֹͣ���ش���
ERR_WEIGHT ScalerStop(HScaler hs);

//�ָ����ش���
ERR_WEIGHT ScalerResume(HScaler hs);

/*!
\brief ���ش���������ĺ���
\param[in]  err  ������
\return		 ���ش������ַ���
*/

//��̬���㴦��
ERR_WEIGHT ScalerDyncSetZero(HScaler hs);

//��̬���㴦��
ERR_WEIGHT ScalerStaticSetZero(HScaler hs);

//��ȡ����ģ�Ͱ汾 
//4�ֽڱ�ʾ������汾��Ϊ��1.2.2.3 ����ֵΪ��0x01020203
long	GetScalerVer(void);

const char* GetError(ERR_WEIGHT err);


#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
}
#endif


#endif
 
/*! @} */ // end of group


