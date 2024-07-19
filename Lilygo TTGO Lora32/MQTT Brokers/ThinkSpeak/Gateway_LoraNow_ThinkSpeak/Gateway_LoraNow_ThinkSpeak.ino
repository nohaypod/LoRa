
/*
  LoRaNow Gateway

  This code receives messages form the node and sends a message back.

*/
//OLED 
#include <Adafruit_SSD1306.h>//biblioteca para controlar displays OLED
#define SCREEN_HEIGHT 64//indica a altura do display em pixels
#define SCREEN_WIDTH 128//indica o tamanho da largura do display em pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);// inicialização e comunicação I2C do display instace

#include <LoRaNow.h> //Luiz H. Cassettari Ricaun based in LoRa.h
/****************************************
*Definir librería, instancias y constantes
*para la conexión a ThingSpeak
****************************************/
#include "ThingSpeak.h"
unsigned long Channel_ID = 2600666; //Ingrese su numero de canal de ThingSpeak
const char * WriteAPIKey = "E37DQ0B2DAUSZKMO"; //Ingrese su clave de API de escritura de canal

/*Definir libreris, instancias y constantes
*para la conexion WIFI
****************************************/
#include <WiFi.h>
const char * ssid = "Estudio 1"; // Ingrese su nombre red wifi
const char * pass = "test1234"; //Ingresa la contraseña de tu red
WiFiClient client;

void onMessage(uint8_t *buffer, size_t size);

void setup() {
  Serial.begin(115200);

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

  WiFi.begin(ssid, pass); //Se inicia la conexión al Wifi
//Minetras se conecta imprimirá ...
while(WiFi.status() != WL_CONNECTED){
  delay(500);
  Serial.print(".");
//Ya que se estableció la conexión al Wifi se imprime conexión establecida
Serial.println("Conexion establecida");
ThingSpeak.begin(client); //Iniciar el servidor de ThingSpeak
}

void loop() {
  LoRaNow.loop();
  //display.clearDisplay();
}

void onMessage(uint8_t *buffer, size_t size)
{
  unsigned long id = LoRaNow.id();
  byte count = LoRaNow.count();
  
  Serial.print("NodeIdTx: ");
  Serial.print(id, HEX);
  Serial.print(" Count: ");
  Serial.print(count);
  Serial.print(" Message: ");
  Serial.write(buffer, size);
  Serial.println();
  Serial.println();

  // Send data to 
  LoRaNow.clear();
  LoRaNow.print("LoRaGateway responds thanks "+ String(id,HEX) +" Packet "+ String(count)+" recived :)");
  LoRaNow.send();
  ThingSpeak.setField(1, count);

  display.setCursor(70, 0);
  display.print("!");
  display.print(LoRaNow.id(),HEX);
  display.display();
};