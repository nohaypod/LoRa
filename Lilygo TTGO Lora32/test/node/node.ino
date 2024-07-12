/*
  LoRaNow Node OLED OK
  This code sends message and listen expecting some valid message from someone like node or gateway 
  
*/
//OLED 
#include <Adafruit_SSD1306.h>//biblioteca para controlar displays OLED
#define SCREEN_HEIGHT 64//indica a altura do display em pixels
#define SCREEN_WIDTH 128//indica o tamanho da largura do display em pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);// inicialização e comunicação I2C do display instace

#include <LoRaNow.h> //Luiz H. Cassettari Ricaun based in LoRa.h
String LoRaMessage = "";
//Sensor BME680 needs
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>
#include <bme68x.h>
#include <bme68x_defs.h>
Adafruit_BME680 bme; // instância do sensor BME
float BME680temperatura,BME680pressao,BME680umidade;//declaração das variáveis que serão usadas para armazenar os dados dos sensores
//All sensor needs

void setup() {
  Serial.begin(115200);
  Serial.println("Inicianmos...");

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

  if (!bme.begin()) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    display.setCursor(0, 40);
    display.print("Conectar bien sensorBME680!");
    display.display();
    //while (1);
  }

  LoRaNow.onMessage(onMessage);
  LoRaNow.onSleep(onSleep);
  LoRaNow.showStatus(Serial);
}

void obtenerBMEleituras(){
  BME680temperatura=bme.readTemperature();
  BME680pressao=bme.readPressure()/100.0F;
  BME680umidade=bme.readHumidity();
}

void loop() {
  LoRaNow.loop();
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
  LoRaNow.print("LoRanode responds thanks node "+ String(id) +" "+ String(count));
  LoRaNow.send();

  display.setCursor(70, 0);
  display.print("!");
  display.print(LoRaNow.id(),HEX);
  display.display();
}

void onSleep()
{
  obtenerBMEleituras();
  LoRaMessage = String(BME680temperatura)+"°C "+String(BME680umidade)+"% "+String(BME680pressao)+"hPA ";
  Serial.print("Send Message from ");
  Serial.print(LoRaNow.id(),HEX);
  LoRaNow.print(LoRaMessage);
  LoRaNow.send();
  Serial.println("...");
  delay(2000); // "Delay"
}