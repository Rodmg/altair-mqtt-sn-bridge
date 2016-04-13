/**
* \file WSerial.cpp
*
* \brief Serial socket over 802.15.4 implementation.
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
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"
#include "lwm/sys/sys.h"
#include "lwm/nwk/nwk.h"
#include "WSerial.h"

static TxPacket packet;
uint8_t packetData[WSERIAL_BUFFER_SIZE];

uint16_t destAddress;
bool allowFromAny = false;
volatile uint8_t _rx_buffer_head;
volatile uint8_t _rx_buffer_tail;
volatile uint8_t _tx_buffer_head;
volatile uint8_t _tx_buffer_tail;

bool appDataReqBusy = false;

uint8_t _rx_buffer[WSERIAL_BUFFER_SIZE];
uint8_t _tx_buffer[WSERIAL_BUFFER_SIZE];

static bool receiveMessage(RxPacket *ind)
{
	// Check if its from the address we are expecting,
	// ignore otherwise
	// If destAddress == BROADCAST, Accept all
	// If allowFromAny == true, accept all
	if( !(destAddress == BROADCAST || allowFromAny == true) && ind->srcAddr != destAddress) return false;
	// Put received data into rx buffer
	// if not enough space, drop data :(
	if(ind->size > (WSERIAL_BUFFER_SIZE - (unsigned int)(WSERIAL_BUFFER_SIZE + _rx_buffer_head - _rx_buffer_tail) % WSERIAL_BUFFER_SIZE) ) return false;

	for(int i = 0; i < ind->size; i++)
	{
		_rx_buffer[ (_rx_buffer_head + i) % WSERIAL_BUFFER_SIZE ] = ind->data[i];
	}

	_rx_buffer_head = (_rx_buffer_head + ind->size) % WSERIAL_BUFFER_SIZE;
}

static void appDataCb(TxPacket *req)
{
  appDataReqBusy = false;
  (void)req;
}

void WirelessSerial::begin(uint16_t destAddr)
{
	_rx_buffer_head = _rx_buffer_tail = 0;
	_tx_buffer_head = _tx_buffer_tail = 0;
	setDest(destAddr);
	Mesh.openEndpoint(WSERIAL_ENDPOINT, receiveMessage);
}

void WirelessSerial::setDest(uint16_t destAddr)
{
	destAddress = destAddr;
}

void WirelessSerial::setAllowFromAny(bool allow)
{
	allowFromAny = allow;
}


void WirelessSerial::end()
{
	// set destination to an invalid address for avoiding connecions
	setDest(0);
}

int WirelessSerial::available(void)
{
	return (unsigned int)(WSERIAL_BUFFER_SIZE + _rx_buffer_head - _rx_buffer_tail) % WSERIAL_BUFFER_SIZE;
}

int WirelessSerial::peek(void)
{
	if (_rx_buffer_head == _rx_buffer_tail) {
		return -1;
	} else {
		return _rx_buffer[_rx_buffer_tail];
	}
}

int WirelessSerial::read(void)
{
	// if the head isn't ahead of the tail, we don't have any characters
	if (_rx_buffer_head == _rx_buffer_tail) {
		return -1;
	} else {
		unsigned char c = _rx_buffer[_rx_buffer_tail];
		_rx_buffer_tail = (uint8_t)(_rx_buffer_tail + 1) % WSERIAL_BUFFER_SIZE;
		return c;
	}
}

void WirelessSerial::flush()
{
	// Should force send whatever is in _tx_buffer
	this->sendPacketNow();
}

size_t WirelessSerial::write(uint8_t c)
{
	// Calc new index
	uint8_t i = (_tx_buffer_head + 1) % WSERIAL_BUFFER_SIZE;
	// Check if buffer is full
	if(i == _tx_buffer_tail)
	{
		// send now whatever is in the buffer
		this->sendPacketNow();
		// and then recalculate index to append to buffer
		i = (_tx_buffer_head + 1) % WSERIAL_BUFFER_SIZE;
	}

	_tx_buffer[_tx_buffer_head] = c;
  	_tx_buffer_head = i;

  	return 1;
}

void WirelessSerial::sendPacketNow()
{
	// check if there is something to send or if busy
	if(_tx_buffer_head == _tx_buffer_tail || appDataReqBusy) return;

	unsigned int size = (WSERIAL_BUFFER_SIZE + _tx_buffer_head - _tx_buffer_tail) % WSERIAL_BUFFER_SIZE;

	for(int i = 0; i < size; i++)
	{
		packetData[i] = _tx_buffer[ (_tx_buffer_tail + i) % WSERIAL_BUFFER_SIZE ];
	}

	// empty buffer
	_tx_buffer_head = _tx_buffer_tail;

	packet.dstAddr = destAddress;
	packet.dstEndpoint = WSERIAL_ENDPOINT;
	packet.srcEndpoint = WSERIAL_ENDPOINT;

	uint8_t requestAck = 0;
  	if(destAddress == BROADCAST) requestAck = 0;
  	else requestAck = NWK_OPT_ACK_REQUEST;

  	packet.options = requestAck;
	packet.data = packetData;
	packet.size = size;
	packet.confirm = appDataCb;
	Mesh.sendPacket(&packet);

	appDataReqBusy = true;
}

void WirelessSerial::loop()
{
	unsigned long now = millis();
	if(now > lastTimeSent + WSERIAL_SEND_INTERVAL)
	{
		this->sendPacketNow();
		lastTimeSent = millis();
	}
}

WirelessSerial WSerial;
