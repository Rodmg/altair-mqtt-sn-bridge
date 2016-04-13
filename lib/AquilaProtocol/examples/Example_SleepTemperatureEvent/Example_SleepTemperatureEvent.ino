#include <Wire.h>
#include <Mesh.h>
#include <AquilaProtocol.h>
#include <AltairTemperature.h>
#include "LowPower.h"

/*
  Examole_SleepTemperatureEvent:
  Puts Altair in deep sleep mode for ultra low power consumption
  and wakes up every two minutes for emitting an event.
 */

float tempC = 0;

Event temperatureReport;

// Using the LowPower library for sleeping for 8 seconds 
// (maxium allowed by the microcontroller) at a time, until
// it gets to two minutes.
void sleepForTwoMinutes()
{
  Mesh.sleep();
  for(int i=0;i<15;i++)
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_ON);

  Mesh.wakeup();
}

// Attending network
void yield()
{
  Mesh.loop();
  Aquila.loop();
}

void setup()
{
  // Aquila setup
  Mesh.begin();
  Aquila.begin();

  Aquila.setClass("mx.makerlab.tempReport");
  Aquila.setName("TempReport");

  temperatureReport = Aquila.addEvent("TemperatureReport");

  Mesh.announce(HUB);
  delay(5000);  // Allow 5 seconds before going to sleep for connecting with aquila-server
}

void loop()
{
  // Get temperature and send
  tempC = getTempC();
  Aquila.emit((uint16_t)HUB, temperatureReport, tempC, true);
  delay(100); // Allow some time for the radio transciever to send the message
  sleepForTwoMinutes(); 
}