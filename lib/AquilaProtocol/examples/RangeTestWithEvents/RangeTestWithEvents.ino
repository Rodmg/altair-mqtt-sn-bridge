#include <Wire.h>
#include <Mesh.h>
#include <AquilaProtocol.h>

/*
  RangeTestWithEvents: Test wireless range with events.

  Usage:
  1. Upload this sketch to two different Altairs
  2. Each Altair will emit an event every second and the other will flash a green LED
  when the event is received
  2. Power the Altairs with batteries and move the Altairs further away until the LED 
  stops flashing
  
 */

#define LED 14
// Event that will be emited every second
Event ping;

// Function that will be called when the event is received
bool onPing(uint8_t param, bool hasParam, uint16_t shortAddr, uint8_t *longAddr)
{
  // Check that the event wasn't emitted by ourselves
  if(shortAddr != Mesh.getShortAddr()) digitalWrite(LED, !digitalRead(LED));
}

// Attend network while on delay() with yield function.
void yield()
{
  Mesh.loop();
  Aquila.loop();
}

void setup()
{
  // LED setup
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  // Aquila setup
  Mesh.begin();
  Aquila.begin();
  Aquila.setClass("mx.makerlab.rangeTest");
  Aquila.setName("RangeTester");
  // Registering event
  ping = Aquila.addEvent("ping");
  // Subscribe to event
  Aquila.on("ping", onPing);
  
  // Anouncing device
  Mesh.announce(HUB);
}

void loop()
{
  // Emit every second
  Aquila.emit(ping);
  delay(1000);
}