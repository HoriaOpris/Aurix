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

static void my_puts(const char *str) {
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

			static unsigned second_cnt;

			if (second_cnt >= 1000) {
				second_cnt = 0;

				static unsigned no_of_seconds_passed;
				char str[128] = "0 minutes 00 seconds have passed";

				str[0] = (no_of_seconds_passed / 60) + 48;
				str[10] = ((no_of_seconds_passed % 60) / 10) + 48;
				str[11] = ((no_of_seconds_passed % 60) % 10) + 48;

				no_of_seconds_passed++;

				my_puts(str);
			} else {
				second_cnt++;
			}
		} else {
			task_idle_count++;
		}
	}

	return 0;
}
