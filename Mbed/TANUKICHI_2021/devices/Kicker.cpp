/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#include "Kicker.h"

Kicker::Kicker(PinName _kickR, PinName _kickL) : kickR(_kickR), kickL(_kickL) {
    interval.start();
}

void Kicker::kick(int _angle, float _power){
    if(_angle > 90)
        _angle = 180 - _angle;
    else if(_angle < -90)
        _angle = -180 - _angle;
    
    _angle = constrain(_angle, -45, 45);
    
    if(interval.read_ms() > intervalTime){
        const float ratioL = 0.85;
        const float ratioR = 1.15;
        kickR = 1;
        kickL = 1;
        interval.reset();
        interval.start();
        timerR.attach(this, &Kicker::clearR, maxTime * sin(45 - _angle) * 1.5 * _power * ratioR);
        timerL.attach(this, &Kicker::clearL, maxTime * cos(45 - _angle) * 1.5 * _power * ratioL);
    }
}

void Kicker::set(int _angle, float _power){
    angle = _angle;
    power = _power;
}

void Kicker::kick(){
    kick(angle, power);
}

void Kicker::clearR(){
    kickR = 0;
}

void Kicker::clearL(){
    kickL = 0;
}
