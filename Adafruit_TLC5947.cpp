/*************************************************** 
  This is a library for our Adafruit 24-channel PWM/LED driver

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/1429

  These drivers uses SPI to communicate, 3 pins are required to  
  interface: Data, Clock and Latch. The boards are chainable

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/


#include <Adafruit_TLC5947.h>
#include <SPI.h>


Adafruit_TLC5947::Adafruit_TLC5947() {
  // constructor empty
  // initialize with init() to allow for Serial.debugging
}

// software SPI mode
void Adafruit_TLC5947::init(uint16_t n, uint8_t c, uint8_t d, uint8_t l) {
  Serial.println("Adafruit_TLC5947 init for Software SPI");
  numdrivers = n;
  _clk = c;
  _dat = d;
  _lat = l;

  //pwmbuffer = (uint16_t *)calloc(2, 24*n);
  pwmbuffer = (uint16_t *)malloc(2 * 24*n);
  memset(pwmbuffer, 0, 2*24*n);
}


// hardware SPI mode
void Adafruit_TLC5947::init(uint16_t n, uint8_t l) {
  Serial.println("Adafruit_TLC5947 init for Hardware SPI");
  numdrivers = n;
  _clk = -1;
  _dat = -1;
  _lat = l;

// set SPI settings in write() call to enable multiple parallel SPI calls/settings
//   SPI.setBitOrder(MSBFIRST);
// #ifdef __arm__
//   SPI.setClockDivider(42);
// #else
//   SPI.setClockDivider(SPI_CLOCK_DIV8);
// #endif
//   SPI.setDataMode(SPI_MODE0);

  pwmbuffer = (uint16_t *)calloc(2, 24*n);
  memset(pwmbuffer, 0, 2*24*n);
}





void  Adafruit_TLC5947::spiwriteMSB(uint32_t d) {
  if (_clk >= 0) {
    uint32_t b = 0x80;        // 0x80 = 128
    //  b <<= (bits-1);
    for (; b!=0; b>>=1) {
      digitalWrite(_clk, LOW);
      if (d & b)  
        digitalWrite(_dat, HIGH);
      else
        digitalWrite(_dat, LOW);
      digitalWrite(_clk, HIGH);
    }
  } else {
    SPI.transfer(d);
  }
}


void Adafruit_TLC5947::write(void) {

    unsigned int chan1 = 0;
    unsigned int chan2 = 0;
    byte address1 = 0;
    byte address2 = 0;
    byte address3 = 0;

    // packing each 2 channel (12bit*2) to 3 byte (8bit*3) for transfering
    SPI.beginTransaction(SPISettings(15000000, MSBFIRST, SPI_MODE0));
    digitalWrite(_lat, LOW);

    for (unsigned int ledpos = (24 / 2) * numdrivers  - 1; ledpos > 0; ledpos--) {
      chan1 = pwmbuffer[ledpos];
      chan2 = pwmbuffer[ledpos -1];
      address1 = (byte)(chan1 >> 4) ;
      address2 = (byte)((chan1 << 4) & (B11110000)) + (byte)((chan2 >> 8) & (B00001111));
      address3 = (byte)chan2;
      // SPI.transfer(address1);  // hardware SPI only
      // SPI.transfer(address2);
      // SPI.transfer(address3);
      spiwriteMSB(address1);   // to enable software-SPI as well
      spiwriteMSB(address2);
      spiwriteMSB(address3);
    }

    digitalWrite(_lat, HIGH);   // TSU2, go HIGH, minimum 30ns 
    SPI.endTransaction();
    digitalWrite(_lat, LOW);
}


void Adafruit_TLC5947::setPWM(uint16_t chan, uint16_t pwm) {
  if (pwm > 4095) pwm = 4095;
  if (chan > 24*numdrivers) return;
  pwmbuffer[chan] = pwm;  
}


void Adafruit_TLC5947::setLED(uint16_t lednum, uint16_t r, uint16_t g, uint16_t b) {
  setPWM(lednum*3, r);
  setPWM(lednum*3+1, g);
  setPWM(lednum*3+2, b);
}


boolean Adafruit_TLC5947::begin() {
  if (!pwmbuffer) return false;

  pinMode(_lat, OUTPUT);
  if (_clk >= 0) {
    pinMode(_clk, OUTPUT);
    pinMode(_dat, OUTPUT);
  } else {
    SPI.begin();
  }

  return true;
}
