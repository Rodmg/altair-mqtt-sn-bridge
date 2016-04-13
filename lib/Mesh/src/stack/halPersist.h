/**
* \file halPersist.h
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


#ifndef PERSIST_H
#define PERSIST_H

#include "config.h"

#include <stdint.h>
#include <stdbool.h>
#include <avr/eeprom.h>

#ifdef  __cplusplus
extern "C" {
#endif

/*
 *	Hal Persist, filesystem-like EEPROM abstraction.
 *	Current implementation uses uint16_t indexes, for a theoretical max of 64 KB.
 *	Allocates part of the EEPROM for application, currently cannot deallocate previously allocated space.
 *	Warning: keep in mind that allocations should be done in the same order every time the device is started,
 *	If this order is altered or something changes in development, you should erase the whole EEPROM after programming,
 *	else you can get wrong data readings.
 */

#ifndef EEPROM_SIZE
#define EEPROM_SIZE 8192
#endif

#define PERSIST_OK 1
#define	PERSIST_ERROR 0

typedef struct
{
	int lastStatus;	//status result of last operation

	uint16_t _start;	//Start address in EEPROM
	uint16_t _size;	//number of bytes allocated
	bool _initialized;
} Persist;

int Persist_init(Persist *self, uint16_t size);

uint8_t Persist_read(Persist *self, uint16_t address);

int Persist_read_block(Persist *self, void * dest, uint16_t address, uint16_t size);

// Persist_write: writes a byte in Persist address, returns success or error.
int Persist_write(Persist *self, uint16_t address, uint8_t value);

int Persist_write_block(Persist *self, void * src, uint16_t address, uint16_t size);


// Persist_remaining: returns remaining unallocated bytes in EEPROM
uint16_t Persist_remaining();

#ifdef  __cplusplus
}
#endif

#endif /* PERSIST_H */
