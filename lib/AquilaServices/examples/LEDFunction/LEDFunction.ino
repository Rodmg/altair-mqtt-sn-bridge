#include <Wire.h>
#include <Mesh.h>
#include <AquilaProtocol.h>
#include <AquilaServices.h>

/*
	LED Function example

	Exposes a function that controlls the LED state.
	If the method is GET, only returns the actual LED state.
	If its any other method, toggles the LED and returns the new state.

	TIP: You could modify this program so that if you send an JSON
	object with the specific state you want (for example: {"state":1}),
	you could parse it and set the LED accordingly. You can use the
	JsonParser of the ArduinoJson library, see how its used in the Advanced_LEDService
	example.

	For more information on how to use the service from the Hub,
	see: http://docs.aquila2.apiary.io/
 */

#define LED 14

bool ledState = true;

float led(uint8_t method, char *data, uint8_t dataSize)
{
	switch(method)
	{
		case GET:
			return ledState;
			break;
		default:
			ledState = !ledState;
			digitalWrite(LED, ledState);
			return ledState;
			break;
	}
}

void setup()
{
	pinMode(LED, OUTPUT);
	digitalWrite(LED, HIGH);

	Mesh.begin();
	Aquila.begin();
	Services.begin();

	Services.function("led", led);

	Aquila.setClass("mx.makerlab.ledexample");
	Aquila.setName("Led");

	Mesh.announce(HUB);
}

void loop()
{
	Mesh.loop();
	Aquila.loop();
	Services.loop();
}