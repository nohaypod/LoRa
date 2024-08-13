#define RH_MESH_MAX_MESSAGE_LEN 50
#define TS_ENABLE_SSL // Para conexão HTTPS SSL

#include <RHMesh.h>
#include <RH_RF95.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <WiFiClient.h>
#include <WiFi.h>
#include "ThingSpeak.h"
#define SECRET_SSID "Estudio 1"		// replace MySSID with your WiFi network name
#define SECRET_PASS "test1234"	// replace MyPassword with your WiFi password

#define SECRET_CH_ID 2600666			// replace 0000000 with your channel number
#define SECRET_WRITE_APIKEY "E37DQ0B2DAUSZKMO"   // replace XYZ with your channel write API Key

// Definições OLED
#define SCREEN_HEIGHT 64
#define SCREEN_WIDTH 128
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Endereço da bridge
#define BRIDGE_ADDRESS 1

// Definições pinos LoRa
#define LLG_SCK 5
#define LLG_MISO 19
#define LLG_MOSI 27
#define LLG_CS  18
#define LLG_RST 23
#define LLG_DI0 26

#define RXTIMEOUT 3000

// Instância única do driver de rádio
RH_RF95 rf95(LLG_CS, LLG_DI0);

// Classe para gerenciar a entrega e o recebimento de mensagens
RHMesh manager(rf95, BRIDGE_ADDRESS);

// Definições Wi-Fi e ThingSpeak
char ssid[] = SECRET_SSID;   // Nome da rede
char pass[] = SECRET_PASS;   // Senha da rede
WiFiClient client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

int number = 0;

void setup() {
  // Inicialização do OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true);
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("LoRa Transmitter");
  display.display();
  
  Serial.begin(115200);
  Serial.print(F("initializing node "));
  Serial.print(BRIDGE_ADDRESS);
  SPI.begin(LLG_SCK,LLG_MISO,LLG_MOSI,LLG_CS);
  if (!manager.init()) {
    Serial.println(" init failed");
  } else {
    Serial.println(" done");
  }

  rf95.setTxPower(2, false);
  rf95.setFrequency(868.0);
  rf95.setCADTimeout(500);

  if (!rf95.setModemConfig(RH_RF95::Bw125Cr45Sf128)) {
    Serial.println(F("set config failed"));
  }

  Serial.println("RF95 ready");

  // Inicialização do Wi-Fi
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
}

void loop() {
  // Recebe e responde a mensagens LoRa
  uint8_t buf[RH_MESH_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  uint8_t from;
  if (manager.recvfromAck(buf, &len, &from)) {
    Serial.print("request from node n.");
    Serial.print(from);
    Serial.print(": ");
    Serial.print((char*)buf);
    Serial.print(" rssi: ");
    Serial.println(rf95.lastRssi());

    // Atualiza o display OLED
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Req from node ");
    display.print(from);
    display.print(": ");
    display.println((char*)buf);
    display.print("RSSI: ");
    display.println(rf95.lastRssi());
    display.display();
  }

  // Conecta ao Wi-Fi e envia dados ao ThingSpeak
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }

  int x = ThingSpeak.writeField(myChannelNumber, 1, number, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  // Atualiza o valor para o próximo envio
  number++;
  if(number > 99){
    number = 0;
  }
  
  delay(20000); // Aguarda 20 segundos antes de atualizar novamente
}