#include <SPI.h>//biblioteca SPI para a comunicação entre dispositivos
#include <LoRa.h>//biblioteca LoRa
#include <WiFi.h>//biblioteca para se conectar ao wifi
#include <PubSubClient.h>//biblioteca para publicação
#include <Wire.h>// biblioteca Wire,usada para a comunicação I2C
#include <Adafruit_SSD1306.h>//biblioteca para controlar displays OLED

#define SCREEN_WIDTH 128//indica o tamanho da largura do display em pixels
#define SCREEN_HEIGHT 64//indica a altura do display em pixels
// inicialização e comunicação I2C do display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
//Configuração dos pinos para  comunicação LoRa
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26
//define a banda de frequência do módulo LoRa
#define BAND 866E6

const char* id = "lora1";// ID do cliente MQTT
const char* pswd = "123";//senha para autenticação no servidor MQTT

//Tópicos para publicação
const char* topic = "dev/test";
const char* topic2 = "dev/test1";
const char* topic3 = "dev/test2";
const char* topic4 = "dev/test3";
const char* topic5 = "dev/test4";
//estabelecer a conexão Wi-Fi com a rede
WiFiClient espClient;
//conectar ao servidor MQTT
PubSubClient client(espClient);
//variável de armazena mento dos dados recebidos via comunicação LoRa
String LoRaData;

const char* ssid = "Estudio 1";//nome da rede
const char* password = "test1234";//senha da rede
const char* mqtt_server = "192.168.1.197";//endereço IP do servidor

void setup() {
  //Inicialização do display
  Serial.begin(115200);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true);
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("LoRa Receiver");
  display.display();
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(BAND)) {
    while (1);
  }
  display.setCursor(0, 10);
  display.print("LoRa Initializing OK!");
  display.display();
  delay(2000);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  display.print("Device connected");
  display.setCursor(50, 30);
  display.print("to WiFi");
  display.setCursor(20, 40);
  display.print(WiFi.localIP());
  display.setCursor(30, 50);
  display.display();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
// Verificação e reestabelecimento da conexão MQTT, se necessário.
  if (!client.connected()) {
    reconnect();
  }
  int packetSize = LoRa.parsePacket();
  // Verificação da chegada de pacotes LoRa
  if (packetSize) {
    while (LoRa.available()) {
 // Processamento de pacotes LoRa e envio para MQTT
      RecepLoRaSendMqtt();
    }
    oled();
  }
}
// // Tratamento das mensagens MQTT recebidas
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
// Tentativa de reconexão ao servidor MQTT
void reconnect() {
  while (!client.connected()) {
    String clientId = "Lora1";
    boolean cleanSession = true;
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(), id, pswd)) {
      client.subscribe(topic);
      client.subscribe(topic2);
      client.subscribe(topic3);
      client.subscribe(topic4);
      client.subscribe(topic5);
      client.publish(topic, "test connection");
      break;
    } else {
      delay(3000);
    }
  }
}

void oled() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Receiver");
  display.setCursor(20, 20);
  display.print("Received message: ");
  display.setCursor(0, 30);
  display.print(LoRaData);
  display.setCursor(20, 40);
  display.print("Sending via MQTT");
  display.display();
}
//Recebimento de dados LoRa e envio para MQTT
void RecepLoRaSendMqtt() {
  LoRaData = LoRa.readString();
  client.loop();
  int pos1 = LoRaData.indexOf('/');
  String stringOne = LoRaData.substring(0, pos1);
  char Buffer[50];
  stringOne.toCharArray(Buffer, 50);
  client.publish(topic, Buffer);
  int pos2 = LoRaData.indexOf('/', pos1 + 1);
  String dato2 = LoRaData.substring(pos1 + 1, pos2);
  char buuf[50];
  dato2.toCharArray(buuf, 50);
  client.publish(topic2, buuf);

  int pos3 = LoRaData.indexOf('&', pos2 + 1);
  String dato3 = LoRaData.substring(pos2 + 1, pos3);
  char buuuf[50];
  dato3.toCharArray(buuuf, 50);
  client.publish(topic3, buuuf);

  int pos4 = LoRaData.indexOf('$', pos3 + 1);
  String dato4 = LoRaData.substring(pos3 + 1, pos4);
  char buuuuf[50];
  dato4.toCharArray(buuuuf, 50);
  client.publish(topic4, buuuuf);
}

