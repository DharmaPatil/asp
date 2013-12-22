/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "usbh_usr.h"
#include "ff.h"       
#include "usbh_msc_core.h"
#include "usbh_msc_scsi.h"
#include "usbh_msc_bot.h"
#include <stdio.h>

extern USB_OTG_CORE_HANDLE          USB_OTG_Core;
/** @defgroup USBH_USR_Private_Variables
* @{
*/ 
//static uint8_t USBH_USR_ApplicationState = USH_USR_FS_INIT;

/*  Points to the DEVICE_PROP structure of current device */
/*  The purpose of this register is to speed up the execution */
USBH_Usr_cb_TypeDef USR_cb =
{
	USBH_USR_Init,
	USBH_USR_DeInit,
	USBH_USR_DeviceAttached,
	USBH_USR_ResetDevice,
	USBH_USR_DeviceDisconnected,
	USBH_USR_OverCurrentDetected,
	USBH_USR_DeviceSpeedDetected,
	USBH_USR_Device_DescAvailable,
	USBH_USR_DeviceAddressAssigned,
	USBH_USR_Configuration_DescAvailable,
	USBH_USR_Manufacturer_String,
	USBH_USR_Product_String,
	USBH_USR_SerialNum_String,
	USBH_USR_EnumerationDone,
	USBH_USR_UserInput,
	USBH_USR_MSC_Application,
	USBH_USR_DeviceNotSupported,
	USBH_USR_UnrecoveredError   
};

/** @defgroup USBH_USR_Private_FunctionPrototypes
* @{
*/
static s32 	s32USB1State = NO_U_DISC;		//����U��״̬
char	sProductString[20]={0};				//����U����������
static s32	s32Ucapacity = 0;				//����U������
/****************************************************************************
* ��	�ƣ�fUSB1_StateRead
* ��	�ܣ���ȡUSB1״̬
* ��ڲ�������
* ���ڲ�����USB1״̬
* ˵	������
****************************************************************************/
s32	fUSB1_StateRead(void) 
{
	return s32USB1State;
}
/****************************************************************************
* ��	�ƣ�fUSB1_CapacityReturn
* ��	�ܣ�����U������
* ��ڲ�������
* ���ڲ�����USB1״̬
* ˵	������
****************************************************************************/
s32	fUSB1_CapacityReturn(void)
{
	return	s32Ucapacity;
}
/**
* @brief  USBH_USR_Init 
*         Displays the message on LCD for host lib initialization
* @param  None
* @retval None
*/
static void USBH_USR_Init(void)
{
  static uint8_t startup = 0;  
  
  if(startup == 0 )
  {
    startup = 1;    	
  }
}
/**
* @brief  USBH_USR_DeviceAttached 
*         Displays the message on LCD on device attached
* @param  None
* @retval None
*/
static void USBH_USR_DeviceAttached(void)
{	//��⵽U��
	s32USB1State = LINK_U_DISC;
}
/**
* @brief  USBH_USR_UnrecoveredError
* @param  None
* @retval None
*/
static void USBH_USR_UnrecoveredError (void)
{ 
}
/**
* @brief  USBH_DisconnectEvent
*         Device disconnect event
* @param  None
* @retval Staus
*/
static void USBH_USR_DeviceDisconnected (void)
{
	s32USB1State = EXTRACT_U_DISC;			//Ӧ�ò��ã����浱ǰ״̬ΪU���Ѱγ�
	s32Ucapacity = 0;						//���U������
}
/**
* @brief  USBH_USR_ResetUSBDevice 
* @param  None
* @retval None
*/
static void USBH_USR_ResetDevice(void)
{
  /* callback for USB-Reset */
}
/**
* @brief  USBH_USR_DeviceSpeedDetected 
*         Displays the message on LCD for device speed
* @param  Device speed
* @retval None
*/
static void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed)
{
}
/**
* @brief  USBH_USR_Device_DescAvailable 
*         Displays the message on LCD for device descriptor
* @param  device descriptor
* @retval None
*/
static void USBH_USR_Device_DescAvailable(void *DeviceDesc)
{ 
}
/**
* @brief  USBH_USR_DeviceAddressAssigned 
*         USB device is successfully assigned the Address 
* @param  None
* @retval None
*/
static void USBH_USR_DeviceAddressAssigned(void)
{
}
/**
* @brief  USBH_USR_Conf_Desc 
*         Displays the message on LCD for configuration descriptor
* @param  Configuration descriptor
* @retval None
*/
static void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,
                                          USBH_InterfaceDesc_TypeDef *itfDesc,
                                          USBH_EpDesc_TypeDef *epDesc)
{
}

/**
* @brief  USBH_USR_Manufacturer_String 
*         Displays the message on LCD for Manufacturer String 
* @param  Manufacturer String 
* @retval None
*/
static void USBH_USR_Manufacturer_String(void *ManufacturerString)
{
}
/**
* @brief  USBH_USR_Product_String 
*         Displays the message on LCD for Product String
* @param  Product String
* @retval None
*/
static void USBH_USR_Product_String(void *ProductString)
{
	//����U����������
	sprintf(sProductString, "%s", (char *)ProductString); 
}
/**
* @brief  USBH_USR_SerialNum_String 
*         Displays the message on LCD for SerialNum_String 
* @param  SerialNum_String 
* @retval None
*/
static void USBH_USR_SerialNum_String(void *SerialNumString)
{
} 
/**
* @brief  EnumerationDone 
*         User response request is displayed to ask application jump to class
* @param  None
* @retval None
*/
static void USBH_USR_EnumerationDone(void)
{ 
} 
/**
* @brief  USBH_USR_DeviceNotSupported
*         Device is not supported
* @param  None
* @retval None
*/
static void USBH_USR_DeviceNotSupported(void)
{
}  
/**
* @brief  USBH_USR_UserInput
*         User Action for application state entry
* @param  None
* @retval USBH_USR_Status : User response for key button
*/
static USBH_USR_Status USBH_USR_UserInput(void)
{
	return USBH_USR_RESP_OK;
}  

/**
* @brief  USBH_USR_OverCurrentDetected
*         Over Current Detected on VBUS
* @param  None
* @retval Staus
*/
static void USBH_USR_OverCurrentDetected (void)
{
}

/**
* @brief  USBH_USR_MSC_Application 
*         Demo application for mass storage
* @param  None
* @retval Staus
*/
static int USBH_USR_MSC_Application(void)
{
	s32USB1State = GET_READY_U_DISC	;		//(���Խ����ļ�����)
	//����U����������λM
	s32Ucapacity = (USBH_MSC_Param.MSCapacity>>11);		
	return(0);
}

static void USBH_USR_DeInit(void)
{
//  USBH_USR_ApplicationState = USH_USR_FS_INIT;
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

