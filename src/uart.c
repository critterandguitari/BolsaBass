/*
 * uart.c
 *
 *  Created on: Jul 20, 2012
 *      Author: owen
 */

#include "uart.h"


void uart_init(void){
	GPIO_InitTypeDef GPIO_InitStructure;

	USART_InitTypeDef USART_InitStructure;

	//configure clock for USART, GPIO
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_USART1, ENABLE);
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOB, ENABLE);

	/*-------------------------- GPIO Configuration ----------------------------*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Connect USART pins to AF */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1); // USART1_TX
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1); // USART1_RX

	/* UART 1 config */
	USART_InitStructure.USART_BaudRate = 31250;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init (USART1, &USART_InitStructure);
	/* Enable USART */
	USART_Cmd (USART1, ENABLE);
}

void put_char (uint8_t data) {
	while (USART_GetFlagStatus (USART1, USART_FLAG_TXE) == RESET);
	USART_SendData (USART1, data);
}
