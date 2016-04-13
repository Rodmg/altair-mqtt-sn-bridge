/**
* \file AquilaProtocol.cpp
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

#include "Arduino.h"
#include "lwm/sys/sys.h"
#include "lwm/nwk/nwk.h"
#include "Mesh.h"
#include "AquilaProtocol.h"

static RxPacket receivedPacket;
static uint8_t receivedData[AQUILAMESH_MAXPAYLOAD];
static uint8_t flagGotPacket;
static uint8_t euiBroadcast[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

static bool Protocol__rxHandler(RxPacket *ind)
{
	receivedPacket.srcAddr = ind->srcAddr;
	receivedPacket.size = ind->size;
	for(int i = 0; i < ind->size; i++)
	{
		receivedData[i] = ind->data[i];
	}
	receivedPacket.data = receivedData;
	flagGotPacket++;
	return true;
}

static void Protocol__DataCb(TxPacket *req)
{
	(void)req;
}

// PRIVATE //

void AquilaProtocol::send(uint16_t address, char *command, uint8_t size)
{
	uint8_t requestAck = 0;

	char data[AQUILAMESH_MAXPAYLOAD];
	data[0] = PROTOCOL_VERSION;		//appends protocol version as first byte
	memcpy(&data[1], command, size);

	static TxPacket packet;	// Maybe make it static?
	packet.dstAddr = address;
	packet.dstEndpoint = PROTOCOL_ENDPOINT;
	packet.srcEndpoint = PROTOCOL_ENDPOINT;
	
	if(address == BROADCAST) requestAck = 0;
	else requestAck = NWK_OPT_ACK_REQUEST;

	packet.options = requestAck;
	packet.data = (uint8_t*)data;
	packet.size = (size + 1);
	packet.confirm = Protocol__DataCb;

	Mesh.sendPacket(&packet);
}

void AquilaProtocol::parsePacket()
{
	flagGotPacket = 0;

	uint16_t sAddr = receivedPacket.srcAddr;

	uint8_t dataLen = receivedPacket.size;
	char *data = (char*)receivedPacket.data;

	if(dataLen < 2) { sendNack(sAddr); return; }

	uint8_t pktVersion = data[0];

	if(pktVersion != PROTOCOL_VERSION) { sendNack(sAddr); return; }	//if protocol version is diferent, nack and do nothing.

	uint8_t pktControl = data[1];
	uint8_t commandType = pktControl & 0b00000111;
	bool hasParam = (bool) (pktControl>>PROTOCOL_HAS_PARAM & 0x01);
	bool hasData = (bool) (pktControl>>PROTOCOL_HAS_DATA & 0x01);

	uint8_t command, param = 0, index;
	uint8_t rxEUIAddress[PROTOCOL_EUIADDRESSLEN];
	Entry entry;

	// Event variables
	uint8_t eNameLen = 0;
	uint8_t eName[AQUILAMESH_MAXPAYLOAD - (4 + PROTOCOL_EUIADDRESSLEN)];
	// String security
	eName[0] = NULL;

	switch(commandType)
	{
		case PROTOCOL_NACK:
			return;
			break;

		case PROTOCOL_ACK:
			return;
			break;

		case PROTOCOL_ACTION:
			if(dataLen < 3 + hasParam) { sendNack(sAddr); return; }
			command = data[2];
			if(hasParam) param = data[3];
			if(doAction(command, param, hasParam)) sendAck(sAddr);
			else sendNack(sAddr);
			return;
			break;

		case PROTOCOL_GET:
			if(dataLen < 3 + hasParam) { sendNack(sAddr); return; }
			command = data[2];
			if(hasParam) param = data[3];

			switch(command)
			{

				case PROTOCOL_COM_NACTIONS:
					sendNActions(sAddr);
					break;
				case PROTOCOL_COM_NEVENTS:
					sendNEvents(sAddr);
					break;
				case PROTOCOL_COM_ID:
					sendId(sAddr);
					break;
				case PROTOCOL_COM_SIZE:
					sendSize(sAddr);
					break;
				case PROTOCOL_COM_NENTRIES:
					sendNEntries(sAddr);
					break;
				case PROTOCOL_COM_ENTRY:
					if(hasParam) sendEntry(param, sAddr);
					else sendNack(sAddr);
					break;
				case PROTOCOL_COM_ACTION:
					if(hasParam) sendAction(param, sAddr);
					else sendNack(sAddr);
					break;
				case PROTOCOL_COM_EVENT:
					if(hasParam) sendEvent(param, sAddr);
					else sendNack(sAddr);
					break;
				case PROTOCOL_COM_NAME:
					sendName(sAddr);
					break;
				case PROTOCOL_COM_EUI:
					sendEUI(sAddr);
					break;
			}
			return;
			break;

		case PROTOCOL_POST:
			if(dataLen < 3 + hasParam) { sendNack(sAddr); return; }
			command = data[2];
			if(hasParam) param = data[3];

			switch(command)
			{

				case PROTOCOL_COM_CLEAR:
					clearEntries(sAddr);
					break;
				case PROTOCOL_COM_ENTRY:
					if(hasParam && hasData && dataLen >= (4 + PROTOCOL_ENTRYLEN))
					{
						memcpy(&entry, &data[4], PROTOCOL_ENTRYLEN);
						setEntry(param, &entry, sAddr);
					}
					else sendNack(sAddr);
					break;
				case PROTOCOL_COM_ADDENTRY:
					if(hasData && !hasParam && dataLen >= (3 + PROTOCOL_ENTRYLEN))
					{
						memcpy(&entry, &data[3], PROTOCOL_ENTRYLEN);
						addEntry(&entry, sAddr);
					}
					else sendNack(sAddr);
					break;
				case PROTOCOL_COM_DELENTRY:
					if(hasParam) delEntry(param, sAddr);
					else sendNack(sAddr);
					break;
			}
			return;
			break;

		case PROTOCOL_CUSTOM:
			sendNack(sAddr);
			return;
			break;

		case PROTOCOL_EVENT:
			if(dataLen < 3 + hasParam + PROTOCOL_EUIADDRESSLEN) { return; }
			index = 2;
			command = data[index++];
			if(hasParam) param = data[index++];
			// Get EUI Address
			memcpy(rxEUIAddress, &data[index], PROTOCOL_EUIADDRESSLEN);
			index += PROTOCOL_EUIADDRESSLEN;
			// Parsing event name
			if(dataLen > 3 + hasParam + PROTOCOL_EUIADDRESSLEN)
			{
				eNameLen = data[index++];
				if( eNameLen <= (dataLen - index) )
				{
					memcpy(eName, &data[index], eNameLen);
					// String security
					eName[eNameLen] = NULL;
				}

			}

			checkEvent(rxEUIAddress, command, param, hasParam, (uint8_t*)eName, sAddr);
			return;
			break;

		default:
			sendNack(sAddr);
			return;
	}
}

void AquilaProtocol::sendAck(uint16_t address)
{
	char packet[1];
	//control byte:
	packet[0] = PROTOCOL_ACK<<PROTOCOL_COMMAND_TYPE | 0<<PROTOCOL_HAS_PARAM | 0<<PROTOCOL_HAS_DATA;
	send(address, packet, 1);
}

void AquilaProtocol::sendNack(uint16_t address)
{
	char packet[1];
	//control byte:
	packet[0] = PROTOCOL_NACK<<PROTOCOL_COMMAND_TYPE | 0<<PROTOCOL_HAS_PARAM | 0<<PROTOCOL_HAS_DATA;
	send(address, packet, 1);
}

void AquilaProtocol::sendNActions(uint16_t address)
{
	char packet[3];
	//control byte:
	packet[0] = PROTOCOL_POST<<PROTOCOL_COMMAND_TYPE | 0<<PROTOCOL_HAS_PARAM | 1<<PROTOCOL_HAS_DATA;
	//command byte:
	packet[1] = PROTOCOL_COM_NACTIONS;
	//data byte:
	packet[2] = this->nActions;

   	send(address, packet, 3);
}

void AquilaProtocol::sendNEvents(uint16_t address)
{
	char packet[3];
	//control byte:
	packet[0] = PROTOCOL_POST<<PROTOCOL_COMMAND_TYPE | 0<<PROTOCOL_HAS_PARAM | 1<<PROTOCOL_HAS_DATA;
	//command byte:
	packet[1] = PROTOCOL_COM_NEVENTS;
	//data byte:
	packet[2] = this->nEvents;

    send(address, packet, 3);
}

void AquilaProtocol::sendId(uint16_t address)
{
	uint8_t idLen = strlen(this->id);

	char packet[AQUILAMESH_MAXPAYLOAD];
	//control byte:
	packet[0] = PROTOCOL_POST<<PROTOCOL_COMMAND_TYPE | 0<<PROTOCOL_HAS_PARAM | 1<<PROTOCOL_HAS_DATA;
	//command byte:
	packet[1] = PROTOCOL_COM_ID;
	//data:
	memcpy(&packet[2], this->id, idLen);

    send(address, packet, 2 + idLen);
}

void AquilaProtocol::sendName(uint16_t address)
{
	uint8_t nameLen = strlen(this->name);

	char packet[AQUILAMESH_MAXPAYLOAD];
	//control byte:
	packet[0] = PROTOCOL_POST<<PROTOCOL_COMMAND_TYPE | 0<<PROTOCOL_HAS_PARAM | 1<<PROTOCOL_HAS_DATA;
	//command byte:
	packet[1] = PROTOCOL_COM_NAME;
	//data:
	memcpy(&packet[2], this->name, nameLen);

    send(address, packet, 2 + nameLen);
}

void AquilaProtocol::sendEUI(uint16_t address)
{
	char packet[2 + PROTOCOL_EUIADDRESSLEN];
	//control byte;
	packet[0] = PROTOCOL_POST<<PROTOCOL_COMMAND_TYPE | 0<<PROTOCOL_HAS_PARAM | 1<<PROTOCOL_HAS_DATA;
	//command byte:
	packet[1] = PROTOCOL_COM_EUI;
	//data:
	memcpy(&packet[2], this->EUIAddress, PROTOCOL_EUIADDRESSLEN);

	send(address, packet, 2 + PROTOCOL_EUIADDRESSLEN);
}

void AquilaProtocol::sendSize(uint16_t address)
{
	char packet[3];
	//control byte:
	packet[0] = PROTOCOL_POST<<PROTOCOL_COMMAND_TYPE | 0<<PROTOCOL_HAS_PARAM | 1<<PROTOCOL_HAS_DATA;
	//command byte:
	packet[1] = PROTOCOL_COM_SIZE;
	//data byte:
	packet[2] = (char)PROTOCOL_MAXENTRIES;

    send(address, packet, 3);
}

void AquilaProtocol::sendNEntries(uint16_t address)
{
	DBHeader header;
	PDB_readHeader(&header);
	int nEntries = header.nEntries;

	if (nEntries > PROTOCOL_MAXENTRIES) nEntries = 0;

	char packet[3];
	//control byte:
	packet[0] = PROTOCOL_POST<<PROTOCOL_COMMAND_TYPE | 0<<PROTOCOL_HAS_PARAM | 1<<PROTOCOL_HAS_DATA;
	//command byte:
	packet[1] = PROTOCOL_COM_NENTRIES;
	//data byte:
	packet[2] = nEntries;

    if(address != BROADCAST) send(address, packet, 3);
}

void AquilaProtocol::sendEntry(uint8_t nEntry, uint16_t address)
{
	Entry entry;
	if(!PDB_getEntry(&entry, nEntry))
	{
		sendNack(address);
		return;
	}
	char packet[3 + PROTOCOL_ENTRYLEN];
	//control byte:
	packet[0] = PROTOCOL_POST<<PROTOCOL_COMMAND_TYPE | 1<<PROTOCOL_HAS_PARAM | 1<<PROTOCOL_HAS_DATA;
	//command byte:
	packet[1] = PROTOCOL_COM_ENTRY;
	//param byte:
	packet[2] = nEntry;
	//data bytes:
	memcpy(&packet[3], &entry, PROTOCOL_ENTRYLEN);

    if(address != BROADCAST) send(address, packet, 3 + PROTOCOL_ENTRYLEN);
}

void AquilaProtocol::clearEntries(uint16_t address)
{
	// cleans database header.
	PDB_format();
	sendAck(address);
}

void AquilaProtocol::addEntry(Entry *entry, uint16_t address)
{
	if(PDB_appendEntry(entry)) sendAck(address);
	else sendNack(address);
}

void AquilaProtocol::delEntry(uint8_t nEntry, uint16_t address)
{
	if(PDB_delEntry(nEntry)) sendAck(address);
	else sendNack(address);
}

void AquilaProtocol::setEntry(uint8_t nEntry, Entry *entry, uint16_t address)
{
	if(PDB_setEntry(entry, nEntry)) sendAck(address);
	else sendNack(address);
}

void AquilaProtocol::sendAction(uint8_t n, uint16_t address)
{
	if(n < PROTOCOL_MAXACTIONS)
	{
		uint8_t cont = 0;
		int i;
		for(i = 0; i < PROTOCOL_MAXACTIONS; i++)
		{
			if(this->actions[i] != NULL)
			{
				if(cont == n)
				{

					char packet[AQUILAMESH_MAXPAYLOAD];
					//control byte:
					packet[0] = PROTOCOL_POST<<PROTOCOL_COMMAND_TYPE | 1<<PROTOCOL_HAS_PARAM | 1<<PROTOCOL_HAS_DATA;
					//command byte:
					packet[1] = PROTOCOL_COM_ACTION;
					//param byte:
					packet[2] = i;
					//data:
					uint8_t descLen = strlen(this->actions[i]->description);
					memcpy(&packet[3], this->actions[i]->description, descLen);

				    send(address, packet, 3 + descLen);
					return;
				}
				cont++;
			}
		}
	}
	sendNack(address);
}

void AquilaProtocol::sendEvent(uint8_t n, uint16_t address)
{
	if(n < PROTOCOL_MAXEVENTS)
	{
		uint8_t cont = 0;
		int i;
		for(i = 0; i < PROTOCOL_MAXEVENTS; i++)
		{
			if(this->events[i] != NULL)
			{
				if(cont == n)
				{
					char packet[AQUILAMESH_MAXPAYLOAD];
					//control byte:
					packet[0] = PROTOCOL_POST<<PROTOCOL_COMMAND_TYPE | 1<<PROTOCOL_HAS_PARAM | 1<<PROTOCOL_HAS_DATA;
					//command byte:
					packet[1] = PROTOCOL_COM_EVENT;
					//param byte:
					packet[2] = i;
					//data:
					uint8_t descLen = strlen(this->events[i]);
					memcpy(&packet[3], this->events[i], descLen);

				    send(address, packet, 3 + descLen);
					return;
				}
				cont++;
			}
		}
	}
	sendNack(address);
}

// Check events subscribed in code (runtime)
void AquilaProtocol::checkLocalEvent(uint8_t *EUIAddress, uint8_t event, uint8_t param, uint8_t hasParam, uint8_t* eName, uint16_t shortAddress)
{
	for(int i = 0; i < this->nOnSubs; i++)
	{
		if(eName == NULL) continue;
		if(strcmp((char*)this->onSubs[i]->eName, (char*)eName) == 0)
		{
			if(EUIAddressEquals(this->onSubs[i]->EUIAddress, euiBroadcast))
			{
				if(this->onSubs[i]->function != NULL)
					this->onSubs[i]->function(param, hasParam);
				else if(this->onSubs[i]->extFunction != NULL)
					this->onSubs[i]->extFunction(param, hasParam, shortAddress, EUIAddress);
			}
			else if(EUIAddressEquals(this->onSubs[i]->EUIAddress, EUIAddress))
			{
				if(this->onSubs[i]->function != NULL)
					this->onSubs[i]->function(param, hasParam);
				else if(this->onSubs[i]->extFunction != NULL)
					this->onSubs[i]->extFunction(param, hasParam, shortAddress, EUIAddress);
			}
		}
	}
}

// Check events subscribed in EEPROM (configured from the hub)
void AquilaProtocol::checkEvent(uint8_t *EUIAddress, uint8_t event, uint8_t param, uint8_t hasParam, uint8_t* eName, uint16_t shortAddress)
{
	checkLocalEvent(EUIAddress, event, param, hasParam, eName, shortAddress);

	DBHeader header;
	Entry entry;
	PDB_readHeader(&header);
	uint8_t nEntries = header.nEntries;

	if (nEntries > PROTOCOL_MAXENTRIES) nEntries = 0;  //checa en caso de numero invalido (si no se ha programado nunca)

	PDB_iteratorBegin();
	int i = 0;
	for(i = 0; i < nEntries; i++)
	{
		if(!PDB_iteratorGetNextEntry(&entry)) continue;
		if(entry.event == event)
		{
			if(EUIAddressEquals(EUIAddress, entry.address))
			{
				if(hasParam)
				{
					doAction(entry.action, param, hasParam);
				}
				else
				{
					doAction(entry.action, entry.param, entry.config.hasParam);
				}
			}
		}
	}
}

bool AquilaProtocol::EUIAddressEquals(uint8_t *addr1, uint8_t *addr2)
{
	int i = 0;
	for(i = 0; i < 8; i++)
	{
		if(addr1[i] != addr2[i]) return false;
	}
	return true;
}

// PUBLIC //

int AquilaProtocol::begin()
{
	flagGotPacket = 0;

	// Get EUI Long Address from id chip
	Mesh.getEUIAddr(this->EUIAddress);

	PDB_init();
	this->id = PROTOCOL_DEFAULT_ID;
	this->name = PROTOCOL_DEFAULT_NAME;
	this->nActions = 0;
	this->nEvents = 0;
	this->nOnSubs = 0;

	// zeroing actions
	int i;
	for(i = 0; i < PROTOCOL_MAXACTIONS; i++)
	{
		this->actions[i] = NULL;
	}
	// zeroing events
	for(i = 0; i < PROTOCOL_MAXEVENTS; i++)
	{
		this->events[i] = NULL;
	}
	// zeroing onSubscriptions
	/*for(i = 0; i < PROTOCOL_MAXONSUBS; i++)
	{
		this->onSubs[i] = NULL;
	}*/

	Mesh.openEndpoint(PROTOCOL_ENDPOINT, Protocol__rxHandler);
	return true;
}

/*
	Main functions:
*/
void AquilaProtocol::loop()
{
	if(flagGotPacket > 0) parsePacket();
}

void AquilaProtocol::setClass(char *nid)
{
	id = nid;
}

void AquilaProtocol::setName(char *nName)
{
	name = nName;
}

void AquilaProtocol::setAction(uint8_t n, char description[], bool (*function)(uint8_t param, bool gotParam))
{
	if(n < PROTOCOL_MAXACTIONS)
	{
		if(this->actions[n] == NULL) this->nActions++;
		Action *newAction;
		newAction = (Action*)malloc(sizeof(Action));
		uint8_t size = strlen(description);
		newAction->description = (char*)malloc(size + 1);
		strcpy(newAction->description, description);
		newAction->function = function;
		this->actions[n] = newAction;
	}
}

void AquilaProtocol::addAction(char description[], bool (*function)(uint8_t param, bool gotParam))
{
	setAction(this->nActions, description, function);
}

void AquilaProtocol::setEvent(uint8_t n, char description[])
{
	if(n < PROTOCOL_MAXEVENTS)
	{
		if(this->events[n] == NULL) this->nEvents++;
		uint8_t size = strlen(description);
		this->events[n] = (char*)malloc(size + 1);
		strcpy(this->events[n], description);
	}
}

Event AquilaProtocol::addEvent(char description[])
{
	uint8_t n = this->nEvents;
	setEvent(n, description);
	return n;
}

void AquilaProtocol::emit(uint8_t event, uint8_t param, bool hasParam)
{
	emit(BROADCAST, event, param, hasParam);
}

void AquilaProtocol::emit(uint16_t dest, uint8_t event, uint8_t param, bool hasParam)
{
	char packet[AQUILAMESH_MAXPAYLOAD];
	uint8_t descLen = 0;
	uint8_t index = 0;
	//control byte:
	packet[index++] = PROTOCOL_EVENT<<PROTOCOL_COMMAND_TYPE | hasParam<<PROTOCOL_HAS_PARAM | 1<<PROTOCOL_HAS_DATA;
	//command byte:
	packet[index++] = event;
	//param byte:
	if(hasParam) packet[index++] = param;
	//Append EUIAddress, as unique identifier for event configuration
	memcpy(&packet[index], this->EUIAddress, PROTOCOL_EUIADDRESSLEN);

	index += PROTOCOL_EUIADDRESSLEN;

	// Append event name, for runtime event subscription
	if(this->events[event] != NULL)
	{
		descLen = strlen(this->events[event]);
		// For avoiding memory overflow, validate string size
		if( descLen < (AQUILAMESH_MAXPAYLOAD - (index + 1)) )
		{
			// Append length
			packet[index++] = descLen;
			memcpy(&packet[index], this->events[event], descLen);
		}
	}

	// Send event to dest (Almost constant time)
	send(dest, packet, index + descLen);
	// Check if we are subscribed to our own event (slows down with more entries)
	checkEvent(this->EUIAddress, event, param, hasParam, (uint8_t*)this->events[event], Mesh.getShortAddr());
}

bool AquilaProtocol::on(char* eventName, bool (*function)(uint8_t param, bool gotParam))
{
	// NULL means accept from any
	this->on(eventName, euiBroadcast, function);
}

bool AquilaProtocol::on(char* eventName, bool (*function)(uint8_t param, bool gotParam, uint16_t shortAddr, uint8_t *longAddr))
{
	// Extended on subscriptions
	if(this->nOnSubs >= PROTOCOL_MAXONSUBS) return false;
	
	uint8_t index = this->nOnSubs;

	OnSubscription *newOnSub;
	newOnSub = (OnSubscription*)malloc(sizeof(OnSubscription));
	uint8_t size = strlen((char*)eventName);
	newOnSub->eName = (char*)malloc(size + 1);
	strcpy((char*)newOnSub->eName, (char*)eventName);
	memcpy(newOnSub->EUIAddress, euiBroadcast, PROTOCOL_EUIADDRESSLEN);
	newOnSub->function = NULL;
	newOnSub->extFunction = function;
	this->onSubs[index] = newOnSub;
	this->nOnSubs++;
	return true;
}
	
bool AquilaProtocol::on(char* eventName, uint8_t* EUIAddress, bool (*function)(uint8_t param, bool gotParam))
{
	if(this->nOnSubs >= PROTOCOL_MAXONSUBS) return false;
	
	uint8_t index = this->nOnSubs;

	OnSubscription *newOnSub;
	newOnSub = (OnSubscription*)malloc(sizeof(OnSubscription));
	uint8_t size = strlen((char*)eventName);
	newOnSub->eName = (char*)malloc(size + 1);
	strcpy((char*)newOnSub->eName, (char*)eventName);
	memcpy(newOnSub->EUIAddress, EUIAddress, PROTOCOL_EUIADDRESSLEN);
	newOnSub->function = function;
	newOnSub->extFunction = NULL;
	this->onSubs[index] = newOnSub;
	this->nOnSubs++;
	return true;
}

/*
	Advanced functions:
*/

void AquilaProtocol::requestAction(uint16_t address, uint8_t action, uint8_t param, bool hasParam)
{
	int i;
	bool forUs = (nwkIb.addr == address);

	if(forUs)	//checa si el mensaje es para nosotros mismos.
	{
		doAction(action, param, hasParam);
	}
	else
	{
		char packet[3];
		//control byte:
		packet[0] = PROTOCOL_ACTION<<PROTOCOL_COMMAND_TYPE | hasParam<<PROTOCOL_HAS_PARAM | 1<<PROTOCOL_HAS_DATA;
		//command byte:
		packet[1] = action;
		//param byte:
		if(hasParam) packet[2] = param;

	    send(address, packet, 2 + hasParam);
	}
}

bool AquilaProtocol::doAction(uint8_t action, uint8_t param, bool gotParam)
{
	if(action < PROTOCOL_MAXACTIONS)
	{
		if(actions[action] != NULL)
		{
			return actions[action]->function(param, gotParam);
		}
	}

	return false;
}

AquilaProtocol Aquila;
