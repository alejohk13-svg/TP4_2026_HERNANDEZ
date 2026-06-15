#ifndef GLOBAL_H_
#define GLOBAL_H_

#include "stm32f4xx.h"

void sysTime_Init(void);
void SysTick_Handler(void);
void delay_ms(uint32_t ms);
void delay_us(uint32_t us);
uint32_t getSystick(void);
uint32_t getSeconds(void);

#endif
