#include <Wire.h>
#include <Mesh.h>
#include <AquilaServices.h>
#include <SimpleTimer.h>
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

// Addresses, ADDR is local address, DEST is destination
#define ADDR 8
#define DEST 7

// We are using the SimpleTimer library for making request every second.
SimpleTimer timer;

// This function will be called when we receive the response of a request
void tempReqCb(uint16_t srcAddr, uint8_t status, char *data, uint8_t dataSize)
{
	/*
		Possible response status are:
		R200 0x04	// OK
		R404 0x05	// Service not found
		R405 0x06	// Method not allowed
		R408 0x07	// Timeout
		R500 0x08	// Service error
	*/
	if(status != R200) { Serial.print("Request Error Status: "); Serial.println(status, HEX); return;}
	if(dataSize > 0)
	{
		Serial.print("Response: "); Serial.println(data);

		// Allocating enough memory for the parser
		// Determining jsonBuffer size, in this case we are expecting only one number or "object"
		// More info: https://github.com/bblanchon/ArduinoJson/wiki/Memory%20model
		const int BUFFER_SIZE = JSON_OBJECT_SIZE(1);
		StaticJsonBuffer<BUFFER_SIZE> parser;
		JsonObject& json = parser.parseObject(data);
		if(!json.success()) { Serial.println("Error parsing JSON"); return; }

		Serial.print("Temperature: ");
		Serial.println((double)json["temp"]);
	}
	else Serial.println("Got null data...");
}

// This function makes the request
void getTemp()
{
	Serial.println("Making Request...");
	Services.request(DEST, GET, "temperature", tempReqCb);
}

void setup()
{
	Serial.begin(9600);
	Mesh.begin(ADDR);
	Services.begin();

	// Execute getTemp() every 1000 ms
	timer.setInterval(1000, getTemp);

	Serial.println("Temp Consumer starting...");
	Mesh.announce(HUB);
}

void loop()
{
	Mesh.loop();
	Services.loop();
	timer.run();
}
