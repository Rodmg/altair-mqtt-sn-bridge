/**
* \file AquilaServices.h
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

#ifndef AQUILASERVICES_H
#define AQUILASERVICES_H

/*
	Aquila Services Specification:

	service: Function
	data: JSON proposed

	Packet:
		methods (2 bits):
			GET		0x00
			PUT		0x01
			POST	0x02
			DELETE	0x03
		size (8bits)
		data (0 to (MESH_MAXSIZE - 2))


	bool myService(uint16_t reqAddr, uint8_t method, uint8_t *data, uint8_t dataSize)
	{
		...
		response(reqAddr, NULL, 0);
	}
*/

#include <stdint.h>
#include <stdbool.h>
#include <Mesh.h>

#define AQUILASERVICES_MAX 20
#define AQUILASERVICES_VAR_MAX 10
#define AQUILASERVICES_FUNC_MAX 10
#define AQUILASERVICES_ENDPOINT 12
#define AQUILASERVICES_VERSION 1

// request methods
#define GET 0x00
#define PUT 0x01
#define POST 0x02
#define DELETE 0x03
// responses
#define R200 0x04	// OK
#define R404 0x05	// Service not found
#define R405 0x06	// Method not allowed
#define R408 0x07	// Timeout
#define R500 0x08	// Service error

#define AQUILASERVICES_MAXNAMESIZE 16
#define AQUILASERVICES_MAXDATASIZE (AQUILAMESH_MAXPAYLOAD - 4)

#define AQUILASERVICES_TIMEOUT 1000

typedef struct
{
	uint8_t version;
	uint8_t method;
	uint8_t nameSize;
	uint8_t dataSize;
	uint8_t name_data[AQUILASERVICES_MAXDATASIZE];
} ServicePacket;

typedef struct
{
	char *name;
	bool (*function)(uint16_t reqAddr, uint8_t method, char *data, uint8_t dataSize);
} Service;

typedef struct 
{
	char *name;
	int *var;
} IntVariableService;

typedef struct 
{
	char *name;
	float *var;
} FloatVariableService;

typedef struct
{
	char *name;
	float (*function)(uint8_t method, char *data, uint8_t dataSize);
} FunctionService;

class AquilaServices
{
public:
	Service services[AQUILASERVICES_MAX];
	IntVariableService intVars[AQUILASERVICES_VAR_MAX];
	FloatVariableService floatVars[AQUILASERVICES_VAR_MAX];
	FunctionService funcServices[AQUILASERVICES_FUNC_MAX];

	void (*lastReqCb)(uint16_t srcAddr, uint8_t status, char *data, uint8_t dataSize);
	bool waitingResponse;
	double lastRequestTime;

	bool varResponse(uint16_t reqAddr, int var);
	bool varResponse(uint16_t reqAddr, float var);

	AquilaServices();
	void begin();
	void loop();

	// SIMPLE API:
	// Create a service that is tied to a variable
	void variable(char *name, int *var);
	void variable(char *name, float *var);

	// Create a service that is tied to a function
	void function(char *name, float (*func)(uint8_t method, char *data, uint8_t dataSize));

	// ADVANCED API:
	// Subscribes a service whith a name
	void add(char *name, bool (*function)(uint16_t reqAddr, uint8_t method, char *data, uint8_t dataSize));
	// Request
	void request(uint16_t destAddr, uint8_t method, char *name, void (*callback)(uint16_t srcAddr, uint8_t status, char *data, uint8_t dataSize), char *data = NULL, uint8_t dataSize = 0);
	// For use inside the service function
	void response(uint16_t destAddr, uint8_t method, char *data = NULL, uint8_t dataSize = 0);
};

extern AquilaServices Services;

#endif // AQUILASERVICES_H
