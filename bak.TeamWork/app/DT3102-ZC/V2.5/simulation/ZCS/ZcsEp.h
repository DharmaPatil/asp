#pragma once
#include "ep.h"


typedef enum _ZcsNode
{
	//input-----------------
	MainScalerAD = 0,
	DownAlexAD,
	BoardAlexAD,
	UpAlexAD,
	LC2,
	GCFront,
	GCEnd,

	//output----------------
	MainScalerNormal,
	DownAlexNormal,
	BoardAlexNormal,
	UpAlexNormal,
	DownAlexReg,
	BoardAlexReg,
	UpAlexReg
}ZcsNode;


class CZcsEp : public CEp
{
public:
	CZcsEp(void);
	virtual ~CZcsEp(void);

protected:
	CString m_strType;
	uint8 m_nType;
	int m_nTypeSize;
	BOOL m_bIsSignal;
	BOOL m_bUseSim;

	CStringArray m_arrEpInput;
	vector<CEp*> m_Eps;						//�˵���




public:
	virtual CString GetTypeString();	//��ȡԭʼ�������ͣ��ַ������� 
	virtual uint8 GetTypeCode();		//��ȡԭʼ�������ͣ�����������
	virtual int	GetTypeSize();			//��ȡԭʼ�������͵�ǿ��
	virtual BOOL IsSignal();			//�Ƿ�Ϊ�źţ����ź���ֻ�е�ֵ����������������

public:
	//������CTreeObject
	virtual BOOL OnDbClick();

	virtual void OnNetMessage(UINT nID);
	virtual void InitNetMenu(CMenu* pMenu);

public:
	virtual void InitSample();
	virtual void StopSample();
	virtual void Calc();
	virtual void SetZero();
	virtual void ResetZero();

public:
	void ClearOutPut();

public:
	virtual const CString GetSymbol();
	virtual BOOL OnProperty();	
	virtual CEp* Spawn();

public:
	virtual BOOL StoreConfig(CArchive &ar);
	virtual BOOL LoadConfig(CArchive &ar, UINT nFileVersion);
	virtual BOOL StoreData(CArchive &ar);
	virtual BOOL LoadData(CArchive &ar, UINT nFileVersion);
};

