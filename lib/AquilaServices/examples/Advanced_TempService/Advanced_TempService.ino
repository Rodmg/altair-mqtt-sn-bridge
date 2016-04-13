#include <Wire.h>
#include <Mesh.h>
#include <AquilaServices.h>
#include <AltairTemperature.h>
#include <ArduinoJson.h>

/*
	Aquila Services Example:

	This example consist in two files: TempConsumer and TempService.

	You should upload each one to a different Altair, then you can monitor
	their status from the serial console.

	The Altair with TempService will define a service called "temperature", that will respond
	with the current temperature of the device.

	The Altair with TempConsumer will be making requests to the other one each second and
	display the response in the serial console.

	We are using the Arduino Json library for encoding and decoding the data.
	You can get its documentation here: https://github.com/bblanchon/ArduinoJson
*/

// Local address
#define ADDR 7

// This function will handle the requests for the "temperature" service
bool tempService(uint16_t reqAddr, uint8_t method, char *data, uint8_t dataSize)
{
	/*
		Possible methods are:
		GET
		POST
		PUT
		DELETE
	*/
	if(method == GET)
	{
		Serial.println("Temperature service requested");
		// getTempC from AltairTemperature library
		float temp = getTempC();
		// Determining jsonBuffer size, in this case we will only have one number or "object"
		// More info: https://github.com/bblanchon/ArduinoJson/wiki/Memory%20model
		const int BUFFER_SIZE = JSON_OBJECT_SIZE(1);
		StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
		JsonObject& json = jsonBuffer.createObject();
		json["temp"] = temp;

		// We need to print the json to a buffer before sending
		// Make shure you have enough buffer space, in this case
		// the result will be: {"temp":xx.xx}, so we only really need about 14 bytes
		char buffer[16];
		json.printTo(buffer, sizeof(buffer));
		Services.response(reqAddr, R200, buffer, strlen(buffer));
	}
	else
	{
		Serial.println("Unsupported method requested for this service");
		Services.response(reqAddr, R405);
	}

}

void setup()
{
	Serial.begin(9600);

	Mesh.begin(ADDR);
	Services.begin();

	// Initialize "temperature" service
	Services.add("temperature", tempService);

	Serial.println("Temperature Service starting...");
	Mesh.announce(HUB);
}

void loop()
{
	Mesh.loop();
	Services.loop();
}
