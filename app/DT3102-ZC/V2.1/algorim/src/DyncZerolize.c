#include "DyncZerolize.h"



/*
 * ����:    ��̬��λ����
 */
float  DyncZeroLize(void *pZCB, float fIn, int bStaticZeroState)
{
    if(bStaticZeroState) 
    {
        SDYNCZERO(pZCB).fZeroValue = fIn;
    }

    return (fIn - SDYNCZERO(pZCB).fZeroValue);
}




