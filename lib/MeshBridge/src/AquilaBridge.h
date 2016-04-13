/**
* \file AquilaBridge.h
*
* \brief Aquila Mesh USB Bridge firmware.
*
* Copyright (C) 2015, Rodrigo Méndez. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
* 3. The name of Makerlab may not be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
* 4. This software may only be redistributed and used in connection with a
*    Makerlab product.
*
* THIS SOFTWARE IS PROVIDED BY MAKERLAB "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
* EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL MAKERLAB BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
* Modification and other use of this code is subject to Makerlab's Limited
* License Agreement (license.txt).
*
*/

#ifndef AQUILABRIDGE_H
#define AQUILABRIDGE_H

#include <Arduino.h>
#include <Mesh.h>
#include "Slip.h"

//#define BRIDGE_DEBUG

#define TIMEOUT 20
#define MAX_FRAME_SIZE 128

#define CMD_DATA 			0
#define CMD_ACK 			1
#define CMD_NACK 			2
#define CMD_GET_OPT 		3
#define CMD_SET_OPT 		4
#define CMD_GET_SECURITY 	5
#define CMD_SET_SECURITY 	6
#define CMD_START 			7
#define CMD_PING 			8
#define CMD_GET_LONG_ADDR 	9
#define CMD_SET_LONG_ADDR 	10

class AquilaBridge
{
private:
	Slip slip;
	bool isProm;
	uint16_t pan;
	uint8_t channel;
	uint8_t euiAddress[16];
public:
	AquilaBridge();
	bool begin(unsigned int baudrate, uint16_t addr, uint8_t channel, uint16_t pan, bool promiscuous);

	void setChannel(uint8_t chan);
	void setPan(uint16_t _pan);

	void loop();

	void sendComBuffer(uint8_t size);

	void sendAck(uint8_t rssi);
	void sendNack(uint8_t cause);
	void sendOpt();
	void sendSecurity();
	void sendStart();
	void sendLongAddr(uint8_t addr[]);

	void parseCommand(char *data, uint8_t size);

	void txSend(uint16_t dstAddr, uint8_t srcEndpoint, uint8_t dstEndpoint, uint8_t size, uint8_t *data);

};

extern AquilaBridge Bridge;

#endif //AQUILABRIDGE_H