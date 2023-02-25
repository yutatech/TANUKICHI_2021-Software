/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef Motor_h
#define Motor_h

#include "mbed.h"
#include "../shared.h"
#include "../libraries/FastPWM.h"
#include "../libraries/Math.h"

typedef struct {
    void set(int _angle, float _power){
        angle = _angle;
        power = _power;
    }
    int angle = DONTMOVE;
    float power = 1;
} Move;

class Motor {
private:
    FastPWM MotorA;
    FastPWM MotorB;
    FastPWM MotorC;
    FastPWM MotorD;
    DigitalOut MotorRST;
    
    Move move;
    
    int PIDvalue;
    bool isPIDEnabled;
    
    Move vectorOP(Move move);
public:
    Motor(PinName _ma, PinName _mb, PinName _mc, PinName _md, PinName _mrst);
    void setValues(double M1, double M2, double M3, double M4);
    void setMove(Move move, double speed);
    void open(void);
    void setPID(int);
    void EnablePID(bool);
    
    int movingX;
    int movingY;
    Move moving;
};

#endif /* Motor_h */
