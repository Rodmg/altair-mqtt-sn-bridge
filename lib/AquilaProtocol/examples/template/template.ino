#include <Wire.h>
#include <Mesh.h>
#include <AquilaProtocol.h>

/*
  template
  This is a simple example template that shows most of
  the functions that you could need for developing
  AquilaProtocol devices.

  It adds two actions ("On" and "Off"), that simply
  emit an event (on or off), and shows how to define
  these events.
*/

// Event definitions:
Event off;
Event on;

// Action definitions:
bool turnOff(uint8_t param, bool gotParam)
{
  Aquila.emit(off);
}

bool turnOn(uint8_t param, bool gotParam)
{
  Aquila.emit(on);
}

void setup()
{
  // Starting comminication hardware and libraries:
  Mesh.begin();
  Aquila.begin();
  // Seting device identification:
  Aquila.setClass("mx.makerlab.test");
  Aquila.setName("Test");
  // Adding Actions:
  Aquila.addAction("Off", turnOff);
  Aquila.addAction("On", turnOn);
  // Adding events:
  off = Aquila.addEvent("Turned Off");
  on = Aquila.addEvent("Turned On");
  // Anouncing to the hub (for auto discovery in the aquila-server ui).
  Mesh.announce(HUB);
}

void loop()
{
  // Constantly listening for requests and tasks:
  Mesh.loop();
  Aquila.loop();
}
