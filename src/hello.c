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
static volatile unsigned task_100ms_active = 0;
static volatile unsigned task_1s_active = 0;

static void my_puts(const char *str) {
	char buffer[128];
	char *ptr;

	usr_sprintf(buffer, "%s\r\n", str);

	for (ptr = buffer; *ptr; ++ptr)
		_out_uart((const unsigned char) *ptr);
}

static void TaskActivate(void) {
	static unsigned count[2];

	task_1ms_active = 1;

	if (count[0] >= 100) {
		count[0] = 0;
		task_100ms_active = 1;
	} else {
		count[0]++;
	}

	if (count[1] >= 1000) {
		count[1] = 0;
		task_1s_active = 1;
	} else {
		count[1]++;
	}

}

static void TaskIdle(void) {
	static volatile unsigned task_idle_count = 0;
	task_idle_count++;
}

static void Task1ms(void) {

}

extern void ASC_Test(void);

static void Task100ms(void) {
	ASC_Test();

//	if (_in_uart() == 'a') {
//		my_puts("a was pressed");
//	}
}

static void Task1s(void) {
	static unsigned no_of_seconds_passed;
	char str[128] = "0 minutes 00 seconds have passed";

	str[0] = (no_of_seconds_passed / 60) + 48;
	str[10] = ((no_of_seconds_passed % 60) / 10) + 48;
	str[11] = ((no_of_seconds_passed % 60) % 10) + 48;

	no_of_seconds_passed++;

//	my_puts(str);
}

/* “There is a school with 1000 pupils and 1000 lockers.

 The headmaster asks the first pupil to go and open all of the lockers.

 He then asks the second pupil to go and close every second locker.

 He then asks the third pupil to close every third locker or, if it as already closed then open it.

 And so on...


 The question is how many doors are open after the Nth pupil has finished?”
 *
 * */
static unsigned HowManyDoorsAreOpen(volatile unsigned no_of_pupils) {
	volatile unsigned open_count = 0;
	static unsigned locker_door_open[1000];
	volatile unsigned i, j, k;

	for (i = 0; i < no_of_pupils; i++) {
		for (j = i; j < 1000;) {
			locker_door_open[j] = !locker_door_open[j];

			j = j + i + 1;
		}
	}

	//count how many are open;
	for (k = 0; k < 1000; k++) {
		if (locker_door_open[k])
			open_count++;
	}

	return open_count;
}

extern void ASC_Demo(void);

int main(void) {
	SYSTEM_Init();
	TimerInit(1000);
	TimerSetHandler(TaskActivate);
//	_init_uart(38400);

	ASC_Demo();

	static volatile unsigned input = 3;
	static volatile unsigned output;

	output = HowManyDoorsAreOpen(input);

	while (1) {
		if (task_1ms_active) {
			task_1ms_active = 0;
			Task1ms();
		}

		if (task_100ms_active) {
			task_100ms_active = 0;
			Task100ms();
		}

		if (task_1s_active) {
			task_1s_active = 0;
			Task1s();
		}

		TaskIdle();
	}

	return 0;
}
