/**
* @file uartHwInit.c
*/
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <uart.h>
#include <interrupt.h>
#include <clock.h>
#include "xilinx_ut.h"

//base address for mmapped uart
//cadance_UT * const CAD_UART0 = (cadance_UT *)0xE0000000;
cadance_UT *const CAD_UART0 = (cadance_UT *) 0xE0001000;

//UART_RST_CTRL_reg
sclr_UT_rst_ctrl *const UT_rst_ctrl = (sclr_UT_rst_ctrl *) 0xF8000228;

//UART clck/ref control
sclr_UT_clk_ctrl *const UT_clk_ctrl = (sclr_UT_clk_ctrl *) 0xF8000154;

mio_PIN_RX_t *const pinRX = (mio_PIN_RX_t *) 0xF80007B8;
mio_PIN_TX_t *const pinTX = (mio_PIN_TX_t *) 0x000007BC;

int CAD_uart_char_count = 0;

void cadance_uart_init(cadance_UT *uart) {

//[ 1.Reset Controller ]
// Set both slcr.UART_RST_CTRL_reg[UARTx_REF_RST,UARTx_CPU1X_RST] bits = 1.
    UT_rst_ctrl->UART_RST_CTRL_reg = 0x1111;

//[ 2.MIO pins ]
// Configure MIO pin 46 for the RxD signal. and pin 47 for the TxD signal.
//    pinRX->MIO_PIN_46_reg = 0x000012E1;
//    pinTX->MIO_PIN_47_reg = 0x000012E0;

// [3. configure ref clock for UART ]
// Disable UART 1 Reference clock , set divisor = 14
    UT_clk_ctrl->UART_CLK_CTRL_reg = 0x00001401;

//[ 4.Configure Controller Functions ]
//Configure UART character frame
    uart->mode_reg0 = CHAR_FRAME_CAD_UART;
//Configure the Baud Rate
//Disable the Rx path: set uart.Control_reg0 [RXEN] = 0 and [RXDIS] = 1.
    uart->Control_reg0 &= ~(RXEN_CAD_UART);
    uart->Control_reg0 |= (RXDIS_CAD_UART);
//Disable the Txpath: set uart.Control_reg0 [TXEN] = 0 and [TXDIS] = 1
    uart->Control_reg0 &= ~(TXEN_CAD_UART);
    uart->Control_reg0 |= (TXDIS_CAD_UART);

//Write the calculated CD value into the uart.Baud_rate_gen_reg0 [CD] bit field.
//Write the calculated BDIV value into the uart.Baud_rate_divider_reg0 [BDIV] bit value.
//  CD = 62   BDIV =  6
    uart->Baud_rate_gen_reg0 = 62;
    uart->Baud_rdiv_reg0 = 6;

//Reset Tx and Rx paths: uart.Control_reg0 [TXRST] and [RXRST] = 1.
    uart->Control_reg0 |= TXRES_CAD_UART;
    uart->Control_reg0 |= RXRES_CAD_UART;

//Enable the Rx path: Set [RXEN] = 1 and [RXDIS] = 0.
    uart->Control_reg0 |= (RXEN_CAD_UART);
    uart->Control_reg0 &= ~(RXDIS_CAD_UART);

//Enable the Tx path: Set [TXEN] = 1 and [TXDIS] = 0.
    uart->Control_reg0 |= (TXEN_CAD_UART);
    uart->Control_reg0 &= ~(TXDIS_CAD_UART);

// Disable the Rx trigger level:
//uart->Rcvr_FIFO_trigger_l0 = 0 ;
    uart->Rcvr_FIFO_trigger_l0 = 1;

//Enable the Controller
    uart->Control_reg0 = ENABLE_CNTROLLR_CAD_UART;
//set reciever timeout
    uart->Rcvr_timeout_reg0 = 10;


}

devcall uartHwInit(device *devptr) {
    volatile cadance_UT *regptr = devptr->csr;

    //Remove init - does not work on paralella without it
    cadance_uart_init(regptr);
    /* TODO:  It doesn't work without this delay, but why? */
//    udelay(1500);

    /* Register the UART's interrupt handler with XINU's interrupt vector, then
     * actually enable the UART's interrupt line.  */
    interruptVector[devptr->irq] = devptr->intr;
//    enable_irq(devptr->irq);
    //make uart blocking
    uartControl(devptr, UART_CTRL_SET_IFLAG, UART_IFLAG_BLOCK, UART_IFLAG_BLOCK);

    return OK;
}
