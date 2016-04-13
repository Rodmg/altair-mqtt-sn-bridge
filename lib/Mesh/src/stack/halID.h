// halID reimplementation for compatibility with Arduino's Wire.h

#ifndef ID_H
#define ID_H

#include "config.h"

#include <stdint.h>
#include <stdbool.h>

#ifdef  __cplusplus
extern "C" {
#endif

/*
 *	halID, support for unique id chips, like EUI-64 standard MAC address preprogrammed id chips.
 *	
 *	AT24MAC602: I2C 2Kbit EEPROM with EUI-64 unique ID
 *	  Memory organization:
 *	    Standard 2-Kbit EEPROM - Address 0x0A
 *	      Address Range (0x00 - 0x7F) - Software and hardware write protection capable
 *	      Address Range (0x80 - 0xFF) - Hardware write protection capable
 *	      
 *	    Extended Memory - Address 0x0B
 *	      128-bit Serial Number (0x80 - 0x8F) - Read only
 *	      EUI-64 Value (0x98 - 0x9F) - Read only
 */

#define EUI_ADDR 0x58

#define ID_OK	 1
#define ID_ERROR 0

void ID_init();

//	getId: 
// @param id: space of memory where the id will be stored.
// returns success or failure
int ID_getId(uint8_t *id);


#ifdef  __cplusplus
}
#endif

#endif /* ID_H */