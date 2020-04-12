/*====================================================================
 * Project:  Board Support Package (BSP)
 * Function: Hardware-dependent module providing a time base
 *           by programming a system timer (TriCore TC27xx version).
 *
 * Copyright HighTec EDV-Systeme GmbH 1982-2019
 *====================================================================*/

#include <machine/wdtcon.h>
#include <machine/intrinsics.h>
#include "interrupts.h"
#include "system_tc2x.h"

#include "tc_inc_path.h"

#include TC_INCLUDE(TCPATH/IfxStm_reg.h)
#include TC_INCLUDE(TCPATH/IfxStm_bf.h)
#include TC_INCLUDE(TCPATH/IfxCpu_reg.h)
#include TC_INCLUDE(TCPATH/IfxCpu_bf.h)

#include "timer.h"
#include "C:\HighTec\toolchains\tricore\v4.9.3.0-infineon-1.0\tricore\include\tc27xd\IfxStm_bf.h"

#define SYSTIME_ISR_PRIO	2

#define STM0_BASE			((Ifx_STM *)&MODULE_STM0)

/* timer reload value (needed for subtick calculation) */
static unsigned int reload_value = 0;

/* pointer to user specified timer callback function */
static TCF user_handler = (TCF) 0;

/* timer interrupt routine */
static void tick_irq(int reload_value) {
	/* set new compare value */
	STM0_BASE->CMP[0].U += (unsigned int) reload_value;
	if (user_handler) {
		user_handler();
	}
}

/* Initialise timer at rate <hz> */
void TimerInit(unsigned int hz) {
	reload_value = SYSTEM_GetStmClock() / hz;

	/* install handler for timer interrupt */
	InterruptInstall(SRC_ID_STM0SR0, tick_irq, SYSTIME_ISR_PRIO,
			(int) reload_value);

	/* prepare compare register */
	STM0_BASE->CMP[0].U = STM0_BASE->TIM0.U + reload_value;
	STM0_BASE->CMCON.B.MSIZE0 = 31; /* use bits 31:0 for compare */
	/* reset interrupt flag */
	STM0_BASE->ISCR.U = (IFX_STM_ISCR_CMP0IRR_MSK << IFX_STM_ISCR_CMP0IRR_OFF);
	STM0_BASE->ICR.B.CMP0EN = 1;
}

/* Install <handler> as timer callback function */
void TimerSetHandler(TCF handler) {
	user_handler = handler;
}
