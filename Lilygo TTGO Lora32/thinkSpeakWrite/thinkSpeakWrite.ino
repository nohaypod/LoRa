

// Mesh has much greater memory requirements, and you may need to limit the
// max message length to prevent wierd crashes
#define RH_MESH_MAX_MESSAGE_LEN 50

// OLED
#define LLG_OLED_SDA 21
#define LLG_OLED_SCL 22
#include <Adafruit_SSD1306.h>//biblioteca para controlar displays OLED
#define SCREEN_HEIGHT 64//indica a altura do display em pixels
#define SCREEN_WIDTH 128//indica o tamanho da largura do display em pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);// inicialização e comunicação I2C do display instace
//OLED 

#include <RHMesh.h>
#include <RH_RF95.h>
#include <SPI.h>

//Network
#include <WiFi.h>
#include <ThingSpeak.h>

const char *ssid = "Estudio 1";     // Change this to your WiFi SSID
const char *password = "test1234";  // Change this to your WiFi password


const char *host = "api.thingspeak.com";        // This should not be changed
const int httpPort = 80;                        // This should not be changed
const String channelID = "2602952";             // Change this to your channel ID
const String writeApiKey = "WXALS2O5H456CDR7";  // Change this to your Write API key
const String readApiKey = "SLUV598MJMSU6H5Q";   // Change this to your Read API key
WiFiClient  client;
int field1=0;
int field2=0;
int field3=0;
int numberOfResults = 3;  // Number of results to be read
int fieldNumber = 1;
// In this small artifical network of 4 nodes,
#define BRIDGE_ADDRESS 1  // address of the bridge ( we send our data to, hopefully the bridge knows what to do with our data )

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

#define RXTIMEOUT 3000  // it is roughly the delay between successive transmissions

// Singleton instance of the radio driver
RH_RF95 rf95(LLG_CS, LLG_DI0); // slave select pin and interrupt pin, [heltec|ttgo] ESP32 Lora OLED with sx1276/8

// Class to manage message delivery and receipt, using the driver declared above
RHMesh manager(rf95, BRIDGE_ADDRESS);

void setup() 
{
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true);
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("LoRa Intermediate");
  display.display();

  ThingSpeak.begin(client);  // Initialize ThingSpeak

  Serial.begin(115200);
  Serial.print(F("initializing Server "));
  Serial.print("Connecting to ");
  Serial.println(ssid);


  WiFi.begin(ssid, password);


  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print(BRIDGE_ADDRESS);
  SPI.begin(LLG_SCK,LLG_MISO,LLG_MOSI,LLG_CS);
  if (!manager.init())
    {Serial.println(" init failed");} 
  else
    {Serial.println(" done");}  // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36 

  rf95.setTxPower(2, false); // with false output is on PA_BOOST, power from 2 to 20 dBm, use this setting for high power demos/real usage
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
}

uint8_t data[] = "Olá, de volta da ponte";
// Dont put this on the stack:
uint8_t buf[RH_MESH_MAX_MESSAGE_LEN];
uint8_t res;

void loop()
{
  uint8_t len = sizeof(buf);
  uint8_t from;
  if (manager.recvfromAck(buf, &len, &from))
    {
    Serial.print("request from node n.");
    Serial.print(from);
    Serial.print(": ");
    Serial.print((char*)buf);
    Serial.print(" rssi: ");
    Serial.println(rf95.lastRssi()); 
     char str[sizeof(buf)];
  memcpy(str, buf, sizeof(buf));

  // Variables para almacenar los valores
  float temp, press, hum, luz, carbon, bateria, rssi;

  // Usar sscanf para extraer los valores
  sscanf(str, 
         "Temp: %f, Press: %f, Hum: %f, Luz: %f, Carbon: %f, Bateria: %f, rssi: %f", 
         &temp, &press, &hum, &luz, &carbon, &bateria, &rssi);

  // Mostrar los valores extraídos
  Serial.print("Temperatura: ");
  Serial.println(temp);
  Serial.print("Presion: ");
  Serial.println(press);
  Serial.print("Humedad: ");
  Serial.println(hum);
  Serial.print("Luz: ");
  Serial.println(luz);
  Serial.print("Carbono: ");
  Serial.println(carbon);
  Serial.print("Bateria: ");
  Serial.println(bateria);
  Serial.print("RSSI: ");
  Serial.println(rssi);

   ThingSpeak.writeField( 2602952, 1, temp, "SLUV598MJMSU6H5Q" ); // Write the data to the channel
    //delay(1000);
  
    // Send a reply back to the originator client
    res = manager.sendtoWait(data, sizeof(data), from);
    if ( res != RH_ROUTER_ERROR_NONE)
      {
      Serial.print("sendtoWait failed:");
      Serial.println(res);
      }

    display.setCursor(0, 10);
    display.print("LLegó un mensaje ALV");
    display.display();
    display.clearDisplay();
    }
  
    

}