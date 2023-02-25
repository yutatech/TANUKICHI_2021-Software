/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef PID_h
#define PID_h

#include "Math.h"

class PID {
private:
    const float None = 3.402823e+38;
    float KP, KI, KD, iMAX;
    float diff[2];
    float integral;
    float pidMax, pidMin;
public:
    PID(float _KP, float _KI, float _KD, float _iMAX, float _min, float _max) : KP(_KP), KI(_KI), KD(_KD), iMAX(_iMAX), pidMin(_min), pidMax(_max) {}
    
    float getPID(int value, int target, unsigned long interval_us) {
        if (value == UINT16_MAX) return 0;
        
        float p = 0, i = 0, d = 0;
        float gap = value - target;
        diff[0] = diff[1];
        diff[1] = gap;
        p = KP * diff[1];
        if(diff[0] != None){
            integral += (diff[1] + diff[0]) / 2 * ((float)interval_us / 1000000);
            integral = constrain(integral, -50, 50);
            i = constrain(KI * integral, -iMAX, iMAX);
            d = KD * (diff[1] - diff[0]) / ((float)interval_us / 1000000);
        }
        return constrain(p + i + d, pidMin, pidMax);
    }
    
    void reset() {
        diff[0] = diff[1] = None;
        integral = 0;
    }
};

#endif /* PID_h */
