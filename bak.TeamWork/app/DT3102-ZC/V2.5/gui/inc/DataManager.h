#ifndef _DATAMANAGER_H_	
#define _DATAMANAGER_H_

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif

#include "stm32f4xx.h"
#include "Common.h"
	
	
	
#define sFLASH_MAXADDR            0x400000  //2012.11.22 lxj

typedef enum
{
	DM_WEIGHTDATASAVE,		   //��¼
	DM_UPLOADACK			   //�����ϴ��ɹ���Ӧ��
} DataManagerID;				//DataManager��ϢID��

typedef struct _DataManagerMsgInfo				//DataManager��Ϣ��Ϣ
{
	u16	 DataManagerID;		//DataManager��Ϣ��
	u32  nParam;
} DataManagerInfo, *LPDataManagerMsgInfo;


#define  DATAHEADLEN            33
#define  DATAPACKMAXLEN         (5*AXLECOUNTMAX+2*AXLEGROUPCOUNTMAX-2)
#define  CalcDataPackLen(axle,axlegroup)   (5*axle+3*axlegroup-2)

typedef struct PACKED
{
    //����ͷ
    u8   ucDataType;                 //��������          0xaa ��������
    u8   ucSync;                     //ͬ����־          0xff δ�ϴ� 0x00 ���ϴ�
    u32  ulNo;                       //���
    u16  usTimeYear;                 //����ʱ��  ��
    u8   ucTimeMonth;                //����ʱ��  ��
    u8   ucTimeDay;                  //����ʱ��  ��
    u8   ucTimeHour;                 //����ʱ��  ʱ
    u8   ucTimeMin;                  //����ʱ��  ��
    u8   ucTimeSec;                  //����ʱ��  ��
    u32  ulWeitht;                   //����
    u8   ucOverLoad;                 //����
	u8   ucDir;                      //0 δ֪ 1����(A->B) 2����(B->A)
    u16  usSpeed;                    //����              Ȩֵ: 0.1km/h
    s16  sAcce;                      //���ٶ�            Ȩֵ: 0.1 km/h2
    u8   ucAxleCount;                //����
    u8	 ucAxleGroupCount;           //������
    u32  ulDataCrc;                  //������CRC
    u32  ulHeadCrc;                  //����ͷCRC

    //������
	unsigned short	usAxleWet[AXLECOUNTMAX];			// ����
	unsigned char	ucAxleType[AXLECOUNTMAX];			// ���� 0 : δ֪  1: ���� 2: ˫��
	unsigned short  usAxleDis[AXLECOUNTMAX-1];			// ��� m
	unsigned short	usAxleGroupWet[AXLEGROUPCOUNTMAX];		// ������
	unsigned char	ucAxleGroupType[AXLEGROUPCOUNTMAX];	// ��������
} SVehDataPack;



u8   SaveWetData(SVehWeight* wet);
void DataFlashCreat(u8 nPrio);
void DataManagerInit(void);		//��ʼ��
void DataManagerProcess(void);	//ҵ�����ݹ��������Ϣ����ȴ�
SVehDataPack* GetWetDataByAddr(u32 addr);
SVehDataPack* GetWetDataByNo(u32 no);

#ifdef __cplusplus		   //�����CPP����C���� //��������
}
#endif

#endif
