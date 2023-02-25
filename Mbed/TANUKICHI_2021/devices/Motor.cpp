/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#include "Motor.h"

Motor::Motor(PinName _ma, PinName _mb, PinName _mc, PinName _md, PinName _mrst) : MotorA(_ma), MotorB(_mb), MotorC(_mc), MotorD(_md),  MotorRST(_mrst) {
    MotorRST.write(0);
    MotorA.period(0.000005);
    MotorB.period(0.000005);
    MotorC.period(0.000005);
    MotorD.period(0.000005);
}

void Motor::setValues(double M1, double M2, double M3, double M4) { //power +-192
    if (isPIDEnabled) {
        double Mmax = max(max(abs(M1-PIDvalue), abs(M2-PIDvalue)), max(abs(M3-PIDvalue), abs(M4-PIDvalue)));
        if(Mmax > 200){
            double amp = (200-abs(PIDvalue)) / Mmax;
            M1 = constrain(M1*amp, -200,200);
            M2 = constrain(M2*amp, -200,200);
            M3 = constrain(M3*amp, -200,200);
            M4 = constrain(M4*amp, -200,200);
        }
        M1 -= PIDvalue;
        M2 -= PIDvalue;
        M3 -= PIDvalue;
        M4 -= PIDvalue;
    }
    
    double Mmax = max(max(abs(M1), abs(M2)), max(abs(M3), abs(M4)));
    if(Mmax > 200){
        double amp = 200 / Mmax;
        M1 = constrain(M1*amp, -200,200);
        M2 = constrain(M2*amp, -200,200);
        M3 = constrain(M3*amp, -200,200);
        M4 = constrain(M4*amp, -200,200);
    }
    
    //0.02 ~ 0.98
    MotorRST = 1;
    MotorA = 0.5 + (M1 / 400);
    MotorB = 0.5 + (M2 / 400);
    MotorC = 0.5 + (M3 / 400);
    MotorD = 0.5 + (M4 / 400);
}

void Motor::setMove(Move move, double speed) {
    if (move.angle != DONTMOVE) {
        if(move.power < 0){
            move.angle += 180;
            move.power = -move.power;
        }
        
        //vectorOP(move);
        //move = vectorOP(move);
        
        //speed = speed * move.power;
        //setValues(sin(move.angle - 45) * speed, sin(move.angle - 135) * speed, sin(move.angle + 135) * speed, sin(move.angle + 45) * speed);
        //return;
        
        //--Distortion correction--
        move.angle += 360;
        move.angle %= 360;
        int ref = move.angle / 90 * 90;
        move.angle %= 90;
        if(move.angle < 45)
            move.angle = (1-cos(move.angle * 2)) * 45;
        else
            move.angle = 90 - (1-cos((90-move.angle) * 2)) * 45;
        move.angle += ref;
        
        speed = speed * move.power;
        
        speed *= 1 + sin(move.angle - 45) * 0.05;
        
        double M1 = sin(move.angle - 45) * speed;
        double M2 = sin(move.angle - 135) * speed;
        double M3 = sin(move.angle + 135) * speed;
        double M4 = sin(move.angle + 45) * speed;
        
        double Mmax = max(max(abs(M1), abs(M2)), max(abs(M3), abs(M4)));
        if(Mmax != 0){
            double amp = speed / Mmax;
            M1 *= amp;
            M2 *= amp;
            M3 *= amp;
            M4 *= amp;
        }
        
        //--Distortion correction--
        setValues(M1, M2, M3, M4);
    }
    else {
        setValues(0, 0, 0, 0);
        movingX = 0;
        movingY = 0;
    }
}

Move Motor::vectorOP(Move move) {
    const float Fmax = 150;
    const float _Fmax = 1 / Fmax;
    const float T = 0.1;
    const float _T = 1 / T;
    
    int targetX = move.power * 100 * sin(move.angle);
    int targetY = move.power * 100 * cos(move.angle);

    int opeX = targetX - movingX;
    int opeY = targetY - movingY;
    int opeR = sqrt(opeX*opeX + opeY*opeY);
    
    int outX = targetX * 2 - movingX;
    int outY = targetY * 2 - movingY;
    int outR = sqrt(outX*outX + outY*outY);
    
    if(T * Fmax >= opeR){
        movingX = targetX;
        movingY = targetY;
        move.angle = atan2(outX, outY);
        move.power = (float)outR * 0.01;
    }
    else{
        if(outR == 0)
            outR = 1;
        movingX += opeX * Fmax * T / outR;
        movingY += opeY * Fmax * T / outR;
        move.angle = atan2(outX, outY);
        move.power = Fmax * 0.01;
    }
    
    moving.angle = atan2(movingX, movingY);
    moving.power = (float)sqrt(movingX * movingX + movingY * movingY) / 100;
    return move;
}

void Motor::open() {
    MotorRST = 0;
    movingX = 0;
    movingY = 0;
}

void Motor::setPID(int data) {
    if (isPIDEnabled)
        PIDvalue = constrain(data, -200, 200);
    else
        PIDvalue = 0;
}

void Motor::EnablePID(bool data) {
    isPIDEnabled = data;
}
