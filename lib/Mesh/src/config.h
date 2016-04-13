#ifndef _LWM_CONFIG_H
#define _LWM_CONFIG_H

// For now, only 256RFR2 is supported
#define HAL_ATMEGA256RFR2
#define PHY_ATMEGARFR2

// Definition of these options is at Atmel-42028-Lightweight-Mesh-Developer-Guide_Application-Note_AVR2130.pdf

#define NWK_BUFFERS_AMOUNT 5
#define NWK_DUPLICATE_REJECTION_TABLE_SIZE 10
#define NWK_DUPLICATE_REJECTION_TTL 1000
#define NWK_ROUTE_TABLE_SIZE 30
#define NWK_ACK_WAIT_TIME 100
#define NWK_ENABLE_ROUTING
#define NWK_ENABLE_SECURITY
//#define NWK_ENABLE_SECURE_COMMANDS	// Breaks unsecured acknowledges
// 0: AES-128
#define SYS_SECURITY_MODE 0



#endif // _LWM_CONFIG_H
