#include "includes.h"
#include "driver.h"
#include "usart6.h"
#include "kfifo.h"
#include "usart_timer.h"

/********************************************ƽ̨���ò���********************************************/
#define USART6_TXD_Pin				GPIO_Pin_6
#define USART6_TXD_Port				GPIOC
#define USART6_TXD_Pin_RCC		RCC_AHB1Periph_GPIOC

#define USART6_RXD_Pin				GPIO_Pin_7
#define USART6_RXD_Port				GPIOC
#define USART6_RXD_Pin_RCC		RCC_AHB1Periph_GPIOC

#define TX_POLL 0
#define TX_IRQ  1
#define TX_DMA  2

#define TX_MODE	 TX_IRQ
#define RX_MODE RX_LENGTH_MODE


#define MAX_RX_BUFF_SIZE 512  //���붨��Ϊ2�Ĵη�
#define MAX_TX_BUFF_SIZE 512  //���붨��Ϊ2�Ĵη�

#define SYS_TICK_MS 10

#define USART6_PUTCHAR(ch) ( USART6->DR = (ch & (uint16_t)0x01FF) )
#define USART6_GETCHAR()   (uint16_t)(USART6->DR & (uint16_t)0x01FF);
/****************************************************************************************************/

static s8 usart6_open(u32 lParam);
static s8 usart6_close(void);

static s32 usart6_read_packet(u8* buff, u32 buff_len);
static s32 usart6_read_length(u8* buff, u32 want_len);
static s32 usart6_read_keychar(u8* buff, u32 buff_len);
static s32 usart6_read(u8* buff, u32 buff_len);
static s32 usart6_write(u8* buff, u32 buff_len);
static s32 usart6_ioctl(u8 cmd, u32 arg);
static u8  usart6_hard_init(USART_InitTypeDef* desc);
static u8  usart6_hard_deinit(void);
static u8  usart6_dma_send(u8* buff, u32 buff_len);	
///////////////////////////////////////////////////
static u8 USART6_pDisc[] = "usart6";
static struct kfifo tx_fifo;
static struct kfifo rx_fifo;
static u8  gInit = 0;


/////////////�����ò���/////////////////
static s8	 key_char 		  = '0';
static u8	 rx_mode 			  = RX_PACKET_MODE;
static u32 rx_timeout_ticks  = 100;  //100tick = 1000ms
static u32 tx_timeout_ticks  = 100;  //100tick = 1000ms
static u32 rx_length 		  = 0;	//����ģʽ����Ҫ���յ����ݳ���
static u32 N_char_timeout = 10; //10���ַ�����ʱ����Ϊ��ʱʱ��
static u32 baudRate			  = 9600;
///////////////////////////////////////

static SYS_EVENT* rx_event = NULL;	//����ͬ���¼�
static SYS_EVENT* tx_event = NULL;	//����ͬ���¼�
static u32 TimeVal = 0;	//�ַ���ʱ���������
static u8 rx_buff[MAX_RX_BUFF_SIZE];	//���ջ�����
static u8 tx_buff[MAX_TX_BUFF_SIZE];	//���ͻ�����

static u8 rx_char;  //�����ж�����ʱ��Ž��յ�������
static u8 tx_char 	= 0;
static u8 rx_char_prev = 0x30;
static volatile u8 dmx_send_complete = 1;
static DEV_REG usart6_cfg = 			//�豸ע����Ϣ��						
{
	CHAR_USART6,				//�豸ID��
	0,  								//�Ƿ���			0:���ɹ���ʹ��, 1:�ɹ���ʹ��
	0, 									//���ڹ����豸���򿪴���
	1,									//��������
	1,									//���д����
	USART6_pDisc,				//��������			�Զ���
	20120001,						//�����汾			�Զ���
	(u32*)usart6_open,	//�豸�򿪺���ָ��
	(u32*)usart6_close, //�豸�رպ���ָ��
	(u32*)usart6_read,	//�ַ�������
	(u32*)usart6_write,	//�ַ�д����
	(u32*)usart6_ioctl	//���ƺ���
};
///////////////////////////////////////////////////	
static s8 usart6_open(u32 lParam)
{
	 if(gInit)
	 {
			return -1;
	 }		 
	 usart6_hard_init((USART_InitTypeDef*)lParam);
	 gInit = 1;
	 return 0;
}
static s8 usart6_close()
{
		usart6_hard_deinit();
		gInit = 0;
		return 0;
}
#if(RX_MODE==RX_PACKET_MODE)
//�ַ���ʱ����ģʽ��ȡ����
static s32 usart6_read_packet(u8* buff, u32 buff_len)
{
	 u8 BoxErr = 0;				//������մ����־
	 s32 ret = 0;
#if SYS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
	 SYS_CPU_SR	cpu_sr = 0;
#endif
	 if(kfifo_len(&rx_fifo) > 0)
	 {
				return kfifo_get(&rx_fifo,buff,buff_len);
	 }
	 SysMboxPend(rx_event, rx_timeout_ticks, &BoxErr);		//������չ���
	
	 if(BoxErr != 0)
	 {
			return 0;
	 }

	SYS_ENTER_CRITICAL();
	ret = kfifo_get(&rx_fifo,buff,buff_len);
	SYS_EXIT_CRITICAL();
	 
	return ret;
}
#endif

/*
������ȡ����
����ڴ��������жϵ�ʱ��˺�����δ�����ã���ôrx_length��û�а취�õ���ʼ������Ĭ��Ϊ0
*/
#if(RX_MODE==RX_LENGTH_MODE)
static s32 usart6_read_length(u8* buff, u32 want_len)
{
	 u8 BoxErr = 0;				//������մ����־
	 s32 len   = 0;
#if SYS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
		SYS_CPU_SR	cpu_sr = 0;
#endif
	
	 SYS_ENTER_CRITICAL();
	 
	 len = kfifo_len(&rx_fifo);
	 if(len >= want_len)
	 {		  
			len = kfifo_get(&rx_fifo,buff,want_len);
		  SYS_EXIT_CRITICAL();
			return len;
	 }
	 
	 rx_length = want_len;
	 SYS_EXIT_CRITICAL();
	 

	 SysMboxPend(rx_event, rx_timeout_ticks, &BoxErr);		//������չ���

	 if(BoxErr != 0)
	 {
			return 0;
	 }

	 SYS_ENTER_CRITICAL();
	 len = kfifo_get(&rx_fifo,buff,want_len);
	 SYS_EXIT_CRITICAL();
	 
	 return len;
}
#endif

#if(RX_MODE==RX_KEY_MODE)
static s32 usart6_read_keychar(u8* buff, u32 buff_len)
{
	 u8 BoxErr = 0;				//������մ����־
	 SysMboxPend(rx_event, rx_timeout_ticks, &BoxErr);		//������չ���

	 if(BoxErr != 0)
	 {
			return 0;
	 }
	 return kfifo_get(&rx_fifo,buff,buff_len);
		
}
#endif

static s32 usart6_read(u8* buff, u32 buff_len)
{
		s32 ret  = 0;
#if 0
		 switch(rx_mode)
		 {
				case 	RX_PACKET_MODE:
						ret = usart6_read_packet(buff, buff_len);
						break;
				case	RX_LENGTH_MODE:
						ret = usart6_read_length(buff, buff_len);
						break;
				case	RX_KEY_MODE:
						ret = usart6_read_keychar(buff, buff_len);
						break;
				default:
						ret = usart6_read_packet(buff, buff_len);
						break;
		 }
 #else
		#if(RX_MODE==RX_PACKET_MODE)
				ret = usart6_read_packet(buff, buff_len);
		 #elif(RX_MODE==RX_LENGTH_MODE)
				ret = usart6_read_length(buff, buff_len);
		 #elif(RX_MODE==RX_KEY_MODE)
				ret = usart6_read_keychar(buff, buff_len);	
		 #endif
 #endif

	return ret;	 
}
static s32 usart6_irq_send(u8* buff, u32 buff_len)
{
		u8 BoxErr  = 0;				//������մ����־
		u8 tx_data = 0;

		if(buff_len == 0)
		{
				return 0;
		}
		kfifo_put(&tx_fifo,buff,buff_len);
		//�п����ٴε��ô˺�����ʱ�򣬷���fifo����δ������ϵ�����
		while(USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET); //����������ô˺������ȴ��������ݼĴ���Ϊ��,Ҳ���ǵȴ���һ�η��͵������Ѿ��������
		//while(USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET); //����������ô˺������ȴ��������ݼĴ���Ϊ��,Ҳ���ǵȴ���һ�η��͵������Ѿ��������

		if( 0 == kfifo_getc( &tx_fifo,&tx_data ) )
		{

			USART6_PUTCHAR(tx_data);
			USART_ITConfig(USART6, USART_IT_TXE, ENABLE);						//���������ж�
		}
		SysMboxPend(tx_event,tx_timeout_ticks,&BoxErr);

		if(BoxErr != 0)
		{
			return 0;
		}
		return buff_len;			
}
static s32 usart6_write(u8* buff, u32 buff_len)
{
	#if(TX_MODE == TX_IRQ)
		return usart6_irq_send(buff,buff_len);
	#elif (TX_MODE == TX_DMA)
		return usart6_dma_send(buff,buff_len);
	#elif (TX_MODE == TX_POLL)
	{
		int i;
		for(i = 0; i < buff_len; i++)
		{
			USART6_PUTCHAR(buff[i]);
			//while(USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET); 
			while(USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET);//������������,����С�������ֱ�������
		}
		return buff_len;
	}
	#endif
}

static void usart6_flush(u8 type)
{
		if(type == 0)
		{
				kfifo_reset(&rx_fifo);
		}
		else if(type == 1)
		{
				kfifo_reset(&tx_fifo);
		}
		else
		{
			kfifo_reset(&rx_fifo);
			kfifo_reset(&tx_fifo);
		}
}
static void usart6_update_timeout(u32 baud,u32 nchar)
{
		if(nchar == 0) nchar = 5;
		TimeVal = ((1000000 / (baud / 10)) * nchar) / 20;		//���ճ�ʱ��	��/������/10*2����/20us TIM3�ļ���Ƶ��
}
static s32 usart6_ioctl(u8 cmd, u32 arg)
{
	s32 val = 0;
	switch(cmd)
	{
		case CMD_SET_RX_TIMEOUT:
			rx_timeout_ticks =(arg/SYS_TICK_MS);
			break;
		case CMD_GET_RX_TIMEOUT:
			*(u32*)(arg) = rx_timeout_ticks * SYS_TICK_MS;
			break;
		case CMD_SET_RX_MODE:
			{
				
				if(arg != rx_mode)
				{
						usart6_flush(0);
						rx_mode = arg;
				}
			}
			
			break;
		case CMD_GET_RX_MODE:
			*(u8*)(arg) = rx_mode;
			break;
		case CMD_SET_KEY_CHAR:
			key_char = arg;
			break;
		case CMD_GET_KEY_CHAR:
			*(u8*)(arg) = key_char;
			break;
		case CMD_SET_N_CHAR_TIMEOUT:
			N_char_timeout = arg;
			usart6_update_timeout(baudRate,N_char_timeout);
			break;
		case CMD_GET_N_CHAR_TIMEOUT:
			*(u32*)(arg) = N_char_timeout;
			break;
		case CMD_GET_INPUT_BUF_SIZE:
			*(u32*)(arg) = kfifo_len(&rx_fifo);
			break;
		case CMD_FLUSH_INPUT:
			usart6_flush(0);
			break;
		case CMD_FLUSH_OUTPUT:
			usart6_flush(1);
			break;
		default:
			val = -1;
			break;
	}
	return val;
}

/*!
	 \brief ����ͨ�Ų�����ʼ��
*/
static void usart6_commu_param_init(USART_InitTypeDef* desc)
{		
	USART_InitTypeDef UART_InitStructure;											//���ڽṹ

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);		//���ڷ���ʱ��

	UART_InitStructure.USART_BaudRate            = desc->USART_BaudRate;											//������
	UART_InitStructure.USART_WordLength          = desc->USART_WordLength;								//����λ8bit
	UART_InitStructure.USART_StopBits            = desc->USART_StopBits;									//ֹͣλ����
	UART_InitStructure.USART_Parity              = desc->USART_Parity ;									//��������żЧ��
	UART_InitStructure.USART_HardwareFlowControl = desc->USART_HardwareFlowControl;		//RTS��CTSʹ��(None��ʹ��)
	UART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;			//���ͺͽ���ʹ��
	USART_Init(USART6, &UART_InitStructure);																					//��ʼ������
}
/*!
	 \brief ����GPIO���ų�ʼ��
*/
void usart6_gpio_init(void)																			//�������ų�ʼ��
{
	GPIO_InitTypeDef GPIO_InitStructure;		//�������Žṹ
		
	//�������ŷ���ʱ��	
	RCC_AHB1PeriphClockCmd(USART6_TXD_Pin_RCC, ENABLE);												//��TXDʱ��
	RCC_AHB1PeriphClockCmd(USART6_RXD_Pin_RCC, ENABLE);												//��RXDʱ��
	
	//TXD����ӳ�䵽USART6
	if(USART6_TXD_Pin == GPIO_Pin_6)
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);		//Connect PXx to PC6
	else
		GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_USART6);		//Connect PXx to PG14
	
	//RXD����ӳ�䵽USART6
  if(USART6_RXD_Pin == GPIO_Pin_7)
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);	//Connect PXx to PC7
	else
		GPIO_PinAFConfig(GPIOG, GPIO_PinSource9, GPIO_AF_USART6);		//Connect PXx to PG9

	//���ô��� Tx (PC.06) Ϊ�����������
  GPIO_InitStructure.GPIO_Pin = USART6_TXD_Pin;			//���ڷ�������
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			//����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//ת��Ƶ��
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//�������
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//����������
  if(USART6_TXD_Pin == GPIO_Pin_6)
		GPIO_Init(GPIOC, &GPIO_InitStructure);						//��ʼ������
	else
		GPIO_Init(GPIOG, &GPIO_InitStructure);						//��ʼ������
    
	// ���ô��� Rx (PC.07) Ϊ��������
  GPIO_InitStructure.GPIO_Pin = USART6_RXD_Pin;				//���ڽ�������
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;				//����ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//ת��Ƶ��
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;		//����������
	if(USART6_RXD_Pin == GPIO_Pin_7)
		GPIO_Init(GPIOC, &GPIO_InitStructure);						//��ʼ������
	else
		GPIO_Init(GPIOG, &GPIO_InitStructure);						//��ʼ������
}
/*!
	 \brief �����жϳ�ʼ��
*/
void usart6_nvic_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 											//�жϿ���������

	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;					//�����ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//�����ȼ�����
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;				//�������ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;						//�򿪴����ж�
	NVIC_Init(&NVIC_InitStructure);														//��ʼ���ж�������
}
#if(TX_MODE==TX_DMA)
u8 usart6_dmatxd_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 		//�жϿ���������
	DMA_InitTypeDef DMA_InitStructure;			//DMA�ṹ

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);							//ʹ��DMA1ʱ��
	DMA_DeInit(DMA2_Stream6);
	
	//DMA�ж����� DMA_Tx ����
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream6_IRQn;						//����DMA�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;					//�����ȼ�����
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;								//�������ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;										//���ж�
	NVIC_Init(&NVIC_InitStructure); 

	//DMA����
	DMA_DeInit(DMA2_Stream6);  		   																									//��λDMA1_Channel4ͨ��ΪĬ��ֵ
  DMA_InitStructure.DMA_Channel = DMA_Channel_5; 
	DMA_InitStructure.DMA_PeripheralBaseAddr = USART6_BASE + 4;												//DMAͨ���������ַ
	if( (tx_buff == 0)	|| (	MAX_TX_BUFF_SIZE < 4))																							//û�����÷��ͻ�����
		return 0;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)tx_buff;				//DMAͨ���洢������ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;														//DMAĿ�ĵ�	(DMA_DIR_PeripheralSRCԴ������)  ˫����
																															//��ʼ��DMAʧ��
	DMA_InitStructure.DMA_BufferSize = MAX_TX_BUFF_SIZE;									//���ͻ�������С
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;									//��ǰ����Ĵ�������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;														//��ǰ�洢�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;						//�������ݿ��Ϊ�ֽ�(8λ)
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;										//�洢�����ݿ��Ϊ�ֽ�(8λ)
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;																			//��������ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;														//DMAͨ�����ȼ��ǳ���
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream6, &DMA_InitStructure);																				//�����������ó�ʼ��DMA
	DMA_ITConfig(DMA2_Stream6, DMA_IT_TC, ENABLE);    																//����DMAͨ���ж�
	return 1;
}
#endif

static void usart6_event_init(void)
{
	if(rx_event == (SYS_EVENT*)0) 					//û�д�����������ź���
		rx_event = SysMboxCreate((void *)0);	//������������ź���

	#if(TX_MODE==TX_IRQ)
		if(tx_event == (SYS_EVENT*)0) 					//û�д�����������ź���
			tx_event = SysMboxCreate((void *)0);	//������������ź���
	#endif
}

static void usart6_fifo_init(void)
{
		kfifo_init(&rx_fifo,rx_buff,MAX_RX_BUFF_SIZE);
	
		#if(TX_MODE==TX_IRQ) //�жϷ���ģʽ�²�ʹ��fifo
			kfifo_init(&tx_fifo,tx_buff,MAX_TX_BUFF_SIZE);
		#elif(TX_MODE==TX_DMA)

		#endif
			
}

static int check_data_lost(u8 ch)
{
	 if(ch == 0x30)
	 {
				if(rx_char_prev != 0x39)
				{
					  rx_char_prev = ch;
						return 1;
				}
	 }
	 else
	 {
			  if( (ch - rx_char_prev) != 1)
				{
					  rx_char_prev = ch;
					  return 1;
				}
	 }
	 rx_char_prev = ch;
	 return 0;
}


#if 0
void USART6_IRQHandler(void)
{
	u16 state;
	
	SysIntEnter();																					//�������ȼ����ȱ���					
	
	//�����жϴ���
	
	state = USART6->SR;
	if((state&0x20) != 0)
	{
		//���ݽ��ն���
		rx_char = USART6_GETCHAR();
		#if 0
			if( check_data(rx_char) != 0)
			{
					rx_char = rx_char;
			}
		#endif
		kfifo_putc( &rx_fifo,  rx_char);

		#if(RX_MODE==RX_PACKET_MODE)		//�����ݼ����ƵĻ�����λ��ʱ���������ȴ���һ�����յ��ַ�
		{
					TIM3->CCR1 = TIM3->CNT + TimeVal;										//�������ӳ�ʱ��

					//���յ�һ������ʱ��������ʱ�жϣ�������ɸ��жϻ��ж�ʱ�жϳ���ر�
					if((TIM3->DIER & TIM_FLAG_CC1) == 0)   							//����Ƚ�ͨ��ʹ��Ϊ�ر�
					{	 			
						TIM3->SR = (uint16_t)~TIM_FLAG_CC1;								//��ձȽ�ͨ��1���жϱ�־ 
						TIM3->DIER |= TIM_IT_CC1;													//�Ƚ�ͨ���򿪵ȴ���һ���ж�
					}
		}
		#elif(RX_MODE==RX_LENGTH_MODE)
		 {
			  int len = kfifo_len(&rx_fifo);

				if( len  == rx_length )
				{
					  rx_length = 0;
						SysMboxPost(rx_event, (void *)(&rx_char));	//���ͼ���ȴ��������ź�  
				}
				else if(len >= MAX_RX_BUFF_SIZE)
				{

					  SysMboxPost(rx_event, (void *)(&rx_char));	//���ͼ���ȴ��������ź�  
				}
		 }
		 #elif(RX_MODE==RX_KEY_MODE)
		 {
				if(rx_char == key_char)
				{
					SysMboxPost(rx_event, (void *)&rx_char);	//���ͼ���ȴ��������ź�
					
				}
		 }
		 #endif

	}
	
	
	else
	{
		state = USART6->SR;
		if( (state&0x08) != 0 )
		{
				USART6_GETCHAR();																//����������־��ֻ���ö����ݵķ�ʽ���������־
		}
	}
	//�����жϴ���

	state = USART6->SR;
	if(( state&0x80 ) != 0 )
	{		

			if(-1 == kfifo_getc(&tx_fifo,&ch)) //fifo��û��������
			{	
				//��ֹ�����ж�
				
				USART_ITConfig(USART6, USART_IT_TXE, DISABLE);
				if(tx_event)
				{
					  SysMboxPost(tx_event, (void *)&tx_char);	//���ͼ���ȴ��������ź�
				}
			}
			else
			{
				//�������ݣ���������fifo�е�����
				USART6_PUTCHAR(ch);
			}		
	
	}

	
	SysIntExit();   																						//�������ȼ����ȱ������
}
#else
void USART6_IRQHandler(void)
{

	SysIntEnter();																					//�������ȼ����ȱ���					
	
	//�����жϴ���

	if(USART_GetITStatus(USART6, USART_IT_RXNE) == SET)
	{
		//���ݽ��ն���
		rx_char = USART6_GETCHAR();
#if 0
		if( check_data_lost(rx_char) != 0)
		{
			  rx_char = rx_char;
		}
#endif
		kfifo_putc( &rx_fifo,  rx_char);

		#if(RX_MODE==RX_PACKET_MODE)		//�����ݼ����ƵĻ�����λ��ʱ���������ȴ���һ�����յ��ַ�
		{
					TIM3->CCR2 = TIM3->CNT + TimeVal;										//�������ӳ�ʱ��

					//���յ�һ������ʱ��������ʱ�жϣ�������ɸ��жϻ��ж�ʱ�жϳ���ر�
					if((TIM3->DIER & TIM_FLAG_CC2) == 0)   							//����Ƚ�ͨ��ʹ��Ϊ�ر�
					{	 			
						TIM3->SR = (uint16_t)~TIM_FLAG_CC2;								//��ձȽ�ͨ��1���жϱ�־ 
						TIM3->DIER |= TIM_IT_CC2;													//�Ƚ�ͨ���򿪵ȴ���һ���ж�
					}
		}
		#elif(RX_MODE==RX_LENGTH_MODE)
		 {
			  int len = kfifo_len(&rx_fifo);

				if( len  == rx_length )
				{
					  rx_length = 0;
						SysMboxPost(rx_event, (void *)(&rx_char));	//���ͼ���ȴ��������ź�  
				}
				else if(len >= MAX_RX_BUFF_SIZE)
				{

					  SysMboxPost(rx_event, (void *)(&rx_char));	//���ͼ���ȴ��������ź�  
				}
		 }
		 #elif(RX_MODE==RX_KEY_MODE)
		 {
				if(rx_char == key_char)
				{
					SysMboxPost(rx_event, (void *)&rx_char);	//���ͼ���ȴ��������ź�
					
				}
		 }
		 #endif

	}
	//��������λ�Ĵ��������������ݣ�׼����������DR�Ĵ����зŵ�ʱ�򣬷���RNXE��־û�б���գ��ͻᴥ�����ж�
	
	else if(USART_GetFlagStatus(USART6, USART_IT_ORE) == SET)		//����Ƿ��н������
  {
			USART6_GETCHAR();																//����������־��ֻ���ö����ݵķ�ʽ���������־
  }
	
	//�����жϴ���

	if(USART_GetITStatus(USART6, USART_IT_TXE) == SET)					//�����ж�
	{		

			if(-1 == kfifo_getc(&tx_fifo,&tx_char)) //fifo��û��������
			{	
				//��ֹ�����ж�
				
				USART_ITConfig(USART6, USART_IT_TXE, DISABLE);
				if(tx_event)
				{
					  SysMboxPost(tx_event, (void *)&tx_char);	//���ͼ���ȴ��������ź�
				}
			}
			else
			{
					//�������ݣ���������fifo�е�����
					USART6_PUTCHAR(tx_char);
			}		
	
	}

	
	SysIntExit();   																						//�������ȼ����ȱ������
}
#endif

static u8 usart6_dma_send(u8* buff, u32 buff_len)	
{
	u8 BoxErr = 0;
	s32 timeout = tx_timeout_ticks;
	if(buff_len > MAX_TX_BUFF_SIZE) return 0;
	if(buff_len == 0) return 0;
	
	while( (dmx_send_complete == 0) && timeout > 0)
	{
			SysTimeDly(1);
			--timeout;
	}
	memcpy(tx_buff,buff,buff_len);
  DMA2_Stream6->NDTR = buff_len;					   						//�����ֽ�����
	dmx_send_complete = 0;
	USART_DMACmd(USART6, USART_DMAReq_Tx, ENABLE); 	//����
	DMA_Cmd(DMA2_Stream6, ENABLE);									//ʼ��DMAͨ��
	
	SysMboxPend(tx_event,tx_timeout_ticks,&BoxErr);
	
	if(BoxErr != 0)
	{
		return 0;
	}
	if(dmx_send_complete != 1)
	{
		
	}
	return buff_len;	

}

#if(TX_MODE==TX_DMA)
void DMA2_Stream6_IRQHandler(void)
{
	SysIntEnter();	
  
	if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7))    //DMA��������ж�
	{
				
		USART_DMACmd(USART6, USART_DMAReq_Tx, DISABLE);     //�ر�DMA����
		DMA_Cmd(DMA2_Stream7, DISABLE);	                    //�ر�DMAͨ�� 

		dmx_send_complete = 1;
		if(tx_event != 0)											//�����˷�������ź���֪ͨ
			SysMboxPost(tx_event, (void *)&tx_char);	//���ͼ���ȴ��������ź�
		
	}
	
	DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_FEIF7 | DMA_FLAG_DMEIF7 | DMA_FLAG_TEIF7 | DMA_FLAG_HTIF7 | DMA_FLAG_TCIF7);  //Modify
	
	SysIntExit();
}
#endif

void usart6_timer_isr(void)
{
	#if (RX_MODE == RX_PACKET_MODE)
		if(rx_event != 0)
		{
			 SysMboxPost(rx_event, (void *)&rx_char);	//���ͼ���ȴ��������ź�
		}
	#endif
}

s8 usart6_init(void)
{
	if(DeviceInstall(&usart6_cfg) != HVL_NO_ERR)			//�豸ע��
		return -1;
	return 0;
}																								//���ö�ʱ����������ճ�ʱ


static u8 usart6_hard_deinit()
{
	USART_Cmd(USART6, DISABLE); 
	USART_ClearITPendingBit(USART6,USART_IT_RXNE);//����ձ�־
	USART_ITConfig(USART6,USART_IT_RXNE, DISABLE);	//���������ж�
	RCC_AHB1PeriphClockCmd(USART6_TXD_Pin_RCC, DISABLE);						//�ر�TXDʱ��
	RCC_AHB1PeriphClockCmd(USART6_RXD_Pin_RCC, DISABLE);						//�ر�RXDʱ��
	return 0;
}
static u8 usart6_hard_init(USART_InitTypeDef* desc)
{
#if (RX_MODE == RX_PACKET_MODE)
	usart_timer_init();																									//���ö�ʱ����������ճ�ʱ
#endif
	baudRate = desc->USART_BaudRate;
  usart6_update_timeout(baudRate,N_char_timeout);
	USART_Cmd(USART6, DISABLE);  																		//ʹ��ʧ�ܴ�������
	usart6_commu_param_init(desc);												//���ڳ�ʼ��
	usart6_gpio_init();																							//�������ų�ʼ��	
	
	usart6_event_init();
	usart6_fifo_init();
	
	usart6_nvic_init();	
	USART_ClearITPendingBit(USART6,USART_IT_RXNE);//����ձ�־
	USART_ClearITPendingBit(USART6,USART_IT_TXE);
	USART_ClearITPendingBit(USART6,USART_IT_TC);
	#if(TX_MODE==TX_IRQ)

	#elif(TX_MODE==TX_DMA)
		usart6_dmatxd_init();
	#endif
	
	USART_ITConfig(USART6,USART_IT_RXNE, ENABLE);	//���������ж�
	USART_Cmd(USART6, ENABLE);  									//ʹ��ʧ�ܴ�������
	
	return 0;
}
