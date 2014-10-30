/**
 * @file scu_mpcore9.c
 *
 * Driver for the ARM CortexA9 MPCore9 SCU private Timer Module (SP804).
 *
 * This was written using the official manual from ARM Ltd.:
 *
 *  "Zynq Paralella - ARM CortexA9 MPCore9 SCU private Timer
 *
 * This driver uses the first timer in the duo as a oneshot timer for
 * clkupdate() and the second timer in the duo as a free-running counter for
 * clkcount().
 *
 */
/* Embedded Xinu, Copyright (C) 2014.  All rights reserved. */

#include <clock.h>
#include <stdint.h>
#import "interrupt.h"

typedef volatile struct{
    uint32_t Load;        /* +0x00 */
    uint32_t Counter;     /* +0x04 */
    uint32_t Control;     /* +0x08 */
    uint32_t IntStatus;   /* +0x0C */
} MPcore_pvt_timer;

#define MPCore_Timer_Enable       1 << 0
#define MPCore_Timer_AutoReload   1 << 1
#define MPCore_Timer_IRQEnable    1 << 2

#define MPCore_Timer_PreScale     16

MPcore_pvt_timer * const timer0 =  (MPcore_pvt_timer *) 0xF8F00600;
uint32_t mp_timer_value = 0;

void MPCore_timer0_update(ulong cycles){
    /* Clear the timer IRQ if pending. writing 1 will clear it  */
    timer0->IntStatus = 1;
//    timer0->Load = 25556 ;
    timer0->Load = cycles ;
//    mp_timer_value = timer0->Counter;
    //disable auto reload --> single shot
//    timer0->Control &= ~(MPCore_Timer_AutoReload);
    //enable IRQ
//    timer0->Control |= MPCore_Timer_IRQEnable ;

//    enable();
    //enable timer
//    timer0->Control |= MPCore_Timer_Enable;
//    mp_timer_value = timer0->Counter;
//    enable();
}

void MPCore_timer0_init(){
    //init a second timer for timing/delays
    timer0->Load = 0 ;
    timer0->Counter = 0 ;
    timer0->IntStatus = 1;

    uint32_t prescale = 0xFF;
//    timer0->Load = cycles ;
//    mp_timer_value = timer0->Counter;
    //disable auto reload --> single shot
//    timer0->Control &= ~(MPCore_Timer_AutoReload);
//    timer0->Control = (prescale << 8) | MPCore_Timer_AutoReload | MPCore_Timer_IRQEnable ;
    timer0->Control = (prescale << 8) | MPCore_Timer_IRQEnable ;
    //enable IRQ
    enable_irq(IRQ_TIMER);
//    timer0->Control |= MPCore_Timer_IRQEnable ;

    //enable timer
    timer0->Load = 100 ;
    timer0->Control |= MPCore_Timer_Enable;
}


//void sp804_init(void)
//{
//    /* Enable the second timer (free-running, no IRQ)  */
//    regs->timers[1].Control = SP804_TIMER_ENABLE | SP804_TIMER_32BIT;
//}

/* clkcount() interface is documented in clock.h  */
ulong clkcount(void)
{
    /* TODO include a second timer for wait/delay loops */
    unsigned long v = 1000 ;
    return (ulong)v;
}

/* clkupdate() interface is documented in clock.h  */
void clkupdate(ulong cycles)
{
    MPCore_timer0_update(cycles);
}
