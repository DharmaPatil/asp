
#ifndef __IOCTRL_H
#define __IOCTRL_H
#include "stm32f4xx.h"

// #define LED_PIN                   GPIO_Pin_14
// #define LED_GPIO_PORT             GPIOD

typedef struct  {
		u8 In0;             // ������Ե�ƽ 0:�͵�ƽ  1:�ߵ�ƽ
    u8 In1;             // ������Ե�ƽ 0:�͵�ƽ  1:�ߵ�ƽ
    u8 In2;             // ������Ե�ƽ 0:�͵�ƽ  1:�ߵ�ƽ
    u8 In3;             // ������Ե�ƽ 0:�͵�ƽ  1:�ߵ�ƽ
    u8 In4;             // ������Ե�ƽ 0:�͵�ƽ  1:�ߵ�ƽ
    u8 In5;             // ������Ե�ƽ 0:�͵�ƽ  1:�ߵ�ƽ
    u8 In6;             // ������Ե�ƽ 0:�͵�ƽ  1:�ߵ�ƽ
    u8 In7;             // ������Ե�ƽ 0:�͵�ƽ  1:�ߵ�ƽ
		u8 In8;             // ������Ե�ƽ 0:�͵�ƽ  1:�ߵ�ƽ
    u8 In9;             // ������Ե�ƽ 0:�͵�ƽ  1:�ߵ�ƽ
    
} INTEST;

typedef struct  {
		u8 Out0;             // ������Ե�ƽ 0:�͵�ƽ  1:�ߵ�ƽ
    u8 Out1;             // ������Ե�ƽ 0:�͵�ƽ  1:�ߵ�ƽ
    u8 Out2;             // ������Ե�ƽ 0:�͵�ƽ  1:�ߵ�ƽ
    u8 Out3;             // ������Ե�ƽ 0:�͵�ƽ  1:�ߵ�ƽ
    
} OUTTEST;

// void IOInit(void);
void OutPortTest(void);
// u8 ReadInfCurtain(void);
// u8 ReadInductCoil(void);
// void LedSet(u8 en);
// void LedToggle(void);

void InPortTest(void);

// #define INTEST char
// #define OUTTEST char

// extern INTEST	InTest;
// extern OUTTEST	OutTest;



#endif


