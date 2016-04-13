/**
* \file Mesh.h
*
* \brief Aquila 802.15.4 Mesh implementation.
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

#ifndef AQUILAMESH_H
#define AQUILAMESH_H

#include <Arduino.h>
#include <stdint.h>
#include <stdbool.h>
#include "lwm/sys/sys.h"
#include "lwm/nwk/nwk.h"
#include "MeshControl.h"

#define BROADCAST 0xFFFF
#define HUB 0x00FF

#define AQUILAMESH_DEFPAN 0xCA5A
#define AQUILAMESH_DEFCHAN 0x1A

// According to Atmel-42028-Lightweight-Mesh-Developer-Guide_Application-Note_AVR2130,
// Max payload size is 105 with security and 32bit MIC and 109 without security
#define AQUILAMESH_MAXPAYLOAD (NWK_MAX_PAYLOAD_SIZE - 4)/*Security 32bit MIC*/

// Frendlier names for LWM types
#define TxPacket NWK_DataReq_t
#define RxPacket NWK_DataInd_t

class AquilaMesh
{
private:
	bool isAsleep;
	MeshControl meshControl;

public:
	AquilaMesh();
	bool begin();
	bool begin(uint16_t addr);

	void end();

	void loop();

	void setAddr(uint16_t addr);
	void setPanId(uint16_t panId);
	void setChannel(uint8_t channel);
	void setSecurityKey(uint8_t *key);
	void setSecurityEnabled(bool enabled);
	bool getSecurityEnabled();
	void openEndpoint(uint8_t id, bool (*handler)(RxPacket *ind));

	bool sendPacket(TxPacket *packet);
	void sendNow();

	void sendEUI(uint16_t dest);
	void announce(uint16_t dest);

	uint16_t getShortAddr();
	void getEUIAddr(uint8_t* address);

	// For sleep management
	bool busy();
	void sleep();
	void wakeup();
	bool asleep();

};

extern AquilaMesh Mesh;

#endif //AQUILAMESH_H
