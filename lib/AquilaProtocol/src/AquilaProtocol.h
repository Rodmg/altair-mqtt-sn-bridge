/**
* \file AquilaProtocol.h
*
* \brief Aquila Events, Actions and Interactions based protocol implementation.
*
* Copyright (C) 2014, Rodrigo Méndez. All rights reserved.
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

#ifndef AQUILAPROTOCOL_H
#define AQUILAPROTOCOL_H

#include <stdint.h>
#include <stdbool.h>
#include <Mesh.h>
#include "lwm/sys/sys.h"
#include "lwm/nwk/nwk.h"
#include "stack/config.h"
#include "ProtocolDB.h"
#include "stack/halID.h"

#define PROTOCOL_ENDPOINT 13
#define PROTOCOL_VERSION 3
#define PROTOCOL_DEFAULT_ID "mx.makerlab.example"
#define PROTOCOL_DEFAULT_NAME "Example"
#define PROTOCOL_EUIADDRESSLEN 8

#ifndef PROTOCOL_MAXACTIONS
#define PROTOCOL_MAXACTIONS 100
#endif

#ifndef PROTOCOL_MAXEVENTS
#define PROTOCOL_MAXEVENTS 100
#endif

#ifndef PROTOCOL_MAXONSUBS
#define PROTOCOL_MAXONSUBS 10
#endif

//Control byte indexes:
#define PROTOCOL_COMMAND_TYPE 0
#define PROTOCOL_HAS_PARAM 3
#define PROTOCOL_HAS_DATA 4

//Command type definitions:
#define PROTOCOL_NACK 	0x00
#define PROTOCOL_ACK  	0x01
#define PROTOCOL_ACTION 0x02
#define PROTOCOL_GET 	0x03
#define PROTOCOL_POST 	0x04
#define PROTOCOL_CUSTOM 0x05
#define PROTOCOL_EVENT 	0x06

//Command definitions:
#define PROTOCOL_COM_NACTIONS 0
#define PROTOCOL_COM_NEVENTS 1
#define PROTOCOL_COM_ID	2
#define PROTOCOL_COM_SIZE 3
#define PROTOCOL_COM_NENTRIES 4
#define PROTOCOL_COM_ABSENTRY 5
#define PROTOCOL_COM_ENTRY 6
#define PROTOCOL_COM_CLEAR 7
#define PROTOCOL_COM_ADDENTRY 8
#define PROTOCOL_COM_DELABSENTRY 9
#define PROTOCOL_COM_DELENTRY 10
#define PROTOCOL_COM_ACTION 11
#define PROTOCOL_COM_EVENT 12
#define PROTOCOL_COM_NAME 13
#define PROTOCOL_COM_EUI 14

typedef struct
{
	char *description;
	bool (*function)(uint8_t param, bool gotParam);
} Action;

#define Event uint8_t

typedef struct 
{
	char *eName;
	uint8_t EUIAddress[PROTOCOL_EUIADDRESSLEN];
	bool (*function)(uint8_t param, bool gotParam);
	bool (*extFunction)(uint8_t param, bool gotParam, uint16_t shortAddr, uint8_t *longAddr);
} OnSubscription;

class AquilaProtocol
{
private:
	char *id;	// class
	char *name;
	uint8_t EUIAddress[PROTOCOL_EUIADDRESSLEN];

	uint8_t nActions;
	uint8_t nEvents;
	uint8_t nOnSubs;
 	Action *actions[PROTOCOL_MAXACTIONS];
 	char *events[PROTOCOL_MAXEVENTS];
 	OnSubscription *onSubs[PROTOCOL_MAXONSUBS];

 	void send(uint16_t address, char *command, uint8_t size);

 	void parsePacket();

 	void sendAck(uint16_t address);
	void sendNack(uint16_t address);
	void sendNActions(uint16_t address);
	void sendNEvents(uint16_t address);
	void sendId(uint16_t address);
	void sendName(uint16_t address);
	void sendEUI(uint16_t address);
	void sendSize(uint16_t address);
	void sendNEntries(uint16_t address);
	void sendEntry(uint8_t nEntry, uint16_t address);
	void clearEntries(uint16_t address);
	void addEntry(Entry *entry, uint16_t address);
	void delEntry(uint8_t nEntry, uint16_t address);
	void setEntry(uint8_t nEntry, Entry *entry, uint16_t address);
	void sendAction(uint8_t n, uint16_t address);
	void sendEvent(uint8_t n, uint16_t address);
	void checkLocalEvent(uint8_t *EUIAddress, uint8_t event, uint8_t param, uint8_t hasParam, uint8_t* eName, uint16_t shortAddress);
	void checkEvent(uint8_t *EUIAddress, uint8_t event, uint8_t param, uint8_t hasParam, uint8_t* eName, uint16_t shortAddress);
	bool EUIAddressEquals(uint8_t *addr1, uint8_t *addr2);

public:
	/*
		Main functions:
	*/
	int begin();

	void loop();

	void setClass(char *nid);

	void setName(char *nName);

	void setAction(uint8_t n, char description[], bool (*function)(uint8_t param, bool gotParam));

	void addAction(char description[], bool (*function)(uint8_t param, bool gotParam));

	void setEvent(uint8_t n, char description[]);

	Event addEvent(char description[]);

	void emit(uint8_t event, uint8_t param=0, bool hasParam=false);

	// Emit to a specific address, for special implementations
	void emit(uint16_t dest, uint8_t event, uint8_t param=0, bool hasParam=false);

	bool on(char* eventName, bool (*function)(uint8_t param, bool gotParam));

	bool on(char* eventName, bool (*function)(uint8_t param, bool gotParam, uint16_t shortAddr, uint8_t *longAddr));
	
	bool on(char* eventName, uint8_t* EUIAddress, bool (*function)(uint8_t param, bool gotParam));

	/*
		Advanced functions:
	*/

	void requestAction(uint16_t address, uint8_t action, uint8_t param=0, bool hasParam=false);

	bool doAction(uint8_t action, uint8_t param, bool gotParam);
};

extern AquilaProtocol Aquila;

#endif //AQUILAPROTOCOL_H
