
#define RH_MESH_MAX_MESSAGE_LEN 50

#include <RHMesh.h>
#include <RH_RF95.h>
#include <SPI.h>
#include <Wire.h>// biblioteca Wire,usada para a comunicação I2C
//OLED 
#include <Adafruit_SSD1306.h>//biblioteca para controlar displays OLED
#define SCREEN_HEIGHT 64//indica a altura do display em pixels
#define SCREEN_WIDTH 128//indica o tamanho da largura do display em pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);// inicialização e comunicação I2C do display instace
//Sensor BME680 needs
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>
#include <bme68x.h>
#include <bme68x_defs.h>
Adafruit_BME680 bme; // instância do sensor BME
float BME680temperatura,BME680pressao,BME680umidade;//declaração das variáveis que serão usadas para armazenar os dados dos sensores
//All sensor needs
// In this small artifical network of 4 nodes,
#define BRIDGE_ADDRESS 1  // address of the bridge ( we send our data to, hopefully the bridge knows what to do with our data )
#define NODE_ADDRESS 3    // address of this node

// lilygo T3 v2.1.6
// lora SX1276/8
#define LLG_SCK 5
#define LLG_MISO 19
#define LLG_MOSI 27
#define LLG_CS  18
#define LLG_RST 23
#define LLG_DI0 26
#define LLG_DI1 33
#define LLG_DI2 32

#define LLG_LED_GRN 25

// oled
#define LLG_OLED_SDA 21
#define LLG_OLED_SCL 22

// tfcard
#define LLG_SD_CS   13
#define LLG_SD_MISO 2
#define LLG_SD_MOSI 15
#define LLG_SD_SCK  14

#define TXINTERVAL 3000  // delay between successive transmissions
unsigned long nextTxTime;

// Singleton instance of the radio driver
RH_RF95 rf95(LLG_CS, LLG_DI0); // slave select pin and interrupt pin, [heltec|ttgo] ESP32 Lora OLED with sx1276/8

// Class to manage message delivery and receipt, using the driver declared above
RHMesh manager(rf95, NODE_ADDRESS);

void obtenerBMEleituras(){
  BME680temperatura=bme.readTemperature();
  BME680pressao=bme.readPressure()/100.0F;
  BME680umidade=bme.readHumidity();
}

void setup() 
{
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true);
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("LoRa Transmitter");
  display.display();

  if (!bme.begin()) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    //while (1);
  }
  
  Serial.begin(115200);
  Serial.print(F("initializing node "));
  Serial.print(NODE_ADDRESS);
  SPI.begin(LLG_SCK,LLG_MISO,LLG_MOSI,LLG_CS);
  if (!manager.init())
    {Serial.println(" init failed");} 
  else
    {Serial.println(" done");}  // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36 

  rf95.setTxPower(10, false); // with false output is on PA_BOOST, power from 2 to 20 dBm, use this setting for high power demos/real usage
  //rf95.setTxPower(1, true); // true output is on RFO, power from 0 to 15 dBm, use this setting for low power demos ( does not work on lilygo lora32 )
  rf95.setFrequency(868.0);
  rf95.setCADTimeout(500);

  // long range configuration requires for on-air time
  boolean longRange = false;
  if (longRange) 
    {
    // custom configuration
    RH_RF95::ModemConfig modem_config = {
      0x78, // Reg 0x1D: BW=125kHz, Coding=4/8, Header=explicit
      0xC4, // Reg 0x1E: Spread=4096chips/symbol, CRC=enable
      0x08  // Reg 0x26: LowDataRate=On, Agc=Off.  0x0C is LowDataRate=ON, ACG=ON
      };
    rf95.setModemRegisters(&modem_config);
    }
  else
    {
    // Predefined configurations( bandwidth, coding rate, spread factor ):
    // Bw125Cr45Sf128     Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on. Default medium range
    // Bw500Cr45Sf128     Bw = 500 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on. Fast+short range
    // Bw31_25Cr48Sf512   Bw = 31.25 kHz, Cr = 4/8, Sf = 512chips/symbol, CRC on. Slow+long range
    // Bw125Cr48Sf4096    Bw = 125 kHz, Cr = 4/8, Sf = 4096chips/symbol, low data rate, CRC on. Slow+long range
    // Bw125Cr45Sf2048    Bw = 125 kHz, Cr = 4/5, Sf = 2048chips/symbol, CRC on. Slow+long range
    if (!rf95.setModemConfig(RH_RF95::Bw125Cr45Sf128))
      {Serial.println(F("set config failed"));}
    }
  Serial.println("RF95 ready");
  nextTxTime = millis();
}


int8_t data[sizeof(float)];
// Dont put this on the stack:
uint8_t buf[RH_MESH_MAX_MESSAGE_LEN];
uint8_t res;

void loop()
{
  // send message every TXINTERVAL millisecs
  if (millis() > nextTxTime)
    {
    nextTxTime += TXINTERVAL;
    Serial.print("Sending to bridge n.");
    Serial.print(BRIDGE_ADDRESS);
    Serial.print(" res=");
    
    // Send a message to a rf95_mesh_server
    // A route to the destination will be automatically discovered.
    obtenerBMEleituras();
    memcpy((uint8_t*)data, &BME680temperatura, sizeof(float));
    //LoRaMessage =String(BME680temperatura)+"°C "+String(BME680umidade)+"% "+String(BME680pressao)+"hPA ";
    res = manager.sendtoWait((uint8_t *)(&data), sizeof(data), BRIDGE_ADDRESS);
    Serial.println(res);

    // Atualiza o display OLED com a mensagem enviada
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Sending to bridge ");
    display.println(BRIDGE_ADDRESS);
    display.print("Result: ");
    display.println(res == RH_ROUTER_ERROR_NONE ? "Success" : "Failed");
    display.display();

    if (res == RH_ROUTER_ERROR_NONE)
      {
      // Data has been reliably delivered to the next node.
      // now we do... 
      }
    else
      {
      // Data not delivered to the next node.
      Serial.println("sendtoWait failed. Are the bridge/intermediate mesh nodes running?");
      }
    }
  
  // radio needs to stay always in receive mode ( to process/forward messages )
  uint8_t len = sizeof(buf);
  uint8_t from;
  if (manager.recvfromAck(buf, &len, &from))
    {
    Serial.print("message from node n.");
    Serial.print(from);
    Serial.print(": ");
    Serial.print((char*)buf);
    Serial.print(" rssi: ");
    Serial.println(rf95.lastRssi()); 

    // Atualiza o display OLED com a mensagem recebida
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Message from ");
    display.println(from);
    display.print((char*)buf);
    display.print(" RSSI: ");
    display.println(rf95.lastRssi());
    display.display();
    }
}
