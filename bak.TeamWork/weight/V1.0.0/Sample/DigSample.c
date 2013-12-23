#include "DigSample.h"


/*
 * ����:    ����
 * ����:
 *          in : ����
 * ���     int�������Ƿ����
 *					pOut���������
 */
int DigSample(void *pDscb, float in, float* pOut)
{
	if ( (pDscb == 0) || (pOut == 0))
	{
		return 0;
	}
	if(SDIGSAMPLE(pDscb).nIndex == 0) SDIGSAMPLE(pDscb).fSum = 0.0f;
	
	SDIGSAMPLE(pDscb).fSum += in;
	SDIGSAMPLE(pDscb).nIndex++;
	if(SDIGSAMPLE(pDscb).nCycle == 0)
	{
		SDIGSAMPLE(pDscb).nIndex = 0;
		*pOut = in;

		return 1;		
	}
	if(SDIGSAMPLE(pDscb).nIndex == SDIGSAMPLE(pDscb).nCycle)
	{
		SDIGSAMPLE(pDscb).fKeep = SDIGSAMPLE(pDscb).fSum / SDIGSAMPLE(pDscb).nCycle;	
		SDIGSAMPLE(pDscb).nIndex = 0;
		*pOut = SDIGSAMPLE(pDscb).fKeep;
		
		return 1;	
	}
	
	*pOut = SDIGSAMPLE(pDscb).fKeep;	
	return 0;	
}




