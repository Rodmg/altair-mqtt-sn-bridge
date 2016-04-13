#include <Wire.h>
#include <Mesh.h>
#include <AquilaProtocol.h>
#include <AquilaServices.h>
#include <AltairTemperature.h>

/*
	Temperature Function example

	Exposes a function for getting the current temperature from
	Altair's integrated temperature sensor.

	For more information on how to use the service from the Hub,
	see: http://docs.aquila2.apiary.io/
 */

float temperature(uint8_t method, char *data, uint8_t dataSize)
{
	float temp = getTempC();
	return temp;
}

void setup()
{
	Serial.begin(9600);
	Mesh.begin();
	Aquila.begin();
	Services.begin();

	Services.function("temperature", temperature);

	Aquila.setClass("mx.makerlab.tempexample");
	Aquila.setName("Thermometer");

	Mesh.announce(HUB);
}

void loop()
{
	Mesh.loop();
	Aquila.loop();
	Services.loop();
}