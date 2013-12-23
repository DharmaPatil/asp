#include "includes.h"
#include "driver.h"
#include "usart1.h"
#include "kfifo.h"
#include "usart_timer.h"
#include "ccm_mem.h"

/********************************************ƽ̨���ò���********************************************/
#define USART1_TXD_Pin				GPIO_Pin_9								//PB6
#define USART1_TXD_Port				GPIOA
#define USART1_TXD_Pin_RCC		RCC_AHB1Periph_GPIOA

#define USART1_RXD_Pin				GPIO_Pin_10								//PB7
#define USART1_RXD_Port				GPIOA
#define USART1_RXD_Pin_RCC		RCC_AHB1Periph_GPIOA

#define TX_POLL 0
#define TX_IRQ  1
#define TX_DMA  2

#define USART1_TX_MODE	 TX_POLL
#define USART1_RX_MODE RX_LENGTH_MODE


#define MAX_RX_BUFF_SIZE 256  //���붨��Ϊ2�Ĵη�
#define MAX_TX_BUFF_SIZE 256  //���붨��Ϊ2�Ĵη�

#define SYS_TICK_MS 10

#define USART1_PUTCHAR(ch) ( USART1->DR = (ch & (uint16_t)0x01FF) );
#define USART1_GETCHAR()   (uint16_t)(USART1->DR & (uint16_t)0x01FF);
/****************************************************************************************************/

static s8 usart1_open(u32 lParam);
static s8 usart1_close(void);

#if(USART1_RX_MODE==RX_PACKET_MODE)
static s32 usart1_read_packet(u8* buff, u32 buff_len);
#endif

static s32 usart1_read_length(u8* buff, u32 want_len);

#if(USART1_RX_MODE==RX_KEY_MODE)
static s32 usart1_read_keychar(u8* buff, u32 buff_len);
#endif

static s32 usart1_read(u8* buff, u32 buff_len);
static s32 usart1_write(u8* buff, u32 buff_len);
static s32 usart1_ioctl(u8 cmd, u32 arg);
static u8  usart1_hard_init(USART_InitTypeDef* desc);
static u8  usart1_hard_deinit(void);

#if (USART1_TX_MODE == TX_DMA)
static u8  usart1_dma_send(u8* buff, u32 buff_len);	
#endif

///////////////////////////////////////////////////
static u8 USART1_pDisc[] = "usart1";
static struct kfifo tx_fifo;
static struct kfifo rx_fifo;
static u8  gInit = 0;


/////////////�����ò���/////////////////
static s8	 key_char 		  = '0';
static u8	 rx_mode 			  = RX_PACKET_MODE;
static u32 rx_timeout_ticks  = 100;  //100tick = 1000ms

#if(USART1_RX_MODE==RX_PACKET_MODE)
static u32 tx_timeout_ticks  = 100;  //100tick = 1000ms
static u32 baudRate			  = 9600;

#endif

static u32 rx_length 		  = 0;	//����ģʽ����Ҫ���յ����ݳ���
static u32 N_char_timeout = 10; //10���ַ�����ʱ����Ϊ��ʱʱ��

///////////////////////////////////////

static SYS_EVENT* rx_event = NULL;	//����ͬ���¼�
static SYS_EVENT* tx_event = NULL;	//����ͬ���¼�

#if(USART1_RX_MODE==RX_PACKET_MODE)
static u32 TimeVal = 0;	//�ַ���ʱ���������
#endif

static u8 *prx_buff = NULL; //[MAX_RX_BUFF_SIZE];	//���ջ�����

#if(USART1_TX_MODE==TX_DMA)
static u8 *ptx_buff = NULL; //[MAX_TX_BUFF_SIZE];	//���ͻ�����
static u8 tx_buff[MAX_TX_BUFF_SIZE];
#endif

static u8 rx_char;  //�����ж�����ʱ��Ž��յ�������
static u8 tx_char 	= 0;


static u8 volatile dmx_send_complete = 1;
static DEV_REG usart1_cfg = 			//�豸ע����Ϣ��						
{
	CHAR_USART1,				//�豸ID��
	0,  								//�Ƿ���			0:���ɹ���ʹ��, 1:�ɹ���ʹ��
	0, 									//���ڹ����豸���򿪴���
	1,									//��������
	1,									//���д����
	USART1_pDisc,				//��������			�Զ���
	20120001,						//�����汾			�Զ���
	(u32*)usart1_open,	//�豸�򿪺���ָ��
	(u32*)usart1_close, //�豸�رպ���ָ��
	(u32*)usart1_read,	//�ַ�������
	(u32*)usart1_write,	//�ַ�д����
	(u32*)usart1_ioctl	//���ƺ���
};
///////////////////////////////////////////////////	
static s8 usart1_open(u32 lParam)
{
	 if(gInit)
	 {
			return -1;
	 }		 
	 usart1_hard_init((USART_InitTypeDef*)lParam);
	 gInit = 1;
	 return 0;
}
static s8 usart1_close()
{
		usart1_hard_deinit();
		gInit = 0;
		return 0;
}
#if(USART1_RX_MODE==RX_PACKET_MODE)
//�ַ���ʱ����ģʽ��ȡ����
static s32 usart1_read_packet(u8* buff, u32 buff_len)
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
#if(USART1_RX_MODE==RX_LENGTH_MODE)
static s32 usart1_read_length(u8* buff, u32 want_len)
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

#if(USART1_RX_MODE==RX_KEY_MODE)
static s32 usart1_read_keychar(u8* buff, u32 buff_len)
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

static s32 usart1_read(u8* buff, u32 buff_len)
{
		s32 ret  = 0;
#if 0
		 switch(rx_mode)
		 {
				case 	RX_PACKET_MODE:
						ret = usart1_read_packet(buff, buff_len);
						break;
				case	RX_LENGTH_MODE:
						ret = usart1_read_length(buff, buff_len);
						break;
				case	RX_KEY_MODE:
						ret = usart1_read_keychar(buff, buff_len);
						break;
				default:
						ret = usart1_read_packet(buff, buff_len);
						break;
		 }
 #else
		#if(USART1_RX_MODE==RX_PACKET_MODE)
				ret = usart1_read_packet(buff, buff_len);
		 #elif(USART1_RX_MODE==RX_LENGTH_MODE)
				ret = usart1_read_length(buff, buff_len);
		 #elif(USART1_RX_MODE==RX_KEY_MODE)
				ret = usart1_read_keychar(buff, buff_len);	
		 #endif
 #endif

	return ret;	 
}


#if(USART1_TX_MODE == TX_IRQ)
static s32 usart1_irq_send(u8* buff, u32 buff_len)
{
		u8 BoxErr  = 0;				//������մ����־
		u8 tx_data = 0;

		if(buff_len == 0)
		{
				return 0;
		}
		kfifo_put(&tx_fifo,buff,buff_len);
		//�п����ٴε��ô˺�����ʱ�򣬷���fifo����δ������ϵ�����
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); //����������ô˺������ȴ��������ݼĴ���Ϊ��,Ҳ���ǵȴ���һ�η��͵������Ѿ��������
		//while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET); //����������ô˺������ȴ��������ݼĴ���Ϊ��,Ҳ���ǵȴ���һ�η��͵������Ѿ��������

		if( 0 == kfifo_getc( &tx_fifo,&tx_data ) )
		{

			USART1_PUTCHAR(tx_data);
			USART_ITConfig(USART1, USART_IT_TXE, ENABLE);						//���������ж�
		}
		SysMboxPend(tx_event,tx_timeout_ticks,&BoxErr);

		if(BoxErr != 0)
		{
			return 0;
		}
		return buff_len;			
}
#endif

static s32 usart1_write(u8* buff, u32 buff_len)
{
	#if(USART1_TX_MODE == TX_IRQ)
		return usart1_irq_send(buff,buff_len);
	#elif (USART1_TX_MODE == TX_DMA)
		return usart1_dma_send(buff,buff_len);
	#elif (USART1_TX_MODE == TX_POLL)
	{
		int i;
		for(i = 0; i < buff_len; i++)
		{
			USART1_PUTCHAR(buff[i]);
			#if 1
			while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET); 
			#else
			while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);//������������,����С�������ֱ�������
			
			#endif
		}

		return buff_len;
	}
	#endif

}

static void usart1_flush(u8 type)
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

#if(USART1_RX_MODE==RX_PACKET_MODE)
static void usart1_update_timeout(u32 baud,u32 nchar)
{
		if(nchar == 0) nchar = 5;
		TimeVal = ((1000000 / (baud / 10)) * nchar) / 20;		//���ճ�ʱ��	��/������/10*2����/20us TIM3�ļ���Ƶ��
}
#endif

static s32 usart1_ioctl(u8 cmd, u32 arg)
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
						usart1_flush(0);
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
#if(USART1_RX_MODE==RX_PACKET_MODE)		
			usart1_update_timeout(baudRate,N_char_timeout);
#endif
		break;
		case CMD_GET_N_CHAR_TIMEOUT:
			*(u32*)(arg) = N_char_timeout;
			break;
		case CMD_GET_INPUT_BUF_SIZE:
			*(u32*)(arg) = kfifo_len(&rx_fifo);
			break;
		case CMD_FLUSH_INPUT:
			usart1_flush(0);
			break;
		case CMD_FLUSH_OUTPUT:
			usart1_flush(1);
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
static void usart1_commu_param_init(USART_InitTypeDef* desc)
{		
	USART_InitTypeDef UART_InitStructure;											//���ڽṹ

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);		//���ڷ���ʱ��

	UART_InitStructure.USART_BaudRate            = desc->USART_BaudRate;											//������
	UART_InitStructure.USART_WordLength          = desc->USART_WordLength;								//����λ8bit
	UART_InitStructure.USART_StopBits            = desc->USART_StopBits;									//ֹͣλ����
	UART_InitStructure.USART_Parity              = desc->USART_Parity ;									//��������żЧ��
	UART_InitStructure.USART_HardwareFlowControl = desc->USART_HardwareFlowControl;		//RTS��CTSʹ��(None��ʹ��)
	UART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;			//���ͺͽ���ʹ��
	USART_Init(USART1, &UART_InitStructure);																					//��ʼ������
}
/*!
	 \brief ����GPIO���ų�ʼ��
*/
void usart1_gpio_init(void)																			//�������ų�ʼ��
{
	u8 GPIO_PinSourceX = 0;
	
	GPIO_InitTypeDef GPIO_InitStructure;													//�������Žṹ
		
	RCC_AHB1PeriphClockCmd(USART1_TXD_Pin_RCC, ENABLE);						//��TXDʱ��
	RCC_AHB1PeriphClockCmd(USART1_RXD_Pin_RCC, ENABLE);						//��RXDʱ��
	
	GPIO_PinSourceX = (USART1_TXD_Pin == GPIO_Pin_9) ? GPIO_PinSource9 : GPIO_PinSource6;				//Connect PXx to PA9 or PB6
	GPIO_PinAFConfig(USART1_TXD_Port,GPIO_PinSourceX,GPIO_AF_USART1);														//TXD����ӳ�䵽USART1
	GPIO_PinSourceX = (USART1_RXD_Pin == GPIO_Pin_10) ? GPIO_PinSource10 : GPIO_PinSource7;			//Connect PXx to PA10 or PB7
	GPIO_PinAFConfig(USART1_RXD_Port,GPIO_PinSourceX,GPIO_AF_USART1);														//TXD����ӳ�䵽USART1	
	
	//���ô��� Tx (PA.09) Ϊ�����������
  GPIO_InitStructure.GPIO_Pin = USART1_TXD_Pin;									//���ڷ�������
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;									//����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;							//ת��Ƶ��
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;								//�������
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;							//����������
	GPIO_Init(USART1_TXD_Port, &GPIO_InitStructure);							//��ʼ������
    
	// ���ô��� Rx (PA.10) Ϊ��������
  GPIO_InitStructure.GPIO_Pin = USART1_RXD_Pin;									//���ڽ�������
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;									//����ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;							//ת��Ƶ��
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;							//����������
	GPIO_Init(USART1_RXD_Port, &GPIO_InitStructure);							//��ʼ������
}
/*!
	 \brief �����жϳ�ʼ��
*/
void usart1_nvic_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 											//�жϿ���������

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;					//�����ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//�����ȼ�����
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;				//�������ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;						//�򿪴����ж�
	NVIC_Init(&NVIC_InitStructure);														//��ʼ���ж�������
}
#if(USART1_TX_MODE==TX_DMA)
u8 usart1_dmatxd_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 		//�жϿ���������
	DMA_InitTypeDef DMA_InitStructure;			//DMA�ṹ

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);							//ʹ��DMA1ʱ��
	DMA_DeInit(DMA2_Stream7);
	
	//DMA�ж����� DMA_Tx ����
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream7_IRQn;						//����DMA�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;					//�����ȼ�����
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;								//�������ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;										//���ж�
	NVIC_Init(&NVIC_InitStructure); 

	//DMA����
	DMA_DeInit(DMA2_Stream7);  		   																									//��λDMA1_Channel4ͨ��ΪĬ��ֵ
  DMA_InitStructure.DMA_Channel = DMA_Channel_4; 
	DMA_InitStructure.DMA_PeripheralBaseAddr = USART1_BASE + 4;												//DMAͨ���������ַ
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
	DMA_Init(DMA2_Stream7, &DMA_InitStructure);																				//�����������ó�ʼ��DMA
	DMA_ITConfig(DMA2_Stream7, DMA_IT_TC, ENABLE);    																//����DMAͨ���ж�
	return 1;
}
#endif

static void usart1_event_init(void)
{
	if(rx_event == (SYS_EVENT*)0) 					//û�д�����������ź���
		rx_event = SysMboxCreate((void *)0);	//������������ź���

	#if(USART1_TX_MODE==TX_IRQ)
		if(tx_event == (SYS_EVENT*)0) 					//û�д�����������ź���
			tx_event = SysMboxCreate((void *)0);	//������������ź���
	#endif
}

static void usart1_fifo_init(void)
{
	
	prx_buff = (unsigned char *)Ccm_Malloc(MAX_RX_BUFF_SIZE * sizeof(u8));
	kfifo_init(&rx_fifo,prx_buff,MAX_RX_BUFF_SIZE);

	#if(USART1_TX_MODE==TX_IRQ) //�жϷ���ģʽ�²�ʹ��fifo		
		ptx_buff = (unsigned char *)Ccm_Malloc(MAX_TX_BUFF_SIZE * sizeof(u8));
		kfifo_init(&tx_fifo,ptx_buff,MAX_TX_BUFF_SIZE);
	#elif(USART1_TX_MODE==TX_DMA)

	#endif
			
}

void USART1_IRQHandler(void)
{

	SysIntEnter();																					//�������ȼ����ȱ���					
	
	//�����жϴ���

	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		//���ݽ��ն���
		rx_char = USART1_GETCHAR();
#if 0
		if( check_data(rx_char) != 0)
		{
			  rx_char = rx_char;
		}
#endif
		kfifo_putc( &rx_fifo,  rx_char);

		#if(USART1_RX_MODE==RX_PACKET_MODE)		//�����ݼ����ƵĻ�����λ��ʱ���������ȴ���һ�����յ��ַ�
		{
					TIM3->CCR1 = TIM3->CNT + TimeVal;										//�������ӳ�ʱ��

					//���յ�һ������ʱ��������ʱ�жϣ�������ɸ��жϻ��ж�ʱ�жϳ���ر�
					if((TIM3->DIER & TIM_FLAG_CC1) == 0)   							//����Ƚ�ͨ��ʹ��Ϊ�ر�
					{	 			
						TIM3->SR = (uint16_t)~TIM_FLAG_CC1;								//��ձȽ�ͨ��1���жϱ�־ 
						TIM3->DIER |= TIM_IT_CC1;													//�Ƚ�ͨ���򿪵ȴ���һ���ж�
					}
		}
		#elif(USART1_RX_MODE==RX_LENGTH_MODE)
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
		 #elif(USART1_RX_MODE==RX_KEY_MODE)
		 {
				if(rx_char == key_char)
				{
					SysMboxPost(rx_event, (void *)&rx_char);	//���ͼ���ȴ��������ź�
					
				}
		 }
		 #endif

	}
	//��������λ�Ĵ��������������ݣ�׼����������DR�Ĵ����зŵ�ʱ�򣬷���RNXE��־û�б���գ��ͻᴥ�����ж�
	
	else if(USART_GetFlagStatus(USART1, USART_IT_ORE) == SET)		//����Ƿ��н������
  {
			USART1_GETCHAR();																//����������־��ֻ���ö����ݵķ�ʽ���������־
  }
	
	//�����жϴ���

	if(USART_GetITStatus(USART1, USART_IT_TXE) == SET)					//�����ж�
	{		

			if(-1 == kfifo_getc(&tx_fifo,&tx_char)) //fifo��û��������
			{	
				//��ֹ�����ж�
				
				USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
				if(tx_event)
				{
					  SysMboxPost(tx_event, (void *)&tx_char);	//���ͼ���ȴ��������ź�
				}
			}
			else
			{
					//�������ݣ���������fifo�е�����
					USART1_PUTCHAR(tx_char);
			}		
	
	}

	
	SysIntExit();   																						//�������ȼ����ȱ������
}



#if (USART1_TX_MODE == TX_DMA)
static u8 usart1_dma_send(u8* buff, u32 buff_len)	
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
  DMA2_Stream7->NDTR = buff_len;					   						//�����ֽ�����
	dmx_send_complete = 0;
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE); 	//����
	DMA_Cmd(DMA2_Stream7, ENABLE);									//ʼ��DMAͨ��
	
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
#endif

#if(USART1_TX_MODE==TX_DMA)
void DMA2_Stream7_IRQHandler(void)
{
	SysIntEnter();	
  
	if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7))    //DMA��������ж�
	{
				
		USART_DMACmd(USART1, USART_DMAReq_Tx, DISABLE);     //�ر�DMA����
		DMA_Cmd(DMA2_Stream7, DISABLE);	                    //�ر�DMAͨ�� 

		dmx_send_complete = 1;
		if(tx_event != 0)											//�����˷�������ź���֪ͨ
			SysMboxPost(tx_event, (void *)&tx_char);	//���ͼ���ȴ��������ź�
		
	}
	
	DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_FEIF7 | DMA_FLAG_DMEIF7 | DMA_FLAG_TEIF7 | DMA_FLAG_HTIF7 | DMA_FLAG_TCIF7);  //Modify
	
	SysIntExit();
}
#endif

void usart1_timer_isr(void)
{
	#if (USART1_RX_MODE == RX_PACKET_MODE)
		if(rx_event != 0)
		{
			 SysMboxPost(rx_event, (void *)&rx_char);	//���ͼ���ȴ��������ź�
		}
	#endif
}

s8 usart1_init(void)
{
	if(DeviceInstall(&usart1_cfg) != HVL_NO_ERR)			//�豸ע��
		return -1;
	return 0;
}																								//���ö�ʱ����������ճ�ʱ


static u8 usart1_hard_deinit()
{
	USART_Cmd(USART1, DISABLE); 
	USART_ClearITPendingBit(USART1,USART_IT_RXNE);//����ձ�־
	USART_ITConfig(USART1,USART_IT_RXNE, DISABLE);	//���������ж�
	RCC_AHB1PeriphClockCmd(USART1_TXD_Pin_RCC, DISABLE);						//�ر�TXDʱ��
	RCC_AHB1PeriphClockCmd(USART1_RXD_Pin_RCC, DISABLE);						//�ر�RXDʱ��
	return 0;
}
static u8 usart1_hard_init(USART_InitTypeDef* desc)
{
#if (USART1_RX_MODE == RX_PACKET_MODE)
	usart_timer_init();																									//���ö�ʱ����������ճ�ʱ
	baudRate = desc->USART_BaudRate;
#endif
	
	
#if(USART1_RX_MODE==RX_PACKET_MODE)	
  usart1_update_timeout(baudRate,N_char_timeout);
#endif
	
	USART_Cmd(USART1, DISABLE);  																		//ʹ��ʧ�ܴ�������
	usart1_commu_param_init(desc);												//���ڳ�ʼ��
	usart1_gpio_init();																							//�������ų�ʼ��	
	
	usart1_event_init();
	usart1_fifo_init();
	
	usart1_nvic_init();	
	USART_ClearITPendingBit(USART1,USART_IT_RXNE);//����ձ�־
	USART_ClearITPendingBit(USART1,USART_IT_TXE);
	USART_ClearITPendingBit(USART1,USART_IT_TC);
	#if(USART1_TX_MODE==TX_IRQ)

	#elif(USART1_TX_MODE==TX_DMA)
		usart1_dmatxd_init();
	#endif
	
	USART_ITConfig(USART1,USART_IT_RXNE, ENABLE);	//���������ж�
	USART_Cmd(USART1, ENABLE);  									//ʹ��ʧ�ܴ�������
	
	return 0;
}
