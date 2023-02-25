/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef Kicker_h
#define Kicker_h

#include "mbed.h"
#include "../libraries/Math.h"

class Kicker {
private:
    DigitalOut   kickR;
    DigitalOut   kickL;
    const int intervalTime = 200;
    Timeout timerR;
    Timeout timerL;
    void clearR();
    void clearL();
public:
    Timer interval;
    
    float maxTime = 0.01;
    
    int angle = 0;
    float power = 1;
    
    Kicker(PinName _kickR, PinName _kickL);
    void kick(int angle, float power);
    void set(int angle, float power);
    void kick();
};

#endif /* Kicker_h */
