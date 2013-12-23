/**
 * \file driver.c
 * \brief EMOSƽ̨�����ӿ�
 */

/*! \addtogroup group1 EMOSƽ̨�����ⲿ�ӿ�
 *  @{
 */

#ifndef _DRIVER_H	
#define _DRIVER_H

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif

#include "includes.h"
		
#ifndef NULL
#define NULL 0
#endif

#define HANDLE 			u8					/*!< �豸���. */ 
#define HANDLENULL	0xFF				/*!< ��Ч�豸���. */ 
	
/*!
	\enum  HVL_ERR_CODE
	�������صĴ������.
*/
typedef enum
{
	HVL_NO_ERR = 0,         		/*!< û�д��� */ 
	HVL_NO_CONTROL_BLOCK,   		/*!< û���豸���ƿ���� */ 
	HVL_INSTALL_FAIL,       		/*!< �豸��װʧ�� */ 
	HVL_NO_FIND_DEV,        		/*!< û������ָ�����豸 */ 
	HVL_OPEN_DEV_FAIL,      		/*!< ���豸ʧ��*/ 
	HVL_CLOSE_DEV_FAIL,     		/*!< �ر��豸ʧ�� */ 
	HVL_HANDLE_DEV_FAIL,     		/*!< �豸�����Ч*/ 
	HVL_PARAM_ERR           		/*!< ��������*/ 
}HVL_ERR_CODE; 							

/*!
	\enum  DEV_ID
	�豸���
*/
typedef enum 
{
	
	CHAR_USART1 = 0, 			/*!<����1 */ 
	CHAR_USART2, 			/*!<����2 */ 
	CHAR_USART3, 			/*!<����3*/ 
	CHAR_UART4, 			/*!<����4 */ 
	CHAR_UART5, 			/*!<����5*/ 
	CHAR_USART6,			/*!<����6*/ 
	CHAR_TIM1,				/*!<��ʱ��1*/ 
	CHAR_TIM2,				/*!<��ʱ��2*/ 
	CHAR_TIM3,				/*!<��ʱ��3*/ 
	CHAR_TIM4,				/*!<��ʱ��4*/ 
	CHAR_TIM5,				/*!<��ʱ��5*/ 
	CHAR_TIM6,				/*!<��ʱ��6*/ 
	CHAR_TIM7,				/*!<��ʱ��7*/ 
	CHAR_TIM8,				/*!<��ʱ��8*/ 
	CHAR_TIM9,				/*!<��ʱ��9*/ 
	CHAR_TIM10,				/*!<��ʱ��10*/ 
	CHAR_TIM11,				/*!<��ʱ��11*/ 
	CHAR_TIM12,				/*!<��ʱ��12*/ 
	CHAR_RTC,					/*!<ʵʱʱ��*/ 
	CHAR_DOG,					/*!<���Ź�*/ 
	CHAR_BEEPJOB,			/*!<������*/ 
	CHAR_TTS,					/*!<TTS����*/ 
	CHAR_KEY,					/*!<�������*/ 
	CHAR_LCD,					/*!<LCD��*/ 
	CHAR_AD,					/*!<ADģ��ת��*/ 
	CHAR_LIGHT1,			/*!<��Ļ1*/ 
	CHAR_LIGHT2,			/*!<��Ļ2*/ 
	CHAR_LAND_1,			/*!<�ظ�1*/ 
	CHAR_LAND_2,			/*!<�ظ�2*/ 
	CHAR_AXIS1,				/*!<����ʶ����1*/ 
	CHAR_AXIS2,				/*!<����ʶ����2*/ 
	CHAR_xINIO1,			/*!<IO����1*/ 
	CHAR_xINIO2,			/*!<IO����2*/ 
	CHAR_xINIO3,			/*!<IO����3*/ 
	CHAR_xINIO4,			/*!<IO����4*/ 
	CHAR_xOUTIO_1,		/*!<IO���1*/ 
	CHAR_xOUTIO_2,		/*!<IO���2*/ 
	CHAR_xOUTIO_3,		/*!<IO���3*/ 
	CHAR_xOUTIO_4,		/*!<IO���4*/ 
	CHAR_UNDEF1,			/*!<δ����ID��������չ����*/ 
	CHAR_UNDEF2,			/*!<δ����ID��������չ����*/ 
	CHAR_UNDEF3,		  /*!<δ����ID��������չ����*/ 
	CHAR_UNDEF4,			/*!<δ����ID��������չ����*/ 
	CHAR_UNDEF5,			/*!<δ����ID��������չ����*/ 

	//=================================================
	CHAR_BLOCK_BOUNDARY,	/*!<�ַ��Ϳ��豸�߽�*/ 
	//=================================================
	
	//���豸
	BLOCK_FRAMEEPROM,	/*!<�����洢��*/ 
	BLOCK_DATAFLASH,	/*!<���ݴ洢��*/ 
	BLOCK_USB1,				/*!<USB1*/ 
	BLOCK_USB2,				/*!<USB2*/ 
	BLOCK_SD,					/*!<SD*/ 
	BLOCK_NORFLASH,		/*!<NORFLASH*/ 
	BLOCK_NANDFLASH,	/*!<NANDFLASH*/ 
	BLOCK_UNDEF1,			/*!<δ����ID��������չ����*/ 
	BLOCK_UNDEF2,			/*!<δ����ID��������չ����*/ 
	BLOCK_UNDEF3,			/*!<δ����ID��������չ����*/ 
	BLOCK_UNDEF4,			/*!<δ����ID��������չ����*/ 
	BLOCK_UNDEF5,			/*!<δ����ID��������չ����*/ 
	//=================================================
	DEV_MAX						/*!<����豸ID��*/ 
	//=================================================
}DEV_ID;						

typedef struct 
{
	u32		ver;					/*!<�����汾			�Զ���*/
	u8*		pDisc;				/*!<����������ַ	�Զ���*/
	u8    canShared;  	/*!<�Ƿ���			0:���ɹ���ʹ��, 1:�ɹ���ʹ��*/
	u8		OpenMax; 			/*!<���ڹ����豸���򿪴���*/
	u8		OpenCount; 		/*!<���ڹ����豸Ŀǰ�򿪴���*/
	u8 		ReadMax; 			/*!<��������*/
	u8		ReadConut;		/*!<Ŀǰ������*/
	u8 		WriteMax; 		/*!<���д����*/
	u8		WriteConut;		/*!<Ŀǰд����*/
}Dev_Info;						/*!<�豸��Ϣ�ṹ��*/


/*!
 \brief �����豸ID�Ų�ѯ�豸��Ϣ
 \param[in] ID �豸ID
 \return �豸��Ϣ�ṹ��
*/
Dev_Info* QueryDev_Info(DEV_ID ID);			

/****************************************�ж�ע��******************************************
˵������
****************************************************************************************/
typedef void (*pIRQHandle)(void); //�жϷ�����
typedef struct _SDEV_IRQ_REGS
{
	u8							EXTI_No;				//�ж��ߺ�			1-16��
	u8							EXTI_Flag;			//�ж��߱�־		0δע��		1ע��
	pIRQHandle      IRQHandler;     //�жϾ��
}SDEV_IRQ_REGS;
SDEV_IRQ_REGS* GetExti_list(void);			//�ж��߻�ȡ�ӿ�

/*!
	\brief �����豸�Ĵ򿪺���		
	\return �豸Open����Ϊ0��ʾ�ɹ�
	*/
typedef		s8 		(*pDevOpen)		(u32 lParam);																												//�豸��
/*!
	\brief �����豸�Ĺرպ���	
	\return �豸Close����Ϊ0��ʶ�ɹ�
	*/	
typedef		s8 		(*pDevClose)	(void);																												
/*!
	\brief �ַ��豸��
	\retval >= 0 ��ʶ�ɹ�
	\retval <0 ʧ��
	*/	
typedef		s32 	(*pCDevRead) 	(u8* buffer,u32 len);		
/*!
	\brief �ַ��豸д
	\retval >= 0 ��ʶ�ɹ�
	\retval <0 ʧ��
	*/	
typedef		s32 	(*pCDevWrite)	(u8* buffer,u32 len);									
/*!
	\brief ���豸��
	\retval >= 0 ��ʶ�ɹ�
	\retval <0 ʧ��
	*/				
typedef  	s32 	(*pBDevRead)	(u32 offset,u8* buffer,u32 len);	
/*!
	\brief ���豸д
	\retval >= 0 ��ʶ�ɹ�
	\retval <0 ʧ��
	*/			
typedef  	s32 	(*pBDevWrite)	(u32 offset,u8* buffer,u32 len);	
/*!
	\brief �豸����
	\retval >= 0 ��ʶ�ɹ�
	\retval <0 ʧ��
	*/	
typedef		s32 	(*pDevIoctl)	(u32 cmd,u32 lParam);																								//

/*!
	\enum DEV_REG
	�豸ע����Ϣ�ṹ��
*/
typedef struct
{
	DEV_ID				ID;							/*!<�豸ID */
	u8 						canShared;  		/*!<�Ƿ���			0:���ɹ���ʹ��, 1:�ɹ���ʹ��*/
	u8 						OpenMax; 				/*!<���ڹ����豸���򿪴���*/
	u8 						ReadMax; 				/*!<��������*/
	u8 						WriteMax; 			/*!<���д����*/
	u8* 					pDisc;					/*!<��������			�Զ���*/
	u32 					ver;						/*!<�����汾			�Զ���*/
	u32*					devOpen;    		/*!<�豸�򿪺���ָ��*/
	u32* 					devClose;   		/*!<�豸�رպ���ָ��*/
	u32* 					devRead;				/*!<�豸������ӳ��ָ��*/
	u32* 					devWrite;				/*!<�豸д����ӳ��ָ��*/
	u32* 					devIoctl;				/*!<�豸���ƺ���ӳ��ָ��*/
}DEV_REG;											

/****************************************�豸����ע�ắ��******************************************
˵������Ҫ��ӵ��豸������ʼ�������С�XXX_Init()
**************************************************************************************************/
/*!
	\brief �豸ע��
	\param dev ע����豸
	\return �����豸ע����
*/
HVL_ERR_CODE DeviceInstall(DEV_REG *dev);			
/*!
	\brief �ж�ע�ắ��
	\param irq ע����ж���Ϣ
	\return �����ж�ע��Ľ��
*/
s8 EXTIIRQHandlerInstall(SDEV_IRQ_REGS* irq);		
/*!
	\brief �豸����Ԫ��ʼ��
*/
void DevList_init(void);																

/****************************************Ӧ�ò�ͳһ���ʽӿں���***********************************
˵�����Ը��豸�ķ��ʽӿ�ͳһ����ͬ���豸�ò�ͬ���豸ID�Ž�������
**************************************************************************************************/
//�豸Ӧ�ò�ӿ�

/*! 
	\brief ���豸
	\param[in]  		ID					�豸ID��
	\param[in]  		lParam			����
	\param[in,out]  err				  ��������
	
	\return ����ƽ̨�ڲ�������豸���,����Ƿǹ����豸���й�һ�δ򿪺�Ͳ����ٴ���	��
	\retval HANDLE �豸���
	\retval HANDLENULL ��Ч�豸���
		
*/

HANDLE DeviceOpen(DEV_ID ID,u32 lParam,HVL_ERR_CODE* err);

/*!

	\brief 			�ر��豸
  \param[in] IDH					��Ҫ�ر��豸�ľ��
	\retval err		�������� 
	\return �����Ƿ�ɹ��ر�
*/
HVL_ERR_CODE DeviceClose(HANDLE IDH);

/*!
  \brief �ַ��豸��.
	\param[in]  IDH					�豸���
	\param[in]  Buffer					��ȡ��Ż�����
	\param[in]  len							ϣ����ȡ�����ݸ���
	\return								�Ƿ�ɹ�		
	\retval >=0 �ɹ�	
	\retval <0  ʧ��
*/
s32 CharDeviceRead(HANDLE IDH,u8* Buffer,u32 len);

/*!

	\brief �ַ��豸д
	\param[in] IDH			�豸���
	\param[in] Buffer			�豸д����Դ������
	\param[in] len					ϣ��д��ĳ���
	\param[in] Reallen		ʵ��д������ݸ���
	\param[in] lParam			����
	\return  �Ƿ�ɹ�	
	\retval >=0 ʵ��д����ֽ���
	\retval <0  д��ʧ��
*/
s32 CharDeviceWrite(HANDLE IDH,u8* Buffer,u32 len);

/*!

	\brief ���豸��
	\param[in] IDH					�豸���
	\param[in] offset					��ʼ��ַ
	\param[in] Buffer					��ȡ��Ż�����
	\param[in] len							ϣ����ȡ�����ݸ���
	\return  �Ƿ�ɹ�	
	\retval >=0 ʵ�ʶ�ȡ���ֽ���
	\retval <0  ��ȡʧ��
*/
s32 BlockDeviceRead(HANDLE IDH,u32 offset,u8* Buffer,u32 len);

/*!

	\brief ���豸д
	\param[in] IDH					�豸���
	\param[in] offset					��ʼ��ַ
	\param[in] Buffer				 д�����ݴ�ŵĻ�����
	\param[in] len							ϣ��д������ݸ���
	\return  �Ƿ�ɹ�	
	\retval >=0 ʵ��д����ֽ���
	\retval <0  д��ʧ��
*/
s32 BlockDeviceWrite(HANDLE IDH,u32 offset,u8* Buffer,u32 len);

/*!
\brief �豸����
\param[in] IDH					�豸���
\param[in] cmd							����
\param[in] lParam					����		��������Ĳ�ͬ����ֵ��ͬ
\return ��������Ĳ�ͬ����ֵ��ͬ	
*/
s32 DeviceIoctl(HANDLE IDH,u32 cmd,u32 lParam);

#ifdef __cplusplus		   		//�����CPP����C���� //��������
}
#endif

#endif


/*! @} */ // end of group1

