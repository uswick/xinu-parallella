/**
 * @file arm_gic.c
 *
 * Driver for the ARM® Generic Interrupt Controller .
 *
 * This was written using the official manual from ARM Ltd.:
 *
 *  Regference "ARM® Generic Interrupt Controller [Architecture version 1.0] Architecture Specification
 *  Manual
 *
 */
/* Embedded Xinu, Copyright (C) 2014.  All rights reserved. */

#include "arm_gic.h"


/******************************
* MPcore9 GIC
************************/

MPCore_GIC_Distb* mp_gicd_ptr = (MPCore_GIC_Distb*) 0xF8F01000;
MPCore_GIC_Cpui* mp_giccpu_ptr = (MPCore_GIC_Cpui*) 0xF8F00100;


void init_GIC() {

//1.configure distributor
//set ICDICFR1 - edge trigerred

//ICDIPR  - interrpt priority


//set interrupt priority for SCU private timer0
// Timer PPI/IRQ ID = 29 ; M = 29 / 4 = 7 ; byte_offset = 29 mod 4 = 1 ;
// correspoding bitmask for priority = 0 (highest) ==> 1111 1111 ...0000 0000 1111 1111 = 0xFFFF00FF
    mp_gicd_ptr->IntPriority[7] = mp_gicd_ptr->IntPriority[7] & 0xFFFF00FF  ;


//ICDISER0 - enable PPIs  Timer PPI/IRQ ID = 29 ==> set bit 29
// TODO remove this and use enable_irq() from timer0
//    mp_gicd_ptr->IntSetEnable[0] |= (1 << 29);


//2. configure cpu interface
//ICCPMR - priority mask for cpu interface
    mp_giccpu_ptr->PriorityMask = 0xFF;


//ICCBPR - set binary point register for preemption
//ICCICR - enable cpu ineterface
    mp_giccpu_ptr->Control = 1 ;


//ICDDCR - enable distributor
    mp_gicd_ptr->Control = 1 ;

    //should clear processor interrupt mask to actually enable interrupts
//    enable();


}

interrupt_handler_t interruptVector[32];

/**
* Call the service routine for each pending IRQ.
*/
void dispatch(void)
{
    //get value from bits 9:0  ; mask => 0000 0000 0000 0000 0000 0011 1111 1111  =  0x3FF
    uint32_t irq = mp_giccpu_ptr->Ack & 0x3FF;
//    mp_giccpu_ptr->Ack = irq + 32;
    interruptVector[irq]();
    mp_giccpu_ptr->EOInt = irq ;
}

/**
 * Enable an interrupt request line.
 *
 * @param irq
 *      Number of the IRQ to enable, which on this platform must be in the
 *      range [0, 31].
 */
void enable_irq(irqmask irq)
{
    mp_gicd_ptr->IntSetEnable[0] |= (1 << irq);

}

/**
 * Disable an interrupt request line.
 *
 * @param irq
 *      Number of the IRQ to disable, which on this platform must be in the
 *      range [0, 31].
 */
void disable_irq(irqmask irq)
{
    mp_gicd_ptr->IntClearEnable[0] |= (1 << irq);
}


