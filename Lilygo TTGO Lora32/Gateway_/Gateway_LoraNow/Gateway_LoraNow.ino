/*
  LoRaNow Simple Gateway

  This code receives messages form the node and sends a message back.

  created 01 04 2019
  by Luiz H. Cassettari
*/
//OLED 
#include <Adafruit_SSD1306.h>//biblioteca para controlar displays OLED
#define SCREEN_HEIGHT 64//indica a altura do display em pixels
#define SCREEN_WIDTH 128//indica o tamanho da largura do display em pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);// inicialização e comunicação I2C do display instace

#include <LoRaNow.h> //Luiz H. Cassettari Ricaun based in LoRa.h

void setup() {
  Serial.begin(115200);
  Serial.println("LoRaNowGateway");

  // LoRaNow.setFrequencyCN(); // Select the frequency 486.5 MHz - Used in China
  // LoRaNow.setFrequencyEU(); // Select the frequency 868.3 MHz - Used in Europe
  // LoRaNow.setFrequencyUS(); // Select the frequency 904.1 MHz - Used in USA, Canada and South America
  // LoRaNow.setFrequencyAU(); // Select the frequency 917.0 MHz - Used in Australia, Brazil and Chile
  
  // LoRaNow.setFrequency(frequency);
  // LoRaNow.setSpreadingFactor(sf);
  // LoRaNow.setPins(ss, dio0);

  // LoRaNow.setPinsSPI(sck, miso, mosi, ss, dio0); // Only works with ESP32

  if (!LoRaNow.begin()) {
    Serial.println("LoRa init failed. Check your connections.");
    while (true);
  }
  //inicializar o display OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true);
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("ID:");
  display.print(LoRaNow.id(),HEX);
  display.display();

  LoRaNow.onMessage(onMessage);
  LoRaNow.gateway();
}

void loop() {
  LoRaNow.loop();
}

void onMessage(uint8_t *buffer, size_t size)
{
  unsigned long id = LoRaNow.id();
  byte count = LoRaNow.count();
  
  Serial.print("Node Id: ");
  Serial.println(id, HEX);
  Serial.print("Count: ");
  Serial.println(count);
  Serial.print("Message: ");
  Serial.write(buffer, size);
  Serial.println();
  Serial.println();

  // Send data to the node
  LoRaNow.clear();
  LoRaNow.print("LoRaGateway responds thanks node "+ String(id));
  LoRaNow.send();

  display.setCursor(70, 0);
  display.print("M:");
  display.print(LoRaNow.id(),HEX);
  display.display();
}