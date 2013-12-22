#ifndef SOFTDOG_H
#define SOFTDOG_H

//������Ź�״̬
typedef enum 
{
    SOFTDOG_RUN = 0,
    SOFTDOG_STOP
}SoftDogState;

void SoftWatchDog_ISR(void);
int SoftDog_Init(void);

//������Ź�����
//true �ر� false ��
void Set_SoftDog_Changer(BOOL flag);

//���ι��
void SoftDog_Feed(uint32 id);

//Ӳ��ι��
void feed_dog(void);

//ע��������Ź���Ҫ�Ĳ���
int Register_SoftDog(uint16 time, SoftDogState state, const char *name);


#endif
