#ifndef BEEP_H
#define BEEP_H

//��������ʼ��
void Beep_Initial(void);

//cnt:  ���д���
//time: ����ʱ�� ��λ10ms
int32 Beep_Voice(uint32 cnt, uint16 time);

//���������� 1 on 0 off
void Beep_Power(uint8 type);


#endif
