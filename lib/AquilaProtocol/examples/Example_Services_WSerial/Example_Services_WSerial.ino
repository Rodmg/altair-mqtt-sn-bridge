#include <Wire.h>
#include <Mesh.h>
#include <AquilaProtocol.h>
#include <WSerial.h>
#include <AquilaServices.h>
#include <ArduinoJson.h>

/*
Example_Services_WSerial
In this example you will be able
to change the red LED intensity from
the Hub, it will log via WSerial the
changes in intensity to the HUB, and
expose a service for consulting
current intensity.
*/
// Internal red LED
#define LED 13

// Intensity value
uint8_t currentIntensity = 0;

// Actions: turn off and turn on with param (intensity)
bool turnOff(uint8_t param, bool gotParam)
{
  currentIntensity = 0;
  digitalWrite(LED, HIGH);
  WSerial.println("Led Off");
}

bool turnOn(uint8_t param, bool gotParam)
{
  if(gotParam)
  {
    currentIntensity = param;
    analogWrite(LED, 255-currentIntensity);
    WSerial.print("Led intensity: ");
    WSerial.println(currentIntensity);
  }
  else
  {
    currentIntensity = 255;
    digitalWrite(LED, LOW);
    WSerial.println("Led On");
  }
}

// Status service:
bool statusService(uint16_t reqAddr, uint8_t method, char *data, uint8_t dataSize)
{
  WSerial.println("Status service requested");
  if(method == GET)
  {
    // Forming response:
    // Determining jsonBuffer size, in this case we will only have one number or "object"
    // More info: https://github.com/bblanchon/ArduinoJson/wiki/Memory%20model
    const int BUFFER_SIZE = JSON_OBJECT_SIZE(1);
    StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["intensity"] = currentIntensity;

    // We need to print the json to a buffer before sending
    // Make shure you have enough buffer space, in this case
    // the result will be: {"intensity":xxx}, so we only really need about 18 bytes
    char buffer[20];
    json.printTo(buffer, sizeof(buffer));
    Services.response(reqAddr, R200, buffer, strlen(buffer));
  }
  else
  {
    WSerial.println("Unsupported method requested for this service");
    Services.response(reqAddr, R405);
  }

}

void setup()
{
  pinMode(LED, OUTPUT);

  Mesh.begin();
  // Security encryption setup:
  uint8_t key[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
  Mesh.setSecurityKey(key);
  Mesh.setSecurityEnabled(false); // Not currently using security, enable it with true...

  WSerial.begin(HUB);
  Services.begin();
  // Initialize "status" service
  Services.add("status", statusService);

  Aquila.begin();
  Aquila.setClass("mx.makerlab.fade");
  Aquila.setName("Fade");

  Aquila.addAction("Off", turnOff);
  Aquila.addAction("Turn On with intensity", turnOn);

  Mesh.announce(HUB);
}

void loop()
{
  Mesh.loop();
  Aquila.loop();
  WSerial.loop();
  Services.loop();
}
