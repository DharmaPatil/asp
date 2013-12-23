#include "overtime_queue.h"
#include "app_rtc.h"
#include "debug_info.h"

#define MAX_QUEUE  10         //���е��������Ԫ����Ŀ

typedef struct queue
{                 
    OvertimeData item[MAX_QUEUE];   
    int front; //��ͷָ�� ��βָ��=(front+size)%MAX_QUEUE
	int size;
}QUEUE;

static QUEUE Q = {{0}};

//������������ˮ��, ��Χ1~59999
static uint16 SerialID = 1;

/**************************************************
 * ���г�ʼ��
 * �������ƣ�	OverQueue_Init
 * �������ܣ�	��ʼ��һ�������ͳ������ݶ��У�����������Ϣ����
 * ������		��
 * ����ֵ��		��
 *************************************************/
void OverQueue_Init(void)
{
	  CSysTime *time = NULL;
	  
	  Update_System_Time();
	  time = Get_System_Time();

	  Q.front = 0;
	  Q.size = 0;
 
	  srand(((unsigned int)time->hour << 16) | ((unsigned int)time->min << 8) | ((unsigned int)time->sec));
	  SerialID = rand();

}

/**************************************************
 * �������
 * �������ƣ�	OverQueue_Enqueue
 * �������ܣ�	��һ������������ӵ�����β��
 * ������		OvertimeData *data��ָ�����ṹ���ָ��
 * ����ֵ��		��
 *************************************************/
int OverQueue_Enqueue(OvertimeData *data)
{
#if SYS_CRITICAL_METHOD == 3  	 
    SYS_CPU_SR  cpu_sr = 0;
#endif

	SYS_ENTER_CRITICAL();
	if (Q.size >= MAX_QUEUE) 
	{
		SYS_EXIT_CRITICAL();
		debug(Debug_Business, "Warning:OverQueue full!\r\n");
		return -1;
	}
	
	data->SerialID = SerialID;
	SerialID++;
	if(SerialID > 59999) SerialID = 1;
	
	Q.size++;
	memcpy(&Q.item[(Q.front+Q.size)%MAX_QUEUE], data, sizeof(OvertimeData));	

	SYS_EXIT_CRITICAL();

	return 0;
}

/**************************************************
 * �������
 * �������ƣ�	OverQueue_GetSeriID
 * �������ܣ�	
 * ������		��
 * ����ֵ��		��ǰ���к�
 *************************************************/
unsigned short OverQueue_GetSeriID()
{
#if SYS_CRITICAL_METHOD == 3  	 
    SYS_CPU_SR  cpu_sr = 0;
#endif
	uint16 tmp;

	SYS_ENTER_CRITICAL();
	tmp = SerialID;
	SYS_EXIT_CRITICAL();

	return tmp;
}

/**************************************************
 * �Ƴ���ͷԪ��
 * �������ƣ�	OverQueue_Remove_Head
 * �������ܣ�	ɾ�����еĶ�ͷԪ��
 * ������		��
 * ����ֵ��	��
 **************************************************/
void OverQueue_Remove_Head(void)
{
#if SYS_CRITICAL_METHOD == 3  	 
    SYS_CPU_SR  cpu_sr = 0;
#endif

	SYS_ENTER_CRITICAL();
	if (Q.size == 0) 
	{
		SYS_EXIT_CRITICAL();
		return;
	}

    Q.front = (Q.front+1)%MAX_QUEUE;
    memset(&Q.item[Q.front], 0, sizeof(OvertimeData));
	if(Q.size > 0)
	{
		Q.size--;
	}
    SYS_EXIT_CRITICAL();
}

/**************************************************
 * �Ƴ���βԪ��
 * �������ƣ�	OverQueue_Remove_Tail
 * �������ܣ�	ɾ�����еĶ�βԪ��
 * ������		��
 * ����ֵ��		��
 **************************************************/
void OverQueue_Remove_Tail(void)
{
#if SYS_CRITICAL_METHOD == 3  	 
        SYS_CPU_SR  cpu_sr = 0;
#endif

	SYS_ENTER_CRITICAL();
	if (Q.size == 0)
	{
		SYS_EXIT_CRITICAL();
		return;
	}

	memset(&Q.item[(Q.front+Q.size)%MAX_QUEUE], 0, sizeof(OvertimeData));
	if(Q.size > 0)
	{
		Q.size--;
	}
    SYS_EXIT_CRITICAL();
}

/**************************************************
 * ��ȡ��ͷԪ��
 * �������ƣ�	OverQueue_Get_Head
 * �������ܣ�	��ȡ���еĶ�ͷԪ��
 * ������		��
 * ����ֵ��		ָ�����ṹ���ָ��
 **************************************************/
OvertimeData *OverQueue_Get_Head(void)
{
#if SYS_CRITICAL_METHOD == 3  	 
		SYS_CPU_SR	cpu_sr = 0;
#endif
	
	OvertimeData *p = NULL;

	SYS_ENTER_CRITICAL();
	if (Q.size == 0) 
	{
	  SYS_EXIT_CRITICAL();
	  return NULL;
	}

	p = &(Q.item[(Q.front+1)%MAX_QUEUE]);
	SYS_EXIT_CRITICAL();

	return(p);
}

/**************************************************
 * ��ȡ��βԪ��
 * �������ƣ�	OverQueue_Get_Tail
 * �������ܣ�	��ȡ���еĶ�βԪ��
 * ������		��
 * ����ֵ��		ָ�����ṹ���ָ��
 **************************************************/
OvertimeData *OverQueue_Get_Tail(void)
{
#if SYS_CRITICAL_METHOD == 3  	 
		SYS_CPU_SR	cpu_sr = 0;
#endif

	OvertimeData *p = NULL;

	SYS_ENTER_CRITICAL();
	if (Q.size == 0) 
	{
	  SYS_EXIT_CRITICAL();
	  return NULL;
	}

	p = &(Q.item[(Q.front+Q.size)%MAX_QUEUE]);
	SYS_EXIT_CRITICAL();
	
	return(p);
}


/**************************************************
 * ���Ӷ�ͷԪ�ط��ʹ���
 * �������ƣ�	OverQueue_Add_HeadSendNum
 * �������ܣ�	���Ӷ�ͷԪ�ط��ʹ���
 * ������		��
 * ����ֵ��		��
 **************************************************/
void OverQueue_Add_HeadSendNum(void)
{
#if SYS_CRITICAL_METHOD == 3  	 
	SYS_CPU_SR  cpu_sr = 0;
#endif

	SYS_ENTER_CRITICAL();
	if (Q.size == 0)
	{
		SYS_EXIT_CRITICAL();
		return ;
	}

	(Q.item[(Q.front+1)%MAX_QUEUE]).SendNum++;
	SYS_EXIT_CRITICAL();
}

/**************************************************
 * �����Ƿ�Ϊ��
 * �������ƣ�	OverQueue_Empty
 * �������ܣ�	�ж϶����Ƿ�Ϊ��
 * ������		��
 * ����ֵ��		bool true��Ϊ�գ�false����Ϊ��
 **************************************************/
__inline BOOL OverQueue_Empty(void)
{
	return (Q.size == 0) ? TRUE : FALSE;
}

//�������ж�
__inline BOOL OverQueue_Full(void)
{
	return (Q.size >= MAX_QUEUE) ? TRUE : FALSE;
}


//��ȡ���г���
int OverQueue_Get_Size(void)
{
#if SYS_CRITICAL_METHOD == 3  	 
		SYS_CPU_SR	cpu_sr = 0;
#endif
	int n;

	SYS_ENTER_CRITICAL();
	n = Q.size;
	SYS_EXIT_CRITICAL();

	return n;    //((Q.rear-Q.front+MAX_QUEUE) % MAX_QUEUE);
}

OvertimeData *OverQueue_Get_Index(uint8 index)
{
#if SYS_CRITICAL_METHOD == 3  	 
	SYS_CPU_SR	cpu_sr = 0;
#endif
	OvertimeData *pCar = NULL;

	SYS_ENTER_CRITICAL();

	if(index < 1) 
	{
		SYS_EXIT_CRITICAL();
		return NULL;
	}

	if((int)index <= OverQueue_Get_Size())
	{
		pCar = &(Q.item[(Q.front+index)%MAX_QUEUE]);
	}

	SYS_EXIT_CRITICAL();

	return pCar;
}

