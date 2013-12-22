#include "LineFit.h"


/*
 * ����:    �������
 * ����:
 *          pNcb : ����
 *					in������ֵ
 * ���     float����Ͻ��
 */
float LineFit(void *pNcb, float in)
{
	unsigned char i;
	unsigned char nSeg;
	unsigned char nFitCount;
	FitPoint* pFitPoint;
	
	if (pNcb == 0)
	{
		return in;
	}
	nFitCount = SLINEFIT(pNcb).nFitCount;
	if(nFitCount == 0) return in;
	
	pFitPoint = SLINEFIT(pNcb).pFitPoint;
		

	//Ѱ����������ڵĶ�
	nSeg = 0;
	for(i = 1; i < nFitCount; i++)
	{
		if(in >= pFitPoint[i].x) nSeg++;

		else break;
	}

	//���
	in = (in - pFitPoint[nSeg].x) * pFitPoint[nSeg].k + pFitPoint[nSeg].y;

	return in;	
}




