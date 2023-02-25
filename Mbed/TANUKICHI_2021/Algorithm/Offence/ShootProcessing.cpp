/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#include "ShootProcessing.h"

ShootProcessing::ShootProcessing(MachinePosition &_machinePosition, BallPosition &_ballPosition) :
    machinePosition(_machinePosition),
    ballPosition(_ballPosition)
{        
    ballCatch.start();
    frontLineDetected.start();
}

Move ShootProcessing::moveToGoal(SensorsArray &sensAry, Dribbler &dribblerF){
    Sensors &sens = sensAry.get(0);
    Move move;
    if(!(sensAry.get(1).hold.f))
        ballCatch.reset();
    
//set move
    if(sens.line.detected.f == 1)
        frontLineDetected.reset();
    
    if(sens.cam.gAngleC != None && abs(sens.cam.gAngleC) < 50){
        int r = 300;
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
        dribblerF.set(40);
    
    int avoidAngle = 0;
    if(0 < sens.distance.lidar.distance && sens.distance.lidar.distance < 650 && sens.cam.gDistance > 400){
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
    
//limit move
    if(ballCatch.read_ms() < 200 || sens.encoder.rpmf < 400){
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
    
    return move;
}

Move ShootProcessing::kick(SensorsArray &sensAry, Dribbler &dribblerF, Kicker &kicker, Gyro &gyro){
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

Move ShootProcessing::moveToCurveShotPosition(SensorsArray &sensAry, Dribbler &dribblerB, Gyro &gyro, Hold &hold, bool detectGoal, bool detectOpponent){
    Sensors sens = sensAry.get(0);
    Move move;
    
    if(!detectGoal)
        sens.cam.gAngleC = None;
    if(!detectOpponent)
        sens.distance.lidar.angleC = None;
    
    if(sens.distance.lidar.angleC != None)
        sens.distance.lidar.angleC += sens.gyro.value;
    if(sens.cam.gAngleC != None)
        sens.cam.gAngleC += sens.gyro.value;
        
    if(hold.timeB.read_ms() > 200){
        int movex = 0, movey = 0;
        if(sens.distance.lidar.angleC != None){
            int tagetx = sens.cam.gAngleC > sens.distance.lidar.angleC ? -100 : 100;
            int tagety = 200;
            int _movex = constrain((sens.distance.lidar.distance * sin(sens.distance.lidar.angleC) - tagetx) * 0.012, -0.8, 0.8) * 100;
            int _movey = constrain((sens.distance.lidar.distance * cos(sens.distance.lidar.angleC) - tagety) * 0.008, -0.8, 0.6) * 100;
            movex = _movex * cos(-sens.gyro.value) - _movey * sin(-sens.gyro.value);
            movey = _movex * sin(-sens.gyro.value) + _movey * cos(-sens.gyro.value);
            
            gyTargetOffset = 0;
            gyOffsetDir = sens.cam.gAngleC > sens.distance.lidar.angleC ? -1 : 1;
        }
        else if(sens.cam.gAngleC != None){
            if(sens.cam.gDistance * cos(sens.cam.gAngleC) < 600){
                int tagetx = sens.cam.gAngleC > 0 ? 40 : -40;
                int tagety = 70;
                int _movex = constrain((sens.cam.gDistance * sin(sens.cam.gAngleC) - tagetx) * 0.012, -0.8, 0.8) * 100;
                int _movey = constrain((sens.cam.gDistance * cos(sens.cam.gAngleC) - tagety) * 0.016, -0.8, 0.6) * 100;
                movex = _movex * cos(-sens.gyro.value) - _movey * sin(-sens.gyro.value);
                movey = _movex * sin(-sens.gyro.value) + _movey * cos(-sens.gyro.value);
                
                gyTargetOffset = 0;
                gyOffsetDir = sens.cam.gAngleC > 0 ? -1 : 1;
            }
            else{
                movex = -constrain(((sens.distance.value[LEFT] - sens.distance.value[RIGHT]) / 2 + 10) * 0.05, -0.8, 0.8) * 100;
                movey = constrain((sens.distance.value[FRONT] - 500) * 0.005, -0.8, 0.6) * 100;
                
                gyTargetOffset = 0;
                gyOffsetDir = sens.cam.gAngleC > 0 ? -1 : 1;
            }
        }
        else{
            movex = -constrain(((sens.distance.value[LEFT] - sens.distance.value[RIGHT]) / 2 + 10) * 0.05, -0.8, 0.8) * 100;
            movey = constrain((sens.distance.value[FRONT] - 500) * 0.005, -0.8, 0.6) * 100;
            
            gyTargetOffset = 0;
            gyOffsetDir = sens.distance.value[RIGHT] - sens.distance.value[LEFT] > 0 ? -1 : 1;
        }
        move.angle = atan2(movex, movey);
        move.power = constrain((float)sqrt(movex * movex + movey * movey) / 100, 0, 0.8);
        
        const int numOfPrvToUse = 10;
        double posArrayX[numOfPrvToUse], posArrayY[numOfPrvToUse];
        machinePosition.prvPos.x.getArray(posArrayX, numOfPrvToUse);
        machinePosition.prvPos.y.getArray(posArrayY, numOfPrvToUse);
        double varianceX = statistics.calculate(posArrayX, numOfPrvToUse, machinePosition.prvPos.x.None).standardDeviation;
        double varianceY = statistics.calculate(posArrayY, numOfPrvToUse, machinePosition.prvPos.y.None).standardDeviation;
        
        if(varianceX + varianceY < 10 && move.power < 0.7)
            curveShotReady++;
    }
    else{
        dribblerB.set(40);
        gyTargetOffset = 0;
        gyOffsetDir = abs(sens.gyro.value) < 15 ? 0 : gyOffsetDir;
        gyOffset = sens.gyro.value;
        
        move.angle = DONTMOVE;
        move.power = 0;
        
        curveShotReady = 0;
    }
    
    const float rotSpeed = 1;
    if(gyTargetOffset * gyOffsetDir > gyro.offset)
        gyOffset += constrain(rotSpeed, 0, gyTargetOffset * gyOffsetDir - gyro.offset);
    else if(gyTargetOffset * gyOffsetDir < gyro.offset)
        gyOffset -= constrain(rotSpeed, 0, -gyTargetOffset * gyOffsetDir + gyro.offset);
    gyro.offset = gyOffset;
    
    return move;
}

Move ShootProcessing::curveShot(SensorsArray &sensAry, Dribbler &dribblerB, Kicker &kicker, Gyro &gyro){
    Sensors &sens = sensAry.get(0);
    Move move;
    
    dribblerB.set(100);
    
    move.angle = DONTMOVE;
    gyOffset++;
    const float rotSpeed = 5;
    int rotAngle = constrain(gyOffset, 1, 180 / rotSpeed);
    gyro.offset = constrain(rotAngle * gyOffsetDir * rotSpeed , -180, 180);
    
    rotAngleWhenShot = 80;
    if(sens.gyro.value - rotAngleWhenShot > 0 && gyro.offset >= 0 && sens.gyro.value > 45)
        kicker.kick(-45, 1);
    else if(sens.gyro.value + rotAngleWhenShot < 0 && gyro.offset < 0 && sens.gyro.value < -45)
        kicker.kick(45, 1);
    
    return move;
}
