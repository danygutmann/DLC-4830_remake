#include <ESP8266WiFi.h>

int program = 1;
int curLooop = 0;
int currStep = 0;

bool ReverseActive = false;
int ReverseCount = 2;
int ReverseCountCurr = 0;

bool InvertActive = false;
int InvertCount = 0;
int InvertCountCurr = 0;

unsigned long nextStep;
unsigned long delaytime = 1000;

static const byte someData[][8] = {
{B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000},
{B10000000, B01000000, B00100000, B00010000, B00001000, B00000100, B00000010, B00000001},
{B10000001, B11000000, B01100000, B00110000, B00011000, B00001100, B00000110, B00000011},
{B10000011, B11000001, B11100000, B01110000, B00111000, B00011100, B00001110, B00000111},
{B10000111, B11000011, B11100001, B11110000, B01111000, B00111100, B00011110, B00001111}
};


void setup() {
  Serial.begin(115200);
  nextStep = millis() + delaytime;

}

void loop(){
  waitForNextStep();
  printStep(someData[program][currStep]);

}

void waitForNextStep()
{
//  do {
//   delay(10);
//  } while (millis() >= nextStep);
//  nextStep = millis() + delaytime ;
  delay(500);
  currStep++;
  if (currStep == 8 ) 
  {
    currStep = 0;
    curLooop++;

    if (ReverseCount > 0){
      ReverseCountCurr++;
      if (ReverseCountCurr == ReverseCount)
      {
        ReverseCountCurr = 0;
        ReverseActive = !ReverseActive;
      }
    }


    
    if (curLooop==10) curLooop = 1;
  }
  return;
}

void printStep(byte b){ 
  int currBit = 0;
  for (int i = 7; i >= 0; i-- )
  {
    currBit = i;
    if (ReverseActive) currBit = 7-i; 
    if (bitRead(b, currBit) == 1)
    {
      if (InvertActive) Serial.print ("_");
      if (!InvertActive) Serial.print ("X");
      Serial.print ("X");
    } else {
      if (InvertActive) Serial.print ("X");
      if (!InvertActive) Serial.print ("_");
    }
  }
  Serial.println();
}
void printBinary(byte b) {
  for (int i = 7; i >= 0; i-- )
  {
    Serial.print((b >> i) & 0X01);//shift and select first bit
  }
  Serial.println();
}
