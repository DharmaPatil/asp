#include "Standlize.h"



/*
 * ����:    ��׼��
 */
float  Standlize(void *pSCB,float fIn)
{
	if (pSCB == 0)
	{
		return 0;
	}
    return fIn*(SSTAND(pSCB).fParam);
}




