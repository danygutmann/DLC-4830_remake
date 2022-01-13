#include <ESP8266WiFi.h>

int program = 1;
int currStep = 0;
byte currOut;

bool ReverseActive = false;
int ReverseCount = 2;
int ReverseCountCurr = 0;

bool InvertActive = false;
int InvertCount = 4;
int InvertCountCurr = 0;

unsigned long nextStep;
unsigned long delaytime = 1000;

static const byte someData[][8] = {
{B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000},
{B10000000, B01000000, B00100000, B00010000, B00001000, B00000100, B00000010, B00000001},
{B10000001, B11000000, B01100000, B00110000, B00011000, B00001100, B00000110, B00000011},
{B10000011, B11000001, B11100000, B01110000, B00111000, B00011100, B00001110, B00000111},
{B10000111, B11000011, B11100001, B11110000, B01111000, B00111100, B00011110, B00001111},
{B01000001, B10000010, B00000101, B00001010, B00010100, B00101000, B01010000, B10100000},
{B01010001, B10100010, B01000101, B10001010, B00010101, B00101010, B01010100, B10101000, }
{B00011111, B00111110, B01111100, B11111000, B11110001, B11100011, B11000111, B10001111, }
{B00111111, B01111110, B11111100, B11111001, B11110011, B11100111, B11001111, B10011111, }
{B10011001, B00110011, B01100110, B11001100, B10011001, B00110011, B01100110, B11001100, }
{B00010001, B00100010, B01000100, B10001000, B00010001, B00100010, B01000100, B10001000, }


};


void setup() {
  Serial.begin(115200);
  nextStep = millis() + delaytime;
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
  }
  byte b = someData[program][currStep];
  if (ReverseActive) b = flipByte(b);
  if (InvertActive) b = ~b;
  
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
}
byte flipByte(byte c){
   c = ((c>>1)&0x55)|((c<<1)&0xAA);
   c = ((c>>2)&0x33)|((c<<2)&0xCC);
   c = (c>>4) | (c<<4) ;
   return c;
 }

void loop(){
  printByte(GetNextStep());
  delay(100);
}
