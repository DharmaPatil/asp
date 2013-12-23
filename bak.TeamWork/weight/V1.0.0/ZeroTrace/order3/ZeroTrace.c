#include "math.h"
#include "ZeroTrace.h"
#include "..\drv\drvproc.h" 

/*
 * ����:    ������������
 * ����:
 *          pZtcb : ע��ʱ����
 *					bSet : 1�����㣻0���������
 * ���     float��������
 */
void SetZero(void *pZtcb, signed char bSet)
{
		if(bSet > 0)
		{
			SZEROTRACE(pZtcb).cHandZerosFlag = 1;   //�ֶ�����
		}
		else
		{
			SZEROTRACE(pZtcb).cHandZerosFlag = -1;
		}
}

/*
 * ����:    ��λ���٣�������٣�ȡ�м��
 * ����:
 *          in : ����
 * ���     float��������
 */
float ZeroTrace(void *pZtcb, float in)
{
	float fDelt;
	float fOut;
    
	if(pZtcb == NULL) return in;

	//������ʱ
	if(SZEROTRACE(pZtcb).nPowerIndex < SZEROTRACE(pZtcb).nPowerTime)
	{		
		SZEROTRACE(pZtcb).nPowerIndex++;
		return 0;
	}
	
	//��������
	if(SZEROTRACE(pZtcb).nPowerIndex == SZEROTRACE(pZtcb).nPowerTime)
	{	
        if(fabs(WetDrvProc(in, SZEROTRACE(pZtcb).fDrv)) <= SZEROTRACE(pZtcb).fPowerRange)
        {
			SZEROTRACE(pZtcb).fPowerOnZero = in;
    		SZEROTRACE(pZtcb).fTraceZero = in;
        }
		
		SZEROTRACE(pZtcb).PointFirst = in;
		SZEROTRACE(pZtcb).PointMid = in;
		SZEROTRACE(pZtcb).PointLast = in;
		
		SZEROTRACE(pZtcb).nPowerIndex++;			//��һ�󲻻��ٽ����������
        SZEROTRACE(pZtcb).cHandZerosFlag = 0;       //�ֶ�����ʹ��
		SZEROTRACE(pZtcb).nTraceIndex = 0;		    //��λ����ʹ��
		
		return (in - SZEROTRACE(pZtcb).fTraceZero);
	}
	
	//�ֶ�����
	if(SZEROTRACE(pZtcb).cHandZerosFlag == 1)
	{	
        SZEROTRACE(pZtcb).cHandZerosFlag = 0;
		
        if(fabs(WetDrvProc(in - SZEROTRACE(pZtcb).fTraceZero, SZEROTRACE(pZtcb).fDrv)) <= SZEROTRACE(pZtcb).fHandRange)
        {
				SZEROTRACE(pZtcb).fPowerOnZero = in;
    		SZEROTRACE(pZtcb).fTraceZero = in;
        }
	}
	else if(SZEROTRACE(pZtcb).cHandZerosFlag == -1)
	{
		//�ֶ��������
		SZEROTRACE(pZtcb).cHandZerosFlag = 0;
		
		SZEROTRACE(pZtcb).fPowerOnZero = 0;
		SZEROTRACE(pZtcb).fTraceZero = 0;		
	}
	
	//������ʱ��������
	if(SZEROTRACE(pZtcb).nTraceTime == 0)
	{
		SZEROTRACE(pZtcb).nTraceIndex = 0;
		return (in - SZEROTRACE(pZtcb).fTraceZero);
	}	
	
	//����ʱ������
	SZEROTRACE(pZtcb).nTraceIndex++;
	
	//��û�ﵽ����ʱ��
	if(SZEROTRACE(pZtcb).nTraceIndex < SZEROTRACE(pZtcb).nTraceTime)
	{
		return (in - SZEROTRACE(pZtcb).fTraceZero);
	}
	
	//�Ѵﵽ����ʱ��	
	SZEROTRACE(pZtcb).nTraceIndex = 0;
	
	//����
	SZEROTRACE(pZtcb).PointFirst = SZEROTRACE(pZtcb).PointMid;
	SZEROTRACE(pZtcb).PointMid = SZEROTRACE(pZtcb).PointLast;
	SZEROTRACE(pZtcb).PointLast = in;
	
	//�жϸ��ٷ�Χ
	if( (fabs(SZEROTRACE(pZtcb).PointFirst - SZEROTRACE(pZtcb).fTraceZero) < SZEROTRACE(pZtcb).fTraceRange) &&
		(fabs(SZEROTRACE(pZtcb).PointMid - SZEROTRACE(pZtcb).fTraceZero) < SZEROTRACE(pZtcb).fTraceRange)  && 
		(fabs(SZEROTRACE(pZtcb).PointLast - SZEROTRACE(pZtcb).fTraceZero) < SZEROTRACE(pZtcb).fTraceRange) )
	{
		//�м��������
		fDelt = SZEROTRACE(pZtcb).PointMid - SZEROTRACE(pZtcb).fTraceZero;
		
		//�ڸ��ٲ�������
		if(fabs(fDelt) <= SZEROTRACE(pZtcb).fTraceStep)
		{
			//ֱ������
			SZEROTRACE(pZtcb).fTraceZero = SZEROTRACE(pZtcb).PointMid;
		}
		else
		{
			if(fDelt > 0)
			{
				SZEROTRACE(pZtcb).fTraceZero += SZEROTRACE(pZtcb).fTraceStep;	
			}
			else
			{
				SZEROTRACE(pZtcb).fTraceZero -= SZEROTRACE(pZtcb).fTraceStep;		
			}				
		}			
	}
	
	//��������ж�
	else if( (fabs(SZEROTRACE(pZtcb).PointFirst - SZEROTRACE(pZtcb).fPowerOnZero) < SZEROTRACE(pZtcb).fTraceRange) &&
		(fabs(SZEROTRACE(pZtcb).PointMid - SZEROTRACE(pZtcb).fPowerOnZero) < SZEROTRACE(pZtcb).fTraceRange)  && 
		(fabs(SZEROTRACE(pZtcb).PointLast - SZEROTRACE(pZtcb).fPowerOnZero) < SZEROTRACE(pZtcb).fTraceRange) )
	{
			//�ָ����������
			SZEROTRACE(pZtcb).fTraceZero = SZEROTRACE(pZtcb).fPowerOnZero;			
	}
	
	//���
	fOut = in - SZEROTRACE(pZtcb).fTraceZero;

	return fOut;	
}




