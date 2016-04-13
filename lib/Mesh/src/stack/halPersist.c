/**
* \file halPersist.c
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


#include "halPersist.h"

static uint16_t lastAllocatedAddress = 0;	//Updated each time an allocation is made (on init);

int Persist_init(Persist *self, uint16_t size)
{
	self->_initialized = false;
	//Check if enough memory left.
	if( (lastAllocatedAddress + size) <= EEPROM_SIZE )
	{
		self->_start = lastAllocatedAddress;
		self->_size = size;
		self->lastStatus = PERSIST_OK;
		self->_initialized = true;
		lastAllocatedAddress += size;
		return PERSIST_OK;
	}
	//Not enough space:
	self->lastStatus = PERSIST_ERROR;
	return PERSIST_ERROR;
}

uint8_t Persist_read(Persist *self, uint16_t address)
{
	if( !self->_initialized || address >= self->_size )
	{
		self->lastStatus = PERSIST_ERROR;
		return 0xFF;	//thats what some microcontrollers do...
	}

	self->lastStatus = PERSIST_OK;
	return eeprom_read_byte( (unsigned char *)(self->_start + address) );
}

int Persist_read_block(Persist *self, void * dest, uint16_t address, uint16_t size)
{
	if( !self->_initialized || address >= self->_size || address + size >= self->_size )
	{
		self->lastStatus = PERSIST_ERROR;
		return PERSIST_ERROR;
	}

	self->lastStatus = PERSIST_OK;
	eeprom_read_block(dest, (void*)(self->_start + address), size);
	return PERSIST_OK;
}

int Persist_write(Persist *self, uint16_t address, uint8_t value)
{
	if( !self->_initialized || address >= self->_size )
	{
		self->lastStatus = PERSIST_ERROR;
		return PERSIST_ERROR;
	}

	eeprom_write_byte( (unsigned char *)(self->_start + address), value );
	self->lastStatus = PERSIST_OK;
	return PERSIST_OK;
}

int Persist_write_block(Persist *self, void * src, uint16_t address, uint16_t size)
{
	if( !self->_initialized || address >= self->_size || address + size >= self->_size )
	{
		self->lastStatus = PERSIST_ERROR;
		return PERSIST_ERROR;
	}

	self->lastStatus = PERSIST_OK;
	eeprom_write_block(src, (void*)(self->_start + address), size);
	return PERSIST_OK;
}

uint16_t Persist_remaining()
{
	return EEPROM_SIZE - lastAllocatedAddress;
}
