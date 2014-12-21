/**
 * 0001-test.c -- modified from 
 * http://ubicomp.lancs.ac.uk/~martyn/sdcc_linux/test.c
 *
 * Ports A and B will be set as all outputs.  Look for successively 
 * slower squarewaves on B0, B1, ... B7, and A0, A1, ... A3.  A4 will 
 * not show anything unless you connect it to +5V through a 10K 
 * resistor, since it is an open-drain output.
 */

#include "pic16f628a.h"

#include "tsmtypes.h"

// Set the __CONFIG word:
// I usually set it to _EXTCLK_OSC&_WDT_OFF&_LVP_OFF&_DATA_CP_OFF&_PWRTE_ON
//Uint16 at 0x2007  __CONFIG = CONFIG_WORD;

Uint16 __at(0x2007) config = _EXTCLK_OSC & _WDT_OFF & _LVP_OFF & DATA_CP_OFF & _PWRTE_ON;

static unsigned char count;

void main(void)
{
#ifdef __16f628a	// Only compile this section for PIC16f628a
	CMCON = 0x07;	/** Disable comparators.  NEEDED FOR NORMAL PORTA
			 *  BEHAVIOR ON PIC16f628a!
			 */
#endif
	TRISB = 0x00;	// Set port B as all outputs
        TRISA = 0x00;	// Set port A as all outputs
        count = 0x00;	// Start counting at 0
        while(1)	// Infinite loop
	{
		PORTA = count;	// Display count on PORTA
                PORTB = count;	// Display count on PORTB
                count ++;	// Increment count
        }

}
