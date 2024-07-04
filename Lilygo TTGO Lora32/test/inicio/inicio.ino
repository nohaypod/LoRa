//LoRa Lilygo TTGO Lora32 OLED 0.0.0 
//protocolos de comunicação
#include <SPI.h>//SPI para a comunicação entre dispositivos
#include <Wire.h>// biblioteca Wire,usada para a comunicação I2C
#include<LoRaNow.h> //biblioteca baseada em lora.h com melhorias

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
Adafruit_BME680 bme; // instância do sensor BME
float BME680temperatura,BME680pressao,BME680umidade;//declaração das variáveis que serão usadas para armazenar os dados dos sensores


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



//initilize packet counter
RTC_DATA_ATTR int bootCount = 0;//registro RTC 
int readingID = 0;
String LoRaMessage = "";
//int idnode=1; //id para cada nodo
unsigned long id;

void onMessage(uint8_t *buffer, size_t size)
{
  Serial.print("Receive Message: ");
  Serial.write(buffer, size);
  Serial.println();
  Serial.println();
  display.print("Valores en Hex: ");
  for (int i = 0; i < sizeof(buffer); i++) {
    display.print("0x");
    if (buffer[i] < 0x10) {
      display.print("0");
    }
    display.print(buffer[i], HEX);
    display.print(" ");
  }
  display.display();
 
}

void onSleep()
{
  Serial.println("Sleep");
  delay(5000); // "kind of a sleep"
  Serial.println("Send Message");
  LoRaNow.print("Pacote: ");
  LoRaMessage = String(readingID)+" " + String(BME680temperatura)+"°C "+String(BME680umidade)+"% "+String(BME680pressao)+"hPA ";

  LoRaNow.print(LoRaMessage);
  LoRaNow.send();
}

void inits(){
    //inicializar o display OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true);
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("ID:");
  
  
  if (!LoRaNow.begin()) {
    Serial.println("LoRa init failed. Check your connections.");
    while (true);
  }
  Serial.println("o sistema lora deveria ter iniciado com sucesso");
  display.setCursor(30, 0);
  id = LoRaNow.id();
  display.print(id,HEX);

  LoRaNow.onMessage(onMessage);
  LoRaNow.onSleep(onSleep);
  LoRaNow.showStatus(Serial);

  if (!bme.begin()) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    display.setCursor(0, 40);
    display.print("conectar bien sensorBME680!");
    display.display();
    //while (1);
  }
  
  display.setCursor(0, 10);
  display.print("OLED LoRa BME680 ok!");
  display.display();

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
  display.print("Node ID:");
  display.setCursor(50, 0);
  display.print(id,HEX);

  
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
}


void setup() {
  Serial.begin(115200);
  inits();// put your setup code here, to run once:
  
}

void loop() {
  obtenerBMEleituras();
  exibirleituras();
  //escuchar nodo vecino
  //forrawr reeenviar mensjae de vencino
  //sleep
  LoRaNow.loop();// 
}