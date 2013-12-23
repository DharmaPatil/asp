#ifndef __COMMON_H__
#define __COMMON_H__

 
#define ARRAY_SIZE(array,type) ((sizeof(array))/(sizeof(type)))
#define PACKED __attribute__((packed))


/*
 * ��λת��
 */
float kg2lg(float wet_kg);      // kgת��Ϊlb
float lg2kg(float wet_lb);      // lbת��Ϊkg
float CurVal2Kg(float wet);     // ��ǰϵͳ��λ������ֵת��Ϊkg
float Kg2CurVal(float wet_kg);  // kg��λ������ֵת��Ϊ��ǰ��ʾ�ĵ�λ
unsigned char GetCurUnit(void);
char *GetCurUnitStr(void);      // ��ȡ��λ�ַ���
void SwitchUnit(void);			//��λת��
//unsigned char GetCurUnit(void); // ��ȡ��λ��־λ 0:kg  1:lb
// void GetCpuIdHex(char* HCpuId);




#endif


