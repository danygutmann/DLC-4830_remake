#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Adafruit_NeoPixel.h>

WiFiClient wifiClient;

constexpr uint16_t PORT = 8266; 
char packetBuffer[255];

byte colorAssigment[][3] = {
{255, 0, 0},
{0, 255, 0},
{0, 0, 255},
{255, 255, 0},
{255, 255, 0},
{0, 0, 255},
{0, 255, 0},
{255, 0, 0},
};

WiFiUDP Udp; 
Adafruit_NeoPixel pixels(8, 5, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  
  pixels.begin();
  
  //WiFi.mode(WIFI_STA);
  //WiFi.disconnect();
  
  delay(100);
  Udp.begin(PORT);

  printByte(0);
}


void printByte(byte b){ 
  pixels.clear();
  for (int i = 7; i >= 0; i-- )
  {
    if (bitRead(b, i) == 1)
    {
      pixels.setPixelColor(i, pixels.Color(colorAssigment[i][0], colorAssigment[i][1], colorAssigment[i][2]));
    } else {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }
  }
  pixels.show();
}

void CheckUdp(){
  uint16_t packetSize = Udp.parsePacket();
  if (packetSize) {                                            
    Udp.read(packetBuffer, sizeof(packetBuffer)); 
    packetBuffer[packetSize] = 0;  

//    Serial.println("Puffer: " + String(packetBuffer));
//
//    float tempC = atof(packetBuffer);
//    Serial.println("float: " + String(packetBuffer)); 

    byte tempB = atof(packetBuffer);
//    Serial.println("byte: " + String(tempB));
//    
//   //byte theByte = int(packetBuffer[1]);
 
    printByte(tempB);
    }
  }

void loop() 
{ 
  if (WiFi.status() != WL_CONNECTED)
  {
    WiFi.begin("K2-NET", "Dikt81mp!");
    while (WiFi.status() != WL_CONNECTED) {
      delay(200);
      Serial.print(".");
      }
  Serial.println("Verbunden!");
  }else{
    CheckUdp();
  }
}
