/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef Algorithm_h
#define Algorithm_h

#include "../../mbed.h"
#include "../../shared.h"
#include "../../Robot.h"
#include "../../libraries/PID.h"
#include "../../libraries/Math.h"

class Algorithm {
private:
    Cam      &cam;
    Line     &line;
    Distance &distance;
    Gyro     &gyro;
    Mouse    &mouse;
    Hold     &hold;
    Motor    &motor;
    Kicker   &kicker;
    Dribbler &dribbler;
    Timer sysTime;
    char &speed;
    
    Timer intervalT;
    
    unsigned long interval = 0;
    
    PID ballX, goalL, goalR, goalC;
    
    Move process_ball(Sensors sensors);
    Move process_balldistance(Sensors sensors);
    
    bool vsRiOneNano_CatBot;
    bool detectGoal = true;
    bool useWireless;
    
    int wirelessCount;
    
    int detected[4] = {0, 0, 0, 0};
    int lastlineval[4] = {0, 0, 0, 0};
    unsigned long lineAppeared_time = None;
    unsigned long lineDissappeared_time = UINT32_MAX - 2000;

    int ballcount;
    int lastmove;
    
    Move process_line(Move move, Sensors sensors);
    void getDetected(Sensors sensors);
    
    Move smoothMove(Move move1, Move move2, float rate){
        if(move2.angle == DONTMOVE || move1.angle == DONTMOVE)
            return move1;
        int x1, x2;
        int y1, y2;
        x1 = move1.power * 200 * sin(move1.angle);
        x2 = move2.power * 200 * sin(move2.angle);
        y1 = move1.power * 200 * cos(move1.angle);
        y2 = move2.power * 200 * cos(move2.angle);
        int xresult = x1 * rate + x2 * (1 - rate);
        int yresult = y1 * rate + y2 * (1 - rate);
        Move result;
        result.angle = atan2(xresult, yresult);
        result.power = (float)sqrt(xresult * xresult + yresult * yresult) / 200;
        return result;
    }
    
    Sensors prevSensors;
    int a = 0;
public:
    Algorithm(Devices &devices, Settings &settings);
    void move();
    
};

#endif /* Algorithm_h */
