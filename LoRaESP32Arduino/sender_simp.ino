#include <SPI.h>//biblioteca SPI para a comunicação entre dispositivos
#include <LoRa.h>//biblioteca LoRa
#include <Wire.h>// biblioteca Wire,usada para a comunicação I2C
#include <Adafruit_SSD1306.h>//biblioteca para controlar displays OLED
#include <DHT.h>//biblioteca usada para interagir com sensores de temperatura e umidade da série DHT 

#define SCREEN_WIDTH 128//indica o tamanho da largura do display em pixels
#define SCREEN_HEIGHT 64//indica a altura do display em pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);// inicialização e comunicação I2C do display

//Configuração dos pinos para  comunicação LoRa
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26
//define a banda de frequência do módulo LoRa
#define BAND 866E6

#define DHTPIN 14//define o pino ao qual o sensor DHT está conectado
#define SensorPin 35 // define o pino do sensor de chuva 
#define LDR 39// define o pino do sensor luminosidade
#define MQ1 34 //define o pino do sensor MQ7

#define TEMPERATURE_PRECISION 9//define a precisão da medição de temperatura
#define DHTTYPE    DHT11// define o modelo do sensor DHT
DHT dht(DHTPIN, DHTTYPE);

//declaração das variáveis que serão usadas para armazenar os dados dos sensores
float temp;
float tempdht;
float umdht;
float hic;


char Bumidade[7];
char Bumidade2[7];
char Btemp[7];
char ldr1[7];

 int LeituraSensorUmidade;
//variavel para o sensor de luz (fotoresistor)
int lecturaldr;

//variavel para o sensor mq7
int lecturasensordioxido;

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

void loop() {
  
  leituraSensores();
  display.setCursor(0, 0);
  display.println("Transmitter");
  display.setCursor(0, 20);
  display.setTextSize(1);
  delay(1000);
}
//ler os dados dos sensores, converter os valores em uma string
void leituraSensores() {
  LeituraSensorUmidade = analogRead(SensorPin) / 40;
  dtostrf(LeituraSensorUmidade, 6, 2, Bumidade);
  lecturaldr = analogRead(LDR);
  dtostrf(lecturaldr, 6, 2, ldr1);
  umdht = dht.readHumidity();
  dtostrf(umdht, 6, 2, Bumidade2);
  tempdht = dht.readTemperature();
  float tempTemp = tempdht;
  dtostrf(tempTemp, 6, 2, Btemp);
  display.clearDisplay();
  display.setCursor(0, 30);
  display.print("Umidade terra");
  display.setCursor(80, 30);
  display.print(Bumidade);
  display.display();
  display.setCursor(0, 40);
  display.print("Temp");
  display.setCursor(80, 40);
  display.print(tempdht);
  display.display();
  display.setCursor(0, 50);
  display.print("Umidade Ar");
  display.setCursor(80, 50);
  display.print(umdht);
  display.display();
  //variavel string que contém os dados dos sensores
  String mensagem1 = String(Bumidade) + "/" + String(Bumidade2)
  + "/" + String(Btemp) + "&" + String(ldr1) + "$";
  // enviar os dados através do módulo LoRa
  LoRa.beginPacket();
  LoRa.print(mensagem1);
  LoRa.endPacket();
  delay(1000);
}
