#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <Arduino.h>
#include "Mesh.h"

static MeshControl *self;

static TxPacket packet;
static uint16_t genAddr;
static uint16_t shortAddr;
static bool waitingAddrConfirm;
static bool addrAssignSuccess;

uint16_t calcrc(uint8_t *ptr, int count);
static void pingCb(TxPacket *req);
static bool meshCb(RxPacket *ind);

uint16_t calcrc(uint8_t *ptr, int count)
{
	int crc;
	char i;
	crc = 0;
	while (--count >= 0)
	{
		crc = crc ^ (int) *ptr++ << 8;
		i = 8;
		do
		{
			if (crc & 0x8000)
			crc = crc << 1 ^ 0x1021;
			else
			crc = crc << 1;
		} while(--i);
	}
	return (crc);
}

static void pingCb(TxPacket *req)
{
	waitingAddrConfirm = false;

	if(req->status == NWK_SUCCESS_STATUS)
	{
		// Got Ack, means someone else has the address
		addrAssignSuccess = false;
	}
	else
	{	// If didnt get ack, means the address is free, use it.
		addrAssignSuccess = true;
		shortAddr = genAddr;
	}

	(void)req;
}

static void euiCb(TxPacket* packet)
{
	(void)packet;
}

static bool meshCb(RxPacket *ind)
{
	// check if address collision packet
	if(ind->size == 0)	// address collision packet
	{
		// send ack
		return true;
	}
	else 	// size at least 1
	{
		if(ind->data[0] == MESH_CMD_GETEUI)
		{
			self->sendEUI(ind->srcAddr);
		}
	}
	return true;
}



MeshControl::MeshControl()
{
	self = this;
	genAddr = 0;
	waitingAddrConfirm = false;
	addrAssignSuccess = false;
}

void MeshControl::begin(AquilaMesh *_mesh)
{
	this->mesh = mesh;
	mesh->openEndpoint(MESH_ADDRENDPOINT, meshCb);
}

uint16_t MeshControl::assignAddr(uint8_t *id)
{
	genAddr = calcrc(id, 8);
	// Check if we are not in manual address range, and fix.
	if(genAddr < 256) genAddr += 256;

	//test collision:
	//genAddr = 3;

	while(!addrAssignSuccess)
	{
		// Send a ping packet to our generated address to check if its already taken
		packet.dstAddr = genAddr;
		packet.dstEndpoint = MESH_ADDRENDPOINT;
		packet.srcEndpoint = MESH_ADDRENDPOINT;
		packet.options = NWK_OPT_ACK_REQUEST;
		packet.data = NULL;
		packet.size = 0;
		packet.confirm = pingCb;
		mesh->sendPacket(&packet);

		waitingAddrConfirm = true;

		while(waitingAddrConfirm)
		{
			mesh->loop();
		}

		// If address already in use, try with another one
		if(!addrAssignSuccess) genAddr += 1 + id[7];
	}
	return shortAddr;
}

void MeshControl::sendEUI(uint16_t dest)
{
	static uint8_t data[9];
	data[0] = MESH_CMD_RESEUI;

	// copy eui
	uint8_t euiAddr[8];
	mesh->getEUIAddr(euiAddr);
	memcpy(&data[1], euiAddr, 8);

	packet.dstAddr = dest;
	packet.dstEndpoint = MESH_ADDRENDPOINT;
	packet.srcEndpoint = MESH_ADDRENDPOINT;

	uint8_t requestAck = 0;
	if(dest == BROADCAST) requestAck = 0;
	else requestAck = NWK_OPT_ACK_REQUEST;

	packet.data = data;
	packet.size = 9;
	packet.confirm = euiCb;
	
	mesh->sendPacket(&packet);
}
