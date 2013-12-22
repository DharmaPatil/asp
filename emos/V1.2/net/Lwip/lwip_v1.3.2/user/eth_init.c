#include "eth_init.h"
#include "stm32f4xx.h"
#include "stm32f4x7_eth.h"
#include "driver.h"

#define DP83848_PHY_ADDRESS       0x01
#define PHY_MICR                  0x11 								//MII�жϿ��ƼĴ���
#define PHY_MICR_INT_EN           ((u16)0x0002) 			//PHY�����ж�
#define PHY_MICR_INT_OE           ((u16)0x0001) 			//PHY��������ж��ź�
#define PHY_MISR                  0x12 								//MII�ж�״̬��MISC���ƼĴ���
#define PHY_MISR_LINK_INT_EN      ((u16)0x0020) 			//��������״̬�ı��ж�
#define PHY_LINK_STATUS           ((u16)0x2000) 			//PHY����״̬�жϱ�־λ

#define ETH_LINK_EXTI_LINE             EXTI_Line0
#define ETH_LINK_EXTI_PORT_SOURCE      EXTI_PortSourceGPIOC
#define ETH_LINK_EXTI_PIN_SOURCE       EXTI_PinSource0
#define ETH_LINK_EXTI_IRQn             EXTI0_IRQn 

#define ETH_LINK_PIN                   GPIO_Pin_0
#define ETH_LINK_GPIO_PORT             GPIOC
#define ETH_LINK_GPIO_CLK              RCC_AHB1Periph_GPIOC

__IO uint8_t EthLinkStatus = 0;

//�ж�ע����Ϣ��
static SDEV_IRQ_REGS ETH_IRQ = 
{
	0,								//�ж��ߺ�
	0,								//�ж��߿�����־
	(pIRQHandle)0,		//�жϾ��
};

/****************************************************************************
* ��	�ƣ�void ETH_GPIO_Config(void)
* ��	�ܣ���̫�����ų�ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵	����EtherNet
��RMII��������MIIһ�룬����Ҫ��ʱ��Ƶ�������һ�����ܺ�MII�ﵽͬ���������շ�
��GMII�����߶�MIIһ��������ͬ���������շ�������������ʱ��Ƶ��ֻҪMII��һ��
RMII:2���ߴ�����		10M����(RMII=5M)		100M����(RMII=50M)
MII:��Ҫ16���ź�,4���ߴ�����		10M����(MII=2.5M)		100M����(MII=25M)
CRS,RX_CLK,COL,RX_DV,RXD2,RXD3,TXD3,RX_EX,TX_EN,TXD0,TXD2,TX_CLK,RXD0,RXD1,TXD1,TX-ER
GMII:8���ߴ����� 	10M����(GMII=1.25M)		100M����(GMII=12.5M)		1000M����(GMII=125M)

//PA0-WKUP			ETH_MII_CRS									//PH2		ETH_MII_CRS			(PB0)				
//��ͻ���
//PA3			ETH_MII_COL												//PH3		ETH_MII_COL			(PH3)
//��������
//PC4			ETH_MII_RXD0 / ETH_RMII_RXD0
//PC5			ETH_MII_RXD1 / ETH_RMII_RXD1
//PB0			ETH_MII_RXD2											//PH6		ETH_MII_RXD2		(PB0)
//PB1			ETH_MII_RXD3											//PH7		ETH_MII_RXD3		(PB1)	
//����ʱ��
//PA1			ETH_MII_RX_CLK / ETH_RMII_REF_CLK
//�������ݳ���ָʾ
//PB10		ETH_MII_RX_ER											//PI10	ETH_MII_RX_ER		(PB10)
//����������Чָʾ
//PA7			ETH_MII_RX_DV / ETH_RMII_CRS_DV
//��������
//PB12		ETH_MII_TXD0 / ETH_RMII_TXD0			//PG13	ETH_MII_TXD0 / ETH_RMII_TXD0		(PB12)	
//PG14		ETH_MII_TXD1 / ETH_RMII_TXD1			//PB13	ETH_MII_TXD1 / ETH_RMII_TXD1		(PB14)	
//PC2			ETH_MII_TXD2
//PB8			ETH_MII_TXD3											//PE2		ETH_MII_TXD3		(PB8)
//������ʹ��
//PB11		ETH_MII_TX_EN / ETH_RMII_TX_EN		//PG11	ETH_MII_TX_EN / ETH_RMII_TX_EN	(PB11)		
//������ʱ��
//PC3			ETH_MII_TX_CLK
//���ýӿ�ʱ��
//PC1			ETH_MDC
//���ýӿ�IO
//PA2			ETH_MDIO
	//PB5		ETH_PPS_OUT											û��
	//PG8		ETH_PPS_OUT											û��		
	
	
****************************************************************************/
void ETH_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_MII);

  //SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII);

  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |  GPIO_Pin_7;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_ETH);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_10| GPIO_Pin_11 |  GPIO_Pin_12;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_ETH);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource2, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource3, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF_ETH);
                          
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_14;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_ETH);
}

/****************************************************************************
* ��	�ƣ�void ETH_NVIC_Config(void)
* ��	�ܣ���̫���жϳ�ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵	����EtherNet					 
****************************************************************************/
void ETH_NVIC_Config(void)
{
  NVIC_InitTypeDef   NVIC_InitStructure;
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	
  NVIC_InitStructure.NVIC_IRQChannel = ETH_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);    
}

/****************************************************************************
* ��	�ƣ�void ETH_MACDMA_Config(u32* Flag)
* ��	�ܣ���̫��MACý����������DMA��ʼ��
* ��ڲ�����u32* EthFlag	��ʼ���ɹ���־	
* ���ڲ�������
* ˵	����EtherNet					 
****************************************************************************/
void ETH_MACDMA_Config(u32* Flag)
{
  ETH_InitTypeDef ETH_InitStructure;

  RCC_AHB1PeriphClockCmd(	RCC_AHB1Periph_ETH_MAC | 
													RCC_AHB1Periph_ETH_MAC_Tx |
													RCC_AHB1Periph_ETH_MAC_Rx, ENABLE);                                             

  RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_ETH_MAC, ENABLE);
  RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_ETH_MAC, DISABLE);					//����̫��AHB���߽��и�λ

  ETH->DMABMR |= ETH_DMABMR_SR;																			//��������MAC�ڲ��Ĵ���

  while((ETH->DMABMR & ETH_DMABMR_SR) != RESET)
      SysTimeDly(100);										//�ȴ����ö������

  ETH_StructInit(&ETH_InitStructure);																//��ȡ����ETH_InitStructure���ò���

  /*------------------------   �޸�MAC����   -----------------------------------*/
  ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;
  //ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Disable; 
  //ETH_InitStructure.ETH_Speed = ETH_Speed_10M;
  //ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;   

  ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;
  ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;
  ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
  ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;
  ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;
  ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
  ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;
  ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;
#ifdef CHECKSUM_BY_HARDWARE
  ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable;
#endif

  /*------------------------   �޸�DMA����   -----------------------------------*/  
  
  /* When we use the Checksum offload feature, we need to enable the Store and Forward mode: 
  the store and forward guarantee that a whole frame is stored in the FIFO, so the MAC can insert/verify the checksum, 
  if the checksum is OK the DMA can handle the frame otherwise the frame is dropped */
  ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable; 
  ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;         
  ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;     
 
  ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;       
  ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;   
  ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;
  ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;      
  ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;                
  ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;          
  ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;
  ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;

  *Flag = ETH_Init(&ETH_InitStructure, DP83848_PHY_ADDRESS);		//��ά����ʼ��

  ETH_DMAITConfig(ETH_DMA_IT_NIS | ETH_DMA_IT_R, ENABLE);				//����DMA�����ж�(��Ҫ���������ж�)
}

/****************************************************************************
* ��	�ƣ�u32 Eth_Link_PHYITConfig(u16 PHYAddress)
* ��	�ܣ���̫�������ӿڵ�оƬPHY����
* ��ڲ�����uu16 PHYAddress		PHY��ַ
* ���ڲ�������
* ˵	����EtherNet					 
****************************************************************************/
u32 Eth_Link_PHYITConfig(u16 PHYAddress)
{
  u32 tmpreg = 0;

  tmpreg = ETH_ReadPHYRegister(PHYAddress, PHY_MICR);						//��ȡMICR�Ĵ���

  tmpreg |= (u32)PHY_MICR_INT_EN | PHY_MICR_INT_OE;
  if(ETH_WritePHYRegister(PHYAddress, PHY_MICR, tmpreg) == 0)		//����INT��������ж��ź�
    return ETH_ERROR;

  tmpreg = ETH_ReadPHYRegister(PHYAddress, PHY_MISR);						//��ȡMISR�Ĵ���

  tmpreg |= (uint32_t)PHY_MISR_LINK_INT_EN;
  if(ETH_WritePHYRegister(PHYAddress, PHY_MISR, tmpreg) == 0)		//�����жϺ�����״̬
    return ETH_ERROR;
	
  return ETH_SUCCESS;   
}

/****************************************************************************
* ��	�ƣ�void Eth_Link_EXTIConfig(void)
* ��	�ܣ�������̫������״̬���ⲿEXTI֪ͨ
* ��ڲ�������
* ���ڲ�������
* ˵	����EtherNet					 
****************************************************************************/
void Eth_Link_EXTIConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the INT (PB14) Clock */
  RCC_AHB1PeriphClockCmd(ETH_LINK_GPIO_CLK, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  /* Configure INT pin as input */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = ETH_LINK_PIN;
  GPIO_Init(ETH_LINK_GPIO_PORT, &GPIO_InitStructure);

  /* Connect EXTI Line to INT Pin */
  SYSCFG_EXTILineConfig(ETH_LINK_EXTI_PORT_SOURCE, ETH_LINK_EXTI_PIN_SOURCE);

  /* Configure EXTI line */
  EXTI_InitStructure.EXTI_Line = ETH_LINK_EXTI_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set the EXTI interrupt to the highest priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  
  NVIC_InitStructure.NVIC_IRQChannel = ETH_LINK_EXTI_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/****************************************************************************
* ��	�ƣ�void EXTI0_IRQHandler(void)
* ��	�ܣ����߰β��ж�
* ��ڲ�������
* ���ڲ�������
* ˵	�������߰γ��������������ж�			 
****************************************************************************/
void ETHPIN_IRQHandler(void)
{
	if(EXTI_GetITStatus(ETH_LINK_EXTI_LINE) != RESET)			//�ж��γ�
  {
		if(((ETH_ReadPHYRegister(DP83848_PHY_ADDRESS, PHY_MISR)) & PHY_LINK_STATUS) != 0)	//����Ƿ��Ѿ��������������ж�
			EthLinkStatus = ~EthLinkStatus;										//�����״̬��־��ת
		
    EXTI_ClearITPendingBit(ETH_LINK_EXTI_LINE);					//����жϱ�־
  }
}

extern SYS_EVENT *s_xSemaphore;
void ETH_IRQHandler(void)
{
	if(ETH_GetDMAFlagStatus(ETH_DMA_FLAG_R) == SET) 		//���յ�����
    SysSemPost(s_xSemaphore);						//�����ź���

    ETH_DMAClearITPendingBit(ETH_DMA_IT_R);		//������ձ�־
    ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);	//��������жϱ�־
}

/****************************************************************************
* ��	�ƣ�u8 ETH_Config(void)
* ��	�ܣ���̫����ʼ��
* ��ڲ�������
* ���ڲ�������ʼ���Ƿ�ɹ�
* ˵	����EtherNet					 
****************************************************************************/
u8 ETH_Config(void)
{
	u32 EthInitFlag = 0;								//��ʼ���ɹ���־	1�ɹ�	0ʧ��
	
  ETH_GPIO_Config();									//��̫��EtherNet���ų�ʼ��
	ETH_NVIC_Config();									//��̫��EtherNet�жϳ�ʼ��	

  while(EthInitFlag == ETH_ERROR)			//�����̫����ʼ������
    ETH_MACDMA_Config(&EthInitFlag);												//������̫��EtherNet��MAC��DMA

  Eth_Link_PHYITConfig(DP83848_PHY_ADDRESS);	//����PHY�ڸı�����״̬ʱ����һ���ж�

  Eth_Link_EXTIConfig();											//������̫������״̬���ⲿEXTI֪ͨ
	
	//�ж�ע��
	ETH_IRQ.EXTI_No = 0;
	ETH_IRQ.EXTI_Flag = 1;
	ETH_IRQ.IRQHandler = ETHPIN_IRQHandler;
	if(EXTIIRQHandlerInstall(&ETH_IRQ) != HVL_NO_ERR)					//�ж�ע��
			return 0;		//ʧ��
	return 1;
}
