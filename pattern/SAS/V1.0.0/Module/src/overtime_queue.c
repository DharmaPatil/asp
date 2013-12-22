#include "overtime_queue.h"
#include "app_rtc.h"

#define MAX_QUEUE  11         //���е��������Ԫ����Ŀ

typedef struct queue
{                 
    OvertimeData item[MAX_QUEUE];   
    int front, rear; //��ͷָ�롢��βָ��
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
	  CSysTime *time = Get_System_Time();

	  Q.front = 0;
	  Q.rear = 0;
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
	if ((Q.rear+1)%MAX_QUEUE == Q.front) 
	{
		SYS_EXIT_CRITICAL();
		debug(Debug_Business, "Warning:OverQueue full!\r\n");
		return -1;
	}
	Q.rear=(Q.rear+1)%MAX_QUEUE;
	Q.size++;
	
	data->SerialID = SerialID;
	memcpy(&Q.item[Q.rear], data, sizeof(OvertimeData));	

	SerialID++;
	if(SerialID > 59999) SerialID = 1;

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
	return SerialID;
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

	memset(&Q.item[Q.rear], 0, sizeof(OvertimeData));

	if(Q.rear > 0)
	{
		Q.rear = (Q.rear-1);
	}
	else
	{
    	Q.rear = MAX_QUEUE - 1;
	}

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

	p = &(Q.item[Q.rear]);
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
 * ��ȡ��ͷԪ�ط��ʹ���
 * �������ƣ�	OverQueue_Get_HeadSendNum
 * �������ܣ�	��ȡ��ͷԪ�ط��ʹ���
 * ������		��
 * ����ֵ��		int ��ͷԪ�صķ��ʹ���
 **************************************************/
int OverQueue_Get_HeadSendNum(void)
{
#if SYS_CRITICAL_METHOD == 3  	 
		SYS_CPU_SR	cpu_sr = 0;
#endif

	int n = -1;

	SYS_ENTER_CRITICAL();
	if (Q.size == 0) 
	{
	  SYS_EXIT_CRITICAL();
	  return -1;
	}

	n = (Q.item[(Q.front+1)%MAX_QUEUE]).SendNum;
	SYS_EXIT_CRITICAL();

	return(n);
}


/**************************************************
 * �����Ƿ�Ϊ��
 * �������ƣ�	OverQueue_Empty
 * �������ܣ�	�ж϶����Ƿ�Ϊ��
 * ������		��
 * ����ֵ��		bool true��Ϊ�գ�false����Ϊ��
 **************************************************/
 #if 0
BOOL OverQueue_Empty(void)
{
	if (Q.front==Q.rear) 
	  return TRUE;
	else 
	  return FALSE;
}
#endif

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

