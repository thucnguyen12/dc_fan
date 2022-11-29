#ifndef __UART_HANDLER_H__
#define __UART_HANDLER_H__

#include "apm32f00x.h"
#include "apm32f00x_usart.h"
#include "apm32f00x_gpio.h"
#include "apm32f00x_rcm.h"



void InitUART(USART_T* USARTx, uint32_t BaudRate);
void UART_Putc(USART_T* USARTx, uint8_t c);
void UART_Puts(USART_T* USARTx, const char *str);
void UART_Putb(USART_T* USARTx, uint8_t *Data, uint16_t Length);

void UPrintf(USART_T* USARTx, const char* str, ...);

#endif // __UART_HANDLER_H__

