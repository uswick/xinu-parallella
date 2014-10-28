/*
 * @file kgetc.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <uart.h>
#include "xilinx_ut.h"

/**
 * @ingroup uarthardware
 *
 * Synchronously read a character from a UART.  This blocks until a character is
 * available.  The interrupt handler is not used.
 *
 * @param devptr
 *      Pointer to the device table entry for a UART.
 *
 * @return
 *      The character read from the UART as an <code>unsigned char</code> cast
 *      to an <code>int</code>.
 */
syscall kgetc(device *devptr)
{
    volatile cadance_UT *regptr;
    struct uart *uartptr;
    uint uart_im;
    uchar c;

    /* Get pointers to the UART and to its registers.  */
    uartptr = &uarttab[devptr->minor];
    regptr = devptr->csr;

    /* TODO
     *   Save the UART's interrupt state and disable the UART's interrupts.  Note:
     * we do not need to disable global interrupts here; only UART interrupts
     * must be disabled, to prevent race conditions with the UART interrupt
     * handler.  */
//    uart_im = regptr->imsc;
//    regptr->imsc = 0;

    /* Get the next character from the UART by reading it from the Data
     * Register.  */
    c = cadance_uart_getc(regptr);

    /* Tally one character received.  */
    uartptr->cin++;

    /* Restore UART interrupts and return the read character.  */
//    regptr->imsc = uart_im;
    return c;
}
