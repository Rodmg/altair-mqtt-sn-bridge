#ifndef HAL_SERIAL_H
#define HAL_SERIAL_H

#include "config.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#ifndef BAUD
#define BAUD 9600
#endif

// Select UART number, default 0 (Altair)
#ifndef UARTN
#define UARTN 0
#endif

#include <util/setbaud.h>
#include <stdio.h>
#include <stdbool.h>

#ifdef  __cplusplus
extern "C" {
#endif

void Serial_init();

void Serial_putchar(char c, FILE *stream);

char Serial_getchar(FILE *stream);

bool Serial_available();

#ifdef  __cplusplus
}
#endif

#endif /* HAL_SERIAL_H */