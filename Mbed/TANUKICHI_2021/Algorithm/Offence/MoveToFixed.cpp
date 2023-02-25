/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#include "MoveToFixed.h"

Move MoveToFixed::neutral(SensorsArray &sensAry){
    Sensors &sens = sensAry.get(0);
    Move move;
    int movex=0,movey=0;
//    if(lostLineDetected.f)
//        movey -=100;
//    if(lostLineDetected.r)
//        movex -=100;
//    if(lostLineDetected.b)
//        movey +=100;
//    if(lostLineDetected.l)
//        movex +=100;
    
    move.set(atan2(movex, movey), 1.0);
    
    move = home(sensAry);
    
    return move;
}

Move MoveToFixed::home(SensorsArray &sensAry){
    Sensors &sens = sensAry.get(0);
    const int offsetX = 10;
    Move move;
    if (abs(sens.gyro.value) < 15) {
        int bx = -(sens.distance.value[LEFT] - sens.distance.value[RIGHT]) / 2 + offsetX;
        int by = -sens.distance.value[2] / 10 + 60;
        if(sens.distance.value[2] >= 8189){
            if(abs(bx) > 35)
                by = 0;
            else
                by = -100;
        }
        move.power = constrain(sqrt((bx * bx) + (by * by)) / 20, 0, 1.2);
        move.angle = atan2(bx, by);
//        move = smoothMove(move, lastMove, 0.8);
    }
    else
        move.angle = DONTMOVE;
    
    return move;
}
