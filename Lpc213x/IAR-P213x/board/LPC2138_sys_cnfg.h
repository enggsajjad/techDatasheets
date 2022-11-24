/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2005
 *
 *    File name   : LPC2138_sys_cnfg.h
 *    Description : Some ssystem definitions
 *
 *    History :
 *    1. Data        : Feb 10, 2005
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 1.1.6.1 $
 **************************************************************************/

#ifndef __LPC2138_SYS_CNFG_H
#define __LPC2138_SYS_CNFG_H

/* OSC [Hz] */
#define FOSC 	            14745600
/* Core clk [Hz] */
#define FCCLK	            14745600
/* Time Precision time [us] */
#define TIMER_PRECISION   1
/* Sys timer tick per seconds */
#define TICK_PER_SECOND   100
/* MAM Initialization State */
#define MAM_CTRL          MAM_FULL
/* MAM Initialization Timing*/
#define MAM_DIV           MAMCycle3

#endif /* __LPC2138_SYS_CNFG_H */


