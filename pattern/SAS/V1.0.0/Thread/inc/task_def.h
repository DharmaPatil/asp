#ifndef TASK_DEF_H
#define TASK_DEF_H

//�������ȼ�
enum {
	StartPrio			= 3,		//�����߳�

 	ADJobPrio 			= 4,			//ƽ̨ad�ɼ��߳�
	
	KeyPrio 			= 14,	//�����߳�

	FRAMMutexPrio		= 8,			//ƽ̨�㻥����

	BeepPrio			= 9,			//ƽ̨��������߳�

	AlgorimPrio	    = 12,    //�㷨�߳�		
	BusinessPrio		= 13,	 //ҵ���߳�

	USBPrio				= 15,  	//usb 

	Com6MutexPrio		= 17, 
	
	SerialRecvPrio		= 18,	//����2�����߳�
	DebugPrio			= 19,	//����1�����߳�

	FlashMutexPrio		= 25,	//ƽ̨�㻥����	
	DataManagerPrio 	= 26,   //���ݹ����߳�
	WatchDogPrio		= 27,
	
	USBStatus			= 28,			//ƽ̨��usb�߳� usb״̬��		

	LwipMutexPrio		= 30,			//ƽ̨�����绥����

	LwipPrio1			= 31,			//ƽ̨�������߳�
	LwipPrio2			= 32,			//ƽ̨�������߳�

	NetPrio		        = 33,	//�����߳�
	NetSendPrio		= 34,
	GUIPrio				= 35,  	//GUI�߳�
};

//void feed_dog(void);

#endif
