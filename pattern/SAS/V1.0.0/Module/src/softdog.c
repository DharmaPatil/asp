/*
 * File: softdog.c
 * Brief: Soft WatchDog File
 
 * History:
 * 1.2013-1-25 create file;
 */

#include "includes.h"
#include "dog_init.h"
#include "softdog.h"

#define HardSoftCnt  	50		//Ӳ�����Ź�50msιһ��
#define MaxSoftDogNum 	8

static HANDLE dog_handle;

struct soft_watchdog_timer {
    uint16 time_out;    //���Ź�������ֵ, ��ʱ��λ10ms
    uint16 time;		//���Ź�������
    SoftDogState  state;		//״̬
    const char *name;
};

static uint8 SoftDogNum = 0;
static volatile BOOL SoftDogChanger = FALSE;

static struct soft_watchdog_timer SoftDogList[MaxSoftDogNum] = {0};

void Set_SoftDog_Changer(BOOL flag)
{
	SoftDogChanger = flag;
}

void SoftWatchDog_ISR(void) 
{
	static uint16 softcnt = 0;
    static BOOL StopDogFlag = FALSE;
	static uint16 cnt = 0;
	static BOOL flag = FALSE;
	uint8 i = 0;

	//��ι��ʱ�䳬ʱ,����쳣,��Ҫ��λ
    if(StopDogFlag) return;

	softcnt += 10;

	//�����ӳ�3s����������Ź�, �ȴ������߳���ȫ����
	if(!flag)
	{
		cnt++;
		if(cnt >= 300)
		{
			flag = TRUE;
		}
	}

	//�̼߳��
	if(flag && !SoftDogChanger)
	{
	    for(i = 0; i < SoftDogNum; i++) 
	    {
	        if(SoftDogList[i].state == SOFTDOG_RUN) 
	        {
				SoftDogList[i].time--;
				//��ι����ʱ
				if(SoftDogList[i].time == 0)
				{
					StopDogFlag = TRUE;
					//��ӡ������Ϣ
					printf("Err:%s SoftDog Out!\r\n", SoftDogList[i].name);
					return;
				}
			}
		}
	}

	//��������¶�ʱι��
	if(softcnt >= HardSoftCnt) 
	{
		softcnt = 0;
		feed_dog();	
	}
}

//���ι��
void SoftDog_Feed(uint32 id)
{
#if SYS_CRITICAL_METHOD == 3  	 
	SYS_CPU_SR  cpu_sr = 0;
#endif

	if(id >= SoftDogNum) return;
	
	SYS_ENTER_CRITICAL();
	SoftDogList[id].time = SoftDogList[id].time_out;
	SYS_EXIT_CRITICAL();
}

int SoftDog_Init(void)
{
	uint8 err;
	
	if(Dog_Init() == 0) {
		debug(Debug_Error, "WatchDog init err\r\n");
		return 0;
	}
	
	dog_handle = DeviceOpen(CHAR_DOG, 0, &err);
	feed_dog();
	
	return 1;
}
	
//Ӳ��ι��
void feed_dog(void)
{
	if(dog_handle != NULL)
		DeviceIoctl(dog_handle,DOGCMD_FEED,0);
}

/*
 * Function: ע��������Ź���Ҫ�Ĳ���
 * Param:
 * time:  �����ʱ��, ��λ10ms
 * state: ��ʼ��״̬
 * name:  �߳�����
 */
int Register_SoftDog(uint16 time, SoftDogState state, const char *name)
{
	int id = 0;
	
	if(time == 0) return -1;
	if(name == NULL) return -1;
	if(SoftDogNum >= MaxSoftDogNum) return -2;

	id = SoftDogNum;
	
	SoftDogList[id].state = state;
	SoftDogList[id].time = time;
	SoftDogList[id].time_out = time;
	SoftDogList[id].name = name;

	SoftDogNum++;

	return id;
}
			
