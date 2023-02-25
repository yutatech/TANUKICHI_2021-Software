/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#include "DodgeOpponent.h"

Move DodgeOpponent::motion1(SensorsArray &sensAry, Dribbler &dribblerF){
    Sensors &sens = sensAry.get(0);
    Move move;
    dribblerF.set(40);
    int moveDirect = FRONT;
    if(sens.line.detected.r | sens.line.detected.l){
        if(sens.line.detected.r)
            moveDirect = LEFT;
        else
            moveDirect = RIGHT;
    }
    else if(sens.cam.gAngleC != None){
        if(sens.cam.gAngleC < 0)
            moveDirect = LEFT;
        else
            moveDirect = RIGHT;
    }
    else{
        if(sens.distance.lidar.angleC > 0)
            moveDirect = LEFT;
        else
            moveDirect = RIGHT;
    }
    
    if(moveDirect == LEFT){
        move.angle = -80;
        move.power = 0.8;
    }
    else{
        move.angle = 80;
        move.power = 0.8;
    }
    
    return move;
}

Move DodgeOpponent::motion2(SensorsArray &sensAry){
    Sensors &sens = sensAry.get(0);
    Move move;
    return move;
}
