/*
	WSerial Example

	WSerialTerminal is a bridge serial interface between
	the Wireless Serial and the USB-Serial connection
	to the PC.

	This program should be loaded to the Altair that will
	be connected to the PC.

	Use:
	1. Set the local address (ADDR) and destination address
	(DEST) correctly.
	2. Upload the program to the Altair.
	3. Open the Serial terminal on your PC.

	You should receive the message "Wireless Serial Terminal",
	now any character that you type will be sent to the second
	Altair, and any message that the second Altair sends
	will be repeated to your serial terminal program.
*/

#include <Wire.h>
#include "Mesh.h"
#include "WSerial.h"

// Local and destination addresses
#define ADDR 2
#define DEST 1

void setup()
{
	// Starting wired serial interface
	Serial.begin(9600);
	Serial.println("Wireless Serial Terminal");
	// Starting mesh network, setting ADDR manually
	Mesh.begin(ADDR);
	// Making a serial connection with DEST
	WSerial.begin(DEST);
	Mesh.announce(HUB);
}

void loop()
{
	// Listening to the network
	Mesh.loop();
	WSerial.loop();
	// When WSerial message is received, redirect it to the
	// wired serial.
	while(WSerial.available())
	{
		Serial.write(WSerial.read());
	}
	// When Serial message is received, redirect it to the
	// WSerial.
	while(Serial.available())
	{
		WSerial.write(Serial.read());
	}
}
