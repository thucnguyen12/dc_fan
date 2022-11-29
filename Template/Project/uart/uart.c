/******************************************************************************
 * @file    	UART_Hander.c
 * @author  	-------------
 * @version 	V1.0.0
 * @date    	10/11/2014
 * @brief   	
 ******************************************************************************/


/******************************************************************************
                                   INCLUDES					    			 
 ******************************************************************************/
#include "uart.h"
#include <stdarg.h>

/******************************************************************************
                                   GLOBAL VARIABLES					    			 
 ******************************************************************************/


/******************************************************************************
                                   GLOBAL FUNCTIONS					    			 
 ******************************************************************************/


/******************************************************************************
                                   DATA TYPE DEFINE					    			 
 ******************************************************************************/
/* These types must be 16-bit, 32-bit or larger integer */
typedef int INT;
typedef unsigned int UINT;

/* These types must be 8-bit integer */
typedef signed char CHAR;
typedef unsigned char UCHAR;
typedef unsigned char BYTE;

/* These types must be 16-bit integer */
typedef short SHORT;
typedef unsigned short USHORT;
typedef unsigned short WORD;
typedef unsigned short WCHAR;

/* These types must be 32-bit integer */
typedef long LONG;
typedef unsigned long ULONG;
typedef unsigned long DWORD;

/******************************************************************************
                                   PRIVATE VARIABLES					    			 
 ******************************************************************************/



/******************************************************************************
                                   LOCAL FUNCTIONS					    			 
 ******************************************************************************/

/*****************************************************************************/
/**
 * @brief	:  
 * @param	:  
 * @retval	:
 * @author	:	-------------
 * @created	:	10/11/2014
 * @version	:
 * @reviewer:	
 */
static uint8_t UART_Inited=0; 
 
void InitUART(USART_T* USARTx, uint32_t BaudRate) 
{
	USART_Config_T USART_Cfg;
	GPIO_Config_T gpioConfig;
	
	RCM_EnableAPBPeriphClock(RCM_PERIPH_USART1);
	
	if(USART1 == USARTx)
	{
		gpioConfig.mode = GPIO_MODE_OUT_PP;
		gpioConfig.speed = GPIO_SPEED_10MHz;
		gpioConfig.pin = GPIO_PIN_5;

		GPIO_Config(GPIOD, &gpioConfig);
	}
	
	USART_Cfg.baudRate 	= BaudRate;
	USART_Cfg.mode		= USART_MODE_TX;
	USART_Cfg.interrupt = USART_INT_NONE;
	USART_Cfg.parity	= USART_PARITY_NONE;
	USART_Cfg.stopBits	= USART_STOP_BIT_1;
	USART_Cfg.wordLength = USART_WORD_LEN_8B;
	
	USART_Config(USART1,&USART_Cfg);
	USART_Enable(USART1);
	USART_TxData8(USART1,'A');
	UART_Inited=1;
}
/*****************************************************************************/
/**
 * @brief	:  
 * @param	:  
 * @retval	:
 * @author	:	-------------
 * @created	:	10/11/2014
 * @version	:
 * @reviewer:	
 */
void UART_Putc(USART_T* USARTx, uint8_t c) 
{
	while(USART_ReadStatusFlag(USART1,USART_FLAG_TXBE) == RESET);
	USART_TxData8(USART1,c);
}
/*****************************************************************************/
/**
 * @brief	:  
 * @param	:  
 * @retval	:
 * @author	:	-------------
 * @created	:	10/11/2014
 * @version	:
 * @reviewer:	
 */
void UART_Puts(USART_T* USARTx, const char *str)
{
    while(*str != '\0')
    {		
		UART_Putc(USARTx, (uint16_t) (*str));
		str++;
    }	
}

/*****************************************************************************/
/**
 * @brief	:  
 * @param	:  
 * @retval	:
 * @author	:	-------------
 * @created	:	10/11/2014
 * @version	:
 * @reviewer:	
 */
void UART_Putb(USART_T* USARTx, uint8_t *Data, uint16_t Length)
{
	uint16_t i;
	
	for(i = 0; i < Length; i++)
	{
		UART_Putc(USARTx,Data[i]);
	}
}

/*****************************************************************************/
/**
 * @brief	:  
 * @param	:  
 * @retval	:
 * @author	:	-------------
 * @created	:	10/11/2014
 * @version	:
 * @reviewer:	
 */
void UART_itoa(USART_T* USARTx, long val, int radix, int len)
{
	BYTE c, r, sgn = 0, pad = ' ';
    BYTE s[20], i = 0;
    DWORD v;


    if(radix < 0)
    {
        radix = -radix;
        if(val < 0)
        {
            val = -val;
            sgn = '-';
        }
    }
    v = val;
    r = radix;
    if(len < 0)
    {
        len = -len;
        pad = '0';
    }
    if(len > 20) return;
    do
    {
        c = (BYTE) (v % r);
        if(c >= 10) c += 7;
        c += '0';
        s[i++] = c;
        v /= r;
    } while(v);
    if(sgn) s[i++] = sgn;
    while(i < len)
        s[i++] = pad;
    do
        UART_Putc(USARTx,s[--i]); while(i);
}
/*****************************************************************************/
/**
 * @brief	:  
 * @param	:  
 * @retval	:
 * @author	:	-------------
 * @created	:	10/11/2014
 * @version	:
 * @reviewer:	
 */
void UPrintf(USART_T* USARTx,const char* str, ...)
{			
    va_list arp;
    int d, r, w, s, l, i;
	if(UART_Inited==0) return;
	// Check if only string
	for(i = 0;; i++)
	{
		if(str[i] == '%') break;
		if(str[i] == '\0')
		{
			UART_Puts(USARTx, str);
			return;
		}
	}
	
    va_start(arp, str);

    while((d = *str++) != 0)
    {
        if(d != '%')
        {
            UART_Putc(USARTx,d);
            continue;
        }
        d = *str++;
        w = r = s = l = 0;
        if(d == '0')
        {
            d = *str++;
            s = 1;
        }
        while((d >= '0') && (d <= '9'))
        {
            w += w * 10 + (d - '0');
            d = *str++;
        }
        if(s) w = -w;
        if(d == 'l')
        {
            l = 1;
            d = *str++;
        }
        if(!d) break;
        if(d == 's')
        {
            UART_Puts(USARTx,va_arg(arp, char*));
            continue;
        }
        if(d == 'c')
        {
            UART_Putc(USARTx,(char) va_arg(arp, int));
            continue;
        }
        if(d == 'u') r = 10;
        if(d == 'd') r = -10;
        if(d == 'X' || d == 'x') r = 16; // 'x' added by mthomas in increase compatibility
        if(d == 'b') r = 2;
        if(!r) break;
        if(l)
        {
            UART_itoa(USARTx,(long) va_arg(arp, long), r, w);
        }
        else
        {
            if(r > 0)
                UART_itoa(USARTx,(unsigned long) va_arg(arp, int), r, w);
            else
                UART_itoa(USARTx,(long) va_arg(arp, int), r, w);
        }
    }

    va_end(arp);
}

/********************************* END OF FILE *******************************/
