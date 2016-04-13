/**
* \file RxBuffer.h
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


#ifndef MESHRXBUFFER_H
#define MESHRXBUFFER_H

#include <util/atomic.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <Mesh.h>
#include "lwm/nwk/nwk.h"

#ifndef MESHRXBUFFER_SIZE
#define MESHRXBUFFER_SIZE 4
#endif

// We have to define data appart because in RxPacket, data is a pointer, and we need storage space.
// When creating RxPacket, we should first copy data contents, then copy the packet and redefine its 
// data poiner.
typedef struct
{
	RxPacket packet;
	uint8_t data[AQUILAMESH_MAXPAYLOAD];

} RxBufPacket;

void RxBufPacket_init(RxBufPacket* bufPacket, RxPacket* packet);

typedef struct
{
	RxBufPacket buffer[MESHRXBUFFER_SIZE];
	RxBufPacket *head;
	RxBufPacket *tail;
	uint8_t count;
} RxBuffer;

void RxBuffer_init(RxBuffer* buffer);

bool RxBuffer_isFull(RxBuffer* buffer);

bool RxBuffer_isEmpty(RxBuffer* buffer);

bool RxBuffer_insert(RxBuffer* buffer, RxBufPacket* data);

bool RxBuffer_remove(RxBuffer* buffer, RxBufPacket* data);

#endif // MESHRXBUFFER_H