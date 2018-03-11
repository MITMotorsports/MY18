/**************************************************************************//**
 * @file     system_LPC11xx.c
 * @brief   CMSIS Cortex-M0 Device Peripheral Access Layer Source File
 *         for the NXP LPC11xx Device Series
 * @version  V1.02
 * @date     21. October 2009
 *
 * @note
 * Copyright (C) 2009 ARM Limited. All rights reserved.
 *
 * @par
 * ARM Limited (ARM) is supplying this software for use with Cortex-M
 * processor based microcontrollers.  This file can be freely distributed
 * within development tools that are supporting such ARM based processors.
 *
 * @par
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ******************************************************************************/


#include <stdint.h>
#include "chip.h"
#include "sysinit.h"

/*
//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------
*/

/*--------------------- Clock Configuration ----------------------------------
//
// <e> Clock Configuration
//   <e1> System Clock Setup
//   <e2> System Oscillator Enable
//     <o3.1> Select System Oscillator Frequency Range
//                   <0=> 1 - 20 MHz
//                   <1=> 15 - 25 MHz
//   </e2>
//   <e4> Watchdog Oscillator Enable
//     <o5.0..4> Select Divider for Fclkana
//                   <0=>   2 <1=>   4 <2=>   6 <3=>   8
//                   <4=>  10 <5=>  12 <6=>  14 <7=>  16
//                   <8=>  18 <9=>  20 <10=> 22 <11=> 24
//                   <12=> 26 <13=> 28 <14=> 30 <15=> 32
//                   <16=> 34 <17=> 36 <18=> 38 <19=> 40
//                   <20=> 42 <21=> 44 <22=> 46 <23=> 48
//                   <24=> 50 <25=> 52 <26=> 54 <27=> 56
//                   <28=> 58 <29=> 60 <30=> 62 <31=> 64
//     <o5.5..8> Select Watchdog Oscillator Analog Frequency (Fclkana)
//                   <0=> Disabled
//                   <1=> 0.5 MHz
//                   <2=> 0.8 MHz
//                   <3=> 1.1 MHz
//                   <4=> 1.4 MHz
//                   <5=> 1.6 MHz
//                   <6=> 1.8 MHz
//                   <7=> 2.0 MHz
//                   <8=> 2.2 MHz
//                   <9=> 2.4 MHz
//                   <10=> 2.6 MHz
//                   <11=> 2.7 MHz
//                   <12=> 2.9 MHz
//                   <13=> 3.1 MHz
//                   <14=> 3.2 MHz
//                   <15=> 3.4 MHz
//   </e4>
//   <o6> Select Input Clock for sys_pllclkin (Register: SYSPLLCLKSEL)
//                   <0=> IRC Oscillator
//                   <1=> System Oscillator
//                   <2=> WDT Oscillator
//                   <3=> Invalid
//   <e7> Use System PLL
//                   <i> F_pll = M * F_in
//                   <i> F_in must be in the range of 10 MHz to 25 MHz
//     <o8.0..4>   M: PLL Multiplier Selection
//                   <1-32><#-1>
//     <o8.5..6>   P: PLL Divider Selection
//                   <0=> 2
//                   <1=> 4
//                   <2=> 8
//                   <3=> 16
//     <o8.7>     DIRECT: Direct CCO Clock Output Enable
//     <o8.8>     BYPASS: PLL Bypass Enable
//   </e7>
//   <o9> Select Input Clock for Main clock (Register: MAINCLKSEL)
//                   <0=> IRC Oscillator
//                   <1=> Input Clock to System PLL
//                   <2=> WDT Oscillator
//                   <3=> System PLL Clock Out
//   </e1>
//   <o14.0..7> System AHB Divider <0-255>
//                   <i> 0 = is disabled
//   <o15.0>   SYS Clock Enable
//   <o15.1>   ROM Clock Enable
//   <o15.2>   RAM Clock Enable
//   <o15.3>   FLASH1 Clock Enable
//   <o15.4>   FLASH2 Clock Enable
//   <o15.5>   I2C Clock Enable
//   <o15.6>   GPIO Clock Enable
//   <o15.7>   CT16B0 Clock Enable
//   <o15.8>   CT16B1 Clock Enable
//   <o15.9>   CT32B0 Clock Enable
//   <o15.10>  CT32B1 Clock Enable
//   <o15.11>  SSP Clock Enable
//   <o15.12>  UART Clock Enable
//   <o15.13>  ADC Clock Enable
//   <o15.15>  SWDT Clock Enable
//   <o15.16>  IOCON Clock Enable
// </e>
*/
#define CLOCK_SETUP        1
#define SYSCLK_SETUP          1
#define SYSOSC_SETUP          0
#define SYSOSCCTRL_Val      0x0
#define WDTOSC_SETUP          0
#define WDTOSCCTRL_Val      0xA0
#define SYSPLLCLKSEL_Val      0x0
#define SYSPLL_SETUP          1
#define SYSPLLCTRL_Val      0x23
#define MAINCLKSEL_Val      0x3
#define SYSAHBCLKDIV_Val      0x1
#define AHBCLKCTRL_Val      0x1005F

/*--------------------- Memory Mapping Configuration -------------------------
//
// <e> Memory Mapping
//   <o1.0..1> System Memory Remap (Register: SYSMEMREMAP)
//                   <0=> Bootloader mapped to address 0
//                   <1=> RAM mapped to address 0
//                   <2=> Flash mapped to address 0
//                   <3=> Flash mapped to address 0
// </e>
 */
#if D__RAM_MODE__
#define MEMMAP_SETUP          1
#else
#define MEMMAP_SETUP          0
#endif
#define SYSMEMREMAP_Val    0x1

/*
//-------- <<< end of configuration section >>> ------------------------------
*/

/*----------------------------------------------------------------------------
  Check the register settings
 *----------------------------------------------------------------------------*/
#define CHECK_RANGE(val, min, max)              ((val < min) || (val > max))
#define CHECK_RSVD(val, mask)                    (val & mask)

/* Clock Configuration -------------------------------------------------------*/
#if (CHECK_RSVD((SYSOSCCTRL_Val),  ~0x3))
   #error "SYSOSCCTRL: Invalid values of reserved bits!"
#endif

#if (CHECK_RSVD((WDTOSCCTRL_Val),  ~0x1FF))
   #error "WDTOSCCTRL: Invalid values of reserved bits!"
#endif

#if (CHECK_RANGE((SYSPLLCLKSEL_Val), 0, 2))
   #error "SYSPLLCLKSEL: Value out of range!"
#endif

#if (CHECK_RSVD((SYSPLLCTRL_Val),  ~0x1FF))
   #error "SYSPLLCTRL: Invalid values of reserved bits!"
#endif

#if (CHECK_RSVD((MAINCLKSEL_Val),  ~0x3))
   #error "MAINCLKSEL: Invalid values of reserved bits!"
#endif

#if (CHECK_RANGE((SYSAHBCLKDIV_Val), 0, 255))
   #error "SYSAHBCLKDIV: Value out of range!"
#endif

#if (CHECK_RSVD((AHBCLKCTRL_Val),  ~0x1FFFF))
   #error "AHBCLKCTRL: Invalid values of reserved bits!"
#endif

#if (CHECK_RSVD((SYSMEMREMAP_Val), ~0x3))
   #error "SYSMEMREMAP: Invalid values of reserved bits!"
#endif


/*----------------------------------------------------------------------------
  DEFINES
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/
#define __XTAL          (12000000UL)    /* Oscillator frequency          */
#define __SYS_OSC_CLK    (  __XTAL) /* Main oscillator frequency        */
#define __IRC_OSC_CLK    (12000000UL)   /* Internal RC oscillator frequency */


#define __FREQSEL   ((WDTOSCCTRL_Val >> 5) & 0xF)
#define __DIVSEL   (((WDTOSCCTRL_Val & 0x1F) << 1) + 2)

#if (CLOCK_SETUP)                        /* Clock Setup           */
  #if (SYSCLK_SETUP)                      /* System Clock Setup    */
    #if (WDTOSC_SETUP)                  /* Watchdog Oscillator Setup*/
        #if  (__FREQSEL ==  0)
          #define __WDT_OSC_CLK     ( 400000 / __DIVSEL)
        #elif (__FREQSEL ==  1)
          #define __WDT_OSC_CLK     ( 500000 / __DIVSEL)
        #elif (__FREQSEL ==  2)
          #define __WDT_OSC_CLK     ( 800000 / __DIVSEL)
        #elif (__FREQSEL ==  3)
          #define __WDT_OSC_CLK     (1100000 / __DIVSEL)
        #elif (__FREQSEL ==  4)
          #define __WDT_OSC_CLK     (1400000 / __DIVSEL)
        #elif (__FREQSEL ==  5)
          #define __WDT_OSC_CLK     (1600000 / __DIVSEL)
        #elif (__FREQSEL ==  6)
          #define __WDT_OSC_CLK     (1800000 / __DIVSEL)
        #elif (__FREQSEL ==  7)
          #define __WDT_OSC_CLK     (2000000 / __DIVSEL)
        #elif (__FREQSEL ==  8)
          #define __WDT_OSC_CLK     (2200000 / __DIVSEL)
        #elif (__FREQSEL ==  9)
          #define __WDT_OSC_CLK     (2400000 / __DIVSEL)
        #elif (__FREQSEL == 10)
          #define __WDT_OSC_CLK     (2600000 / __DIVSEL)
        #elif (__FREQSEL == 11)
          #define __WDT_OSC_CLK     (2700000 / __DIVSEL)
        #elif (__FREQSEL == 12)
          #define __WDT_OSC_CLK     (2900000 / __DIVSEL)
        #elif (__FREQSEL == 13)
          #define __WDT_OSC_CLK     (3100000 / __DIVSEL)
        #elif (__FREQSEL == 14)
          #define __WDT_OSC_CLK     (3200000 / __DIVSEL)
        #else
          #define __WDT_OSC_CLK     (3400000 / __DIVSEL)
        #endif
    #else
          #define __WDT_OSC_CLK     (1600000 / 2)
    #endif  // WDTOSC_SETUP

    /* sys_pllclkin calculation */
    #if   ((SYSPLLCLKSEL_Val & 0x3) == 0)
      #define __SYS_PLLCLKIN           (__IRC_OSC_CLK)
    #elif ((SYSPLLCLKSEL_Val & 0x3) == 1)
      #define __SYS_PLLCLKIN           (__SYS_OSC_CLK)
    #elif ((SYSPLLCLKSEL_Val & 0x3) == 2)
      #define __SYS_PLLCLKIN           (__WDT_OSC_CLK)
    #else
      #define __SYS_PLLCLKIN           (0)
    #endif

    #if (SYSPLL_SETUP)                  /* System PLL Setup      */
      #define  __SYS_PLLCLKOUT       (__SYS_PLLCLKIN * ((SYSPLLCTRL_Val & 0x1F) + 1))
    #else
      #define  __SYS_PLLCLKOUT       (__SYS_PLLCLKIN * (1))
    #endif  // SYSPLL_SETUP

    /* main clock calculation */
    #if   ((MAINCLKSEL_Val & 0x3) == 0)
      #define __MAIN_CLOCK           (__IRC_OSC_CLK)
    #elif ((MAINCLKSEL_Val & 0x3) == 1)
      #define __MAIN_CLOCK           (__SYS_PLLCLKIN)
    #elif ((MAINCLKSEL_Val & 0x3) == 2)
      #define __MAIN_CLOCK           (__WDT_OSC_CLK)
    #elif ((MAINCLKSEL_Val & 0x3) == 3)
      #define __MAIN_CLOCK           (__SYS_PLLCLKOUT)
    #else
      #define __MAIN_CLOCK           (0)
    #endif

    #define __SYSTEM_CLOCK           (__MAIN_CLOCK / SYSAHBCLKDIV_Val)

  #else // SYSCLK_SETUP
    #if (SYSAHBCLKDIV_Val == 0)
      #define __SYSTEM_CLOCK           (0)
    #else
      #define __SYSTEM_CLOCK           (__XTAL / SYSAHBCLKDIV_Val)
    #endif
  #endif // SYSCLK_SETUP

#else
  #define __SYSTEM_CLOCK               (__XTAL)
#endif  // CLOCK_SETUP


/*----------------------------------------------------------------------------
  Clock Variable definitions
 *----------------------------------------------------------------------------*/
// uint32_t SystemCoreClock; !< System Clock Frequency (Core Clock)
// uint32_t msTickCount;


/*----------------------------------------------------------------------------
  Clock functions
 *----------------------------------------------------------------------------*/

/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system.
 *       Initialize the System.
 */
void SystemInit (void) {
#if (CLOCK_SETUP)                                /* Clock Setup           */
  SystemCoreClock = __SYSTEM_CLOCK;
  TicksPerMS = __SYSTEM_CLOCK/1000;
#if (SYSCLK_SETUP)                              /* System Clock Setup      */
#if (SYSOSC_SETUP)                              /* System Oscillator Setup  */
  uint32_t i;

  LPC_SYSCTL->PDRUNCFG   &= ~(1 << 5);        /* Power-up System Osc      */
  LPC_SYSCTL->SYSOSCCTRL    = SYSOSCCTRL_Val;
  for (i = 0; i < 200; i++) __NOP();
#endif
  LPC_SYSCTL->SYSPLLCLKSEL  = SYSPLLCLKSEL_Val;   /* Select PLL Input        */
  LPC_SYSCTL->SYSPLLCLKUEN  = 0x1;              /* Update Clock Source    */
  LPC_SYSCTL->SYSPLLCLKUEN  = 0x0;              /* Toggle Update Register   */
  LPC_SYSCTL->SYSPLLCLKUEN  = 0x1;
  while (!(LPC_SYSCTL->SYSPLLCLKUEN & 0x1));      /* Wait Until Updated    */
#if (SYSPLL_SETUP)                              /* System PLL Setup      */
  LPC_SYSCTL->SYSPLLCTRL    = SYSPLLCTRL_Val;
  LPC_SYSCTL->PDRUNCFG   &= ~(1 << 7);        /* Power-up SYSPLL          */
  while (!(LPC_SYSCTL->SYSPLLSTAT & 0x1));      /* Wait Until PLL Locked    */

#endif
#if (WDTOSC_SETUP)                              /* Watchdog Oscillator Setup*/
  LPC_SYSCTL->WDTOSCCTRL    = WDTOSCCTRL_Val;
  LPC_SYSCTL->PDRUNCFG   &= ~(1 << 6);        /* Power-up WDT Clock    */
#endif
  LPC_SYSCTL->MAINCLKSEL    = MAINCLKSEL_Val;    /* Select PLL Clock Output  */
  LPC_SYSCTL->MAINCLKUEN    = 0x0;              /* Toggle Update Register   */
  LPC_SYSCTL->MAINCLKUEN    = 0x1;
  while (!(LPC_SYSCTL->MAINCLKUEN & 0x1));      /* Wait Until Updated      */
#endif

  LPC_SYSCTL->SYSAHBCLKDIV  = SYSAHBCLKDIV_Val;
  LPC_SYSCTL->SYSAHBCLKCTRL = AHBCLKCTRL_Val;
#endif

#if (MEMMAP_SETUP || MEMMAP_INIT)                 /* Memory Mapping Setup              */
  LPC_SYSCTL->SYSMEMREMAP = SYSMEMREMAP_Val;
#endif
}
