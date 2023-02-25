/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#include "BallTracking_PS.h"

BallTracking_PS::BallTracking_PS(MachinePosition &_machinePosition, BallPosition &_ballPosition) :
    machinePosition(_machinePosition),
    ballPosition(_ballPosition),
    ballX(0.015, 0, 0.0015, 0, -1.2, 1.2) {}

Move BallTracking_PS::frontSide(Sensors sens, int speed){
    Move move;
    speed = constrain(speed, 10, 200);
    
    if(sens.cam.bAngle != None)
        sens.cam.bAngle += sens.gyro.value;
    
    if(sens.cam.bAngle == None){
        move.angle = DONTMOVE;
        move.power = 1;
        ballX.reset();
    }
    else{
        int r = 35;
        double relPosX = ballPosition.positionX - machinePosition.positionX;
        double relPosY = ballPosition.positionY - machinePosition.positionY;
        
        int movex = 0;
        int movey = 0;
        if((abs(relPosX) < 150 && relPosY < 300 && abs(sens.cam.bAngle) < 15) ||
           (abs(relPosX) < 150 && 150 < relPosY && relPosY < 300)){
            movex = ballX.getPID(relPosX, 0, sens.intervalus) * 100;
            movey = constrain(12.3 + 5 - sqrt(abs(constrain(relPosX, -150,150))), 0, 12.3) / 12.3 * 100;
            movey *= constrain(sens.cam.bDistance-30, 20, 30) / 30.0;
            
            movey = constrain(movey * speed / 100, 0, 120) * 100 / speed;
            
            movex += ballPosition.velocityX / 7 * 200 / constrain(speed, 50,200);
            movey += ballPosition.velocityY / 7 * 200 / constrain(speed, 50,200);
        }
        else if(abs(relPosX) < 150 && abs(sens.cam.bAngle) < 15){
            movex = ballX.getPID(relPosX, 0, sens.intervalus) * 100;
            movey = 120;
            movey *= constrain(sens.cam.bDistance-20, 10, 20) / 20.0;
        }
        else if(sens.cam.bDistance < r){
            int roundDir = sens.cam.bAngle >= 0 ? 1 : -1;
            int anglePlus = (r - sens.cam.bDistance) * 2.5 + 90;
            //int anglePlus = constrain(abs(sens.cam.bAngle) * 0.6, 0, 90);
            
            if(abs(sens.cam.bAngle) < 45)
                move.angle = 90 * roundDir;
            else
                move.angle = sens.cam.bAngle + anglePlus * roundDir;
            if(abs(sens.cam.bAngle) > 120)
                move.power = 1;
            else if(abs(sens.cam.bAngle) > 30)
                move.power = 1.2;
            else
                move.power = 0.8;
        
            movex = 100 * move.power * sin(move.angle);
            movey = 100 * move.power * cos(move.angle);
            
            movex += ballPosition.velocityX / 7 * 200 / constrain(speed, 50,200);
            movey += ballPosition.velocityY / 7 * 200 / constrain(speed, 50,200);
            ballX.reset();
        }
        else{
            int roundDir = sens.cam.bAngle >= 0 ? 1 : -1;
            move.angle = sens.cam.bAngle + asin(r / (float)sens.cam.bDistance) * 180 / 3.14 * 1 * roundDir;
            move.power = abs(sens.cam.bAngle) > 80 ? 1.2 : 1;
            
            movex = 100 * move.power * sin(move.angle);
            movey = 100 * move.power * cos(move.angle);
            
            if(sens.cam.bDistance < 80){
                movex += ballPosition.velocityX / 7 * 200 / constrain(speed, 50,200);
                movey += ballPosition.velocityY / 7 * 200 / constrain(speed, 50,200);
            }
            else{
                movex *= 1.5;
                movey *= 1.5;
            }
            ballX.reset();
        }
        move.angle = atan2(movex, movey);
        move.power = (float)sqrt(movex * movex + movey * movey) / 100.0;
    }
    
    return move;
}

Move BallTracking_PS::backSide(Sensors sens, int speed){
    Move move;
    
    if(sens.cam.bAngle != None)
        sens.cam.bAngle += sens.gyro.value;
    
    if(sens.cam.bAngle == None){
        move.angle = DONTMOVE;
        move.power = 1;
        ballX.reset();
    }
    else{
        int r = 35;
        double relPosX = ballPosition.positionX - machinePosition.positionX;
        double relPosY = ballPosition.positionY - machinePosition.positionY;
        
        int movex = 0;
        int movey = 0;
        if((abs(relPosX) < 150 && relPosY > -300 && abs(sens.cam.bAngle) > 165) ||
           (abs(relPosX) < 150 && -150 > relPosY && relPosY > -300) ){
            movex = ballX.getPID(relPosX, 0, sens.intervalus) * 100;
            movey = -(150 - constrain(abs(relPosX), 0,150)) / 200 * 100;
            movey *= constrain(sens.cam.bDistance-20, 15, 20) / 20.0;
            
            movex += ballPosition.velocityX / 8;
            movey += ballPosition.velocityY / 8;
        }
        else if(abs(relPosX) < 150 && abs(sens.cam.bAngle) > 165){
            movex = ballX.getPID(relPosX, 0, sens.intervalus) * 100;
            movey = -80;
            movey *= constrain(sens.cam.bDistance-20, 15, 20) / 20.0;
        }
        else if(sens.cam.bDistance < r){
            int roundDir = sens.cam.bAngle <= 0 ? 1 : -1;
            int anglePlus = (r - sens.cam.bDistance) * 2.5 + 90;
            //int anglePlus = constrain(abs(sens.cam.bAngle) * 0.6, 0, 90);
            
            if(abs(sens.cam.bAngle) > 135)
                move.angle = 90 * roundDir;
            else
                move.angle = sens.cam.bAngle + anglePlus * roundDir;
            
            move.power = 1;
        
            movex = 100 * move.power * sin(move.angle);
            movey = 100 * move.power * cos(move.angle);
            
            movex += ballPosition.velocityX / 8;
            movey += ballPosition.velocityY / 8;
            ballX.reset();
        }
        else{
            int roundDir = sens.cam.bAngle <= 0 ? 1 : -1;
            move.angle = sens.cam.bAngle + asin(r / (float)sens.cam.bDistance) * 180 / 3.14 * roundDir;
            move.power = abs(sens.cam.bAngle) < 100 ? 1.2 : 1;
            
            movex = 100 * move.power * sin(move.angle);
            movey = 100 * move.power * cos(move.angle);
            ballX.reset();
        }
        move.angle = atan2(movex, movey);
        move.power = (float)sqrt(movex * movex + movey * movey) / 100.0;
    }
    
    return move;
}
