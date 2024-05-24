//protocolos de comunicação
#include <SPI.h>//SPI para a comunicação entre dispositivos
#include <LoRa.h>//LoRa Long Range Low Power
#include <Wire.h>// biblioteca Wire,usada para a comunicação I2C
//OLED 
#include <Adafruit_SSD1306.h>//biblioteca para controlar displays OLED
#define SCREEN_HEIGHT 64//indica a altura do display em pixels
#define SCREEN_WIDTH 128//indica o tamanho da largura do display em pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);// inicialização e comunicação I2C do display instace

//Libraries for DHT22/11
#include <DHT.h>//biblioteca usada para interagir com sensores de temperatura e umidade da série DHT 
//#include <DHT_U.h>//Unified Sensor Driver 

//Configuração dos pinos para comunicação LoRa
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26
#define BAND 866E6
//define a banda de frequência do módulo LoRa
//433E6 for Asia
//866E6 for Europe
//915E6 for North America

//Configuração dos pinos para  sensores
#define DHTPIN 14//define o pino ao qual o sensor DHT está conectado
#define DHTTYPE    DHT11// define o modelo do sensor DHT
#define TEMPERATURE_PRECISION 9//define a precisão da medição de temperatura
#define SensorPin 35 // define o pino do sensor de chuva 
#define LDR 39// define o pino do sensor luminosidade
#define MQ1 34 //define o pino do sensor MQ7

DHT dht(DHTPIN, DHTTYPE);//instancia sensor dth
//uint32_t delayMS;

//declaração das variáveis que serão usadas para armazenar os dados dos sensores
float var1;
float tempdht;
float umdht;
float hic;

void inits(){
    //inicializar o display OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true);
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("LoRa Transmitter");
  display.display();
  //configuração dos pinos que serão usados para a comunicação com o módulo LoRa
  LoRa.setPins(SS, RST, DIO0);
  // inicia o módulo LoRa com a banda de frequência "BAND"
  if (!LoRa.begin(BAND)) {
    while (1);
  }
  display.setCursor(0, 10);
  display.print("LoRa Initializing OK!");
  display.display();
  delay(2000);

}
void setup() {
  //inicializa a comunicação serial a uma taxa de transmissão de 115200 baud
  Serial.begin(115200);
  inits();

}

void loop() {
  
}
