#include <AltairTemperature.h>

/*
 *	Using the internal temperature sensor
 *	in Altair's microcontroller.
 *	This example reads and prints the temperature
 *	every 500ms via serial port.
 *
 *	Functions in this library:
 *		getTempC()	 : Returns temperature in ºC.
 *		getTempADC() : Returns the sensor value 
 *					   straight from the ADC.
 */

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  float temp = getTempC();
  Serial.println(temp);
  delay(500);
}