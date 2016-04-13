/**
* \file ProtocolDB.c
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


#include "ProtocolDB.h"
#include <string.h>

//#include <stdio.h>

static uint8_t PDBsignature[3] = {'m', 'a', 'k'};
Persist persist;

typedef enum
{
	GET,
	SET,
	DEL
} PDB_Action;

// Iterator variables:
static int currentSector = 0, currentBit = 0, currentCount = 0;
static uint8_t currentEntry = 0;

int PDB_readHeader(DBHeader *header)
{
	return Persist_read_block(&persist, header, 0, sizeof(DBHeader));
}

int PDB__writeHeader(DBHeader *header)
{
	return Persist_write_block(&persist, header, 0, sizeof(DBHeader));
}

// Uses absolute entry number, not relative as the protocol.
int PDB__writeEntry(Entry *entry, uint8_t absEntry)
{
	uint16_t index = sizeof(DBHeader) + sizeof(Entry)*absEntry;
	return Persist_write_block(&persist, entry, index, sizeof(Entry));
}

int PDB__readEntry(Entry *entry, uint8_t absEntry)
{
	uint16_t index = sizeof(DBHeader) + sizeof(Entry)*absEntry;
	return Persist_read_block(&persist, entry, index, sizeof(Entry));
}

int PDB__getSetDelEntry(Entry *entry, uint8_t nEntry, PDB_Action action, int *startSector, int *startBit, int *startCount)
{
	DBHeader header;
	PDB_readHeader(&header);

	if(!(nEntry < header.nEntries)) return PDB_ERROR;
	// Seek the entry
	int a=0, b=0, c=0;
	int *i, *bit, *count;


	if(startSector != NULL && startBit != NULL && startCount != NULL)
	{
		i = startSector;
		bit = startBit;
		count = startCount;
	}
	else
	{
		i = &a;
		bit = &b;
		count = &c;
	}

	for(; *i < PROTOCOL_NINDEXSECTORS; (*i)++)
	{
		for(; (*bit) < 8; (*bit)++)
		{
			if( (header.indexSectors[*i] >> (*bit)) & 0x01 )
			{
				if((*count) == nEntry)
				{
					uint16_t absEntry = (*i)*8 + (*bit);
					switch(action)
					{
						case GET:
							PDB__readEntry(entry, absEntry);
							return PDB_OK;
						break;
						case SET:
							PDB__writeEntry(entry, absEntry);
							return PDB_OK;
						break;
						case DEL:
							// Disable entry and update header
							header.indexSectors[*i] &= ~(0x01<<(*bit));
							header.nEntries--;
							PDB__writeHeader(&header);
							return PDB_OK;
						break;
					}
				}

				(*count)++;
			}
		}
		(*bit) = 0;
	}
	return PDB_ERROR;

	/*int i, bit, count = 0;
	for(i = 0; i < PROTOCOL_NINDEXSECTORS; (i)++)
	{
		for((bit) = 0; (bit) < 8; (bit)++)
		{
			if( (header.indexSectors[i] >> (bit)) & 0x01 )
			{
				if((count) == nEntry)
				{
					uint16_t absEntry = (i)*8 + (bit);
					switch(action)
					{
						case GET:
							PDB__readEntry(entry, absEntry);
							return PDB_OK;
						break;
						case SET:
							PDB__writeEntry(entry, absEntry);
							return PDB_OK;
						break;
						case DEL:
							// Disable entry and update header
							header.indexSectors[i] &= ~(0x01<<(bit));
							PDB__writeHeader(&header);
							return PDB_OK;
						break;
					}
				}

				(count)++;
			}
		}
	}
	return PDB_ERROR;*/
}

int PDB_init()
{
	if(!Persist_init(&persist, PROTOCOL_PERSIST_SIZE)) return PDB_ERROR;
	// Check header signature
	DBHeader currentHeader;
	PDB_readHeader(&currentHeader);
	if(memcmp(currentHeader.signature, PDBsignature, 3) != 0 || currentHeader.entrySize != sizeof(Entry))
	{
		if(PDB_format())
		{
			return PDB_FORMATED;
		}
		else
		{
			return PDB_ERROR;
		}
	}
	return PDB_OK;
}

int PDB_format()
{
	uint8_t i = 0;
	DBHeader newHeader;
	memcpy(newHeader.signature, PDBsignature, 3);
	newHeader.entrySize = sizeof(Entry);
	newHeader.nEntries = 0;
	for(i = 0; i < PROTOCOL_NINDEXSECTORS; i++)
	{
		newHeader.indexSectors[i] = 0;
	}
	return PDB__writeHeader(&newHeader);
}

int PDB_appendEntry(Entry *entry)
{
	DBHeader header;
	PDB_readHeader(&header);

	if(!(header.nEntries < PROTOCOL_MAXENTRIES)) return PDB_ERROR;
	// Search for first free space
	int i = 0, bit = 0;
	for(i = 0; i < PROTOCOL_NINDEXSECTORS; i++)
	{
		//printf("%x\n", header.indexSectors[i]);

		for(bit = 0; bit < 8; bit++)
		{
			if( !( (header.indexSectors[i] >> bit) & 0x01 ) )
			{
				// Write entry
				uint16_t absEntry = i*8 + bit;
				PDB__writeEntry(entry, absEntry);
				// Update Header
				header.indexSectors[i] |= (0x01 << bit);
				header.nEntries++;
				PDB__writeHeader(&header);
				return PDB_OK;
			}
		}
	}
	return PDB_ERROR;
}

// Uses relative nEntry
int PDB_getEntry(Entry *entry, uint8_t nEntry)
{
	return PDB__getSetDelEntry(entry, nEntry, GET, NULL, NULL, NULL);
}

int PDB_setEntry(Entry *entry, uint8_t nEntry)
{
	return PDB__getSetDelEntry(entry, nEntry, SET, NULL, NULL, NULL);
}

int PDB_delEntry(uint8_t nEntry)
{
	return PDB__getSetDelEntry(NULL, nEntry, DEL, NULL, NULL, NULL);
}

void PDB_iteratorBegin()
{
	currentSector = 0;
	currentBit = 0;
	currentCount = 0;
	currentEntry = 0;
}

int PDB_iteratorGetNextEntry(Entry *entry)
{
	int result = PDB__getSetDelEntry(entry, currentEntry, GET, &currentSector, &currentBit, &currentCount);
	currentEntry++;

	//printf("currentEntry: %d, currentSector: %d, currentBit: %d, currentCount: %d\n", currentEntry, currentSector, currentBit, currentCount);

	return result;
}
