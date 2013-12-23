#ifndef _STATIC_INIT_H_	
#define _STATIC_INIT_H_

#ifdef __cplusplus			//�����CPP����C����
extern "C" {
#endif

#include "stm32f4xx.h"
	
//�̼���ַ��
#define AppSpaceSize						262144										//�̼����ռ��С			256K	��Сת���ɿռ���Ҫ-1 �ռ��0��ʼ���
#define AppStartAddress					0x8020000									//�̼�����ʼ��ַ			0x8020000
#define AppEndAddress						0x805FFFF									//�̼���������ַ			0x805FFFF
	
//�̼����ݵ�ַ��
#define AppBakSpaceSize					262144										//�̼��������ռ��С	256K	��Сת���ɿռ���Ҫ-1 �ռ��0��ʼ���
#define AppStartAddressBak			0x8060000									//�̼����ݿ�ʼ��ַ		0x8060000
#define AppEndAddressBak				0x809FFFF									//�̼����ݽ�����ַ		0x809FFFF
#define AppCRCAddress						0x809FFFC									//CRCЧ�����ַ				0x809FFFC - 0x809FFFF		4Byte
#define AppSizeAddress					0x809FFF8									//��С��ŵ�ַ				0x809FFF8 - 0x809FFFB		4Byte
#define AppUpSrcAddress					0x809FFF4									//�̼���Դ						0x809FFF4 - 0x809FFF7		4Byte	

//������־��ַ��		16K AppFlag [0x8008000 - 0x800BFFF]
#define AppFlagSpaceSize				16384											//������־���ռ��С	16K
#define AppFlagAddress					0x8008000									//������־�� 					0x8010000 - 0x801FFFF		64K
#define AppUPAddress						0x8008000									//�Ƿ���µ�ַ				0x8008000 - 0x8008003		4Byte
#define AppERRAddress						0x8008004									//����ʧ��״̬��ַ		0x8008004 - 0x8008007		4Byte

//����Ӧ�ó����ַ��
#define AuxiliaryAppSpaceSize			65535										//�����������ռ��С	64K
#define AuxiliaryAppStartAddress	0x8010000								//��������ʼ��ַ��	0x8010000
#define AuxiliaryAppCRCAddress		0x801FFFC								//��������CRCЧ����		0x801FFFC
#define AuxiliaryAppSizeAddress		0x801FFF8								//���������С				0x801FFF8
#define AuxiliaryAppUpSrcAddress	0x801FFF4								//����������Դ				0x801FFF4
#define AuxiliaryAppEndAddress		0x801FFFF								//�������������ַ��	0x801FFFF

//ASCII�ֿ��ַ��		16K ASCIIFont [0x80A0000 - 0x80BFFFF]
#define ASCIIFontSpaceSize			16384											//ASCII�ֿ����ռ��С	16K
#define	ASCIIFontStartAddress		0x80A0000									//ASCII�ֿ�����ʼ��ַ	0x800C000	
#define	ASCIIFontEndAddress			0x80BFFFF									//ASCII�ֿ���������ַ	0x800FFFF		16K
#define ASCIIFontCRCAddress			0x80BFFFC									//ASCII�ֿ�CRCЧ����	0x80BFFFC - 0x80BFFFF		4Byte
#define ASCIIFontSizeAddress		0x80BFFF8									//ASCII�ֿ��С				0x80BFFF8 - 0x80BFFFB		4Byte
#define ASCIIFontUpSrcAddress		0x80BFFF4									//ASCII�ֿ��С				0x80BFFF4 - 0x80BFFF7		4Byte	


//GB�ֿ��ַ��			225K GBFont [0x80C0000 - 0x80FFFFF]
#define GBFontSpaceSize					262144										//GB�ֿ����ռ��С		256K
#define	GBFontStartAddress			0x80C0000									//GB�ֿ�����ʼ��ַ		0x80A0000
#define	GBFontEndAddress				0x80FFFFF									//GB�ֿ���������ַ		0x80DFFFF		256K
#define GBFontCRCAddress				0x80FFFFC									//GB�ֿ�CRCЧ����			0x80FFFFC - 0x80FFFFF		4Byte
#define GBFontSizeAddress				0x80FFFF8									//GB�ֿ��С					0x80FFFF8 - 0x80FFFFB		4Byte
#define GBFontUpSrcAddress			0x80FFFF4									//GB�ֿ���Դ					0x80FFFF4 - 0x80FFFF7		4Byte
	
//��־����־��ַ
//Application�����ñ�־
#define ReadAppCRCAddress()					(*((volatile u32*)(AppCRCAddress)))							//��ȡ�̼��ļ�CRCУ��ֵ
#define ReadAppSizeAddress()				(*((volatile u32*)(AppSizeAddress)))						//��ȡ�̼��ļ�Size��С
#define ReadAppUpSrcAddress()				(*((volatile u32*)(AppUpSrcAddress)))						//��ȡ�̼��ļ���Դ
#define ReadAppUPFlagAddress()			(*((volatile u32*)(AppUPAddress)))							//��ȡ���±�־	PCBOOT NETBOOT
#define ReadAppErrFlagAddress()			(*((volatile u32*)(AppERRAddress)))							//��ȡ����״̬��־	����0	��0�������������ԭ��

//���������־��ַ
//�������������ñ�־
#define ReadAuxiliaryAppCRCAddress()				(*((volatile u32*)(AuxiliaryAppCRCAddress)))		//��ȡ���������ļ�CRCУ��ֵ
#define ReadAuxiliaryAppSizeAddress()				(*((volatile u32*)(AuxiliaryAppSizeAddress)))		//��ȡ���������ļ�Size��С
#define ReadAuxiliaryAppUpSrcAddress()			(*((volatile u32*)(AuxiliaryAppUpSrcAddress)))	//��ȡ���������ļ���Դ


//GB�ֿ������ñ�־
#define ReadGBCRCAddress()				(*((volatile u32*)(GBFontCRCAddress)))						//��ȡGB�ֿ�У��ֵ
#define ReadGBSizeAddress()				(*((volatile u32*)(GBFontSizeAddress)))						//��ȡGB�ֿ��С
#define ReadGBUpSrcAddress()			(*((volatile u32*)(GBFontUpSrcAddress)))					//��ȡGB�ֿ���Դ

//ASCII�ֿ������ñ�־
#define ReadASCIICRCAddress()			(*((volatile u32*)(ASCIIFontCRCAddress)))					//��ȡASCII�ֿ�У��ֵ
#define ReadASCIISizeAddress()		(*((volatile u32*)(ASCIIFontSizeAddress)))				//��ȡASCII�ֿ��С
#define ReadASCIIUpSrcAddress()		(*((volatile u32*)(ASCIIFontUpSrcAddress)))				//��ȡASCII�ֿ���Դ

//��־����־ֵ
//������Դ��־
#define PCBOOT 								0x2012				//PC���̼�������־
#define NETBOOT 							0x2013				//ZX���̼�������־
//���ݸ��±�־
#define UPBOOTFlag 						0x1004				//������־		�д˱�־�����¹̼��� û������Ҫ����

//��־������ֵ
#define	Succeed		0					//�����ɹ�
#define	CRCErr		1					//����ʧ��	CRCЧ�����	
#define	SizeErr		2					//����ʧ��	�ļ���С����
#define	CopyErr		3					//����ʧ��	���������뵽Ӧ�������뿽������

//Ƶ������
#define SYSCLK		0					//SYSCLKƵ��
#define HCLK			1					//HCLKƵ��
#define PCLK1			2					//PCLK1Ƶ��
#define PCLK2			3					//PCLK2Ƶ��

void Delay(const u32 nCount);						//һ��������Ƶ����ʱ
void Delay_MS(const u32 nCount);				//���뼶��ʱ
void Delay_S(const u32 nCount_temp);		//�뼶��ʱ
void GetCpuIdHex(u8* HCpuId);			//��ȡCPUID
u32 BeiNumber(const u32 num_temp,const u8 len_temp);		//����
u32 ASCIItoNumber(u8* data_temp,const u8 len_temp);	//ASCII to Number
u32 Get_SyS_CLK(const u8 temp);		//��ȡϵͳƵ��

#ifdef __cplusplus		   //�����CPP����C���� //��������
}
#endif

#endif
