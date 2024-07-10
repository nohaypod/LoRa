//protocolos de comunicação
#include <SPI.h>//SPI para a comunicação entre dispositivos
#include <LoRa.h>//LoRa
#include <Wire.h>// biblioteca Wire,usada para a comunicação I2C

//OLED library, variable e instancia 
#include <Adafruit_SSD1306.h>//biblioteca para controlar displays OLED
#define SCREEN_HEIGHT 64//indica a altura do display em pixels
#define SCREEN_WIDTH 128//indica o tamanho da largura do display em pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);// inicialização e comunicação I2C do display instace

//Sensor BME680 need
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>
#include <bme68x.h>
#include <bme68x_defs.h>
Adafruit_BME680 bme; // instância do sensor BME
float BME680temperatura,BME680pressao,BME680umidade;//declaração das variáveis que serão usadas para armazenar os dados dos sensores

//Configuração dos pinos para  comunicação LoRa
//#define SCK 5
//#define MISO 19
//#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26
#define BAND 866E6 //define a banda de frequência do módulo LoRa //433E6 for Asia //866E6 for Europe //915E6 for North America

//initilize packet counter
RTC_DATA_ATTR int bootCount = 0;//registro RTC 
int readingID = 0;
String LoRaMessage = "";
const String nodeID = "Nodo2";  // Identificación
byte localAddress = 0x02;     // address of this device
byte destination = 0xFF;      // destination to send to
String incoming = "";


void setup() {
  //inicializa a comunicação serial a uma taxa de transmissão de 115200 baud
  Serial.begin(115200);

  //inicializar o display OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true);
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("ID:");
  //configuração dos pinos que serão usados para a comunicação com o módulo LoRa
  LoRa.setPins(SS, RST, DIO0);
  // inicia o módulo LoRa com a banda de frequência "BAND"
  if (!LoRa.begin(BAND)) {
    Serial.println("LoRa init failed. Check your connections.");
    while (1);
  }
  Serial.println("o sistema LoRa deveria ter iniciado com sucesso"); 
  display.setCursor(20, 0);
  display.print(nodeID);
  display.display();

  if (!bme.begin()) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    display.setCursor(0, 40);
    display.print("conectar bien sensorBME680!");
    display.display();
    //while (1);
  }
  display.setCursor(0, 40);
  display.print("OLED LoRa BME680 ok!");
  display.display();
  delay(3000);
}

void obtenerBMEleituras(){
  BME680temperatura=bme.readTemperature();
  BME680pressao=bme.readPressure()/100.0F;
  BME680umidade=bme.readHumidity();
}



void exibirleituras(){
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.setTextColor(WHITE);
  display.print("ID:");
  display.setCursor(20, 0);
  display.print(nodeID);
  display.setCursor(70, 0);
  display.print(readingID);
  
  display.setCursor(0,10);
  display.print("Temperatura:");
  display.setCursor(75,10);
  display.print(BME680temperatura);
  display.print("");
  display.drawCircle(105,10,1, WHITE);
  display.setCursor(110,10);
  display.print("C");
  display.setCursor(0,20);
  display.print("(RH)Umidade:");
  display.setCursor(75,20);
  display.print(BME680umidade);
  display.setCursor(105,20);
  display.print("(%)");
  display.setCursor(0, 30);
  display.print("Pressao:");
  display.setCursor(50,30);
  display.print(BME680pressao);
  display.setCursor(85,30);
  display.print("mmHg");
  display.setCursor(0,40);
  display.print("Luz");
  display.setCursor(20,40);
  display.print(":");
  display.setCursor(30,40);
  //espaciopara agregar mostrar variable sensor deluz
  display.setCursor(0,50);  
  display.print("mensagem:");
  
  display.display();

  Serial.print("Temperatura:");
  Serial.print(BME680temperatura);
  Serial.print(" °C\tUmidade:");
  Serial.print(BME680umidade);
  Serial.print(" %\tPressao:");
  Serial.print(BME680pressao);
  Serial.print(" hPa\t Luz: ");
  Serial.print("\tPacote: ");
  Serial.println(readingID);
  readingID++;
  //delay(2000);
}

void onReceive(int packetSize) {
  if (packetSize == 0) {

    return;          // if there's no packet, return
  }
  // read packet header bytes:
  int recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingLength = LoRa.read();    // incoming msg length
  while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }

  //if (incomingLength != incoming.length()) {   // check length for error
  //  Serial.println("error: message length does not match length");
  //  return;                             // skip rest of function
  //}

  // if the recipient isn't this device or broadcast,
  //if (recipient != localAddress && recipient != 0xFF) {
  //  Serial.println("This message is not for me.");
  //  return;                             // skip rest of function
  //}

  // if message is for this device, or broadcast, print details:
  Serial.print("Received from: " + String(sender));
  Serial.print(" ");
  Serial.print("Sent to: " + String(recipient));
  Serial.print(" ");
  Serial.print("Message ID: " + String(incomingMsgId));
  Serial.print(" ");
  Serial.print("length: " + String(incomingLength));
  Serial.print(" ");
  Serial.print("Message: " + incoming);
  Serial.print(" ");
  Serial.print("RSSI: " + String(LoRa.packetRssi()));
  Serial.print(" ");
  Serial.println("Snr: " + String(LoRa.packetSnr()));
  //display.clearDisplay();
  display.setCursor(60,50);  
  //display.print(incoming);
  //display.setCursor(60,60);
  LoRa.beginPacket();
  LoRa.print(incoming);
  LoRa.endPacket();

  display.print("RSSI: " + String(LoRa.packetRssi()));
  display.setCursor(60,60);
  display.print("Snr: " + String(LoRa.packetSnr()));
  display.display();
}


void loop() {

  // parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());

  obtenerBMEleituras();
  exibirleituras();
  LoRaMessage = String(nodeID)+":"+String(readingID)+":" + String(BME680temperatura)+"°C "+String(BME680umidade)+"% "+String(BME680pressao)+"hPA ";
  LoRa.beginPacket();
  LoRa.print(LoRaMessage);
  LoRa.endPacket();

  
}




