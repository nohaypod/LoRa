
//TTGO Lora-OLED V3 node tx/rx 
//protocolos de comunicação
#include <SPI.h>//SPI para a comunicação entre dispositivos
#include <LoRa.h>//LoRa Long Range Low Power
#include <Wire.h>// biblioteca Wire,usada para a comunicação I2C
//OLED 
#include <Adafruit_SSD1306.h>//biblioteca para controlar displays OLED
#define SCREEN_HEIGHT 64//indica a altura do display em pixels
#define SCREEN_WIDTH 128//indica o tamanho da largura do display em pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);// inicialização e comunicação I2C do display instace

//Sensor BME680 need
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>
#include <bme68x.h>
#include <bme68x_defs.h>
//Libraries for DHT22/11
#include <DHT.h>//biblioteca usada para interagir com sensores de temperatura e umidade da série DHT 
#include <DHT_U.h>//Unified Sensor Driver 

//Configuração dos pinos para comunicação LoRa
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26
#define BAND 915E6
//define a banda de frequência do módulo LoRa
//433E6 for Asia
//866E6 for Europe
//915E6 for North America

//Configuração dos pinos para  sensores
//#define BME_SCK 13
//#define BME_MISO 12
//#define BME_MOSI 11
//#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME680 bme; // I2C
//Adafruit_BME680 bme(BME_CS); // hardware SPI
//Adafruit_BME680 bme(BME_CS, BME_MOSI, BME_MISO,  BME_SCK);
#define DHTPIN 14//define o pino ao qual o sensor DHT está conectado
#define DHTTYPE    DHT22// define o modelo do sensor DHT
//#define TEMPERATURE_PRECISION 9//define a precisão da medição de temperatura
//#define SensorPin 35 // define o pino do sensor de chuva 
#define LDR 39// define o pino do sensor luminosidade
//#define MQ1 34 //define o pino do sensor MQ7

DHT dht(DHTPIN, DHTTYPE);//instancia sensor dth
//uint32_t delayMS;

//declaração das variáveis que serão usadas para armazenar os dados dos sensores
float BME680temperatura,BME680pressao,BME680umidade;
float tempdht;
float umdht;
float hic;
float chuva;
int luz;
float ldrVoltage;

//initilize packet counter
int readingID = 0;
String LoRaMessage = "";
int idnode=1;

void inits(){
    //inicializar o display OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true);
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("LoRa Node ID: ");
  display.setCursor(80, 0);
  display.print(idnode);
  //display.display();
  //LoRa SPI
  SPI.begin(SCK, MISO, MOSI, SS);
  //configuração dos pinos que serão usados para a comunicação com o módulo LoRa
  LoRa.setPins(SS, RST, DIO0);
  // inicia o módulo LoRa com a banda de frequência "BAND"
  if (!LoRa.begin(BAND)) {
    Serial.println("Falha ao iniciar LoRa");
    //while (1);
  }
  display.setCursor(0, 10);
  display.print("OLED OK!");
  display.display();
  if (!bme.begin()) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    display.setCursor(0, 20);
    display.print("conectar bien sensor!");
    display.display();
    //while (1);
  }
}
void initDHT()
{
  // Initialize DHT device.
  Serial.println(F("Inicializando o sensor DHT"));
  dht.begin();
  Serial.println(F("DHT sensor is OK!"));
}

void obterDHTleituras (){
  // Delay between measurements.
  //delay(delayMS);
  // Get temperature event and print its value.
  sensors_event_t event;
  //dht.temperature().getEvent(&event);
  umdht = dht.readHumidity();
  tempdht = dht.readTemperature();
  luz = analogRead(LDR);  // Read LDR sensor value
  
  ldrVoltage = luz * (5.0 / 1023.0);  // Convert to voltage
}

void obtenerBMEleituras(){
  BME680temperatura=bme.readTemperature();
  BME680pressao=bme.readPressure()/100.0F;
  BME680umidade=bme.readHumidity();
}
void obterleituras(){
  obterDHTleituras();
  obtenerBMEleituras();
  //delay(500);
}

void exibirleituras(){
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("Pacote enviado:");
  display.setCursor(90,0);
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
  display.setCursor(0,40);
  display.print("Luz");
  display.setCursor(20,40);
  display.print(":");
  display.setCursor(30,40);
  display.print(ldrVoltage);
  display.setCursor(0,50);
  display.print("Pacote recebido");
  display.display();
  
  Serial.print("Temperatura:");
  Serial.print(BME680temperatura);
  Serial.print(" °C\tUmidade:");
  Serial.print(BME680umidade);
  Serial.print(" %\tPressao:");
  Serial.print(BME680pressao);
  Serial.print(" hPa\t Luz: ");
  Serial.print(luz);
  Serial.print("\tPacote: ");
  Serial.println(readingID);
  
  readingID++;
}
void enviarleituras(){

  LoRaMessage = String(readingID) + "/" + String(BME680temperatura)+""+String(BME680umidade)+""+String(BME680pressao)+""+String(luz);
  //Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.print(LoRaMessage);
  LoRa.endPacket();
  Serial.println("Pacote enviado!");
  //exibirleituras();
  //delay(10000);

}

void setup() {
  //inicializa a comunicação serial a uma taxa de transmissão de 115200 baud
  Serial.begin(115200);
  inits();
  //initDHT();
}

void loop() {
  delay(1000);
  obterleituras(); 
  exibirleituras();
  enviarleituras();
}
