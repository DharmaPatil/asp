#include <math.h>
#include "StateCheck.h"

#define STATE_ZERO      0x01
#define STATE_STEADY    0x10
/*
 * ����:    �ȶ��ж�
 * ����:
 *          pDcb : ����
 *					float������ֵ
 * ���     int���жϽ��
 */
int StateCheck(void *pStcb, float in)
{
    int state = 0;
    
	if (pStcb == 0)
	{
		return state;
	}
    SSTATECHECK(pStcb).nTimeRange = 10;
        
    SSTATECHECK(pStcb).nTimeIndex++;

    if(SSTATECHECK(pStcb).nTimeIndex >= SSTATECHECK(pStcb).nTimeRange)
    {
        SSTATECHECK(pStcb).nTimeIndex = 0;
        
    	if(fabs(in - SSTATECHECK(pStcb).fPrev) < SSTATECHECK(pStcb).fSteadyRange)
    	{
            state |= STATE_STEADY;
    	}
    	if(fabs(in) < SSTATECHECK(pStcb).fZeroRange)
    	{
            state |= STATE_ZERO;
    	}
    	
    	SSTATECHECK(pStcb).fPrev = in;
    	SSTATECHECK(pStcb).nPreState = state;
    } else {
    	state = SSTATECHECK(pStcb).nPreState;
    }

    return state;
}


char GetSteadySta(int State)
{
    char ret = 0;
    
    if(State & STATE_STEADY) {
        ret = 1;
    }

    return ret;
}

char GetZeroSta(int State)
{
    char ret = 0;
    
    if(State & STATE_ZERO) {
        ret = 1;
    }

    return ret;
}



