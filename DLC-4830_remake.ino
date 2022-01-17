#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>
#include "data.h"
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

Adafruit_NeoPixel pixels(8, 5, NEO_GRB + NEO_KHZ800);

int program = 1;
int currStep = 0;
int currSpeed = 1;
byte currOut;

bool ReverseActive = false;
int ReverseCount = 0;
int ReverseCountCurr = 0;

bool InvertActive = false;
int InvertCount = 0;
int InvertCountCurr = 0;

int AutoProgCount = 0;
int AutoProgCountCurr = 0;

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

  WiFi.softAP("DLC-4830", "");
  IPAddress myIP = WiFi.softAPIP();
  delay(500);

  httpUpdater.setup(&server); 
  server.on("/", handleRoot);
  server.onNotFound(HandleHttpData);
  server.begin();
}

byte GetNextStep(){
  currStep++;
  if (currStep == 8 ) 
  {
    currStep = 0;
    if (ReverseCount > 0){
      ReverseCountCurr++;
      if (ReverseCountCurr == ReverseCount)
      {
        ReverseCountCurr = 0;
        ReverseActive = !ReverseActive;
      }
    }
    if (InvertCount > 0){
      InvertCountCurr++;
      if (InvertCountCurr == InvertCount)
      {
        InvertCountCurr = 0;
        InvertActive = !InvertActive;
      }
    }
    if (AutoProgCount > 0){
      AutoProgCountCurr++;
      if (AutoProgCountCurr == AutoProgCount)
      {
        AutoProgCountCurr = 0;
        program++;
        if (program == 51) program = 1;
      }
    }

    
  }
  byte b = someData[program][currStep];
  if (ReverseActive) b = flipByte(b);
  if (InvertActive) b = ~b;
  
  return b;
}
void printByte(byte b){ 
  
//  for (int i = 7; i >= 0; i-- )
//  {
//    if (bitRead(b, i) == 1)
//    {
//      Serial.print ("█");
//    } else {
//      Serial.print ("░"); 
//    }
//  }
//  Serial.println();

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
byte flipByte(byte c){
   c = ((c>>1)&0x55)|((c<<1)&0xAA);
   c = ((c>>2)&0x33)|((c<<2)&0xCC);
   c = (c>>4) | (c<<4) ;
   return c;
 }

void SpeedPlus(){
  currSpeed++;
  }
void SpeedMinus(){
  currSpeed--;
  }
  
void ProgNext(){
  program++;
  }
void ProgLast(){
  program--;
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
  ReverseActive = false;;
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
  printByte(GetNextStep());
  delay(50*currSpeed);
}

void handleRoot() {
  server.send(200, "text/html", "Hallo vom DLC-4830");
  }
void HandleHttpData() {
  String erg = server.uri() + "<br>";
  String Cmd = server.arg("CMD");
  String Data = server.arg("DATA");

  if (Cmd=="ProgNext") ProgNext();
  if (Cmd=="ProgLast") ProgLast();
  if (Cmd=="DirToggle") DirToggle();
  if (Cmd=="DirForward") DirForward();
  if (Cmd=="DirReverse") DirReverse();
  if (Cmd=="InvertToggle") InvertToggle();
  if (Cmd=="InvertOn") InvertOn();
  if (Cmd=="InvertOff") InvertOff();
  if (Cmd=="SpeedPlus") SpeedPlus();
  if (Cmd=="SpeedMinus") SpeedMinus();
  
  server.send(200, "text/html", erg);
  }
