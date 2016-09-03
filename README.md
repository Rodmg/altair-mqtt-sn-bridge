# altair-mqtt-sn-bridge

MQTT-SN Bridge firmware for the [Altair](http://www.aquila.io/en) development board, designed to work with [Aquila MQTT-SN Gateway](https://github.com/Rodmg/aquila-mqtt-sn-gateway).

## Requirements

For compiling and uploading this code to the board you need the PlatformIO tools and the [PlatformIO IDE](http://platformio.org/get-started).

You also need to install the [Altair board support for PlatformIO](https://github.com/Rodmg/altair-platformio).

## Using

Open the project folder from the Atom IDE with PlatformIO IDE plugin installed, connect your board and press the upload button.

## Note

Pair mode is not supported in this implementation, as it's not a necessity thanks to the Aquila Mesh auto address assignment, however it would be nice to have for some implementations and should be implemented in the future.
