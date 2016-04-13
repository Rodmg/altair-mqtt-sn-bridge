#ifndef CONFIG_H
#define CONFIG_H

// Project configuration

// Size of hardware EEPROM
#define EEPROM_SIZE 8192

// Baudrate of serial communications (if any)
#ifndef BAUD
#define BAUD 57600
#endif

// Max EEPROM bytes used for protocol configuration storage.
#define PROTOCOL_PERSIST_SIZE 1024

#define PROTOCOL_MAXACTIONS 100
#define PROTOCOL_MAXEVENTS 100

#endif	//CONFIG_H