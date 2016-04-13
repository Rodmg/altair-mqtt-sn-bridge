#ifndef HAL_H
#define HAL_H

#include "config.h"

#include <stdint.h>
#include "Arduino.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#ifdef  __cplusplus
extern "C" {
#endif

/*
 *	Hardware Abstraction Layer for ATmega256rfr2 mcu
 *
 */

#define Hal_init() init()

#define Hal_setPin(pin, level) digitalWrite(pin, level)

#define Hal_pinMode(pin, mode) pinMode(pin, mode)

#define Hal_readPin(pin) digitalRead(pin)

//void Hal_setInterrupt(uint8_t interruptNum, void (*userFunc)(void), int mode);

#define Hal_setInterrupt(a, b, c) attachInterrupt(a, b, c)

#define Hal_millis() millis()

#define Hal_micros() micros()

#define Hal_delay(ms) delay(ms)

#define Hal_delayus(us) delayMicroseconds(us)

#ifdef  __cplusplus
}
#endif

#endif /* HAL_H */