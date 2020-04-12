/*====================================================================
 * Project:  Board Support Package (BSP) examples
 * Function: example demonstrating timer interrupt functionality
 *
 * Copyright HighTec EDV-Systeme GmbH 1982-2019
 *====================================================================*/

#include "bspconfig.h"
#include "timer.h"
#include "system_tc2x.h"
#include "uart_poll.h"
#include "usr_sprintf.h"

static volatile unsigned task_1ms_active = 0;

static void my_puts(const char *str)
{
	char buffer[128];
	char *ptr;

	usr_sprintf(buffer, "%s\r\n", str);

	for (ptr = buffer; *ptr; ++ptr)
		_out_uart((const unsigned char) *ptr);
}

/* timer callback handler */
static void Task1msActivate(void) {
	task_1ms_active = 1;
}

int main(void) {
	static volatile unsigned task_idle_count = 0;

	SYSTEM_Init();
	TimerInit(1000);
	/* add own handler for timer interrupts */
	TimerSetHandler(Task1msActivate);

	_init_uart(38400);

	while (1) {
		if (task_1ms_active) {
			task_1ms_active = 0;

			my_puts("Your choice please");

		} else {
			task_idle_count++;
		}
	}

	return 0;
}
