#include "halSerial.h"

#if UARTN == 0
    #define UBRRXH UBRR0H
    #define UBRRXL UBRR0L
    #define UCSRXA UCSR0A
    #define U2XX U2X0
    #define UCSRXC UCSR0C
    #define UCSRXB UCSR0B
    #define UCSZX1 UCSZ01
    #define UCSZX0 UCSZ00
    #define RXENX RXEN0
    #define TXENX TXEN0
    #define UDREX UDRE0
    #define UDRX UDR0
    #define RXCX RXC0
#else
    #define UBRRXH UBRR1H
    #define UBRRXL UBRR1L
    #define UCSRXA UCSR1A
    #define U2XX U2X1
    #define UCSRXC UCSR1C
    #define UCSRXB UCSR1B
    #define UCSZX1 UCSZ11
    #define UCSZX0 UCSZ10
    #define RXENX RXEN1
    #define TXENX TXEN1
    #define UDREX UDRE1
    #define UDRX UDR1
    #define RXCX RXC1
#endif

FILE Serial_output = FDEV_SETUP_STREAM(Serial_putchar, NULL, _FDEV_SETUP_WRITE);
FILE Serial_input = FDEV_SETUP_STREAM(NULL, Serial_getchar, _FDEV_SETUP_READ);

void Serial_init()
{
	UBRRXH = UBRRH_VALUE;
	UBRRXL = UBRRL_VALUE;

#if USE_2X
    UCSRXA |= _BV(U2XX);
#else
    UCSRXA &= ~(_BV(U2XX));
#endif

    UCSRXC = _BV(UCSZX1) | _BV(UCSZX0); /* 8-bit data */
    UCSRXB = _BV(RXENX) | _BV(TXENX);   /* Enable RX and TX */

	stdout = &Serial_output;
    stdin  = &Serial_input;
}

void Serial_putchar(char c, FILE *stream)
{
	if (c == '\n') {
        Serial_putchar('\r', stream);
    }
    loop_until_bit_is_set(UCSRXA, UDREX); /* Wait until data register empty. */
    UDRX = c;
}

char Serial_getchar(FILE *stream)
{
    loop_until_bit_is_set(UCSRXA, RXCX); /* Wait until data exists. */
    return UDRX;
}

bool Serial_available()
{
    return UCSRXA >> RXCX;
}