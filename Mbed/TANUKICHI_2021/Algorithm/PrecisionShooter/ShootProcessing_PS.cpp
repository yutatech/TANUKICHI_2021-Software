/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#include "ShootProcessing_PS.h"

ShootProcessing_PS::ShootProcessing_PS(MachinePosition &_machinePosition, BallPosition &_ballPosition) :
    machinePosition(_machinePosition),
    ballPosition(_ballPosition)
{        
    ballCatch.start();
    frontLineDetected.start();
}

Move ShootProcessing_PS::moveToGoal(SensorsArray &sensAry, Dribbler &dribblerF){
    Sensors &sens = sensAry.get(0);
    Move move;
    if(!(sensAry.get(1).hold.f))
        ballCatch.reset();
    
//set move
    if(sens.line.detected.f == 1)
        frontLineDetected.reset();
    
    if(sens.cam.gAngleC != None && abs(sens.cam.gAngleC) < 50){
        int r = 60;
        if(sens.cam.gDistance < r){
            move.angle = sens.cam.gAngleC * 3;
            move.power = 1;
        }
        else{
            int roundDir = sens.cam.gAngleC > 0 ? 1 : -1;
            move.angle = sens.cam.gAngleC + asin((float)r / (float)sens.cam.gDistance) * 180 / 3.14 * roundDir;
            move.power = 1;
        }
    }
    else if(sens.cam.gAngleC != None){
        move.angle = constrain(abs(sens.cam.gAngleC)*sens.cam.gAngleC*0.05, -180,180);
        move.power = 1.5 - constrain((float)abs(move.angle) / 100.0, 0, 1);
    }
    else if(frontLineDetected.read_ms() > 1000){
        move.angle = 180;
        move.power = constrain(0.8 * 100, 0, 120) / 100;
    }
    else{
        int movex = -(sens.distance.value[LEFT] - sens.distance.value[RIGHT]) / 2;
        int movey = 80;
        move.set(atan2(movex, movey), 1.5);
    }
    
    if(abs(move.angle) < 45)
        dribblerF.set(30);
    else
        dribblerF.set(30);
    
//limit move
    if(ballCatch.read_ms() < 500 || sens.encoder.rpmf < 400){
        dribblerF.set(30);
        if(45 < move.angle && move.angle < 90){
            move.angle = 45;
        }
        else if(-45 > move.angle && move.angle > -90){
            move.angle = -45;
        }
        else if(abs(move.angle) > 90){
            move.set(0, 0.5);
        }
    }
    
    int avoidAngle = 0;
    if(0 < sens.distance.lidar.distance && sens.distance.lidar.distance < 650 && sens.cam.gDistance > 70){
        float R = 400;
        int dir;
        if(sens.cam.gAngleC != None && range(sens.cam.gAngleL-15, sens.distance.lidar.angleC, sens.cam.gAngleR + 15))
            dir = sens.cam.gAngleC > sens.distance.lidar.angleC ? 1 : -1;
        else
            dir = sens.distance.lidar.angleC < 0 ? 1 : -1;
        
        if(sens.distance.lidar.distance >= R)
            avoidAngle = sens.distance.lidar.angleC + asin(R / sens.distance.lidar.distance) * 180 / 3.14 * dir;
        else
            avoidAngle = sens.distance.lidar.angleC + 90 * dir;
    }
    if(avoidAngle > 0 && avoidAngle > move.angle)
        move.set(avoidAngle, 1);
    else if(avoidAngle < 0 && avoidAngle < move.angle)
        move.set(avoidAngle, 1);
    
    return move;
}

Move ShootProcessing_PS::kick(SensorsArray &sensAry, Dribbler &dribblerF, Kicker &kicker, Gyro &gyro){
    Sensors &sens = sensAry.get(0);
    Move move;
    
    dribblerF.set(10);
    move.set(kicker.angle, 1.5);
    if((machinePosition.velocityX * cos(kicker.angle - 90) + machinePosition.velocityY * cos(kicker.angle) > 600) ||
       machinePosition.velocity < 5 ||
       (sens.line.detected.l && sens.cam.gAngleC < -5) || (sens.line.detected.r && sens.cam.gAngleC > 5) ||
       (sens.line.detected.f && (sens.cam.gAngleL < 5 && sens.cam.gAngleR > -5)) ){
        if(kicker.angle > 0)
            gyro.offset = 45;
        else if(kicker.angle < 0)
            gyro.offset = -45;
        else
            gyro.offset = 0;
        
        double kickx = 1500 * sin(kicker.angle) - machinePosition.velocityX;
        double kicky = 1500 * cos(kicker.angle) - machinePosition.velocityY;
        kicker.angle = atan2((int)kickx, (int)kicky);
        kicker.kick();
    }
    
    if(machinePosition.velocity < 50 && sens.line.detected.allOR() == 1){
        if(kicker.angle > 15)
            gyro.offset = 45;
        else if(kicker.angle < -15)
            gyro.offset = -45;
        else
            gyro.offset = 0;
        kicker.kick();
    }
    
    return move;
}
