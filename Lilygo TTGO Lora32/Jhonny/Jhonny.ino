// Mesh has much greater memory requirements, and you may need to limit the
// max message length to prevent wierd crashes
#define RH_MESH_MAX_MESSAGE_LEN 50

#include <RHMesh.h>
#include <RH_RF95.h>
#include <SPI.h>

//Network
#include <WiFi.h>

const char *ssid = "Estudio 1";     // Change this to your WiFi SSID
const char *password = "test1234";  // Change this to your WiFi password


const char *host = "api.thingspeak.com";        // This should not be changed
const int httpPort = 80;                        // This should not be changed
const String channelID = "2602952";             // Change this to your channel ID
const String writeApiKey = "WXALS2O5H456CDR7";  // Change this to your Write API key
const String readApiKey = "SLUV598MJMSU6H5Q";   // Change this to your Read API key
int field1,field2;
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

void readResponse(NetworkClient *client) {
  unsigned long timeout = millis();
  while (client->available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client->stop();
      return;
    }
  }


  // Read all the lines of the reply from server and print them to Serial
  while (client->available()) {
    String line = client->readStringUntil('\r');
    Serial.print(line);
  }


  Serial.printf("\nClosing connection\n\n");
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
    field1=
    Serial.print(" rssi: ");
    Serial.println(rf95.lastRssi()); 

    // Send a reply back to the originator client
    res = manager.sendtoWait(data, sizeof(data), from);
    if ( res != RH_ROUTER_ERROR_NONE)
      {
      Serial.print("sendtoWait failed:");
      Serial.println(res);
      }
    }

    NetworkClient client;
  String footer = String(" HTTP/1.1\r\n") + "Host: " + String(host) + "\r\n" + "Connection: close\r\n\r\n";


  // WRITE --------------------------------------------------------------------------------------------
  if (!client.connect(host, httpPort)) {
    return;
  }


  //client.print("GET /update?api_key=" + writeApiKey + "&field1=" + field1 + footer);
  client.print("GET /update?api_key=" + writeApiKey + "&field1=" + field1 + "&field2=" + field2 + footer);
  readResponse(&client);


  // READ --------------------------------------------------------------------------------------------


  String readRequest = "GET /channels/" + channelID + "/fields/" + fieldNumber + ".json?results=" + numberOfResults + " HTTP/1.1\r\n" + "Host: " + host + "\r\n"
                       + "Connection: close\r\n\r\n";


  if (!client.connect(host, httpPort)) {
    return;
  }


  client.print(readRequest);
  readResponse(&client);


  // -------------------------------------------------------------------------------------------------


  ++field1;
  ++field2;
  delay(5000);

}