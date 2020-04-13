#include <machine/wdtcon.h>
#include <machine/intrinsics.h>
#include "interrupts.h"
#include "system_tc2x.h"

#include "tc_inc_path.h"

#include TC_INCLUDE(TCPATH/IfxStm_reg.h)
#include TC_INCLUDE(TCPATH/IfxStm_bf.h)
#include TC_INCLUDE(TCPATH/IfxCpu_reg.h)
#include TC_INCLUDE(TCPATH/IfxCpu_bf.h)
#include TC_INCLUDE(TCPATH/IfxAsclin_reg.h)
#include TC_INCLUDE(TCPATH/IfxSrc_reg.h)
#include TC_INCLUDE(TCPATH/IfxPort_reg.h)

#include "interrupts.h"

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

/* Service Request Control register */
typedef union _Ifx_SRC_t {
	volatile unsigned int R;
	struct _bits {
		volatile unsigned int SRPN :8; /* [7:0] Service Request Priority Number (rw) */
		volatile unsigned int :2;
		volatile unsigned int SRE :1; /* [10:10] Service Request Enable (rw) */
		volatile unsigned int TOS :2; /* [12:11] Type of Service Control (rw) */
		volatile unsigned int :3;
		volatile unsigned int ECC :6; /* [21:16] ECC (rwh) */
		volatile unsigned int :2;
		volatile unsigned int SRR :1; /* [24:24] Service Request Flag (rh) */
		volatile unsigned int CLRR :1; /* [25:25] Request Clear Bit (w) */
		volatile unsigned int SETR :1; /* [26:26] Request Set Bit (w) */
		volatile unsigned int IOV :1; /* [27:27] Interrupt Trigger Overflow Bit (rh) */
		volatile unsigned int IOVCLR :1; /* [28:28] Interrupt Trigger Overflow Clear Bit (w) */
		volatile unsigned int SWS :1; /* [29:29] SW Sticky Bit (rh) */
		volatile unsigned int SWSCLR :1; /* [30:30] SW Sticky Clear Bit (w) */
		volatile unsigned int :1;
	} B;
} Ifx_SRC_t;

#define ASCLIN_FRAMECON_MODE_INIT (0U)
#define ASCLIN_FRAMECON_MODE_ASC  (1U)
#define ASCLIN_FRAMECON_MODE_SPI  (2U)
#define ASCLIN_FRAMECON_MODE_LIN  (3U)
#define ASCLIN_FRAMECON_MODE_SHIFT (16U)
#define ASCLIN_FRAMECON_STOP_SHIFT (9U)
#define ASCLIN_FRAMECON_STOP       (1U)

#define ASCLIN_DATCON_DATLEN       (7U) /* 8 bit */
#define ASCLIN_DATCON_DATLEN_SHIFT (0U)
#define ASCLIN_DATCON_DATLEN_MASK  (0xFFFFFFF0U)

#define ASCLIN_CSR_CLKSEL_NOCLK       (0U)
#define ASCLIN_CSR_CLKSEL_CLC       (1U)
#define ASCLIN_CSR_CLKSEL_EVROSC    (2U)
#define ASCLIN_CSR_CLKSEL_FMAX      (8U)
#define ASCLIN_CSR_CLKSEL_SHIFT (0U)
#define ASCLIN_CSR_CLKSEL       (ASCLIN_CSR_CLKSEL_EVROSC)

#define ASCLIN_IOCR_FILTER_DEPTH   (4U) /* digital filter glitch depth */

#define ASCLIN_FLAGSCLEAR_MASK (0xFFFFFffFU)

static volatile Ifx_ASCLIN *psASCLIN;

/* Variable to count Seconds */
static volatile uint32 Counter_1s;

static void ASC_Init(void);
static unsigned char ASC_GetChar(void);
static void ASC_PutChar(char c);
static void tick_irq(int reload_value);

void ASC_Demo(void) {
	static Ifx_SRC_t * const tabSRC = (Ifx_SRC_t *) &MODULE_SRC;

	/* Initialise ASC */
	ASC_Init();

	Counter_1s = 0U;

	/* ToDo: Configure STM0 to generate an interrupt after 1s */
	/* Info: STM frequency is 100MHz */

	/* ToDo: Configure and enable STM interrupt (SRC_STM_STM0_SR0) */
	//done in timer init
}

/* Interrupt service routine for STM interrupt.
 * This will be triggered once every Second */
static void tick_irq(int reload_value) {
	/* ToDo: Reload STM to get the next interrupt after 1s */

	/* Increment Second counter */
	Counter_1s++;

	/* ToDo: Use ASC_PutChar() function to print the current Second value in Decimal */
	/* Leading 0's shall be avoided */

}

/* ASC Initialisation (UART) */
/* Baudrate: 38400 */
/* Data: 8-bit */
/* Stop: 1-bit */
/* Parity: None */
static void ASC_Init(void) {
	volatile int i;

	/* Tx P15.7 */
	P15_IOCR4.B.PC7 = 0x12;
	/* Rx P32.2 */
	P32_IOCR0.B.PC2 = 0x02;

	psASCLIN = &MODULE_ASCLIN3;

	unlock_wdtcon();
	psASCLIN->CLC.U = 0U;
	lock_wdtcon();

	while (psASCLIN->CLC.U)
		;

	psASCLIN->CSR.B.CLKSEL = ASCLIN_CSR_CLKSEL_NOCLK;
	for (i = 0; i < 1000; i++) {
		;
	}

	/* select the operation mode (INIT)*/
	psASCLIN->FRAMECON.B.MODE = ASCLIN_FRAMECON_MODE_INIT;
	for (i = 0; i < 1000; i++) {
		;
	}
	/* activate the clock source */
	psASCLIN->CSR.B.CLKSEL = ASCLIN_CSR_CLKSEL;

	/* provide delay of 2-fA clock cycles */
	for (i = 0; i < 1000; i++) {
		;
	}

	/* deactivate the clock source */
	psASCLIN->CSR.B.CLKSEL = ASCLIN_CSR_CLKSEL_NOCLK;
	for (i = 0; i < 1000; i++) {
		;
	}
	psASCLIN->FRAMECON.B.MODE = ASCLIN_FRAMECON_MODE_ASC;
	for (i = 0; i < 1000; i++) {
		;
	}
	/* activate the clock source */
	psASCLIN->CSR.B.CLKSEL = ASCLIN_CSR_CLKSEL;

	/* provide delay of 2-fA clock cycles */
	for (i = 0; i < 1000; i++) {
		;
	}

	/* deactivate the clock source */
	psASCLIN->CSR.B.CLKSEL = ASCLIN_CSR_CLKSEL_NOCLK;
	/* select 1 stop bit */
	psASCLIN->FRAMECON.B.MSB = 0U;
	psASCLIN->FRAMECON.B.LEAD = 0U;
	psASCLIN->FRAMECON.B.IDLE = 0;
	psASCLIN->FRAMECON.B.PEN = 0U;

	psASCLIN->FRAMECON.U |= (1U << 9);

	psASCLIN->DATCON.B.DATLEN = ASCLIN_DATCON_DATLEN;

	/* ToDo: Configure RXFIFOCON */
	psASCLIN->RXFIFOCON.B.BUF = 0U;
	psASCLIN->RXFIFOCON.B.OUTW = 1U;
	psASCLIN->RXFIFOCON.B.ENI = 1;

	/* ToDo: Configure TXFIFOCON */
	psASCLIN->TXFIFOCON.B.INW = 1U;
	psASCLIN->TXFIFOCON.B.ENO = 1;

	/* Configure BITCON */
	psASCLIN->BITCON.B.PRESCALER = 259;
	psASCLIN->BITCON.B.OVERSAMPLING = 1;

	psASCLIN->BRG.B.NUMERATOR = 1;
	psASCLIN->BRG.B.DENOMINATOR = 1;

	/* Enable the digital glitch filter - number of microticks to sample
	 * Microticks is CLKSEL/PRESCALAR i.e. 20MHz/20 = 1us
	 */
	psASCLIN->IOCR.B.DEPTH = ASCLIN_IOCR_FILTER_DEPTH;

	/* ARX3D used. */
	psASCLIN->IOCR.B.ALTI = 3;

	/* activate the clock source - 20MHz Crystal */
	psASCLIN->CSR.B.CLKSEL = ASCLIN_CSR_CLKSEL;

	/* clear the interrupt event flags*/
	psASCLIN->FLAGSCLEAR.U = ASCLIN_FLAGSCLEAR_MASK;

}

extern void ASC_Test(void) {
	ASC_PutChar('a');
}

static void ASC_PutChar(char c) {
	/* ToDo: Load transmit buffer */
	psASCLIN->TXDATA.U = c;

	/* ToDo: Wait until Transmit FIFO becomes empty */
	while (psASCLIN->TXFIFOCON.B.FILL)
		;
}
static unsigned char received;
static unsigned char ASC_GetChar(void) {


	/* ToDo: Wait until Receive FIFO is not empty */
	while (!psASCLIN->RXFIFOCON.B.FILL)
		;

	/* ToDo: Get data from receive buffer */
	received = psASCLIN->RXDATA.U;
	/* ToDo: return the receive character */

	return received;

}

