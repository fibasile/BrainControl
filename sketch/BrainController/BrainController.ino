#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
#include <SoftwareSerial.h>
//#include <Brain.h>
#include "Mindwave.h"


#define PIXEL_PIN 12
#define PIXEL_COUNT 16

#define DIR_LEFT 0
#define DIR_UP 1
#define DIR_DOWN 2
#define DIR_RIGHT 3

#define DIR_COUNT 5
#define COUNTER 10


int currStep = 0;
int currDir = 0;
int dirSum = 0;
int blink = 0;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_8x8matrix matrix = Adafruit_8x8matrix();
SoftwareSerial bluetooth(7,8);
Mindwave* brain;

static const uint8_t PROGMEM
  right_bmp[] =
  { B00001000,
    B00001100,
    B00001110,
    B11111111,
    B11111111,
    B00001110,
    B00001100,
    B00001000},
  left_bmp[] = 
  {
    B00010000,
    B00110000,
    B01110000,
    B11111111,
    B11111111,
    B01110000,
    B00110000,
    B00010000},
  up_bmp[] = {
    B00011000,
    B00111100,
    B01111110,
    B11111111,
    B00011000,
    B00011000,
    B00011000,
    B00011000, 
  },
  down_bmp[] = {
    B00011000, 
    B00011000, 
    B00011000, 
    B00011000, 
    B11111111,
    B01111110,
    B00111100,
    B00011000   
  },
  stop_bmp[] = {
    B00111100,
    B01000010,
    B10000001,
    B11111111,
    B11111111,
    B10000001,
    B01000010,
    B00111100    
  };
    
  

void setup(){
  pinMode(7,INPUT);
  pinMode(8,OUTPUT);
  strip.begin();
  clearAttention();
  strip.setBrightness(128);
  strip.show();
  matrix.begin(0x70);
  matrix.drawBitmap(0, 0, stop_bmp, 8, 8, LED_ON);  
  matrix.writeDisplay();
  Serial.begin(57600);
  bluetooth.begin(115200);
  delay(100);  // Short delay, wait for the Mate to send back CMD
  bluetooth.print("$");  // Print three times individually
  bluetooth.print("$");
  bluetooth.print("$");  // Enter command mode
  delay(200);  // Short delay, wait for the Mate to send back CMD
  bluetooth.println("U,9600,N");  // Temporarily Change the baudrate to 9600, no parity
  // 115200 can be too fast at times for NewSoftSerial to relay the data reliably
  bluetooth.begin(9600);  // Start bluetooth serial at 9600
//bluetooth.println("U,115200,N");
  delay(2000);
  bluetooth.println("SR,74E5439C62D3");
  delay(200);
  bluetooth.println("SM,3");
  delay(200);
  bluetooth.println("SP,0000");
  delay(200);
  bluetooth.println("C");
  delay(200);
//  bluetooth.println("U,57600,N");  // Temporarily Change the baudrate to 9600, no parity
//  // 115200 can be too fast at times for NewSoftSerial to relay the data reliably
//  bluetooth.begin(57600);  // Start bluetooth serial at 9600

  delay(2000);
  brain = new Mindwave(bluetooth);
  brain->setup();
  brain->setDebug(false);

}


void clearAttention(){
  for (int i=0; i<PIXEL_COUNT; i++){
     strip.setPixelColor(i, 0,0,0); 
  }
}

void showDirection(int dir){
  matrix.clear();
   switch (dir) {
      case DIR_LEFT:
        matrix.drawBitmap(0, 0, left_bmp, 8, 8, LED_ON);        
        break;
      case DIR_RIGHT:
        matrix.drawBitmap(0, 0, right_bmp, 8, 8, LED_ON);        
        break;
      case DIR_UP:
        matrix.drawBitmap(0, 0, up_bmp, 8, 8, LED_ON);        
        break;
      case DIR_DOWN:
        matrix.drawBitmap(0, 0, down_bmp, 8, 8, LED_ON);        
        break;
      default:
        matrix.drawBitmap(0, 0, stop_bmp, 8, 8, LED_ON);     
        break;
   } 
  matrix.writeDisplay();
}


void showAttention(int maxPix){
  clearAttention();
  for (int i=PIXEL_COUNT-maxPix-1; i<PIXEL_COUNT; i++){
     strip.setPixelColor(i, 0,255,0); 
  }
  strip.show();
}



void loop(){
  brain->update();
  if (brain->hasNewData()) {
        showDirection(currDir);
        dirSum +=  brain->getAttention();
        showAttention(brain->getAttention() / 10);
        blink = brain->getBlink();
//         Serial.print("Blink:");
//         Serial.println(blink);
        currStep++;
  }
  if (currStep > COUNTER) {
     int avg = dirSum / COUNTER;
//     Serial.println("Average ");
//     Serial.print(avg);
      if (avg > 55) {
         for (int k=0;k<3;k++){
           matrix.clear();
           matrix.writeDisplay();
           delay(500);
           showDirection(currDir);
           delay(500);
         }
         Serial.println(currDir);
          
      }
     dirSum = 0;
    currStep = 0;
     currDir++;
     if (currDir >= 5) {
        currDir = 0; 
     }
  }
}


