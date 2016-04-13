# Aquila Services

REST-like services library for Altair.
Inspired on HTTP REST services, this library allows simplified REST implementations over an 802.15.4 mesh network.

## Important differencies with HTTP REST services:

1. Because of 802.15.4 packet size limitations, the service name + the data must be less than 101 bytes (AQUILAMESH_MAXPAYLOAD - 4).
2. There is only one level for the service names. (e.g. You can have services like "temperature" and "state", but not "state/12" or "temperature/bla").
3. Requests only have a service name, method and data (body), there is no header.

## Supported Methods:

- GET
- POST
- PUT
- DELETE

## Supported response status:

- R200: OK
- R404: Service not found
- R405: Method not allowed
- R408: Timeout
- R500: Service error

## Supported data formats:

Data can be anything you want as long it's formed by bytes.
As a suggestion, you can use JSON encoded strings with help of the ArduinoJson library, you can see how in the examples.

## Rationale

This library complements the Aquila Protocol because with that we have:

Actions: Things that the device can DO
Events: Things that can HAPPEN to the device
Services: Things that you can ASK to the device and get a response
WSerial: Logging and Debugging

However, this is provided as a separate library because it's not always needed and is nice to have the option to choose.

## Licence

Use or modification of this software is subject to the Makerlab licence, please see ``license.txt`` for details.
