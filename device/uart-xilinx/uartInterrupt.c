/**
 * @file uartInterrupt.c
 */

/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <uart.h>
#include <thread.h>
#include "xilinx_ut.h"

/**
 * @ingroup uarthardware
 *
 * Handle an interrupt request from a PL011 UART.
 */
interrupt uartInterrupt(void)
{
    uint u;

    /* Set resdefer to prevent other threads from being scheduled before this
     * interrupt handler finishes.  This prevents this interrupt handler from
     * being executed re-entrantly.  */
    extern int resdefer;
    resdefer = 1;

    /* Check for interrupts on each UART.  Note: this assumes all the UARTs in
     * 'uarttab' are PL011 UARTs.  */
    for (u = 0; u < NUART; u++)
    {
        uint mis, count;
        uchar c;
        volatile cadance_UT *regptr ;
        struct uart *uartptr;

        /* Get a pointer to the UART structure and a pointer to the UART's
         * hardware registers.  */
        uartptr = &uarttab[u];
        regptr = uartptr->csr;
    }

    /* Now that the UART interrupt handler is finished, we can safely wake up
     * any threads that were signaled.  */
    if (--resdefer > 0)
    {
        resdefer = 0;
        resched();
    }
}
