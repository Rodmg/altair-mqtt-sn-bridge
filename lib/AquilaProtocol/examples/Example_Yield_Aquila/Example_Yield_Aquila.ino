#include <Wire.h>
#include <Mesh.h>
#include <AquilaProtocol.h>
#include <avr/wdt.h>

/*
  Example_Yield_Aquila: Example of the usage of the yield function
  with Aquila.

  The yield function allows us to run routines continually even when using delay().

  yield() is always called internally by delay()

  This sketch flashes the red LED while still allowing us to control the Green LED
  from the Aquila UI.
 */

#define LEDR 13
#define LEDG 14

// Yield function:
// Put every function you want to be continually called here.
// IMPORTANT: Dont put delay() any function that calls delay() inside yield()
void yield()
{
  // reset watch dog timer
  wdt_reset();
  // attend network
  Mesh.loop();
  Aquila.loop();
}
// "On" Action function
bool ledOn(uint8_t param, bool gotParam)
{
  digitalWrite(LEDG, LOW);
}
// "Off" Action function
bool ledOff(uint8_t param, bool gotParam)
{
  digitalWrite(LEDG, HIGH);
}

void setup()
{
  // Setup watch dog timer (resets the microcontroller if it crashes)
  wdt_disable();
  wdt_reset();
  wdt_enable(WDTO_1S);
  // Setup LEDs
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  // Setup Aquila
  Mesh.begin();
  Aquila.begin();
  Aquila.setClass("mx.makerlab.yieldexample");
  Aquila.setName("YieldExample");
  Aquila.addAction("On", ledOn);
  Aquila.addAction("Off", ledOff);
  Mesh.announce(HUB);
}

void loop()
{
  // Always call yield on loop. not really necesary here as we always call delay, 
  // which calls yield inside, however, if your code doesn't allways call delay,
  // it's a good idea to call it here 
  yield();
  digitalWrite(LEDR, LOW);
  delay(1000);
  digitalWrite(LEDR, HIGH);
  delay(1000);
}
