/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef Cam_h
#define Cam_h

#include "mbed.h"
#include "../Shared.h"


typedef struct {
    int bAngle = None;
    double bDistance = None;
    int gAngleC = None, gAngleL = None, gAngleR = None, gDistance = None, fps = 0;
    int intervalus = 0;
    bool updated = false;
    void print(){
        printf("Cam:bAngle=%d,bDistance=%lf,gAngleL=%d,gAngleC=%d,gAngleR=%d,gDistance=%d\t",bAngle, bDistance, gAngleL, gAngleC, gAngleR, gDistance);
    }
} Cam_S;

class Cam {
private:
    Cam_S lastData;
    Serial openMV;
    Timer intervalTimer;
    int intervalus = 0;
    
    void interrupt();
    
    int bufcont = 0;
    int buf[7] = {255,255,255,255,255,255, 0};
    bool ready = false;
    int decompress(int data);
public:
    const bool Blue = true;
    const bool Yellow = false;
    bool impossible = true;
    bool color = Yellow;
    Cam_S data;
    
    Cam(PinName _tx, PinName _rx);
    void init();
    void getValue(Cam_S&);
    void setGoal(bool color);
    
};

#endif /* Ball_h */
