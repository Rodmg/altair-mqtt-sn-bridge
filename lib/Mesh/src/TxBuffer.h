/**
* \file TxBuffer.h
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


#ifndef MESHTXBUFFER_H
#define MESHTXBUFFER_H

#include <util/atomic.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <Mesh.h>
#include "lwm/nwk/nwk.h"

#ifndef MESHTXBUFFER_SIZE
#define MESHTXBUFFER_SIZE 8
#endif

// We have to define data appart because in TxPacket, data is a pointer, and we need storage space.
// When creating TxPacket, we should first copy data contents, then copy the packet and redefine its 
// data poiner.
typedef struct
{
	TxPacket packet;
	uint8_t data[AQUILAMESH_MAXPAYLOAD];

} TxBufPacket;

void TxBufPacket_init(TxBufPacket* bufPacket, TxPacket* packet);

typedef struct
{
	TxBufPacket buffer[MESHTXBUFFER_SIZE];
	TxBufPacket *head;
	TxBufPacket *tail;
	uint8_t count;
} TxBuffer;

void TxBuffer_init(TxBuffer* buffer);

bool TxBuffer_isFull(TxBuffer* buffer);

bool TxBuffer_isEmpty(TxBuffer* buffer);

bool TxBuffer_insert(TxBuffer* buffer, TxBufPacket* data);

bool TxBuffer_remove(TxBuffer* buffer, TxBufPacket* data);

#endif // MESHTXBUFFER_H
