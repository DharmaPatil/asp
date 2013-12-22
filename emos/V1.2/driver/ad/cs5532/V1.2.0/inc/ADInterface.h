/************************************************************************************
**																			 
**																			 
**																			 
**--------�� �� �� Ϣ----------------------------------------------------------------
**��   ��   ����AD&DAInterface.h														 
**��   ��   �ˣ�														 
**�� ��  ʱ �䣺													 
**����޸�ʱ�䣺															      
**��        ����
************************************************************************************/
#ifndef _AD_DA_INTERFACE_H_
#define _AD_DA_INTERFACE_H_


#define ADCheck		0
#define ADRUNINIT	1
#define ADRUN		2


#define	OS_EVENT	SYS_EVENT
#define	OSTimeDly	SysTimeDly

//ƫ�ƣ����棬ͨ���Ĵ���	��ȫ���򵥸�����	
//���üĴ���				ֻ�е�������
#define ReadALLRegister		0x48	//�����мĴ���	
#define WriteALLRegister	0x40	//д���мĴ���
#define ReadRegister		0x08	//�������Ĵ���
#define WriteRegister		0x00	//д�����Ĵ���

#define SYNC1	0xFF		//����ؼ���
#define SYNC0	0xFE		//����ؼ���

//����Ĵ���	���ͷ���15���ֽڵ�SYNC1(0xFFH)+1�ֽڵ�SYNC0(0xFEH)���������Ĵ���
/******************************************
D07	D06	D05	D04	|	D03	D02	 D01	D00
C	ARA	CS1	CS0		R/W	RSB2 RSB1	RSB0
*******************************************/
// C			����Ĵ����Ƿ���Ч	0��Ч	1��Ч
// ARA			�����з�ʽ���ʼĴ���	0����	1���з�ʽ���ʸ��Ĵ���(ƫ�ơ������ͨ�����üĴ���)��
// 				�Ĵ���ѡ��λ(RSB)��������Ҫ���ʵ�����Ĵ��������ʼĴ���ʱ��λ(MSB)��ǰ����������ͨ��0��1��2�����ν���
// CS1-CS0		ͨ��ѡ��λ
// 				00 CS1-CS0�ṩһ������ͨ����ַ��Ҳ�����ڷ����������ͨ����ص�У׼�Ĵ������������ݼĴ���ʱ����λʧЧ
// R/W			��д��ѡ�Ĵ���	0д	1��
// RSB2-RSB0	�Ĵ���ѡ��λ
// 				001	1	ƫ�ƼĴ���
// 				010	2	����Ĵ���
// 				011	3	���üĴ���
// 				101	5	ͨ�����üĴ���
// 				����	��

//�Ե���ƫ�ƼĴ�������Ϊ ExcursionRegister + Excursion_1
#define ExcursionRegister	0x01	//ƫ�ƼĴ���
#define Excursion_1	0x00			//ƫ�ƼĴ���1
#define Excursion_2	0x10			//ƫ�ƼĴ���2
#define Excursion_3	0x20			//ƫ�ƼĴ���3(cs5533/cs5534)
#define Excursion_4	0x30			//ƫ�ƼĴ���4(cs5533/cs5534)

//�Ե�������Ĵ�������Ϊ GainRegister + GainSet_1
#define GainRegister		0x02	//����Ĵ�����������
#define GainSet_1	0x00			//����Ĵ���1
#define GainSet_2	0x10			//����Ĵ���2
#define GainSet_3	0x20			//����Ĵ���3(cs5533/cs5534)
#define GainSet_4	0x30			//����Ĵ���4(cs5533/cs5534)

//���üĴ���	0x03	��������
/**** 0 ******************* 0 *************** 0 ***************** 0 ******************* 0 ************** 0 ****************** 0 **************** 0 ********
D31	D30	D29	D28	|	D27	D26	D25	D24	|	D23	D22	D21	D20	|	D19	D18	D17	D16	|	D15	D14	D13	D12	|	D11	D10	D09	D08	|	D07	D06	D05	D04	|	D03	D02	D01	D00
PSS	PDW	RS	RV	|	IS	GB	VRS	A1	|	A0	OLS	N	OGS	|	FRS	N	N	N	|	N	N	N	N	|	N	N	N	N	|	N	N	N	N	|	N	N	N	N
**********************************************************************************************************************************************************/
#define ConfigRegister	0x03	//���üĴ���

#define PSS		//�ڵ�ģʽѡ��	
//0		����ģʽ(������������������ϵ�)	
//1		����ģʽ(����������)
#define PDW		//�ڵ�ģʽ		
//0		����ģʽ	
//1		�ڵ�ģʽ
#define RS1		0x20000000		//ϵͳ��λ	1		����һ�θ�λ(��λ���Զ���0)
#define RS0		0x00000000		//ϵͳ��λ	0		��������
#define RV		//��λ��Ч		
//0		��������	
//1		ϵͳ�Ѹ�λ��ֻ���������üĴ�������0
#define IS		//�����·		
//0		��������	
//1		��ͨ�������붼���ڲ���·
#define GB		//�����ź�λ	
//0		��A0����Ϊ�����������������	
//1		A0������Ϊ����Ǳ�Ŵ����Ĺ�ģ�����ѹ(����ֵ2.5V)��������������������ʱ���������ѡ��λ������
#define VRS0	0x00000000		//��ѹ�ο�ѡ��	0	2.5V < Vref <= VA+
#define VRS1	0x02000000		//��ѹ�ο�ѡ��	1	1V <= Vref <= 2.5V

#define A1_A0	//�������λ ( 00 A0=0,A1=0 | 01 A0=0,A1=1 | 10 A0=1,A1=0 | 11 A0=1,A1=1 )
//�����üĴ����е��������ѡ��λOLSΪ1ʱ���������λ(A0-A1)��������ִ�е������ֱ�����Ϊ��Ӧ���߼�״̬��ע������߼���ѹ��VA+��VA-����
#define OLS		//�������ѡ��
//0		�͵�ƽʱ��ͨ�����üĴ�����ΪA0��A1��Դ
//1		�ߵ�ƽʱ�����üĴ�����ΪA0��A1��Դ
#define OGS		//ƫ��������ѡ��
//0		�����õ����õ�Ԫ(Setup)�е�CS1-CS0λѡ��У׼�Ĵ���
//1		�����õ����õ�Ԫ(Setup)�е�OG1-OG0λѡ��У׼�Ĵ���
#define FRS0	0x00000000		//60HZ	�˲�����ѡ��	0	ʹ��ȱʡ��������� ��
#define FRS1	0x00080000		//50HZ	�˲�����ѡ��	1	��������ʼ���Ӧ���˲����Գ���ϵ��5/6 ��1/6

//ͨ���Ĵ���	0x05	��������	��������
/*************************** Setup1 ****************************************|******************************** Setup2 **************************************
D31	D30	D29	D28	|	D27	D26	D25	D24	|	D23	D22	D21	D20	|	D19	D18	D17	D16	|	D15	D14	D13	D12	|	D11	D10	D09	D08	|	D07	D06	D05	D04	|	D03	D02	D01	D00
CS1	CS0	G2	G1	|	G0	WR3	WR2	WR1	|	WR0	U/B	OL1	OL0	|	DT	OCD	OG1 OG0	|	CS1	CS0	G2	G1	|	G0	WR3	WR2	WR1	|	WR0	U/B	OL1	OL0	|	DT	OCD	OG1 OG0
****** 0 **************** 0 ******************* 0 ***************** 0 **************** 0 ****************** 0 ****************** 0 *************** 0 *****/
//�Ե���ͨ�����üĴ�������Ϊ ChannelRegister + ChannelSet_1
//������ͨ�����üĴ�������Ϊ ChannelRegister
#define ChannelRegister	0x05	//ͨ���Ĵ�����������
#define ChannelSet_1	0x00	//ͨ�����üĴ���1
#define ChannelSet_2	0x10	//ͨ�����üĴ���2
#define ChannelSet_3	0x20	//ͨ�����üĴ���3
#define ChannelSet_4	0x30	//ͨ�����üĴ���4

//CS1 CS0	����ͨ��ѡ��λ
#define SelectChannel_1	0x00000000		//ͨ��ѡ��λ(ѡ������ͨ��)		00	ѡ������ͨ��1 	//һ������ͨ����2�����õ�Ԫ����Ԫ��ѡ����ת������ʱ����
#define SelectChannel_2	0x40000000		//ͨ��ѡ��λ(ѡ������ͨ��)		01	ѡ������ͨ��2
#define SelectChannel_3	0x80000000		//ͨ��ѡ��λ(ѡ������ͨ��)		10	ѡ������ͨ��3(��CS5533/34)
#define SelectChannel_4	0xC0000000		//ͨ��ѡ��λ(ѡ������ͨ��)		11	ѡ������ͨ��4(��CS5533/34)

//G2 G1		����ѡ��λ			��VRS=0��A=2��VRS=1��A=1��˫�������뷶Χ�ǵ��������뷶Χ������
#define Gain_1	0x00000000		//000	����=1 �����������뷶Χ=[(VREF+)-(VREF-)]/1*A
#define Gain_2	0x08000000		//001	����=2 �����������뷶Χ=[(VREF+)-(VREF-)]/2*A
#define Gain_4	0x10000000		//010	����=4 �����������뷶Χ=[(VREF+)-(VREF-)]/4*A
#define Gain_8	0x18000000		//011	����=8 �����������뷶Χ=[(VREF+)-(VREF-)]/8*A		
#define Gain_16	0x20000000		//100	����=16�����������뷶Χ=[(VREF+)-(VREF-)]/16*A
#define Gain_32	0x28000000		//101	����=32�����������뷶Χ=[(VREF+)-(VREF-)]/32*A
#define Gain_64	0x30000000		//110	����=64�����������뷶Χ=[(VREF+)-(VREF-)]/64*A

//WR3-WR0	������				ʱ��Ƶ��Ϊ4.9152MHZ������ת��ģʽ������ת��ģʽ�ĵ�һ��ת������ʱ��ȵ���ת��ʱ�䳤
#define FRS_Seed_4	0x00000000		//0000	120  sps		100  sps
#define FRS_Seed_3	0x00800000		//0001	60   sps		50   sps
#define FRS_Seed_2	0x01000000		//0010	30   sps		25   sps
#define FRS_Seed_1	0x01800000		//0011	15   sps		12.5 sps
#define FRS_Seed_0	0x02000000		//0100	7.5  sps		6.25 sps
#define FRS_Seed_9	0x04000000		//1000	3840 sps		3200 sps
#define FRS_Seed_8	0x04800000		//1001	1920 sps		1600 sps
#define FRS_Seed_7	0x05000000		//1010	960  sps		800  sps
#define FRS_Seed_6	0x05800000		//1011	480  sps		400  sps
#define FRS_Seed_5	0x06000000		//1100	240  sps		200  sps

//U/B	��/˫����
#define UB0	0x00000000		//0	������ģʽ	 //������0Ϊ˫����ģʽ
#define UB1	0x00400000		//1	˫����ģʽ	 //������1Ϊ������ģʽ

//OL1 OL0	�������λ		�����üĴ����е��������ѡ��(OLS)λ��Ϊ�߼���0��ʱ���������λ��������ִ�е������ֱ�����Ϊ��Ӧ���߼�״̬��ע��Ƭ���߼������VA+��VA--����
#define Latch_0	0x00000000		//00	A0=0��A1=0
#define Latch_1	0x00100000		//01	A0=0��A1=1
#define Latch_2	0x00200000		//10	A0=1��A1=0
#define Latch_3	0x00300000		//11	A0=1��A1=1

//DT	ʱ��λ	��λʱ��ADC����ת��ǰ�ȴ�һ���ӳ�ʱ�䣬�⽫��ת����ʼǰ����A0��A1������ȶ�ʱ�䣬��FRS=0ʱ������ʱʱ��Ϊ1280��MCLK���ڣ���FRS=1ʱ������ʱʱ��Ϊ1536��MCLK����
#define DT0	0x00000000		//0	������ʼת��
#define DT1	0x00080000		//1	�ȴ�1280��1536��MCLK���ں�ʼת��

//OCD	��·���λ	��λʱ����ͨ��ѡ��λ��ѡ������ͨ��(AIN+)������һ��300nA�ĵ���Դ��ע��õ���Դֵ�����¶�Ϊ25���϶�ʱ��ֵ����-55���϶�ʱ���õ���Դ���ӵ�600nA�����û���Ҫ������һ�����������ߵı����ɿ�·���ȵ���ʱ���˹����ر�����
#define OCD0	0x00000000	//����ģʽ
#define OCD1	0x00040000	//�������Դ

//OG1 OG0	ƫ��/����Ĵ���ָ��λ		���üĴ����е�OGSλ��1ʱ��Ч�������û�ѡ��ת����У׼ʱʹ�õ�ƫ�ƺ�����Ĵ�����OGS=0ʱ����������ѡ����ͨ��(��CS1-CS0ѡ��)��Ӧ��ƫ�ƺ�����Ĵ���
#define Gain_Excursion_1	0x00000000	//00	������ͨ��1ѡ��ƫ�ƺ�����Ĵ���
#define Gain_Excursion_2	0x00010000	//01	������ͨ��2ѡ��ƫ�ƺ�����Ĵ���
#define Gain_Excursion_3	0x00020000	//10	������ͨ��3ѡ��ƫ�ƺ�����Ĵ���
#define Gain_Excursion_4	0x00030000	//11	������ͨ��4ѡ��ƫ�ƺ�����Ĵ���

//ִ��ת��
#define StartConvert		0x80	//ִ��ת��
#define Single				0x00	//����ת��
#define Continuation		0x40	//����ת��
#define ChannelPointer_1	0x00	//000 Setup1		 //һ������ͨ����2�����õ�Ԫ��SETUP1 SETUP2
#define ChannelPointer_2	0x08	//001 Setup2
#define ChannelPointer_3	0x10	//010 Setup3
#define ChannelPointer_4	0x18	//011 Setup4
#define ChannelPointer_5	0x20	//100 Setup5
#define ChannelPointer_6	0x28	//101 Setup6
#define ChannelPointer_7	0x30	//110 Setup7
#define ChannelPointer_8	0x38	//111 Setup8



#define	SETUP1_VRS		VRS0
#define	SPIB_SETUP1_CH		SelectChannel_1
#define	SPIB_SETUP1_GAIN	Gain_64
#define	SPIB_SETUP1_FRS		FRS_Seed_7
#define	SPIB_SETUP1_UB		UB0
#define SPIB_SETUP1_LATCH	Latch_0
#define	SPIB_SETUP1_DT		DT0
#define	SPIB_SETUP1_OCD		OCD1
#define	SPIB_SETUP1_GE		Gain_Excursion_1

#define	SPIB_SETUP2_CH		SelectChannel_1
#define	SPIB_SETUP2_GAIN	Gain_64
#define	SPIB_SETUP2_FRS		FRS_Seed_7
#define	SPIB_SETUP2_UB		UB0
#define SPIB_SETUP2_LATCH	Latch_0
#define	SPIB_SETUP2_DT		DT0
#define	SPIB_SETUP2_OCD		OCD0
#define	SPIB_SETUP2_GE		Gain_Excursion_2	

#define		SETUP1		(SPIB_SETUP1_CH + 	\
						SPIB_SETUP1_GAIN + 	\
						SPIB_SETUP1_FRS + 	\
						SPIB_SETUP1_UB + 	\
						SPIB_SETUP1_LATCH + \
						SPIB_SETUP1_DT + 	\
						SPIB_SETUP1_OCD + 	\
						SPIB_SETUP1_GE)

#define		SETUP2		((SPIB_SETUP2_CH + 	\
						SPIB_SETUP2_GAIN + 	\
						SPIB_SETUP2_FRS + 	\
						SPIB_SETUP2_UB + 	\
						SPIB_SETUP2_LATCH + \
						SPIB_SETUP2_DT + 	\
						SPIB_SETUP2_OCD + 	\
						SPIB_SETUP2_GE)>>16)	
						
#define NS_50   6

#define CS5532_CS_Delay(x)   {u8 i; \
                              i = x;    \
                              while(i--);}
#endif
