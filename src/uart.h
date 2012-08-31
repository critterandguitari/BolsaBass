/*
 * uart.h
 *
 *  Created on: Jul 20, 2012
 *      Author: owen
 */

#ifndef UART_H_
#define UART_H_

#include "stm32f4xx.h"

void uart_init(void);
void put_char (uint8_t data);

#endif /* UART_H_ */
