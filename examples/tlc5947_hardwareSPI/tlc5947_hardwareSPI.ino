/*************************************************** 

  Run with Arduino Due, and Arduino 1.8.4
  
  Example to run the Adafruit 24-channel PWM/LED driver
  with hardware SPI (= faster!)

  http://www.adafruit.com/products/1429

  Connections: (see https://www.arduino.cc/en/Reference/SPI)
  Data > SPI MOSI
  Clock > SPI SCK 
  Latch > SS (master)

  
 ****************************************************/

#include "Adafruit_TLC5947.h"

// How many boards do you have chained?
#define NUM_TLC5974 2

#define latch   4  // Arduino Due Chip-Select 
#define oe  -1  // set to -1 to not use the enable pin (its optional)

Adafruit_TLC5947 tlc;

int cycle = 0;



void setup() {
  Serial.begin(9600);
  Serial.println("TLC5974 hardware-SPI test");

  tlc.init(NUM_TLC5974, latch);
  tlc.begin();

  if (oe >= 0) {
    pinMode(oe, OUTPUT);
    digitalWrite(oe, LOW);
  }
}



void loop() {

  for (int i=0; i<24*NUM_TLC5974; i++) {
    if (i==cycle) {
      tlc.setPWM(i, 4095);
    } else {
      tlc.setPWM(i,0);
    }
  }

  tlc.write();

  cycle++;
  if(cycle >= 24*NUM_TLC5974) cycle = 0;

  delay(100);


}

