/**
 * @file initialize.c
 * The system begins intializing after the C environment has been
 * established.  After intialization, the null thread remains always in
 * a ready (THRREADY) or running (THRCURR) state.
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <kernel.h>
#include <backplane.h>
#include <clock.h>
#include <device.h>
#include <gpio.h>
#include <memory.h>
#include <bufpool.h>
#include <mips.h>
#include <thread.h>
#include <tlb.h>
#include <queue.h>
#include <semaphore.h>
#include <monitor.h>
#include <mailbox.h>
#include <network.h>
#include <nvram.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <syscall.h>
#include <safemem.h>
#include <platform.h>

#ifdef WITH_USB
#  include <usb_subsystem.h>
#endif

/* Function prototypes */
extern thread main(void);       /* main is the first thread created    */
static int sysinit(void);       /* intializes system structures        */

/* Declarations of major kernel variables */
struct thrent thrtab[NTHREAD];  /* Thread table                   */
struct sement semtab[NSEM];     /* Semaphore table                */
struct monent montab[NMON];     /* Monitor table                  */
qid_typ readylist;              /* List of READY threads          */
struct memblock memlist;        /* List of free memory blocks     */
struct bfpentry bfptab[NPOOL];  /* List of memory buffer pools    */

/* Active system status */
int thrcount;                   /* Number of live user threads         */
tid_typ thrcurrent;             /* Id of currently running thread      */

/* Params set by startup.S */
void *memheap;                  /* Bottom of heap (top of O/S stack)   */
ulong cpuid;                    /* Processor id                        */

struct platform platform;       /* Platform specific configuration     */
#include <stdint.h>


/******************************
* MPcore9 GIC
************************/
/*

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

typedef volatile struct {
    uint32_t Control;                 //ICCICR 0x00
    uint32_t PriorityMask;            //ICCPMR 0x04
    uint32_t BinaryPoint ;            //ICCBPR 0x08
    uint32_t Ack         ;            //ICCIAR 0x0C
}MPCore_GIC_Cpui;

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
mp_gicd_ptr->IntSetEnable[0] |= (1 << 29);

//2. configure cpu interface
//ICCPMR - priority mask for cpu interface
mp_giccpu_ptr->PriorityMask = 0xFF;
//ICCBPR - set binary point register for preemption
//ICCICR - enable cpu ineterface
mp_giccpu_ptr->Control = 1 ;
//ICDDCR - enable distributor
mp_gicd_ptr->Control = 1 ;

enable();


}



//Call the service routine for each pending IRQ.

void dispatch2(void)
{
    //get value from bits 9:0  ; mask => 0000 0000 0000 0000 0000 0011 1111 1111  =  0x3FF
    uint32_t irq = mp_giccpu_ptr->Ack & 0x3FF;
    mp_giccpu_ptr->Ack = irq + 32;
//    do
//    {
//        uint irq = 31 - __builtin_clz(status);
//        interruptVector[irq]();
//        status ^= 1U << irq;
//    }
//    while (status);
}

*/
/******************************
* end
********************/

/******************************
* MPcore9 private Timer setup
********************/

#define MPCORE_PERIPHBASE   0xF8F00000

//typedef volatile struct{
//    uint32_t Load;        /* +0x00 */
//    uint32_t Counter;     /* +0x04 */
//    uint32_t Control;     /* +0x08 */
//    uint32_t IntStatus;   /* +0x0C */
//} MPcore_pvt_timer;
/*
#define MPCore_Timer_Enable       1 << 0
#define MPCore_Timer_AutoReload   1 << 1
#define MPCore_Timer_IRQEnable    1 << 2

#define MPCore_Timer_PreScale     16

MPcore_pvt_timer * const timer0 =  (MPcore_pvt_timer *) 0xF8F00600;
uint32_t mp_timer_value = 0;
uint32_t mp_timer_value2 = 0;

void MPCore_timer0_init(){
//    timer0->Load = 25556 ;
    timer0->Load = 10 ;
    mp_timer_value = timer0->Counter;
    //disable auto reload
    timer0->Control &= ~(MPCore_Timer_AutoReload);
    //enable IRQ
    timer0->Control |= MPCore_Timer_IRQEnable ;

//    enable();
    //enable timer
    timer0->Control |= MPCore_Timer_Enable;

    mp_timer_value = timer0->Counter;
    mp_timer_value2 = timer0->Load;
}


       */
/******************************
* END MPcore9 private Timer
********************/

/**********************************
* UART TEST CODE
***********************************/

/*
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

//base address for mmmapped uart
cadance_UT * const CAD_UART0 = (cadance_UT *)0xE0000000;

//UART_RST_CTRL_reg
sclr_UT_rst_ctrl * const UT_rst_ctrl = (sclr_UT_rst_ctrl *)0xF8000228;

//UART clck/ref control
sclr_UT_clk_ctrl * const UT_clk_ctrl = (sclr_UT_clk_ctrl *)0xF8000154;

mio_PIN_RX_t * const pinRX = (mio_PIN_RX_t *) 0xF80007B8 ;
mio_PIN_TX_t * const pinTX = (mio_PIN_TX_t *) 0x000007BC ;

int CAD_uart_char_count = 0;

void cadance_uart_putc(cadance_UT *uart, char c) {
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

int cadance_uart_getc(cadance_UT *uart){
    char c;
    if (1) {
        //Wait until the RxFIFO is filled up to the trigger level
        while (((uart->Channel_sts_reg0 & RTRIG_CAD_UART) == 0) &&
                ((uart->Chnl_int_sts_reg0 & IXR_TOUT_CAD_UART) == 0)) {

        }
        //wait until RxFIFO is not empty
//        while (((uart->Channel_sts_reg0 & RFULL_CAD_UART) == 0)){
//
//        }
        while (((uart->Chnl_int_sts_reg0 & IXR_RXOVR_CAD_UART) == 0)){

        }
        c = uart->TX_RX_FIFO0;
        uart->Chnl_int_sts_reg0 &= ~(IXR_RXOVR_CAD_UART);
        return c;
    }
    return  -1;
}

void cadance_uart_init(cadance_UT *uart) {

//[ 1.Reset Controller ]
// Set both slcr.UART_RST_CTRL_reg[UARTx_REF_RST,UARTx_CPU1X_RST] bits = 1.
UT_rst_ctrl->UART_RST_CTRL_reg =  0x1111 ;

//[ 2.MIO pins ]
// Configure MIO pin 46 for the RxD signal. and pin 47 for the TxD signal.
pinRX->MIO_PIN_46_reg = 0x000012E1 ;
pinTX->MIO_PIN_47_reg = 0x000012E0 ;

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
uart->Baud_rate_gen_reg0 = 62 ;
uart->Baud_rdiv_reg0 = 6 ;

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
uart->Rcvr_FIFO_trigger_l0 = 1 ;

//Enable the Controller
uart->Control_reg0 = ENABLE_CNTROLLR_CAD_UART;
//set reciever timeout
uart->Rcvr_timeout_reg0 = 10;


}
    */
/**
* END OF UART TEST CODE
*/



/**
 * Intializes the system and becomes the null thread.
 * This is where the system begins after the C environment has been 
 * established.  Interrupts are initially DISABLED, and must eventually 
 * be enabled explicitly.  This routine turns itself into the null thread 
 * after initialization.  Because the null thread must always remain ready 
 * to run, it cannot execute code that might cause it to be suspended, wait 
 * for a semaphore, or put to sleep, or exit.  In particular, it must not 
 * do I/O unless it uses kprintf for synchronous output.
 */
void nulluser(void)
{
//    init_GIC();
//    MPCore_timer0_init();

    /*
    cadance_uart_init(CAD_UART0);
    cadance_uart_putc(CAD_UART0, 'a');
    cadance_uart_putc(CAD_UART0, 'b');
    cadance_uart_putc(CAD_UART0, 'c');
    int i = 0 ;
    for (i = 0 ; i < 1000 ; i++){
        cadance_uart_putc(CAD_UART0, 'z');
    }
    cadance_uart_putc(CAD_UART0, 'k');
    cadance_uart_putc(CAD_UART0, '\n');
    int k = 0;
    while (1){
        int ret = cadance_uart_getc(CAD_UART0);
        if(ret == -1)     {
            cadance_uart_putc(CAD_UART0, 'e');
        }else{
            cadance_uart_putc(CAD_UART0, (char) ret);
        }
        if(k == 50) break;
        k++;
    }
    cadance_uart_putc(CAD_UART0, 'n');    */

    /* Platform-specific initialization  */
    platforminit();

    /* General initialization  */
    sysinit();

    /* Enable interrupts  */
    enable();

    /* Spawn the main thread  */
    ready(create(main, INITSTK, INITPRIO, "MAIN", 0), RESCHED_YES);

//    kprintf("init done \n");
    /* null thread has nothing else to do but cannot exit  */
    while (TRUE)
    {
#ifndef DEBUG
        pause();
#endif                          /* DEBUG */
        kprintf("null process \n");
    }
}

/**
 * Intializes all Xinu data structures and devices.
 * @return OK if everything is initialized successfully
 */
static int sysinit(void)
{
    int i;
    struct thrent *thrptr;      /* thread control block pointer  */
    struct memblock *pmblock;   /* memory block pointer          */

    /* Initialize system variables */
    /* Count this NULLTHREAD as the first thread in the system. */
    thrcount = 1;

    /* Initialize free memory list */
    memheap = roundmb(memheap);
    platform.maxaddr = truncmb(platform.maxaddr);
    memlist.next = pmblock = (struct memblock *)memheap;
    memlist.length = (uint)(platform.maxaddr - memheap);
    pmblock->next = NULL;
    pmblock->length = (uint)(platform.maxaddr - memheap);

    /* Initialize thread table */
    for (i = 0; i < NTHREAD; i++)
    {
        thrtab[i].state = THRFREE;
    }

    /* initialize null thread entry */
    thrptr = &thrtab[NULLTHREAD];
    thrptr->state = THRCURR;
    thrptr->prio = 0;
    strlcpy(thrptr->name, "prnull", TNMLEN);
    thrptr->stkbase = (void *)&_end;
    thrptr->stklen = (ulong)memheap - (ulong)&_end;
    thrptr->stkptr = 0;
    thrptr->memlist.next = NULL;
    thrptr->memlist.length = 0;
    thrcurrent = NULLTHREAD;

    /* Initialize semaphores */
    for (i = 0; i < NSEM; i++)
    {
        semtab[i].state = SFREE;
        semtab[i].queue = queinit();
    }

    /* Initialize monitors */
    for (i = 0; i < NMON; i++)
    {
        montab[i].state = MFREE;
    }

    /* Initialize buffer pools */
    for (i = 0; i < NPOOL; i++)
    {
        bfptab[i].state = BFPFREE;
    }

    /* initialize thread ready list */
    readylist = queinit();

#if SB_BUS
    backplaneInit(NULL);
#endif                          /* SB_BUS */

#if RTCLOCK
    /* initialize real time clock */
    clkinit();
#endif                          /* RTCLOCK */

#ifdef UHEAP_SIZE
    /* Initialize user memory manager */
    {
        void *userheap;             /* pointer to user memory heap   */
        userheap = stkget(UHEAP_SIZE);
        if (SYSERR != (int)userheap)
        {
            userheap = (void *)((uint)userheap - UHEAP_SIZE + sizeof(int));
            memRegionInit(userheap, UHEAP_SIZE);

            /* initialize memory protection */
            safeInit();

            /* initialize kernel page mappings */
            safeKmapInit();
        }
    }
#endif

#if USE_TLB
    /* initialize TLB */
    tlbInit();
    /* register system call handler */
    exceptionVector[EXC_SYS] = syscall_entry;
#endif                          /* USE_TLB */

#if NMAILBOX
    /* intialize mailboxes */
    mailboxInit();
#endif

#if NDEVS
    for (i = 0; i < NDEVS; i++)
    {
        devtab[i].init((device*)&devtab[i]);
    }
#endif

#ifdef WITH_USB
    usbinit();
#endif

#if NVRAM
    nvramInit();
#endif

#if NNETIF
    netInit();
#endif

#if GPIO
    gpioLEDOn(GPIO_LED_CISCOWHT);
#endif
    return OK;
}
