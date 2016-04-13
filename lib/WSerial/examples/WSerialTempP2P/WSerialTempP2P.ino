/*
	WSerial example

	WSerialTemp is a temperature logger example
	that uses the WSerial interface for sending
	the temperature every half a second.

	This Program should be loaded to the Altair
	that will be far from the PC.

	You can receive the lectures of the Altair
	from another one connected to the PC, for
	this, please see WSerialTerminal example.

	Use:
	1. Set the local address (ADDR) and destination address
	(DEST) correctly.
	2. Upload the program to the Altair.

*/

#include <Wire.h>
#include "Mesh.h"
#include "WSerial.h"
#include <AltairTemperature.h>
#include <SimpleTimer.h>

// Local and destination addresses
#define ADDR 1
#define DEST 2

// Initialize software timer
SimpleTimer timer;

// Function that will send the temperature
void sendTemp()
{
	float temp = getTempC();
	WSerial.println(temp);
}

void setup()
{
	// Starting mesh network, setting ADDR manually
	Mesh.begin(ADDR);
	// Making a serial connection with DEST
	WSerial.begin(DEST);
	// Setup timer to send temperature every 500ms
	timer.setInterval(500, sendTemp);
	Mesh.announce(HUB);
}

void loop()
{
	// Listening to the network
	Mesh.loop();
	WSerial.loop();
	// Running timer
	timer.run();
}
