#include <Wire.h>
#include <Mesh.h>
#include <AquilaProtocol.h>

/*
  Example_RemoteBlinkingLED
  In this example we will be able to
  turn on and off Altair's internal red
  LED.
*/

// Altair's internal red LED
#define LED 13

bool ledState = LOW;

// Actions: turn on and off the LED
bool turnOff(uint8_t param, bool gotParam)
{
  ledState = HIGH;
  digitalWrite(LED, ledState);
}

bool turnOn(uint8_t param, bool gotParam)
{
  ledState = LOW;
  digitalWrite(LED, ledState);
}

bool toggle(uint8_t param, bool gotParam)
{
  ledState = !ledState;
  digitalWrite(LED, ledState);
}

void setup()
{
  // Setup the LED
  pinMode(LED, OUTPUT);
  // Aquila protocol setup
  Mesh.begin();
  Aquila.begin();
  Aquila.setClass("mx.makerlab.blink");
  Aquila.setName("Blink");
  // Registering actions
  Aquila.addAction("Off", turnOff);
  Aquila.addAction("On", turnOn);
  Aquila.addAction("Toggle", toggle);
  // Anouncing device
  Mesh.announce(HUB);
}

void loop()
{
  // Attending requests
  Mesh.loop();
  Aquila.loop();
}
