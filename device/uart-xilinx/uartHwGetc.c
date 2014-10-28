/**
 * @file uartHwPutc.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <uart.h>
#include "xilinx_ut.h"

uchar cadance_uart_getc(cadance_UT *uart){
    uchar c;
    if (1) {
        //Wait until the RxFIFO is filled up to the trigger level
        while (((uart->Channel_sts_reg0 & RTRIG_CAD_UART) == 0) &&
                ((uart->Chnl_int_sts_reg0 & IXR_TOUT_CAD_UART) == 0)) {

        }
        //wait until RxFIFO is not empty
//        while (((uart->Channel_sts_reg0 & RFULL_CAD_UART) == 0)){
//
//        }
        //wait until data byte is available on UART RxFIFO
        while (((uart->Chnl_int_sts_reg0 & IXR_RXOVR_CAD_UART) == 0)){

        }
        c = uart->TX_RX_FIFO0;
        uart->Chnl_int_sts_reg0 &= ~(IXR_RXOVR_CAD_UART);
        return c;
    }
}


uchar uartHwGetc(void *csr)
{
    volatile cadance_UT *regptr = csr;
    return cadance_uart_getc(regptr);
}
