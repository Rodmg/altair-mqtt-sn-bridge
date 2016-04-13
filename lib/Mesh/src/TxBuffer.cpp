/**
* \file TxBuffer.cpp
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


#include "TxBuffer.h"

void TxBufPacket_init(TxBufPacket* bufPacket, TxPacket* packet)
{
	// Copy the data of the packet to the bufPacket
	memcpy(bufPacket->data, packet->data, packet->size);
	// Copy the packet inside bufPacket
	memcpy(&bufPacket->packet, packet, sizeof(TxPacket));
	// Update data pointer of packet
	//bufPacket->packet.data = bufPacket->data;
}


void TxBuffer_init(TxBuffer* buffer)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		buffer->head = buffer->buffer;
		buffer->tail = buffer->buffer;
		buffer->count = 0;
	}
}

bool TxBuffer_isFull(TxBuffer* buffer)
{
	uint8_t count;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		count = buffer->count;
	}
	return count >= MESHTXBUFFER_SIZE;
}

bool TxBuffer_isEmpty(TxBuffer* buffer)
{
	uint8_t count;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		count = buffer->count;
	}
	return count == 0;
}

bool TxBuffer_insert(TxBuffer* buffer, TxBufPacket* data)
{
	if(TxBuffer_isFull(buffer)) return false;
	memcpy(buffer->head, data, sizeof(TxBufPacket));

	if (++buffer->head == &buffer->buffer[MESHTXBUFFER_SIZE])
	{
		buffer->head = buffer->buffer;
	}

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		buffer->count++;
	}
	return true;
}

bool TxBuffer_remove(TxBuffer* buffer, TxBufPacket* data)
{
	if(TxBuffer_isEmpty(buffer)) return false;

	memcpy(data, buffer->tail, sizeof(TxBufPacket));

	//data->packet.data = data->data;

	if (++buffer->tail == &buffer->buffer[MESHTXBUFFER_SIZE])
	{
		buffer->tail = buffer->buffer;
	}


	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		buffer->count--;
	}
	return true;

}
