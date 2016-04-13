#include <Wire.h>
#include <Mesh.h>
#include <AquilaProtocol.h>
#include <AquilaServices.h>

/*
	Aquila Services Variables example

	In this example we explore how we can expose
	int or float variables as services.

	This way, we can treat global variables as we
	normally do, while being able to consult their 
	current value at any time.

	We have 2 variables, called count1 and count2.
	In the setup, we expose them as services with
	the function Services.variable("serviceName", &variableName).
	Please note the 'ampersand' (&), this is needed as we are
	passing the pointer of the variable.

	Then we can normally access and change the variables in the
	loop.

	To consult the current value of the variable, we can make a 
	request to the Hub API and get the value as a JSON object,
	for example, we could get: {"val":123}.

	For more information, see: http://docs.aquila2.apiary.io/
 */

int count1;
float count2;

void setup()
{
	Mesh.begin();
	Aquila.begin();
	Services.begin();

	count1 = 0;
	count2 = 0.0;

	Services.variable("count1", &count1);
	Services.variable("count2", &count2);

	Aquila.setClass("mx.makerlab.varexample");
	Aquila.setName("Variables");

	Mesh.announce(HUB);
}

void loop()
{
	Mesh.loop();
	Aquila.loop();
	Services.loop();

	count1++;
	count2 += 10.93;
}