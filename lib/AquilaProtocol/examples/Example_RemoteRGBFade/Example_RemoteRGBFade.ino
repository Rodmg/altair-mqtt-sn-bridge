#include <Wire.h>
#include <Mesh.h>
#include <AquilaProtocol.h>

/*
  Example_RemoteRGBFade
  In this example we will be able to
  change te intesity of each color of
  Altair's RGB LED from the Hub.
*/

// RGB LEDs pin definition
#define LEDR 13
#define LEDG 14
#define LEDB 15

// Actions: set each color intensity
bool setR(uint8_t param, bool gotParam)
{
  if(gotParam)
  {
     // the LED is active low, so we invert the param.
     analogWrite(LEDR, 255 - param);
  }
}

bool setG(uint8_t param, bool gotParam)
{
  if(gotParam)
  {
    analogWrite(LEDG, 255 - param);
  }
}

bool setB(uint8_t param, bool gotParam)
{
  if(gotParam)
  {
    analogWrite(LEDB, 255 - param);
  }
}

void setup()
{
  // Setting up LEDs
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  // Turning off all LEDs
  digitalWrite(LEDR, HIGH);
  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDB, HIGH);

  Mesh.begin();
  Aquila.begin();
  Aquila.setClass("mx.makerlab.rgb");
  Aquila.setName("RGB");

  Aquila.addAction("Red", setR);
  Aquila.addAction("Green", setG);
  Aquila.addAction("Blue", setB);

  Mesh.announce(HUB);
}

void loop()
{
  Mesh.loop();
  Aquila.loop();
}
