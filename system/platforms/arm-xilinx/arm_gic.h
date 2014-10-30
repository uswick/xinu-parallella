/**
 * @file pl190.c
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
#ifndef _ARM_GIC_v1_H_
#define _ARM_GIC_v1_H_

#include <interrupt.h>
#include <stdint.h>


/******************************
* MPcore9 GIC
************************/

void init_GIC();

typedef volatile struct {
    uint32_t Control;           //ICDDCR  0x000
    uint32_t IntCtrlType;     //ICDICTR  0x004
    uint32_t Distb_ID;        //ICDIIDR  0x008
    uint32_t Reserved[29];
    uint32_t Secure[32];        //ICDISR  0x080 - 0x0FC
    uint32_t IntSetEnable[32];  //ICDISERn 0x100-0x17C
    uint32_t IntClearEnable[32];  //ICDICERn 0x180-0x1FC
    uint32_t IntSetPending[32];  //ICDISPR 0x200-0x27C
    uint32_t IntClearPending[32];  //ICDICPR 0x280-0x2FC
    uint32_t ActiveBit[32];     //ICDIABR 0x300-0x37C
    uint32_t Reserved2[32];     // 0x380-0x3FC
    uint32_t IntPriority[255];  // ICDIPR 0x400-0x7F8
    uint32_t Reserved3;         // 0x7FC
    uint32_t IntTargets[255];  // ICDIPTR 0x800-0x81C - 0x820-0xBF8
    uint32_t Reserved4;         // 0xBFC
    uint32_t IntConfig[64];     // ICDICFR 0xC00-0xCFC
    uint32_t Unkown[64];         // Implementation defined 0xD00-0xDFC
    uint32_t Reserved5[64];     // reserved 0xE00-0xEFC
    uint32_t SGI;              // ICDSGIR 0xF00
} MPCore_GIC_Distb;

/**
* CPU interfaces - banked
*/
typedef volatile struct {
    uint32_t Control;                 //ICCICR 0x00
    uint32_t PriorityMask;            //ICCPMR 0x04
    uint32_t BinaryPoint ;            //ICCBPR 0x08
    uint32_t Ack         ;            //ICCIAR 0x0C
    uint32_t EOInt         ;          //ICCIAR 0x10
}MPCore_GIC_Cpui;

#endif