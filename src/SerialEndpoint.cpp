#include "SerialEndpoint.h"

static SerialEndpointClass *self;
static uint8_t recBuffer[SERIAL_BUFF_SIZE];

void SerialEndpointClass::sendAck()
{
  // First, lqi and rssi
  buffer[0] = 0;
  buffer[1] = 0;
  // Length, base encaspsulator len + original msg len
  buffer[2] = 2;
  // MsgType
  buffer[3] = 0x01;

  uint8_t size = buffer[2] + 2; // + lqi and rssi
  size = appendCrc((char*)buffer, size);
  slip.send((char*)buffer, size);
}

void SerialEndpointClass::sendNack()
{
  // First, lqi and rssi
  buffer[0] = 0;
  buffer[1] = 0;
  // Length, base encaspsulator len + original msg len
  buffer[2] = 2;
  // MsgType
  buffer[3] = 0x00;

  uint8_t size = buffer[2] + 2; // + lqi and rssi
  size = appendCrc((char*)buffer, size);
  slip.send((char*)buffer, size);
}

static void meshSendConfirm(TxPacket *req)
{
	// Here, you could check req->status
	// For knowing if the packet was succesfully sent.
  self->sendAck();
}

static void attendSerial(char *data, uint8_t size)
{
  StatusLeds.blinkTx();
  // Attend
  if(size < 5) return self->sendNack(); // bad data
  //uint8_t capLen = data[0];
  uint8_t msgType = data[1];
  if(msgType != 0xFE) return self->sendNack(); // unsupported message
  //uint8_t borderRadius = data[2]; // not used
  uint16_t addr;
  addr = data[3] & 0x00FF;
  addr |= ((uint16_t)data[4] << 8) & 0xFF00;
  uint8_t plSize = data[5];
  memcpy(recBuffer, &data[5], plSize); // NOTE: Only supporting 1 byte lenght TODO: consideer supporting 3 byte header

  // forming packet
	static TxPacket packet;
	packet.dstAddr = addr;
	packet.dstEndpoint = 11;
	packet.srcEndpoint = 11;
	// Request acknowledge, use only if not sending to BROADCAST
	packet.options = NWK_OPT_ACK_REQUEST;
	packet.data = recBuffer;
	packet.size = plSize;
	packet.confirm = meshSendConfirm;
	Mesh.sendPacket(&packet);
}

SerialEndpointClass::SerialEndpointClass()
{
  self = this;
}

void SerialEndpointClass::begin()
{
  slip.begin(115200, attendSerial);
}

void SerialEndpointClass::loop()
{
  slip.loop();
}

void SerialEndpointClass::send(uint16_t address, uint8_t *message, uint8_t len, uint8_t lqi, uint8_t rssi)
{
  // First, lqi and rssi
  buffer[0] = lqi;
  buffer[1] = rssi;
  // Length, base encaspsulator len + original msg len
  buffer[2] = 5 + len;
  // MsgType, always 0xFE
  buffer[3] = 0xFE;
  // Ctrl, border radius, max 0x03 according to spec
  buffer[4] = 0x01;
  // Wireless node id low byte
  buffer[5] = (uint8_t)(address & 0xFF);
  // Wireless node id high byte
  buffer[6] = (uint8_t)(address >> 8);
  // original mqtt-sn msg
  memcpy(&buffer[7], message, len);

  uint8_t size = buffer[2] + 2; // + lqi and rssi
  size = appendCrc((char*)buffer, size);
  slip.send((char*)buffer, size);
}

SerialEndpointClass SerialEndpoint;
