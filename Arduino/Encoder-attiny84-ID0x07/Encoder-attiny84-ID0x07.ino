/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#include <Wire.h>

#define ID  7

void setup() {
  pinMode (1, INPUT);
  pinMode (2, INPUT);

  Wire.begin(ID);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
}

byte prvData = 0;
byte curData = 0;
const int increment[4][4] = {{0, 1, -1, 0}, { -1, 0, 0, 1}, {1, 0, 0, -1}, {0, -1, 1, 0}};

int pos = 0;
int rpm = 0;
int smoothedRpm = 0;

unsigned long lastPosUpdatedTime = 0;

void loop() {
  curData = 0;
  if (digitalRead(1) == HIGH)  curData |= 0b00000001;
  if (digitalRead(2) == HIGH)  curData |= 0b00000010;

  unsigned long curTime = millis();
  if (increment[prvData][curData] != 0) {
    rpm = 5000 / (curTime - lastPosUpdatedTime) * increment[prvData][curData];
    lastPosUpdatedTime = curTime;
  }
  else if (curTime - lastPosUpdatedTime > 100) {
    rpm = 0;
    lastPosUpdatedTime = curTime;
  }
  smoothedRpm = smoothedRpm * 0.95 + rpm * 0.05;

  pos += increment[prvData][curData];
  prvData = curData;
}

void requestEvent() {
  Wire.write(pos);
  Wire.write(pos >> 8);
  Wire.write(smoothedRpm);
  Wire.write(smoothedRpm >> 8);
}

void receiveEvent(int data) {
  switch (Wire.read()) {
    case 0x10:
      pos = 0;
      break;
  }
}
