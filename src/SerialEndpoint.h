#ifndef SERIAL_ENDPOINT_H
#define SERIAL_ENDPOINT_H

#include <Arduino.h>
#include <Mesh.h>
#include "StatusLeds.h"
#include "Slip.h"
#include "CRC.h"
#include "Storage.h"

#define SERIAL_BUFF_SIZE 255
#define LEN_INDEX 0
#define PLLEN_INDEX 5

#define DEFAULT_PAN 0x01

class SerialEndpointClass
{
private:
  Slip slip;
  uint8_t buffer[SERIAL_BUFF_SIZE];
public:
  bool pairMode;
  SerialEndpointClass();
  void begin();
  void loop();
  void send(uint16_t address, uint8_t *message, uint8_t len, uint8_t lqi, uint8_t rssi);
  void sendAck();
  void sendNack();

  void enterPairMode();
  void enterNormalMode();
  void sendPair(uint16_t address, uint8_t *message, uint8_t len, uint8_t lqi, uint8_t rssi);

  void loadPreferences();
  void sendConfigReq();
};

extern SerialEndpointClass SerialEndpoint;

#endif //SERIAL_ENDPOINT_H
