/**
* \file AquilaBridge.cpp
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

#include "AquilaBridge.h"
#include "CRC.h"

AquilaBridge *self;
static char comTxBuffer[255];


// Called on slip data received
static void onData(char *data, uint8_t size)
{
    if(checkCrc(data, size))
    {
        size -= 2;
        self->parseCommand(data, size);
    }
    else
    {
        //Serial1.println("CRC error");
    }
}

// Called on rx success, gets rx data
static bool rxHandler(RxPacket *ind)
{
    uint8_t i = 0, j = 0;

    uint8_t size = ind->size + 10;

    comTxBuffer[j++] = CMD_DATA;
    comTxBuffer[j++] = ind->lqi;
    comTxBuffer[j++] = ind->rssi;
    comTxBuffer[j++] = ind->srcAddr;
    comTxBuffer[j++] = ind->srcAddr >> 8;
    comTxBuffer[j++] = ind->dstAddr;
    comTxBuffer[j++] = ind->dstAddr >> 8;
    comTxBuffer[j++] = ind->srcEndpoint;
    comTxBuffer[j++] = ind->dstEndpoint;
    comTxBuffer[j++] = ind->size;

    for(i = 0; i < ind->size; i++)
    {
        comTxBuffer[j++] = ind->data[i];
    }

    self->sendComBuffer(size);

    #ifdef BRIDGE_DEBUG
    Serial1.println("got packet");
    #endif
    return true;
}

static void txCb(TxPacket *req)
{
    /*
        Possible status:

        NWK_SUCCESS_STATUS
        NWK_ERROR_STATUS
        NWK_OUT_OF_MEMORY_STATUS

        NWK_NO_ACK_STATUS
        NWK_NO_ROUTE_STATUS

        NWK_PHY_CHANNEL_ACCESS_FAILURE_STATUS
        NWK_PHY_NO_ACK_STATUS
    */

    #ifdef BRIDGE_DEBUG
    Serial1.print("txSuccess: ");
    Serial1.println(req->status);
    #endif

    if(req->status == NWK_SUCCESS_STATUS)
    {
        // Send Ack with rssi
        self->sendAck(req->control);
    }
    else
    {
        // Send Nack with cause
        self->sendNack(req->status);
    }

    (void)req;
}

AquilaBridge::AquilaBridge()
{
    self = this;
    isProm = false;
}

bool AquilaBridge::begin(unsigned int baudrate, uint16_t addr, uint8_t channel, uint16_t pan, bool promiscuous)
{
    slip.begin(baudrate, onData);

    if(addr == NULL)
    {
        if(!Mesh.begin()) return false;
    }
    else 
    {
        if(!Mesh.begin(addr)) return false;
    }

    setChannel(channel);
    this->pan = pan;
    setPan(pan);
    Mesh.getEUIAddr(euiAddress);


    // Subscribe handler for all endpoints
    // 0 is reserved for LWM commands, dont use
    for(int i = 1; i < 16; i++)
    {
        Mesh.openEndpoint(i, rxHandler);
    }

    // Anounce bridge ready to PC
    sendStart();
    delay(10);
    sendLongAddr(euiAddress);

    #ifdef BRIDGE_DEBUG
    Serial1.begin(9600);
    Serial1.println("Bridge Debug");
    #endif

    return true;
}

void AquilaBridge::setChannel(uint8_t chan)
{
    this->channel = chan;
    Mesh.setChannel(chan);
}

void AquilaBridge::setPan(uint16_t _pan)
{
    this->pan = _pan;
    Mesh.setPanId(_pan);
}

void AquilaBridge::loop()
{
    slip.loop();
    Mesh.loop();
}

void AquilaBridge::sendComBuffer(uint8_t size)
{
    size = appendCrc(comTxBuffer, size);
    slip.send(comTxBuffer, size);
}


void AquilaBridge::sendAck(uint8_t rssi)
{
    comTxBuffer[0] = CMD_ACK;
    comTxBuffer[1] = rssi;
    sendComBuffer(2);
}

void AquilaBridge::sendNack(uint8_t cause)
{
    comTxBuffer[0] = CMD_NACK;
    comTxBuffer[1] = cause;
    sendComBuffer(2);
}

void AquilaBridge::sendOpt()
{
    uint16_t shortAddr = Mesh.getShortAddr();
    comTxBuffer[0] = CMD_SET_OPT;
    comTxBuffer[1] = (uint8_t) isProm;
    comTxBuffer[2] = (uint8_t) (pan & 0xFF);
    comTxBuffer[3] = (uint8_t) (pan >> 8);
    comTxBuffer[4] = channel;
    comTxBuffer[5] = (uint8_t) (shortAddr & 0xFF);
    comTxBuffer[6] = (uint8_t) (shortAddr >> 8);
    sendComBuffer(7);
}

void AquilaBridge::sendSecurity()
{
    comTxBuffer[0] = CMD_SET_SECURITY;
    comTxBuffer[1] = Mesh.getSecurityEnabled();
    sendComBuffer(2);
}

void AquilaBridge::sendStart()
{
    comTxBuffer[0] = CMD_START;
    sendComBuffer(1);
}

void AquilaBridge::sendLongAddr(uint8_t addr[])
{
    comTxBuffer[0] = CMD_SET_LONG_ADDR;
    comTxBuffer[1] = addr[0];
    comTxBuffer[2] = addr[1];
    comTxBuffer[3] = addr[2];
    comTxBuffer[4] = addr[3];
    comTxBuffer[5] = addr[4];
    comTxBuffer[6] = addr[5];
    comTxBuffer[7] = addr[6];
    comTxBuffer[8] = addr[7];
    sendComBuffer(9);
}

// Indexes
#define COMMAND     0
#define OPT_PROM    1
#define OPT_PANL    2
#define OPT_PANH    3
#define OPT_CHAN    4
#define OPT_ADDRL   5
#define OPT_ADDRH   6
#define SEC_EN      1
#define SEC_KEY     2

#define D_SRCADDRL  3
#define D_SRCADDRH  4
#define D_DSTADDRL  5
#define D_DSTADDRH  6
#define D_SRCEP     7
#define D_DSTEP     8
#define D_SIZE      9
#define D_DATA      10

void AquilaBridge::parseCommand(char *data, uint8_t size)
{
    if(size < 1) return;

    uint8_t temp8, key[16];
    uint16_t temp16;
    int i;

    uint16_t srcAddr, dstAddr;
    uint8_t srcEndpoint, dstEndpoint, frameSize;
    static uint8_t frame[MAX_FRAME_SIZE];

    switch(data[COMMAND])
    {
        case CMD_DATA:
            if(size < 10) return;
            srcAddr = data[D_SRCADDRL] & 0x00FF;
            srcAddr |= ((uint16_t)data[D_SRCADDRH] << 8) & 0xFF00;
            dstAddr = data[D_DSTADDRL] & 0x00FF;
            dstAddr |= ((uint16_t)data[D_DSTADDRH] << 8) & 0xFF00;
            srcEndpoint = data[D_SRCEP];
            dstEndpoint = data[D_DSTEP];
            frameSize = data[D_SIZE];
            if((size < 10 + frameSize) || frameSize > MAX_FRAME_SIZE) return;
            memcpy(frame, &data[D_DATA], frameSize);
            txSend(dstAddr, srcEndpoint, dstEndpoint, frameSize, frame);

        break;

        case CMD_GET_OPT:
            Serial1.println("setopt");

            sendOpt();
        break;

        case CMD_SET_OPT:
            Serial1.println("setopt");
            if(size < 7) return;
            //temp8 = data[OPT_PROM]; // Not used yet
            temp8 = data[OPT_CHAN];
            setChannel(temp8);
            temp16 = data[OPT_ADDRL] & 0x00FF;
            temp16 |= ((uint16_t)data[OPT_ADDRH] << 8) & 0xFF00;
            Mesh.setAddr(temp16);
            temp16 = data[OPT_PANL] & 0x00FF;
            temp16 |= ((uint16_t)data[OPT_PANH] << 8) & 0xFF00;
            setPan(temp16);
            sendOpt();
        break;

        case CMD_GET_SECURITY:
            sendSecurity();
        break;

        case CMD_SET_SECURITY:
            if(size < 18) return;
            for(i = 0; i < 16; i++)
            {   
                key[i] = data[SEC_KEY + i];
            }
            Mesh.setSecurityKey(key);
            temp8 = data[SEC_EN];
            Mesh.setSecurityEnabled((bool)temp8);
            sendSecurity();
        break;

        case CMD_PING:
            sendStart();
        break;

        case CMD_GET_LONG_ADDR:
            sendLongAddr(this->euiAddress);
        break;
    }
}

void AquilaBridge::txSend(uint16_t dstAddr, uint8_t srcEndpoint, uint8_t dstEndpoint, uint8_t size, uint8_t *data)
{
    static TxPacket packet;

    packet.dstAddr = dstAddr;
    packet.dstEndpoint = dstEndpoint;
    packet.srcEndpoint = srcEndpoint;

    uint8_t requestAck = 0;
    if(dstAddr == BROADCAST) requestAck = 0;
    else requestAck = NWK_OPT_ACK_REQUEST;

    packet.options = requestAck;

    packet.data = data;
    packet.size = size;
    packet.confirm = txCb;

    Mesh.sendPacket(&packet);
}



AquilaBridge Bridge;
