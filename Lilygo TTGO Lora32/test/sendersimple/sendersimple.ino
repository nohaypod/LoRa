#include <SPI.h>
#include <LoRa.h>
#define SS 18
#define RST 14
#define DIO0 26
int counter = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("LoRa Sender");
  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(866E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  Serial.print("Sending packet: ");
  Serial.println(counter);

  // send packet
  LoRa.beginPacket();
  LoRa.print("hello desde otro nodo ");
  LoRa.print(counter);
  LoRa.endPacket();

  counter++;

  delay(5000);
}