/**
* \file Mesh.cpp
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


#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include "Arduino.h"
#include "lwm/sys/sys.h"
#include "lwm/nwk/nwk.h"
#include "lwm/nwk/nwkSecurity.h"
#include "stack/halID.h"
#include "Mesh.h"
#include "TxBuffer.h"
#include "RxBuffer.h"

#include "lwm/phy/atmegarfr2.h"

/////

static bool secEnabled;
static uint8_t euiAddr[8];
uint16_t shortAddr;

// Transmission management
static TxBuffer txBuffer;
static bool txBusy;
void (*txUserConfirm)(TxPacket *packet);
void txConfirm(TxPacket *packet)
{
	// Tx finished
	txBusy = false;
	// Call user callback
	if(txUserConfirm != NULL) txUserConfirm(packet);
	txUserConfirm = NULL;
	(void)packet;
}

// Reception management
static RxBuffer rxBuffer;
bool (*rxCallbacks[16])(RxPacket *ind);

static bool rxBootstrap(RxPacket *ind)
{
	// Send rssi with Ack
	NWK_SetAckControl(abs(ind->rssi));

	if(RxBuffer_isFull(&rxBuffer)) { return false; }

	// Enforce security
	// if security enabled and the package was not secured, ignore it.
	if( secEnabled && !(ind->options & NWK_IND_OPT_SECURED) ) { return false; }

	RxBufPacket bufPacket;
	RxBufPacket_init(&bufPacket, ind);

	if(!RxBuffer_insert(&rxBuffer, &bufPacket)) { return false; }

	// Send Ack immediately
	return true;
}

void rxAttendPend()
{
	// If ack pending (busy), dont attend yet.
	if(RxBuffer_isEmpty(&rxBuffer) || NWK_Busy()) return;

	// Get package from buffer
	static RxBufPacket bufPacket;
	if(!RxBuffer_remove(&rxBuffer, &bufPacket)) { return; }

	bufPacket.packet.data = bufPacket.data;

	if(rxCallbacks[bufPacket.packet.dstEndpoint] != NULL)
	{
		rxCallbacks[bufPacket.packet.dstEndpoint](&(bufPacket.packet));
	}
}

/////



AquilaMesh::AquilaMesh()
{
	secEnabled = false;
	isAsleep = false;
	shortAddr = 0;
	TxBuffer_init(&txBuffer);
	RxBuffer_init(&rxBuffer);
	txUserConfirm = NULL;
	txBusy = false;

	// Zeroing rxCallbacks
	for(int i = 0; i < 16; i++)
	{
		rxCallbacks[i] = NULL;
	}
}

// Begin with automatic address
bool AquilaMesh::begin()
{
	// Init as address 0:
	if(!begin(0)) return false;

	shortAddr = meshControl.assignAddr(euiAddr);
	NWK_SetAddr(shortAddr);

	return true;
}

// Begin with manual address addr
bool AquilaMesh::begin(uint16_t addr)
{

	SYS_Init();
	PHY_SetRxState(true);
	shortAddr = addr;
	NWK_SetAddr(addr);
	NWK_SetPanId(AQUILAMESH_DEFPAN);
	PHY_SetChannel(AQUILAMESH_DEFCHAN);

	ID_init();
	if( !ID_getId(euiAddr) ) return false;        //Error

	meshControl.begin(this);
	return true;
}

void AquilaMesh::end()
{
	PHY_SetRxState(false);
}

void AquilaMesh::loop()
{
	SYS_TaskHandler();
	// Attend any pending packets
	rxAttendPend();
	// Send any pending packets
	sendNow();
}

void AquilaMesh::setAddr(uint16_t addr)
{
	NWK_SetAddr(addr);
}

void AquilaMesh::setPanId(uint16_t panId)
{
	NWK_SetPanId(panId);
}

void AquilaMesh::setChannel(uint8_t channel)
{
	PHY_SetChannel(channel);
}

void AquilaMesh::setSecurityKey(uint8_t *key)
{
	NWK_SetSecurityKey(key);
}

void AquilaMesh::setSecurityEnabled(bool enabled)
{
	secEnabled = enabled;
}

bool AquilaMesh::getSecurityEnabled()
{
	return secEnabled;
}

void AquilaMesh::openEndpoint(uint8_t id, bool (*handler)(RxPacket *ind))
{
	rxCallbacks[id] = handler;
	NWK_OpenEndpoint(id, rxBootstrap);
}

bool AquilaMesh::sendPacket(TxPacket *packet)
{
	if(TxBuffer_isFull(&txBuffer)) return false;

	TxBufPacket bufPacket;
	TxBufPacket_init(&bufPacket, packet);

	if(!TxBuffer_insert(&txBuffer, &bufPacket)) return false;

	return true;
}

void AquilaMesh::sendNow()
{
	if(TxBuffer_isEmpty(&txBuffer) || txBusy || NWK_Busy()) return;
	// Get package from buffer
	static TxBufPacket bufPacket;
	if(!TxBuffer_remove(&txBuffer, &bufPacket)) return;
	bufPacket.packet.data = bufPacket.data;

	// Enforce security
	if(secEnabled)
		bufPacket.packet.options = bufPacket.packet.options | NWK_OPT_ENABLE_SECURITY;
	else
		bufPacket.packet.options = bufPacket.packet.options & ~NWK_OPT_ENABLE_SECURITY;

	// Setup the callback
	// Save user confirm callback
	txUserConfirm = bufPacket.packet.confirm;
	// Assign our callback for tx control
	bufPacket.packet.confirm = txConfirm;
	
	txBusy = true;

	// Request send to nwk layer
	NWK_DataReq(&bufPacket.packet);
}

// Announce device to the network
void AquilaMesh::announce(uint16_t dest)
{
	meshControl.sendEUI(dest);
}

uint16_t AquilaMesh::getShortAddr()
{
	return shortAddr;
}

void AquilaMesh::getEUIAddr(uint8_t* address)
{
	memcpy(address, euiAddr, 8);
}

bool AquilaMesh::busy()
{
	return NWK_Busy();
}

void AquilaMesh::sleep()
{
	while(this->busy()) this->loop();
	NWK_SleepReq();
	isAsleep = true;
}

void AquilaMesh::wakeup()
{
	NWK_WakeupReq();
	isAsleep = false;
}

bool AquilaMesh::asleep()
{
	return isAsleep;
}

AquilaMesh Mesh;
