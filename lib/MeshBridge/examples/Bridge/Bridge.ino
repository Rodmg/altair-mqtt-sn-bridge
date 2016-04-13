#include <Wire.h>
#include <Mesh.h>
#include "AquilaBridge.h"

// Version 0.3.0, Please note that this version is only compatible with Aquila Server 0.2.0 or newer

/*
	MeshBridge
	This is the Firmware for the Altair that will act as a bridge interface between the Mesh network and the
	Aquila Server. It implements a simple serial protocol for configuring it, sending and receiving Mesh packets.
*/

/*
	New Data Format 10/11/14: Now we are using LWM, thus all MAC processing is done in the uC, we only send relevant data to PC.
		lqi rssi srcAddr(16) dstAddr(16) srcEndpoint dstEndpoint frameSize [LWM Data]
*/

/*
 *	Update: 24/04/15: Changed encapsulation protocol to SLIP (RFC 1055)(http://en.wikipedia.org/wiki/Serial_Line_Internet_Protocol),
 *					  added 16bit CRC to end of packet
 *
 * 	Serial packet: [SLIP END (0xC0)][DATA (SLIP Escaped)][SLIP END (0xC0)]
 * 	DATA packet: [Command][Command specific][CRC (16bit)]
 *
 *	Comands:
 *		CMD_DATA: 			[Command specific] = lqi rssi srcAddr(16) dstAddr(16) srcEndpoint dstEndpoint frameSize [MAC Frame]
 *		CMD_ACK:			[Command specific] = RSSI												*Sent on data transmit success
 *		CMD_NACK:			[Command specific] = CAUSE												*Sent on data tranmit error
 *		CMD_GET_OPT:		[Command specific] = PROM* PAN_LOW PAN_HIGH CHAN**	ADDR_LOW ADDR_HIGH	*Returns the current options
 *		CMD_SET_OPT:		[Command specific] = PROM* PAN_LOW PAN_HIGH CHAN**	ADDR_LOW ADDR_HIGH	*0 or 1, **11-26
 *		CMD_GET_SECURITY:	[Command specific] = ENABLED											*Get if security enabled
 *		CMD_SET_SECURITY: 	[Command specific] = ENABLED [SEC_KEY](16 byte, 128 bit)				*Set if security enabled and key
 *		CMD_START																					*Sent on bridge start or reset and in response to CMD_PING
 *		CMD_PING:																					*Sent by PC, response is CMD_START
 *		CMD_GET_LONG_ADDR:																			*Get bridge MAC address
 */

/*
 *	Cases:
 *	PC 	| CMD_PING	------> | Bridge
 *		| <------ CMD_START	|
 *		|					|
 *
 *	PC 	| CMD_DATA	------------------> | Bridge
 *		| <-- CMD_ACK or CMD_NACK		|
 *		|								|
 *
 *	PC 	| CMD_SET_*	------------------> | Bridge
 *		| <--- CMD_SET_* (Confirmation)	|
 *		|								|
 *
 *	On Data reception:
 * 	PC 	| <------ CMD_DATA 	| Bridge
 *		|					|
 *
 *	On Bridge Startup:
 *	PC 	| <------ CMD_START	| Bridge
 *		|					|
 *
 */
#include <avr/wdt.h>  

#define ADDRESS 0x00FF
#define CHANNEL 0X1A
#define PAN 0xCA5A
#define PROMISCUOUS false
#define BAUDRATE 57600

#define ERROR_LED 13

void fatalError()
{
	pinMode(ERROR_LED, OUTPUT);
	for(;;)
	{
		digitalWrite(ERROR_LED, HIGH);
		delay(200);
		digitalWrite(ERROR_LED, LOW);
		delay(200);
	}
}

void setup()
{
	wdt_disable();
	wdt_reset();
	// Check reset source
	Serial1.begin(9600);
	Serial1.println("reset");
	Serial1.println(MCUSR, HEX);
	if(!Bridge.begin(BAUDRATE, ADDRESS, CHANNEL, PAN, PROMISCUOUS)) fatalError();
	wdt_enable(WDTO_1S);
}

void loop()
{
	wdt_reset();
	Bridge.loop();
}
