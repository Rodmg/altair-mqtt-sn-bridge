/**
* \file ProtocolDB.h
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


#ifndef PROTOCOLDB_H
#define PROTOCOLDB_H

#include <stdint.h>
#include <stdbool.h>
#include "stack/halPersist.h"

#ifndef PROTOCOL_PERSIST_SIZE
#define PROTOCOL_PERSIST_SIZE 1024
#endif

/*
	Memory structure:
		DBHeader
		Entry
		Entry
		...
		Entry

	DBHeader structure:
		signature0	->	'm'
		signature1	->	'a'
		signature2	->	'k'
		entrySize	->	sizeof(Entry)
		nEntries	->	<number of current active entries>
		indexSectors[0]
		indezSectors[1]	->  Each bit of this indexes represents one entry
		...					on memory, 1 active, 0 free.
		indexSectors[11]	Example: bit 0 of indezSectors[0] is the first entry
							(entry 0) after the Header.
							Bit 2 of indezSectors[2] is the entry number 18.

	How it works:
		At init, checks if signature and entrySize are correct, if not,
		an empty new header is written. (this could happen when the device is new,
		the memory gets corrupted or a change in this code is made.)
*/

#define PROTOCOL_ENTRYLEN sizeof(Entry)
// Lets use a fixed value for a PERSIST_SIZE of 1024 for now...
// Chosen so that PERSIST_SIZE < sizeof(DBHeader) + (PROTOCOL_NINDEXSECTORS * sizeof(IndexSector) * sizeof(Entry))
#define PROTOCOL_NINDEXSECTORS 10
#define PROTOCOL_MAXENTRIES 80	// PROTOCOL_NINDEXSECTORS * sizeof(IndexSector)
//Decimal points are IMPORTANT (for float casting)
//#define PROTOCOL_NINDEXSECTORS (ceil( (PROTOCOL_PERSIST_SIZE - 1.0) / ( (8.0 * PROTOCOL_ENTRYLEN) + 1.0) ))
//#define PROTOCOL_MAXENTRIES (floor((PROTOCOL_PERSIST_SIZE - 1.0 - PROTOCOL_NINDEXSECTORS)/PROTOCOL_ENTRYLEN))

#ifdef  __cplusplus
extern "C" {
#endif

#define PDB_OK 1
#define PDB_ERROR 0
#define PDB_FORMATED -1

/*typedef struct
{
	bool i0: 1;
	bool i1: 1;
	bool i2: 1;
	bool i3: 1;
	bool i4: 1;
	bool i5: 1;
	bool i6: 1;
	bool i7: 1;
} IndexSector;*/

#define IndexSector uint8_t

typedef struct
{
	uint8_t signature[3];
	uint8_t entrySize;
	uint8_t nEntries;
	IndexSector indexSectors[PROTOCOL_NINDEXSECTORS];
} DBHeader;

typedef struct
{
	bool hasParam :1;
	uint8_t reserved : 7;

} EntryConfig;

typedef struct
{
	EntryConfig config;
	uint8_t event;
	uint8_t address[8];	// 64 bit address
	uint8_t action;
	uint8_t param;
} Entry;

int PDB_init();

int PDB_format();

int PDB_readHeader(DBHeader *header);

int PDB_appendEntry(Entry *entry);

int PDB_getEntry(Entry *entry, uint8_t nEntry);

int PDB_setEntry(Entry *entry, uint8_t nEntry);

int PDB_delEntry(uint8_t nEntry);

void PDB_iteratorBegin();

int PDB_iteratorGetNextEntry(Entry *entry);

#ifdef  __cplusplus
}
#endif
#endif	/* PROTOCOLDB_H */
