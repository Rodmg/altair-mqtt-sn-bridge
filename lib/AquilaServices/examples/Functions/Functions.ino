#include <Wire.h>
#include <Mesh.h>
#include <AquilaProtocol.h>
#include <AquilaServices.h>

/*
	Aquila Services Functions example

	In this example we expose two functions as services.

	Functions should have the following signature:
	float myFunction(uint8_t method, char *data, uint8_t dataSize)

	When we receive a request for the function service, the function
	is called and its return value sent back to the requester as a 
	JSON object.

	For more information, see: http://docs.aquila2.apiary.io/

 */

// Function definitions
// doSomething: Prints that its doing something and responds the number 42
float doSomething(uint8_t method, char *data, uint8_t dataSize)
{
	Serial.println("Doing something and returning 42");
	return 42.00;
}

// printInfo: Prints the request's data and responds with number 0
float printInfo(uint8_t method, char *data, uint8_t dataSize)
{
	switch(method)
	{
		case GET:
			Serial.println("Got GET");
			break;
		case POST:
			Serial.println("Got POST");
			break;
		case PUT:
			Serial.println("Got PUT");
			break;
		case DELETE:
			Serial.println("Got DELETE");
			break;
	}

	if(dataSize > 0)
	{
		Serial.print("Got ");
		Serial.print(dataSize);
		Serial.println(" bytes of data:");
		for(int i = 0; i < dataSize; i++)
		{
			Serial.print(data[i], HEX);
			Serial.print(" ");
		}
		Serial.println();
	}	

	return 0;

}

void setup()
{
	Serial.begin(9600);

	Mesh.begin();
	Aquila.begin();
	Services.begin();

	Services.function("doSomething", doSomething);
	Services.function("printInfo", printInfo);

	Aquila.setClass("mx.makerlab.funcexample");
	Aquila.setName("Functions");

	Mesh.announce(HUB);
}

void loop()
{
	Mesh.loop();
	Aquila.loop();
	Services.loop();
}