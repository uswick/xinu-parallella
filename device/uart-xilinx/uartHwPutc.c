/**
 * @file uartHwPutc.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <uart.h>
#include "xilinx_ut.h"

void cadance_uart_putc(cadance_UT *uart, uchar c) {
//    while((uart->Channel_sts_reg0 & TEMPTY_CAD_UART) != 1){
    //wait
//    }
    if(CAD_uart_char_count < 64){
        uart->TX_RX_FIFO0 = c ;
        CAD_uart_char_count++;
    }else {
        while((uart->Channel_sts_reg0 & TEMPTY_CAD_UART) == 0){
            //wait
        }
        CAD_uart_char_count = 0;
        uart->TX_RX_FIFO0 = c ;
        CAD_uart_char_count++;

    }

}


void uartHwPutc(void *csr, uchar c)
{
    volatile cadance_UT *regptr = csr;

    cadance_uart_putc(regptr, c);
    struct uart *uartptr;
    uartptr = &uarttab[devtab[SERIAL0].minor];
    uartptr->oidle = TRUE ;

}
