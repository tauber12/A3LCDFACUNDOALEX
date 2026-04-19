/*
 * delay.h
 *
 *  Created on: Apr 15, 2026
 *      Author: alexm
 */

#ifndef SRC_DELAY_H_
#define SRC_DELAY_H_

#include "stm32l4xx_hal.h"

void SysTick_Init(void);
void delay_us(const uint32_t time_us);

#endif /* SRC_DELAY_H_ */
