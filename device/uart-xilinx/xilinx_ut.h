/**
 * @file xilinx_ut.h
 */

/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */
#include <stdint.h>

#ifndef _XILINX_UT_H_
#define _XILINX_UT_H_


typedef volatile struct {
    uint32_t Control_reg0;       //0x00000000
    uint32_t mode_reg0;          //0x00000004
    uint32_t Intrpt_en_reg0;     //0x00000008
    uint32_t Intrpt_dis_reg0;    //0x0000000C
    uint32_t Intrpt_mask_reg0;    //0x00000010
    uint32_t Chnl_int_sts_reg0;   //0x00000014
    uint32_t Baud_rate_gen_reg0;  //0x00000018
    uint32_t Rcvr_timeout_reg0;   //0x0000001C
    uint32_t Rcvr_FIFO_trigger_l0;//0x00000020
    uint32_t Modem_ctrl_reg0;     //0x00000024
    uint32_t Modem_sts_reg0;      //0x00000028
    uint32_t Channel_sts_reg0;    //0x0000002C
    uint32_t TX_RX_FIFO0;         //0x00000030
    uint32_t Baud_rdiv_reg0;      //0x00000034
    uint32_t Flow_delay_reg0;     //0x00000038
    uint32_t Tx_FIFO_tl_reg0;     //0x00000044
} cadance_UT;

typedef volatile struct {
    uint32_t UART_RST_CTRL_reg;
}  sclr_UT_rst_ctrl ;

typedef volatile struct {
    uint32_t MIO_PIN_46_reg;
}  mio_PIN_RX_t ;

typedef volatile struct {
    uint32_t MIO_PIN_47_reg;
}  mio_PIN_TX_t ;

typedef volatile struct {
    uint32_t UART_CLK_CTRL_reg;
}  sclr_UT_clk_ctrl ;

#define    CHAR_FRAME_CAD_UART  0x00000020
#define     ENABLE_CNTROLLR_CAD_UART  0x00000117
#define     TXFF  0x20
#define     RXRES_CAD_UART  1 << 0
#define     TXRES_CAD_UART  1 << 1
#define     RXEN_CAD_UART   1 << 2
#define     RXDIS_CAD_UART  1 << 3
#define     TXEN_CAD_UART   1 << 4
#define     TXDIS_CAD_UART  1 << 5

#define     TEMPTY_CAD_UART 1 << 3
#define     RTRIG_CAD_UART  1 << 0
#define     REMPTY_CAD_UART 1 << 1
#define     RFULL_CAD_UART  1 << 2
#define     IXR_TOUT_CAD_UART 1 << 8
#define     IXR_RXOVR_CAD_UART 1 << 0

#define     UART0_CPU1X_RST 1 << 0
#define     UART1_CPU1X_RST 1 << 1
#define     UART0_REF_RST   1 << 2
#define     UART1_REF_RST   1 << 3
#define     UT_EOF 1

extern int CAD_uart_char_count;

uchar cadance_uart_getc(cadance_UT *uart);
void cadance_uart_putc(cadance_UT *uart, uchar c);

#endif                          /* XILINX_H */
