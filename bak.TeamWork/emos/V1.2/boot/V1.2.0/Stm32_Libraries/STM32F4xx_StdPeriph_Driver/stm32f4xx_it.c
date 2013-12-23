/**
  ******************************************************************************
  * @file    GPIO/IOToggle/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    30-September-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"

/** @addtogroup STM32F4xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup IOToggle
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
#include "printf_init.h"
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
	printf("HardFault_Handler\r\n");
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

#define BootLoaderUse		//�Ƿ�ΪBootLoaderʹ��		������(��)		����(����)

#ifndef BootLoaderUse		//�������BootLoaderʹ��
//EXTI�ж�
#include "driver.h"
#include "C2.h"					//SysIntEnter

void EXTI0_IRQHandler(void)
{
	SDEV_IRQ_REGS* EXIT_List = (SDEV_IRQ_REGS*)0;
	
	SysIntEnter();
	EXIT_List = GetExti_list();
	
	if(EXIT_List[0].EXTI_Flag == 1)			//�ж�����ע��
		EXIT_List[0].IRQHandler();				//��ת���жϺ���ִ��
	SysIntExit();	
}

void EXTI1_IRQHandler(void)
{
	SDEV_IRQ_REGS* EXIT_List = (SDEV_IRQ_REGS*)0;
	
	SysIntEnter();
	EXIT_List = GetExti_list();
	
	if(EXIT_List[1].EXTI_Flag == 1)			//�ж�����ע��
		EXIT_List[1].IRQHandler();				//��ת���жϺ���ִ��
	SysIntExit();	
}

void EXTI2_IRQHandler(void)
{
	SDEV_IRQ_REGS* EXIT_List = (SDEV_IRQ_REGS*)0;
	
	SysIntEnter();
	EXIT_List = GetExti_list();
	
	if(EXIT_List[2].EXTI_Flag == 1)			//�ж�����ע��
		EXIT_List[2].IRQHandler();				//��ת���жϺ���ִ��
	SysIntExit();	
}

void EXTI3_IRQHandler(void)
{
	SDEV_IRQ_REGS* EXIT_List = (SDEV_IRQ_REGS*)0;
	
	SysIntEnter();
	EXIT_List = GetExti_list();
	
	if(EXIT_List[3].EXTI_Flag == 1)			//�ж�����ע��
		EXIT_List[3].IRQHandler();				//��ת���жϺ���ִ��
	SysIntExit();	
}

void EXTI4_IRQHandler(void)
{
	SDEV_IRQ_REGS* EXIT_List = (SDEV_IRQ_REGS*)0;
	
	SysIntEnter();
	EXIT_List = GetExti_list();
	
	if(EXIT_List[4].EXTI_Flag == 1)			//�ж�����ע��
		EXIT_List[4].IRQHandler();				//��ת���жϺ���ִ��
	SysIntExit();	
}

void EXTI9_5_IRQHandler(void)
{
	SDEV_IRQ_REGS* EXIT_List = (SDEV_IRQ_REGS*)0;
	
	SysIntEnter();
	EXIT_List = GetExti_list();
	
	if(EXTI_GetITStatus(EXTI_Line5) != RESET)					//�ж���5����
	{
		if(EXIT_List[5].EXTI_Flag == 1)								//�ж�����ע��
			EXIT_List[5].IRQHandler();										//��ת���жϺ���ִ��
	}
	else if(EXTI_GetITStatus(EXTI_Line6) != RESET)		//�ж���6����
	{
		if(EXIT_List[6].EXTI_Flag == 1)								//�ж�����ע��
			EXIT_List[6].IRQHandler();										//��ת���жϺ���ִ��
	}
	else if(EXTI_GetITStatus(EXTI_Line7) != RESET)		//�ж���7����
	{
		if(EXIT_List[7].EXTI_Flag == 1)								//�ж�����ע��
			EXIT_List[7].IRQHandler();										//��ת���жϺ���ִ��
	}
	else if(EXTI_GetITStatus(EXTI_Line8) != RESET)		//�ж���8����
	{
		if(EXIT_List[8].EXTI_Flag == 1)								//�ж�����ע��
			EXIT_List[8].IRQHandler();										//��ת���жϺ���ִ��
	}
	else if(EXTI_GetITStatus(EXTI_Line9) != RESET)		//�ж���9����
	{
		if(EXIT_List[9].EXTI_Flag == 1)								//�ж�����ע��
			EXIT_List[9].IRQHandler();										//��ת���жϺ���ִ��
	}
	SysIntExit();	
}

void EXTI15_10_IRQHandler(void)
{
	SDEV_IRQ_REGS* EXIT_List = (SDEV_IRQ_REGS*)0;
	
	SysIntEnter();
	EXIT_List = GetExti_list();
	
	if(EXTI_GetITStatus(EXTI_Line10) != RESET)				//�ж���10����
	{
		if(EXIT_List[10].EXTI_Flag == 1)								//�ж�����ע��
			EXIT_List[10].IRQHandler();										//��ת���жϺ���ִ��
	}
	else if(EXTI_GetITStatus(EXTI_Line11) != RESET)		//�ж���11����
	{
		if(EXIT_List[11].EXTI_Flag == 1)								//�ж�����ע��
			EXIT_List[11].IRQHandler();										//��ת���жϺ���ִ��
	}
	else if(EXTI_GetITStatus(EXTI_Line12) != RESET)		//�ж���12����
	{
		if(EXIT_List[12].EXTI_Flag == 1)								//�ж�����ע��
			EXIT_List[12].IRQHandler();										//��ת���жϺ���ִ��
	}
	else if(EXTI_GetITStatus(EXTI_Line13) != RESET)		//�ж���13����
	{
		if(EXIT_List[13].EXTI_Flag == 1)								//�ж�����ע��
			EXIT_List[13].IRQHandler();										//��ת���жϺ���ִ��
	}
	else if(EXTI_GetITStatus(EXTI_Line14) != RESET)		//�ж���14����
	{
		if(EXIT_List[14].EXTI_Flag == 1)								//�ж�����ע��
			EXIT_List[14].IRQHandler();										//��ת���жϺ���ִ��
	}
	else if(EXTI_GetITStatus(EXTI_Line15) != RESET)		//�ж���15����
	{
		if(EXIT_List[15].EXTI_Flag == 1)								//�ж�����ע��
			EXIT_List[15].IRQHandler();										//��ת���жϺ���ִ��
	}
	SysIntExit();	
}
#endif

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
