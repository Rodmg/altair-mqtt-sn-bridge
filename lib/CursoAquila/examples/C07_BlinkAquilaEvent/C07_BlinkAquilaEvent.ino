#include <Wire.h>
#include <Mesh.h>
#include <AquilaProtocol.h>

#define LED 13
#define BUTTON 33

Event buttonPressed;

bool turnOff(uint8_t param, bool gotParam)
{
  digitalWrite(LED, HIGH);
}

bool turnOn(uint8_t param, bool gotParam)
{
  digitalWrite(LED, LOW);
}

void setup()
{
  Serial.begin(9600);

  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT);

  Mesh.begin();
  Aquila.begin();
  Aquila.setClass("mx.makerlab.blink");
  Aquila.setName("Blink");

  Aquila.addAction("Off", turnOff);
  Aquila.addAction("On", turnOn);

  buttonPressed = Aquila.addEvent("Button Pressed");

  Mesh.announce(HUB);
}

void loop()
{
  Mesh.loop();
  Aquila.loop();

  if(!digitalRead(BUTTON))
  {
    Aquila.emit(buttonPressed);
    while(!digitalRead(BUTTON));
  }
}
