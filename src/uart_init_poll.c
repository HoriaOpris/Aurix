/*====================================================================
* Project:  Board Support Package (BSP)
* Function: Transmit and receive characters via TriCore's serial line
*           (Infineon controller TC27xx)
*           (polling variant)
*
* Copyright HighTec EDV-Systeme GmbH 1982-2019
*====================================================================*/
#define MODULE_UART_POLL

#include "bspconfig.h"

#include <machine/wdtcon.h>

/* Initialise asynchronous interface to operate at baudrate,8,n,1 */
void _init_uart(int baudrate)
{
	unsigned int numerator;
	unsigned int denominator;
	unsigned int pisel = 0;

#if (RUN_ON_APPKIT == 1)
	/* on board wiggler is connected to ASCLIN0 */
	/* ARX0A/P14.1 (RXD), ATX0/P14.0 (TXD) */
	/* Set TXD/P14.0 to "output" and "high" */
	port->IOCR0.B.PC0 = OUT_PPALT2;
	port->OMR.B.PS0 = 1;
#elif (ON_SHIELDBUDDY == 1)
	/* on board wiggler is connected to ASCLIN3 */
	/* ARX3D/P32.2 (RXD), ATX3/P15.7 (TXD) */
	/* Set TXD/P15.7 to "output" and "high" */
	port->IOCR4.B.PC7 = OUT_PPALT2;
	port->OMR.B.PS7 = 1;
	/* select ARXnD */
	pisel = 3;
#else
	/* on board wiggler is connected to ASCLIN0 */
	/* ARX0B/P15.3 (RXD), ATX0/P15.2 (TXD) */
	/* Set TXD/P15.2 to "output" and "high" */
	port->IOCR0.B.PC2 = OUT_PPALT2;
	port->OMR.B.PS2 = 1;
	/* select ARXnB */
	pisel = 1;
#endif /* RUN_ON_APPKIT */

	/* baudrate values at 100 MHz */
	denominator = 3125;

	switch (baudrate)
	{
		case   9600 : numerator =   BAUD_9600; break;
		case  19200 : numerator =  BAUD_19200; break;
		default     :
		case  38400 : numerator =  BAUD_38400; break;
		case  57600 : numerator =  BAUD_57600; break;
		case 115200 : numerator = BAUD_115200; break;
	}

	/* Enable ASCn */
	unlock_wdtcon();
	UARTBASE->CLC.U = 0;
	/* TXD and RXD pins: automotive level, fast */
#if (RUN_ON_APPKIT == 1)
	/* ARX0A/P14.1 (RXD), ATX0/P14.0 (TXD) */
	port->PDR0.B.PD0 = 0;
	port->PDR0.B.PD1 = 0;
#elif (ON_SHIELDBUDDY == 1)
	/* ARX3D/P32.2 (RXD), ATX3/P15.7 (TXD) */
	P32_PDR0.B.PD2 = 0;
	P15_PDR0.B.PD7 = 0;
#else
	/* ARX0B/P15.3 (RXD), ATX0/P15.2 (TXD) */
	port->PDR0.B.PD2 = 0;
	port->PDR0.B.PD3 = 0;
#endif /* RUN_ON_APPKIT */
	lock_wdtcon();
	/* read back for activating module */
	(void)UARTBASE->CLC.U;

	/* select RXD as input pin */
	UARTBASE->IOCR.B.ALTI = pisel;

	/* Program ASC0 */
	UARTBASE->CSR.U = 0;

	/* configure TX and RX FIFOs */
	UARTBASE->TXFIFOCON.U = (1 << 6)	/* INW: (1 == 1 byte) */
						  | (1 << 1)	/* ENO */
						  | (1 << 0);	/* FLUSH */
	UARTBASE->RXFIFOCON.U = (1 << 6)	/* OUTW: (1 == 1 byte) */
						  | (1 << 1)	/* ENI */
						  | (1 << 0);	/* FLUSH */

	UARTBASE->BITCON.U = ( 9 << 0)		/* PRESCALER: 10 */
					   | (15 << 16)		/* OVERSAMPLING: 16 */
					   | ( 9 << 24)		/* SAMPLEPOINT: position 7,8,9 */
					   | (1u << 31);	/* SM: 3 samples per bit */

	/* data format: 8N1 */
	UARTBASE->FRAMECON.U = (1 << 9)		/* STOP: 1 bit */
						 | (0 << 16)	/* MODE: Init */
						 | (0 << 30);	/* PEN: no parity */
	UARTBASE->DATCON.U = (7 << 0);		/* DATLEN: 8 bit */

	/* set baudrate value */
	UARTBASE->BRG.U = (denominator << 0)	/* DENOMINATOR */
					| (numerator << 16);	/* NUMERATOR */

	UARTBASE->FRAMECON.B.MODE = 1;		/* ASC mode */
	UARTBASE->CSR.U = 1;				/* select CLC as clock source */

	TX_START(UARTBASE);
}
