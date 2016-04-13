#include "AltairTemperature.h"

// From ATmega256RFR2 datasheet:
uint16_t getTempADC(void)
{
  ADCSRC = 10<<ADSUT0; // set start-up time
  ADCSRB = 1<<MUX5; // set MUX5 first
  ADMUX = (3<<REFS0) + (9<<MUX0); // store new ADMUX, 1.6V AREF // switch ADC on, set prescaler, start conversion
  ADCSRA = (1<<ADEN) + (1<<ADSC) + (4<<ADPS0);
  do
  {} while( (ADCSRA & (1<<ADSC))); // wait for conversion end ADCSRA = 0; // disable the ADC
  return (ADC);
}

float getTempC()
{
  float temp = getTempADC();
  temp = 1.13*temp-272.8;
  return temp;
}