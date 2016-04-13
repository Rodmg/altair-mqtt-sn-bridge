#ifndef ALTAIRTEMPERATURE_H
#define ALTAIRTEMPERATURE_H

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

uint16_t getTempADC(void);

float getTempC();

#endif //ALTAIRTEMPERATURE_H