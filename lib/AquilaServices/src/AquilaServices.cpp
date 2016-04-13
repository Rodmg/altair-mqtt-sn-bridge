/**
* \file AquilaServices.cpp
*
* \brief Aquila REST-like Services implementation.
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
#include "AquilaServices.h"
#include "Mesh.h"
#include <string.h>
#include <stdio.h>

// For accessing class data from C style functions
AquilaServices *self;

static TxPacket nwkPacket;

static void nwkTxCb(TxPacket *req)
{
	(void) req;
}

static bool rxHandler(RxPacket *ind)
{
	if(ind->srcEndpoint != AQUILASERVICES_ENDPOINT || ind->dstEndpoint != AQUILASERVICES_ENDPOINT) return false;

	/*
		ind->srcAddr;
		ind->size;
		ind->data;
		[version][method][nameSize][dataSize][name...][data...]
	*/

	// packet data must be at least 4 bytes for version, method, nameSize and dataSize
	if(ind->size < 4) return false;
	ServicePacket pkt;

	memcpy(&pkt, ind->data, ind->size);

	// Check version
	if(pkt.version != AQUILASERVICES_VERSION) return false;

	// Parsing name
	char name[AQUILASERVICES_MAXNAMESIZE];
	char data[AQUILASERVICES_MAXDATASIZE];

	memcpy(name, pkt.name_data, pkt.nameSize);
	name[pkt.nameSize] = NULL;	// End byte

	memcpy(data, &pkt.name_data[pkt.nameSize], pkt.dataSize);
	data[pkt.dataSize] = NULL;	// End byte for string security

	// Handle responses:
	if(pkt.method == R200 || pkt.method == R404 || pkt.method == R500 || pkt.method == R405 || pkt.method == R408)
	{
		if(self->lastReqCb != NULL) self->lastReqCb(ind->srcAddr, pkt.method, data, pkt.dataSize);
		// NULL out self->lastReqCb so it only works once:
		self->lastReqCb = NULL;
		self->waitingResponse = false;
		return true;
	}

	// Handle requests:
	// Check int and float variables
	for(uint8_t i = 0; i < AQUILASERVICES_VAR_MAX; i++)
	{
		if(self->intVars[i].name != NULL)
		{
			if( 0 == strcmp(self->intVars[i].name, name) )
			{
				if( NULL != self->intVars[i].var ) return self->varResponse(ind->srcAddr, *self->intVars[i].var);
			}
		}

		if(self->floatVars[i].name != NULL)
		{
			if( 0 == strcmp(self->floatVars[i].name, name) )
			{
				if( NULL != self->floatVars[i].var ) return self->varResponse(ind->srcAddr, *self->floatVars[i].var);
			}
		}
	}

	// Check functions
	for(uint8_t i = 0; i < AQUILASERVICES_FUNC_MAX; i++)
	{
		if(self->funcServices[i].name == NULL) continue;

		if( 0 == strcmp(self->funcServices[i].name, name) )
		{
			if( NULL != self->funcServices[i].function ) return self->varResponse(ind->srcAddr, self->funcServices[i].function(pkt.method, data, pkt.dataSize));
		}
	}

	// Check advanced functions
	for(uint8_t i = 0; i < AQUILASERVICES_MAX; i++)
	{
		if(self->services[i].name == NULL) continue;

		if( 0 == strcmp(self->services[i].name, name) )
		{
			if( NULL != self->services[i].function ) return self->services[i].function(ind->srcAddr, pkt.method, data, pkt.dataSize);
		}
	}

	return false;
}

AquilaServices::AquilaServices()
{
	self = this;
	// zeroing services:
	for(uint8_t i = 0; i < AQUILASERVICES_MAX; i++)
	{
		services[i].name = NULL;
		services[i].function = NULL;
	}

	for(uint8_t i = 0; i < AQUILASERVICES_VAR_MAX; i++)
	{
		intVars[i].name = NULL;
		intVars[i].var = NULL;
		floatVars[i].name = NULL;
		floatVars[i].var = NULL;
	}

	for(uint8_t i=0; i < AQUILASERVICES_FUNC_MAX; i++)
	{
		funcServices[i].name = NULL;
		funcServices[i].function = NULL;
	}

	lastReqCb = NULL;

	waitingResponse = false;
}

bool AquilaServices::varResponse(uint16_t reqAddr, int var)
{
	char buffer[AQUILASERVICES_MAXDATASIZE];
	sprintf(buffer, "{\"val\":%d}", var);
	response(reqAddr, R200, buffer, strlen(buffer));
}

bool AquilaServices::varResponse(uint16_t reqAddr, float var)
{
	char buffer[AQUILASERVICES_MAXDATASIZE];
	char varStr[16];
	dtostrf(var, 4, 3, varStr);
	sprintf(buffer, "{\"val\":%s}", varStr);
	response(reqAddr, R200, buffer, strlen(buffer));
}

void AquilaServices::begin()
{
	// suscribe to endpoint 12
	Mesh.openEndpoint(AQUILASERVICES_ENDPOINT, rxHandler);
}

void AquilaServices::loop()
{
	// handle response timeout:
	double now = millis();
	if(lastRequestTime + AQUILASERVICES_TIMEOUT < now)
	{
		// call cb with timeout code
		if(lastReqCb != NULL) lastReqCb(0, R408, NULL, 0);
		// NULL out lastReqCb so it only works once:
		lastReqCb = NULL;
		waitingResponse = false;
	}
}

// SIMPLE API:
// Create a service that is tied to a variable
void AquilaServices::variable(char *name, int *var)
{
	for(uint8_t i = 0; i < AQUILASERVICES_VAR_MAX; i++)
	{
		if(intVars[i].name == NULL)
		{
			intVars[i].name = name;
			intVars[i].var = var;
			return;
		}
	}
}

void AquilaServices::variable(char *name, float *var)
{
	for(uint8_t i = 0; i < AQUILASERVICES_VAR_MAX; i++)
	{
		if(floatVars[i].name == NULL)
		{
			floatVars[i].name = name;
			floatVars[i].var = var;
			return;
		}
	}
}

// Create a service that is tied to a function
void AquilaServices::function(char *name, float (*func)(uint8_t method, char *data, uint8_t dataSize))
{
	for(uint8_t i = 0; i < AQUILASERVICES_FUNC_MAX; i++)
	{
		if(funcServices[i].name == NULL)
		{
			funcServices[i].name = name;
			funcServices[i].function = func;
			return;
		}
	}
}

// Inscribe un servicio a name
void AquilaServices::add(char *name, bool (*function)(uint16_t reqAddr, uint8_t method, char *data, uint8_t dataSize))
{
	for(uint8_t i = 0; i < AQUILASERVICES_MAX; i++)
	{
		if(services[i].name == NULL)
		{
			services[i].name = name;
			services[i].function = function;
			return;
		}
	}
}
// Petición
void AquilaServices::request(uint16_t destAddr, uint8_t method, char *name, void (*callback)(uint16_t srcAddr, uint8_t status, char *data, uint8_t dataSize), char *data, uint8_t dataSize)
{
	// lose packet
	if(/*nwkTxBusy ||*/ waitingResponse) return;

	// Form packetData
	ServicePacket pkt;
	pkt.version = AQUILASERVICES_VERSION;
	pkt.method = method;
	pkt.nameSize = strlen(name);
	pkt.dataSize = dataSize;
	memcpy(pkt.name_data, name, pkt.nameSize);
	memcpy(&pkt.name_data[pkt.nameSize], data, dataSize);

	nwkPacket.dstAddr = destAddr;
	nwkPacket.dstEndpoint = AQUILASERVICES_ENDPOINT;
	nwkPacket.srcEndpoint = AQUILASERVICES_ENDPOINT;

	uint8_t requestAck = 0;
  	if(destAddr == BROADCAST) requestAck = 0;
  	else requestAck = NWK_OPT_ACK_REQUEST;

  	nwkPacket.options = requestAck;
	nwkPacket.data = (uint8_t*)&pkt;
	nwkPacket.size = pkt.nameSize + pkt.dataSize + 4;
	nwkPacket.confirm = nwkTxCb;
	Mesh.sendPacket(&nwkPacket);

	//nwkTxBusy = true;

	if(callback == NULL) return;
	// Wait for response and call callback...
	// will be called on rxHandler
	lastReqCb = callback;
	waitingResponse = true;
	lastRequestTime = millis();
}
// Para usar dentro del servicio
void AquilaServices::response(uint16_t destAddr, uint8_t method, char *data, uint8_t dataSize)
{
	// lose packet
	//if(nwkTxBusy) return;

	// Form packetData
	ServicePacket pkt;
	pkt.version = AQUILASERVICES_VERSION;
	pkt.method = method;
	pkt.nameSize = 0;
	pkt.dataSize = dataSize;
	memcpy(pkt.name_data, data, dataSize);

	nwkPacket.dstAddr = destAddr;
	nwkPacket.dstEndpoint = AQUILASERVICES_ENDPOINT;
	nwkPacket.srcEndpoint = AQUILASERVICES_ENDPOINT;

	uint8_t requestAck = 0;
  	if(destAddr == BROADCAST) requestAck = 0;
  	else requestAck = NWK_OPT_ACK_REQUEST;

  	nwkPacket.options = requestAck;
	nwkPacket.data = (uint8_t*)&pkt;
	nwkPacket.size = dataSize + 4;
	nwkPacket.confirm = nwkTxCb;
	Mesh.sendPacket(&nwkPacket);

	//nwkTxBusy = true;
}

AquilaServices Services;
