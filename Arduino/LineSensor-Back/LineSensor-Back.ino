/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>

#define Address  100

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(8, 17, NEO_GRB + NEO_KHZ800);

int pins[10] = {19, 18, 20, 21, 4, 5, 0, 1, 6, 7};
int eepromoffset = 0;

float value[10];
int calculate_mode = 1;
int oldcalibration[10];
int newMaxcalibration[10];
int newMincalibration[10];
int middleValue[10];
int line_send;
int line;
bool ledState, ledSet;

const float Kthreshould = 0.4;

unsigned long whenSended = 0;

void setup() {
  pinMode(25, OUTPUT);
  WDTCSR = 0b01100001;
  SPCR = 0b11000000;
  pixels.begin();
  for (int i = 0; i < 8; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 150, 0));
  }
  pixels.show();

  for (int i = 0; i < 10; i++) {
    middleValue[i] = EEPROM.read(i + eepromoffset) * 8;
  }
}

void loop() {
  __asm__ __volatile__ ("wdr");
  getValues();
  if (ledState != ledSet) {
    for (int i = 0; i < 8; i++) {
      pixels.setPixelColor(i, pixels.Color(0, ledSet ? 150 : 0, 0));
    }
    pixels.show();
    ledState = ledSet;
  }
  switch (calculate_mode) {
    case 1: //計測モード
      if (value[0] > middleValue[0])
        line |= 0b0000000001;
      if (value[1] > middleValue[1])
        line |= 0b0000000010;
      if (value[2] > middleValue[2])
        line |= 0b0000000100;
      if (value[3] > middleValue[3])
        line |= 0b0000001000;
      if (value[4] > middleValue[4])
        line |= 0b0000010000;
      if (value[5] > middleValue[5])
        line |= 0b0000100000;
      if (value[6] > middleValue[6])
        line |= 0b0001000000;
      if (value[7] > middleValue[7])
        line |= 0b0010000000;
      if (value[8] > middleValue[8])
        line |= 0b0100000000;
      if (value[9] > middleValue[9])
        line |= 0b1000000000;
//      if (value[10] > middleValue[10])
//        line |= 0b010000000000;
//      if (value[11] > middleValue[11])
//        line |= 0b100000000000;
      SPDR = Address;
      break;
    case 2:
    case 3:   //転送待機
      if (micros() - whenSended > 2000) {
        SPDR = Address;
        calculate_mode = 1;
      }
      break;
    case 4:   //キャリブレーション
      for (int i = 0; i < 10; i++) {
        newMaxcalibration[i] = max(value[i], newMaxcalibration[i]);
        newMincalibration[i] = min(value[i], newMincalibration[i]);
        middleValue[i] = (float)newMaxcalibration[i] * Kthreshould + (float)newMincalibration[i] * ((float)1 - Kthreshould);
        middleValue[i] /= 8;
        middleValue[i] *= 8;
      }
      break;
    case 5:
      calculate_mode = 1;
      break;
    case 6:
      break;
    case 7:
      for (int i = 0; i < 8; i++) {
        pixels.setPixelColor(i, pixels.Color(0, 0, 0));
      }
      for (int j = 1; j < 20; j++) {
        for (int i = 0; i < 8; i++) {
          pixels.setPixelColor(i, pixels.Color(j * 13, j * 13, j * 13));
        }
        pixels.show();
        delay(25);
      }
      delay(500);

      for (int j = 51; j > 10; j--) {
        for (int i = 0; i < 8; i++) {
          pixels.setPixelColor(i, pixels.Color(j * 5, j * 5, j * 5));
        }
        pixels.show();
        delay(15);
      }
      for (int j = 10; j < 52; j++) {
        for (int i = 0; i < 8; i++) {
          pixels.setPixelColor(i, pixels.Color(j * 5, j * 5, j * 5));
        }
        pixels.show();
        delay(15);
      }
      delay(500);

      for (int j = 0; j < 52; j++) {
        for (int i = 0; i < 8; i++) {
          pixels.setPixelColor(i, pixels.Color(255 - j * 5, 255 - j, 255 - j * 5));
        }
        pixels.show();
        delay(10);
      }

      for (int i = 0; i < 8; i++) {
        pixels.setPixelColor(i, pixels.Color(0, 200, 0));
      }
      pixels.show();
      calculate_mode = 1;
      break;
    default:
      calculate_mode = 1;
      break;
  }
}

int index = 0;
int data[10][5];
void getValues() {
  for (int i = 0; i < 10; i++) {
    data[i][index] = analogRead(pins[i]);
    value[i] = data[i][0];
    for (int j = 1; j < 5; j++)
      value[i] = max(value[i], data[i][j]);
  }
  index++;
  index %= 5;
}

int lastvalue[3];
ISR (SPI_STC_vect) {
  calculate_mode = SPDR;
  switch (calculate_mode) {
    case 1:
      break;
    case 2:
      ledSet = true;
      lastvalue[0] = lastvalue[1];
      lastvalue[1] = lastvalue[2];
      lastvalue[2] = line;
      line = ~lastvalue[0] & lastvalue[1] | lastvalue[2];
      line_send = line & 0b0011111111;
      SPDR = line_send;
      whenSended = micros();
      break;
    case 3:
      line_send = line >> 8;
      line = 0;
      SPDR = line_send;
      whenSended = micros();
      break;
    case 4:
      for (int i = 0; i < 10; i++) {
        newMaxcalibration[i] = 0;
        newMincalibration[i] = 0;
        middleValue[i] = 0;
      }
      break;
    case 5:
      for (int i = 0; i < 10; i++) {
        EEPROM.write(i + eepromoffset, middleValue[i] / 8);
      }
      calculate_mode = 1;
      SPDR = Address;
      break;
    case 6:
      ledSet = false;
      SPDR = 0;
      break;
    case 7:
      break;
    default:
      SPDR = Address;
      calculate_mode = 1;
      break;
  }
}
