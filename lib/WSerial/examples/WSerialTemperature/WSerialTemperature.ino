/*
  WSerial example

  WSerialTemperature is a temperature logger example
  that uses the WSerial interface for sending
  the temperature every half a second.


  You can receive the lectures of the Altair
  from the aquila-server in the HUB, in the "Console" section.

  Use:
  1. Upload the program to an Altair
  2. Access "Console" section of aquila-server web UI

*/

#include <Wire.h>
#include "Mesh.h"
#include "WSerial.h"
#include <AltairTemperature.h>
#include <AquilaProtocol.h>

// We use yield for always attending to the network
// yield is called in the loop and inside delay()
void yield()
{
  // Listening to the network
  Mesh.loop();
  Aquila.loop();
  WSerial.loop();
}

void setup()
{
  // Starting mesh network
  Mesh.begin();
  Aquila.begin();
  Aquila.setClass("mx.makerlab.wserialtemp");
  Aquila.setName("WSerialTemperature");
  // Making a serial connection with DEST
  WSerial.begin(HUB);
  Mesh.announce(HUB);
}

void loop()
{
  yield();
  float temp = getTempC();
  WSerial.println(temp);
  delay(1000);
}