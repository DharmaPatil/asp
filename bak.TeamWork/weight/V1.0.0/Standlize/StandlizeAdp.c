#include "Standlize.h"



/*
 * ����:    ��׼��
 */
float  Standlize(void *pSCB,float fIn)
{
	return fIn;

	if (pSCB == 0)
	{
		return 0;
	}
    return fIn*(SSTAND(pSCB).fParam);
}




