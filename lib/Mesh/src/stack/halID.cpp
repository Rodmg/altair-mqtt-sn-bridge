// halID reimplementation for compatibility with Arduino's Wire.h

#include "halID.h"
#include <Wire.h>

void ID_EUI_Read(uint8_t * eui)
{
    Wire.beginTransmission(EUI_ADDR);
    Wire.write(0x98);
    Wire.endTransmission(true);
    Wire.requestFrom(EUI_ADDR, 8, true);
    int i = 0;
    if( Wire.available() >= 8 )
    {
        for(i = 0; i < 8; i++)
        {
          eui[i] = Wire.read();
        }
    } 
}

void ID_init()
{
	Wire.begin();
}


int ID_getId(uint8_t *id)
{
	ID_EUI_Read(id);
	return ID_OK;
}