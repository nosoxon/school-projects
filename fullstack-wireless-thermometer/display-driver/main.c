#include <msp430g2553.h>

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define MAX_CMD_LEN 64

static const uint8_t digits[] = {
	0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f,
	0x40, 0x00, 0b01111001, 0b01010000 };

static volatile uint8_t digit_idx = 0;
static uint8_t digit_buf[4] = { 0 };

static volatile uint8_t display_idx = 0;
static uint8_t display_buf[4] = { 0 };

int main(void) {
	WDTCTL = WDTPW + WDTHOLD; // Stop WDT

	/* DCO freq = 16MHz (SLAU144J:5.2.5) */
	BCSCTL1 = CALBC1_16MHZ;
	DCOCTL  = CALDCO_16MHZ;

	/* Select pin functions (SLAS735J) */
	P1SEL   = 0b11100000;
	P1SEL2  = 0b11100000;	// P1.5 = UCB0CLK, P1.6 = UCB0SOMI, P1.7 = UCB0SIMO
	P1DIR  |= 0b00001111;

	P1REN  |= 0b00010000;
	P1OUT  |= 0b00010000;

	P2SEL  = 0b00000000;
	P2SEL2 = 0b00000000;
	P2DIR  = 0b11111111;

	TACCR0 = 0xfa00;
	TACTL  = TASSEL1	// SMCLK
		+ MC0		// Count to TACCR0
		+ TAIE;		// Enable interrupt

	/* Configure SPI (SLAU144J:16) */
	UCB0CTL1  = UCSWRST;           			// software reset
	UCB0CTL0  = UCCKPL + UCMSB + UCSYNC;            // synchronous mode
	UCB0CTL1 &= ~UCSWRST;          			// disable reset

	IE2 |= UCB0RXIE;    // enable USCI_B0 RX interrupts

	__delay_cycles(100);
	_BIS_SR(LPM0_bits + GIE);      // Enter LPM0, interrupts enabled
	return 0;
}

void on_spi_recv(uint8_t rx) {
	if (rx == 0x77) {
		memcpy(display_buf, digit_buf, 4);
	} else {
		if (rx == 0x66) {
			digit_idx = 0;
		} else if (rx >= 0x00 && rx <= 0x0d) {
			digit_buf[digit_idx] = rx;
			digit_idx = (digit_idx + 1) % 4;
		}
	}

	while (!(IFG2 & UCB0TXIFG));
	UCB0TXBUF = rx;
}

void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCIAB0RX_ISR(void) {
	if (IFG2 & UCB0RXIFG)
		on_spi_recv(UCB0RXBUF);
}

void __attribute__ ((interrupt(TIMER0_A1_VECTOR))) TIMER0_A1_ISR(void) {
	if (TAIV & TA0IV_TAIFG) {
		P1OUT &= 0xf0;
		if (!(P1IN & 0b00010000)) {
			P2OUT = digits[display_buf[display_idx]] + (display_idx == 2 ? 0x80 : 0);
			P1OUT |= (0x0f & (1 << display_idx));
			display_idx = (display_idx + 1) % 4;
		}
	}
}
