/*! 
\file ADCValueCheck.c
*/

#include "ADCValueCheck.h"

/*!
 * \brief		���������߼��
 * \param[in]	ad  ADֵ
 * \return		���ؼ����         
 * \retval 0    ����   
 * \retval -1   �쳣
 */
int  ADCValueCheck(long ad)
{
    return ((ad > MAXADCVALUE)||(ad < MinADCVALUE))?(char)-1:0;
}





