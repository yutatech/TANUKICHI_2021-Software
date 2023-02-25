/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef Dribbler_h
#define Dribbler_h

#include "mbed.h"
#include "../libraries/Math.h"

class Dribbler {
private:
    PwmOut dribbler;
public:
    Dribbler(PinName _dribbler);
    void init();
    void set(int val); //0~100
};

#endif /* Dribbler_h */
