#include <Wire.h>
#include <Mesh.h>

#define LOCAL_ADDRESS 2
#define DESTINATION_ADDRESS 1

static bool receiveMessage(RxPacket *ind)
{

  Serial.print("Received message - From: ");
  Serial.println(ind->srcAddr, HEX);

  Serial.println(ind->size);
  Serial.println(ind->data[0]);

  if(ind->data[0] == 1)
  {
    Serial.print(ind->data[1], HEX);
    Serial.print(ind->data[2], HEX);
    Serial.print(ind->data[3], HEX);
    Serial.print(ind->data[4], HEX);
    Serial.print(ind->data[5], HEX);
    Serial.print(ind->data[6], HEX);
    Serial.print(ind->data[7], HEX);
    Serial.println(ind->data[8], HEX);
  }

  return true;
}

static void pingConfirm(TxPacket *req)
{
  // Here, you could check req->status
  // For knowing if the packet was succesfully sent.
}

static void sendMessage(void)
{
  Serial.print("pinging address ");
  Serial.println(DESTINATION_ADDRESS);

  uint8_t data[] = {0};

  // forming packet
  static TxPacket packet;
  packet.dstAddr = DESTINATION_ADDRESS;
  packet.dstEndpoint = 15;
  packet.srcEndpoint = 15;
  // Request acknowledge, use only if not sending to BROADCAST
  packet.options = NWK_OPT_ACK_REQUEST;
  packet.data = data;
  packet.size = 1;
  packet.confirm = pingConfirm;
  Mesh.sendPacket(&packet);
}

void setup()
{
  Serial.begin(9600);
  Mesh.begin(LOCAL_ADDRESS);

  // Security encryption setup:
  uint8_t key[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
  Mesh.setSecurityKey(key);
  Mesh.setSecurityEnabled(true);

  // Get and print local address:
  Serial.print("Local Address: ");
  Serial.println(Mesh.getShortAddr());
  uint8_t hwAddr[8];
  Mesh.getEUIAddr(hwAddr);
  Serial.print(hwAddr[0], HEX);
  Serial.print(hwAddr[1], HEX);
  Serial.print(hwAddr[2], HEX);
  Serial.print(hwAddr[3], HEX);
  Serial.print(hwAddr[4], HEX);
  Serial.print(hwAddr[5], HEX);
  Serial.print(hwAddr[6], HEX);
  Serial.print(hwAddr[7], HEX);
  Serial.println(hwAddr[8], HEX);

  Mesh.openEndpoint(15, receiveMessage);

  Mesh.announce(HUB);

  delay(100);

  sendMessage();
}

void loop()
{
  Mesh.loop();
}
