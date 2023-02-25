/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#include <TimerTwo.h>
#include <SPI.h>
#include <Adafruit_NeoPixel.h>

#define Address 100

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(23, A3, NEO_GRB + NEO_KHZ800);

int trig[2] = {4, A1};
int echo[2] = {3, A0};
int battery;
int force;

int command = 0;
bool dataset = false;
int step = 0;

const int colorRGB[11][3] = {{0, 0, 0}, {200, 200, 200}, {0, 200, 0}, {200, 0, 0}, {0, 0, 200}, {200, 200, 0}, {200, 0, 200}, {200, 50, 0}, {0, 200, 200}, {200, 50, 50}, {50, 0, 200}};
volatile int color[23] = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
volatile int heartstep = 0;

void setup() {
  pinMode(4, OUTPUT); //TrigR
  pinMode(3, INPUT);  //EchoR
  pinMode(A1, OUTPUT); //TrigL
  pinMode(A0, INPUT);  //EchoL
  pinMode(A6, INPUT);  //FORCE

  pinMode(12, OUTPUT);
  SPCR |= _BV(SPE);
  SPI.attachInterrupt();

  pinMode(9, OUTPUT);
  delay(500);
  command = 2;
  Timer2.attachInterrupt(backLight);
  Timer2.initialize(5000);

  pixels.begin();

  for (int i = 1; i < 10; i++) {
    pixels.setPixelColor(4 + i, pixels.Color(255, 255, 255));
    pixels.setPixelColor(23 - i, pixels.Color(255, 255, 255));
    pixels.setPixelColor(0, pixels.Color(i * 27, i * 27, i * 27));
    pixels.setPixelColor(1, pixels.Color(i * 27, i * 27, i * 27));
    pixels.setPixelColor(2, pixels.Color(i * 27, i * 27, i * 27));
    pixels.setPixelColor(3, pixels.Color(i * 27, i * 27, i * 27));
    pixels.setPixelColor(4, pixels.Color(i * 27, i * 27, i * 27));
    pixels.show();
    delay(25);
    pixels.setPixelColor(0, pixels.Color(i * 27 + 12, i * 27 + 12, i * 27 + 12));
    pixels.setPixelColor(1, pixels.Color(i * 27 + 12, i * 27 + 12, i * 27 + 12));
    pixels.setPixelColor(2, pixels.Color(i * 27 + 12, i * 27 + 12, i * 27 + 12));
    pixels.setPixelColor(3, pixels.Color(i * 27 + 12, i * 27 + 12, i * 27 + 12));
    pixels.setPixelColor(4, pixels.Color(i * 27 + 12, i * 27 + 12, i * 27 + 12));
    pixels.show();
    delay(25);
  }
  delay(500);

  for (int j = 51; j > 10; j--) {
    for (int i = 0; i < 23; i++) {
      pixels.setPixelColor(i, pixels.Color(j * 5, j * 5, j * 5));
    }
    pixels.show();
    delay(15);
  }
  for (int j = 10; j < 52; j++) {
    for (int i = 0; i < 23; i++) {
      pixels.setPixelColor(i, pixels.Color(j * 5, j * 5, j * 5));
    }
    pixels.show();
    delay(15);
  }
  delay(500);

  for (int j = 0; j < 52; j++) {
    for (int i = 0; i < 23; i++) {
      pixels.setPixelColor(i, pixels.Color((51-j)*5 + colorRGB[color[i]][0]*j/51, (51-j)*5 + colorRGB[color[i]][1]*j/51, (51-j)*5 + colorRGB[color[i]][2]*j/51));
    }
    pixels.show();
    delay(10);
  }
}

double Duration[2];
double Distance[2];

int sonicData[2] = {0, 0};
int lastData[2] = {0, 0};

bool Stop = false;
void loop() {
  int batt = analogRead(A4);
  force = (analogRead(A6) / 4) * 0.1 + force * 0.9;
  if (batt < 100)
    battery = 255;
  else {
    if (battery == 255)
      battery = constrain(batt - 550, 0, 240);
    else
      battery = constrain(batt - 550, 0, 240) * 0.05 + battery * 0.95;

    if (battery == 100)
      battery = 99;
  }
  if (!Stop) {
    for (int i = 0; i < 2; i++) {
      digitalWrite(trig[i], LOW);
      delayMicroseconds(2);
      digitalWrite(trig[i], HIGH);
      delayMicroseconds(10);
      digitalWrite(trig[i], LOW);
      Duration[i] = pulseIn(echo[i], HIGH, 20000);
      if (Duration[i] > 0) {
        Duration[i] = Duration[i] / 2;
        Distance[i] = Duration[i] * 340 * 100 / 1000000;
      }
      else
        Distance[i] = 0;
    }
    sonicData[0] = constrain(Distance[0], 0, 255) * 0.1 + lastData[0] * 0.9;
    sonicData[1] = constrain(Distance[1], 0, 255) * 0.1 + lastData[1] * 0.9;
    lastData[0] = sonicData[0];
    lastData[1] = sonicData[1];
    SPDR = Address;
  }
  if (dataset) {
    switch (command) {
      case 6:
        Timer2.initialize(1000);
        step = 0;
        break;
      case 7:
        step = 255;
        Timer2.initialize(500);
        break;
      case 8:
        step = 0;
        Timer2.initialize(2500);
        break;
    }
    Timer2.start();
    dataset = false;
  }
  pixels.show();
}

void backLight() {
  switch (command) {
    case 2:
      analogWrite(9, step);
      step += 1;
      if (step > 255) {
        analogWrite(9, 255);
        step = 0;
        Timer2.attachInterrupt(heartbeat);
        Timer2.initialize(5000);
        Timer2.start();
      }
      break;
    case 6:
      step += 1;
      if (step > 100)
        analogWrite(9, step);
      else
        analogWrite(9, 0);
      if (step > 252) {
        analogWrite(9, 255);
        step = 0;
        Timer2.attachInterrupt(heartbeat);
        Timer2.initialize(5000);
        Timer2.start();
      }
      break;
    case 7:
      step -= 1;
      analogWrite(9, step);
      if (step < 1) {
        analogWrite(9, 0);
        step = 0;
        Timer2.stop();
        for (int i = 0; i < 23; i++)
          pixels.setPixelColor(i, colors(color[i], 255));
      }
      break;
    case 8:
      step += 1;
      analogWrite(9, step);
      if (step > 254) {
        analogWrite(9, 255);
        step = 0;
        heartstep = 0;
        Timer2.attachInterrupt(heartbeat);
        Timer2.initialize(5000);
        Timer2.start();
      }
      break;
  }
}

int direction[5];
int num;
ISR (SPI_STC_vect) {
  direction[num] = SPDR;
  switch (direction[0]) {
    case 0:
    case 1:
      SPDR = sonicData[direction[0]];
      Stop = true;
      num = 0;
      break;
    case 5:
      SPDR = battery;
      num = 0;
      break;
    case 7:
        for (int i = 0; i < 23; i++)
          color[i] = 2;
    case 6:
    case 8:
      command = direction[0];
      dataset = true;
      Timer2.attachInterrupt(backLight);
      num = 0;
      break;
    case 9:
      num++;
      if (num == 2) {
        for (int i = 0; i < 23; i++)
          color[i] = direction[1];
        num = 0;
      }
      break;
    case 10:
      num++;
      if (num == 3) {
        if (0 <= direction[1] && direction[1] < 23)
          color[direction[1]] = direction[2];
        num = 0;
      }
      break;
    case 11:
      SPDR = force;
      num = 0;
      break;
    default:
      Stop = false;
      num = 0;
      break;
  }
}

long colors(int c, int bright) {
  bright = constrain(bright, 0, 255);
  switch (c) {
    case 0://Black
    case 1://White
    case 2://Green
    case 3://Red
    case 4://Blue
    case 5://Yellow
    case 6://Purple
    case 7://Orange
    case 8://Cyan
    case 9://Pink
    case 10://Indigo
      return pixels.Color(colorRGB[c][0] * bright / 255, colorRGB[c][1] * bright / 255, colorRGB[c][2] * bright / 255);
    default:
      return pixels.Color(0, 0, 0);
  }
}

long colors(int c) {
  return colors(c, 255);
}

void heartbeat() {
  if (heartstep < 100) {
    for (int i = 0; i < 23; i++)
      pixels.setPixelColor(i, colors(color[i], 255 - heartstep * 2));
  }
  else {
    for (int i = 0; i < 23; i++)
      pixels.setPixelColor(i, colors(color[i], 255 - (200 - heartstep) * 2));
  }
  heartstep++;
  heartstep %= 200;
}
