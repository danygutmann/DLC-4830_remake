#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>
#include "data.h"
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WiFiUdp.h>

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;
Adafruit_NeoPixel pixels(8, 5, NEO_GRB + NEO_KHZ800);

WiFiUDP Udp;
IPAddress broadcastIP(255, 255, 255, 255);
constexpr uint16_t PORT = 8266;
char packetBuffer[255];   

int program = 1;
int currStep = 0;
int currSpeed = 1;
int speedDelay = 20;
unsigned long nextStepAt = millis() + 50; 
byte currOut;

bool ReverseActive = false;
bool ReverseAutoActive = false;
bool ReverseBounceActive = false;
int ReverseCount = 0;
int ReverseCountCurr = 0;

bool InvertActive = false;
bool InvertAutoActive = false;
int InvertCount = 0;
int InvertCountCurr = 0;

bool AutoProgActive = false;
unsigned long AutoProgChange = millis();

unsigned long nextStep;
unsigned long delaytime = 1000;

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

void setup() {
  Serial.begin(115200);
  nextStep = millis() + delaytime;
  pixels.begin();

  //WiFi.softAP("DLC-4830", "");
  //IPAddress myIP = WiFi.softAPIP();
  //delay(500);

  WiFi.begin("K2-NET", "Dikt81mp!");
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("Verbunden!");
  Serial.println(WiFi.localIP());


  httpUpdater.setup(&server); 
  server.on("/", handleRoot);
  server.onNotFound(HandleHttpData);
  server.begin();

  
}

String GerJsonResponse() {
  String result;
  StaticJsonDocument<200> doc;
  doc["dir"]["state"] = ReverseActive;
  doc["dir"]["auto"] = ReverseCount;
  doc["inv"]["state"] = InvertActive;
  doc["inv"]["auto"] = InvertCount;
  doc["prog"]["actual"] = program;
  doc["Speed"] = currSpeed;
  
  serializeJson(doc, result);
  return result;
}

byte GetNextStep(){
  currStep++;
  if (currStep == 8 ) 
  {
    if (AutoProgActive) {
      if (millis() >= AutoProgChange) {
        AutoProgChange = millis()+15000;
        program++;
        if (program == 101) program = 1;
      }
    }

    currStep = 0;

    if (ReverseAutoActive){
      ReverseCountCurr++;
      if (ReverseCountCurr == ReverseCount)
      {
        ReverseCountCurr = 0;
        ReverseActive = !ReverseActive;
      }
    }

    if (ReverseBounceActive) {

    }

    if (InvertAutoActive){
      InvertCountCurr++;
      if (InvertCountCurr == InvertCount)
      {
        InvertCountCurr = 0;
        InvertActive = !InvertActive;
      }
    }

  }

  byte b = someData[program][currStep];
  if (ReverseActive) b = flipByte(b);
  if (InvertActive) b = ~b;

  //Udp.begin(PORT);
  //Udp.beginPacket(broadcastIP, PORT);
  //Udp.print(b);
  //Udp.endPacket();

  Serial.println(String(b));
  
  return b;
}
void printByte(byte b){ 
  
 for (int i = 7; i >= 0; i-- )
 {
   if (bitRead(b, i) == 1)
   {
     Serial.print ("█");
   } else {
     Serial.print ("░"); 
   }
 }
 Serial.println();

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

  // next step
 if (currSpeed > 0 ) {
  nextStepAt =  millis() + speedDelay;
 }

}
byte flipByte(byte c){
   c = ((c>>1)&0x55)|((c<<1)&0xAA);
   c = ((c>>2)&0x33)|((c<<2)&0xCC);
   c = (c>>4) | (c<<4) ;
   return c;
 }

void SpeedSet(int speed){
  currSpeed = speed;
  if (currSpeed == 1 ) speedDelay = 20;
  if (currSpeed == 2 ) speedDelay = 40;
  if (currSpeed == 3 ) speedDelay = 80;
  if (currSpeed == 4 ) speedDelay = 120;
  if (currSpeed == 5 ) speedDelay = 160;
  if (currSpeed == 6 ) speedDelay = 200;
  if (currSpeed == 7 ) speedDelay = 280;
  if (currSpeed == 8 ) speedDelay = 380;
  if (currSpeed == 9 ) speedDelay = 500;
  }
void SpeedPlus(){
  SpeedSet(currSpeed +1);
  }
void SpeedMinus(){
  SpeedSet(currSpeed -1);
  }
  
void ProgNext(){
  program++;
  }
void ProgLast(){
  program--;
  }
void ProgAutoOn(){
  AutoProgChange = millis()+15000;
  AutoProgActive = true;
  }
void ProgAutoOff(){
  AutoProgActive = false;
  }
void ProgAutoToggle() {
    if (AutoProgActive == false) {
      AutoProgActive = true;
    }else{
      AutoProgActive = false;
    }
  }
void ProgSet(int prog){
  program = prog;
  }

void DirToggle(){
  ReverseActive = !ReverseActive;
  }
void DirForward(){
  ReverseActive = false;;
  }
void DirReverse(){
  ReverseActive = true;;
  }
void DirAutoSteps(int steps){
  ReverseCount = steps;
  }

void InvertToggle(){
  InvertActive = !InvertActive;
  }
void InvertOn(){
  InvertActive = true;;
  }
void InvertOff(){
  InvertActive = false;
  }
void InvertAutoSteps(int steps){
  InvertCount = steps;
  }
  


void loop(){
  server.handleClient();
  if (currSpeed > 0 ) {
    if (millis() >= nextStepAt) printByte(GetNextStep());
  } 
}

void handleRoot() {

  String Cmd = server.arg("CMD");
  String Data = server.arg("DATA");
  HandleCmd(Cmd, Data);

  String out = "<b>DLC-4830-V2</b><hr>";

  out += "<table border='0'>";

  out += "<tr><td><b>SPEED</b> (" + String(currSpeed)  +"/" + String(speedDelay)+ ")</td>";
  out += "<td><a href='?CMD=SpeedPlus'>langsamer</a></td>";
  out += "<td><a href='?CMD=SpeedMinus'>schneller</a></td>";
  out += "<td><a href='?CMD=aStep'>ein Schritt</a></td>";
  out += "</tr>";

  out += "<tr><td><b>PROGRAM</b> (" + String(program) + ")</td>";
  out += "<td><a href='?CMD=ProgLast'>zurueck</a></td>";
  out += "<td><a href='?CMD=ProgNext'>vor</a></td>";
  out += "<td><a href='?CMD=ProgAuto'>Auto ()</a></td>";
  out += "</tr>";

  out += "<tr><td><b>RICHTUNG</b> (" + String(ReverseActive) + ")</td>";
  out += "<td><a href='?CMD=DirForward'>nach rechts</a></td>";
  out += "<td><a href='?CMD=DirReverse'>nach links</a></td>";
  out += "<td><a href='?CMD=DirToggle'>toggle</a></td>";
  out += "</tr>";

  out += "<tr><td><b>INVERT</b> (" + String(InvertActive) + ")</td>";
  out += "<td><a href='?CMD=InvertOff'>normal</a></td>";
  out += "<td><a href='?CMD=InvertOn'>invert</a></td>";
  out += "<td><a href='?CMD=InvertToggle'>toggle</a></td>";
  out += "</tr>";

  out += "</table><br><br><br>";


  out += GerJsonResponse();
  server.send(200, "text/html", out);
  }



void HandleHttpData() {
  String Cmd = server.arg("CMD");
  String Data = server.arg("DATA");
  HandleCmd(Cmd, Data);
  server.send(200, "application/json", GerJsonResponse());
  }

  String HandleCmd(String Cmd, String Data) {
    String erg = server.uri() + "<br>";
    if (Cmd=="ProgNext") ProgNext();
    if (Cmd=="ProgLast") ProgLast();
    if (Cmd=="ProgAuto") ProgAutoToggle();
    if (Cmd=="DirToggle") DirToggle();
    if (Cmd=="DirForward") DirForward();
    if (Cmd=="DirReverse") DirReverse();
    if (Cmd=="InvertToggle") InvertToggle();
    if (Cmd=="InvertOn") InvertOn();
    if (Cmd=="InvertOff") InvertOff();
    if (Cmd=="SpeedPlus") SpeedPlus();
    if (Cmd=="SpeedMinus") SpeedMinus();
    if (Cmd=="aStep") printByte(GetNextStep());
    return Cmd;
  }

