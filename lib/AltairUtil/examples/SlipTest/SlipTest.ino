#include "Wire.h"
#include "Mesh.h"
#include "Slip.h"
#include "CRC.h"

/*
	Test in HEX:
	
	C0 48 65 6C 6C 6F 20 57 6F 72 6C 64 21 D3 0C C0

	C0 66 6F 6F 62 61 72 25 B0 C0

	C0 63 6F DB DC 6F 6C DB DD 61 62 51 49 C0
 */

char test1[] = "Hello World!";
char test2[] = "foobar";
char test3[] = {'c', 'o', 0xC0, 'o', 'l', 0xDB, 'a', 'b', 0};

Slip slip;

void onData(char *data, uint8_t size)
{
	if(checkCrc(data, size))
	{
		Serial.println("Received: ");
		// for(int i = 0; i < size-2; i++)
		// {
		// 	Serial1.print(data[i], HEX);
		// }
		// Serial1.println();
		for(int i = 0; i < size-2; i++)
		{
			Serial1.print(data[i]);
		}
		Serial1.println();
	}
	else
	{
		Serial1.println("CRC error");
	}
}

void send(char *data, uint8_t size)
{
	char buffer[255];
	memcpy(buffer, data, size);
	size = appendCrc(buffer, size);
	slip.send(buffer, size);
}

void setup()
{
	Serial1.begin(9600);
	slip.begin(57600, onData);

	send(test1, strlen(test1));
	send(test2, strlen(test2));
	send(test3, strlen(test3));
}

void loop()
{
	slip.loop();
}