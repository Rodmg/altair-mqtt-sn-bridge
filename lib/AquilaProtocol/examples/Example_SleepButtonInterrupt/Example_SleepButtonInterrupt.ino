#include <Wire.h>
#include <Mesh.h>
#include <AquilaProtocol.h>
#include "LowPower.h"

/*
  Example_SleepButtonInterrupt:
  Puts the Altair in deep sleep mode for ultra 
  low power consumption.
  When a button is pressed, the Altair wakes up, 
  sends an event and goes back to sleep.

  Circuit:
  You need to connect a button to pin 11 of Altair
  whit a pullup resistor.
  
             VCC
              |
          10k Z
              Z
              |    /
  [pin 11]____|___/  ____GND

 */

#define BUTTON 11

Event buttonPressed;

volatile bool pressed = false;

// Interrupt vector, will be called when the interrupt is fired
ISR(PCINT0_vect)
{
  // Check that the interrupt corresponds to the button we want
  if(digitalRead(BUTTON) == LOW) pressed = true;
}

// Allways attend network with yield (only when not sleeping)
void yield()
{
  Mesh.loop();
  Aquila.loop();
}

void setup()
{
  // Configure button
  pinMode(BUTTON, INPUT);
  // Enable pin change interrupt to the desired pin
  // (register Values from ATMega256rfr2 datasheet)
  PCICR |= _BV(PCIE0);
  PCMSK0 |= _BV(PCINT6);
  sei();  // enable interrupts
  // Aquila setup
  Mesh.begin();
  Aquila.begin();
  Aquila.setClass("mx.makerlab.sleepyButton");
  Aquila.setName("SleepyButton");
  buttonPressed = Aquila.addEvent("buttonPressed");
  Mesh.announce(HUB);
  delay(5000); // Allow 5 seconds before going to sleep for connecting with aquila-server
}

void loop()
{
  Mesh.sleep();
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_ON); // Sleep forever, until an interrupt is fired
  Mesh.wakeup();
  if(pressed)
  {
    // Emit event
    Aquila.emit(buttonPressed);
    pressed = false;
    delay(100); // Allow some time for the radio transciever to send the message
  }
}