#include <Wire.h>
#include <Mesh.h>
#include <AquilaProtocol.h>
#include <SimpleTimer.h>
#include <AltairTemperature.h>

/*
  Example_TemperatureEvent
  In this example, the Altair will emit an event
  whit current temperature as param.
*/

// We will use simple timer for reading the temperature
// Repeatedly every second without using delay()
SimpleTimer timer;
int timerId;

// Event to be emited when we get a temperature reading
Event tempMeasured;

// Function that will be called by the timer
void measureAndEvent()
{
  float temp = getTempC();
  Aquila.emit(tempMeasured, temp, true);
}

// Action that will enable temperature measuring
bool startMeasuringTemp(uint8_t param, bool gotParam)
{
  timer.enable(timerId);
}

// Action to disable temperature measuring
bool stopMeasuringTemp(uint8_t param, bool gotParam)
{
  timer.disable(timerId);
}

void setup()
{
  // Usual Aquila protocol initialization
  Mesh.begin();
  Aquila.begin();
  Aquila.setClass("mx.makerlab.thermometer");
  Aquila.setName("Thermometer");
  // Registering actions
  Aquila.addAction("Stop Measuring", stopMeasuringTemp);
  Aquila.addAction("Start Measuring", startMeasuringTemp);
  // Registering event
  tempMeasured = Aquila.addEvent("Temperature Measured");
  // Setting up timer
  timerId = timer.setInterval(1000, measureAndEvent);
  timer.disable(timerId);
  // Announcing device
  Mesh.announce(HUB);
}

void loop()
{
  // For attending requests
  Mesh.loop();
  Aquila.loop();
  // Checking if we should measure temperature or wait
  timer.run();
}
