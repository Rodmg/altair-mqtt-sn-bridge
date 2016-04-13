# Aquila Mesh

Based upon atmel's lwm. see src/lwm/licence.txt for it's licence

Endpoints:

Atmel's LWM provides Endpoints, that work in a similar fashion as TCP/UDP Ports. There are 16 endpoints available.

With Aquila, endpoints 0 to 7 are free for user's custom implementations, endpoints 8-16 are reserved as follows:

- 15: Short Address collision discovery and EUI anouncement
- 14: WSerial
- 13: Aquila Protocol
- 12: Services
- 8 - 11: Reserved


## Channel

- We are using channel 26 by default.

## Automatic Address Assignment:

- Address 0x0000 is reserved for uninitialized address.
- Addresses 0x0001 to 0x00FE are only for manual configuration
- Address 0x00FF is for the bridge.
- Addresses 0x0100 to 0xFFFD are for automatic assignment
- Address 0xFFFE is reserved in 802.15.4 standard
- Address 0xFFFF is the BROADCAST address.

## Licence

Use or modification of this software is subject to the Makerlab licence, please see ``license.txt`` for details.
