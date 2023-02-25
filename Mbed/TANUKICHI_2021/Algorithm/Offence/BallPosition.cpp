/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#include "BallPosition.h"

BallPosition::BallPosition() :
    prvPos(qtyOfPrevious),
    prvVel(qtyOfPrevious),
    prvRawPos(qtyOfPrevious),
    prvRawVel(qtyOfPrevious),
    prvTime(qtyOfPrevious, LONG_MAX),
    prvCamTime(qtyOfPrevious, LONG_MAX) {}

void BallPosition::calculate(SensorsArray &sensAry, MachinePosition &machinePosition){
//Exists
    int noneCnt = 0;
    for(int i=0; i<10; i++){
        if(sensAry.get(i).cam.bAngle == None)
            noneCnt++;
    }
    
    if(noneCnt > 5 && exists){
        if(existsCnt > 9){
            exists = false;
            return;
        }
    }
    if(!exists)
        existsCnt = 0;
    if(exists && existsCnt <= 9)
        existsCnt++;
    
    if(!exists && sensAry.get(0).cam.bAngle != None){  //出現した
        exists = true;
        
        prvRawPos.x.initialization(prvRawPos.x.None);
        prvRawPos.y.initialization(prvRawPos.y.None);
        prvRawVel.x.initialization(prvRawVel.x.None);
        prvRawVel.y.initialization(prvRawVel.y.None);
        prvCamTime.initialization(prvCamTime.None);
        prvCamTime.update(0);
        
        rawPosX = machinePosition.positionX + convert(sensAry.get(0).cam.bDistance) * sin(sensAry.get(0).cam.bAngle + sensAry.get(0).gyro.value);
        rawPosY = machinePosition.positionY + convert(sensAry.get(0).cam.bDistance) * cos(sensAry.get(0).cam.bAngle + sensAry.get(0).gyro.value);
        prvRawPos.x.update(rawPosX);
        prvRawPos.y.update(rawPosY);
        
        rawVelX = 0;
        rawVelY = 0;
        prvRawVel.x.update(rawVelX);
        prvRawVel.y.update(rawVelY);
        
        
        positionX = rawPosX;
        positionY = rawPosY;
        velocityX = rawVelX;
        velocityY = rawVelY;
        acceleX   = 0;
        acceleY   = 0;
    }
    else if(sensAry.get(0).cam.updated && sensAry.get(0).cam.bAngle != None){ //更新された
        //Time
        prvCamTime.update(0);
        for(int i=0; i<qtyOfPrevious; i++){
            if(prvCamTime.getPrv(i) != prvCamTime.None)
                prvCamTime.getPrv(i) += sensAry.get(0).cam.intervalus;
        }
        //Camの今回の値はcam.intervalus分だけ前のデータとみなす
        
        //rawPos
        rawPosX = machinePosition.positionX + convert(sensAry.get(0).cam.bDistance) * sin(sensAry.get(0).cam.bAngle + sensAry.get(0).gyro.value);
        rawPosY = machinePosition.positionY + convert(sensAry.get(0).cam.bDistance) * cos(sensAry.get(0).cam.bAngle + sensAry.get(0).gyro.value);
        prvRawPos.x.update(rawPosX);
        prvRawPos.y.update(rawPosY);
        
        //rawVel
        rawVelX = (prvRawPos.x.getPrv(0) - prvRawPos.x.getPrv(1)) * 1000000 / sensAry.get(0).cam.intervalus;
        rawVelY = (prvRawPos.y.getPrv(0) - prvRawPos.y.getPrv(1)) * 1000000 / sensAry.get(0).cam.intervalus;
        prvRawVel.x.update(rawVelX);
        prvRawVel.y.update(rawVelY);
        
        //velocity, postion
        
        const int numOfPrvToUse = 5;
        double posArrayX[numOfPrvToUse], posArrayY[numOfPrvToUse];
        prvRawPos.x.getArray(posArrayX, numOfPrvToUse);
        prvRawPos.y.getArray(posArrayY, numOfPrvToUse);
        double varianceX = statistics.calculate(posArrayX, numOfPrvToUse, prvRawPos.x.None).standardDeviation;
        positionX = statistics.average;
        double varianceY = statistics.calculate(posArrayY, numOfPrvToUse, prvRawPos.y.None).standardDeviation;
        positionY = statistics.average;
        
        if(varianceX + varianceY < 20){ //静止している時
            velocityX = 0;
            velocityY = 0;
            
            //positionX = average of previous;
            //positionY = average of previous;
        }
        else{   //動いている時
            if(abs(prvVel.x.getPrv(1)) + abs(prvVel.y.getPrv(1)) == 0){
                const float ratio = 0.8;
                velocityX = rawVelX * ratio;
                velocityY = rawVelY * ratio;
            }
            else{
                const float ratioLPF = 0.3;
                velocityX = prvVel.x.getPrv(0) * (1 - ratioLPF) + rawVelX * ratioLPF;
                velocityY = prvVel.y.getPrv(0) * (1 - ratioLPF) + rawVelY * ratioLPF;
            }
            
            positionX = prvPos.x.getPrv(0) + velocityX * sensAry.get(0).cam.intervalus * 0.8 / 1000000;
            positionY = prvPos.y.getPrv(0) + velocityY * sensAry.get(0).cam.intervalus * 0.8 / 1000000;
            
            const float ratioKLMN = 0.3;
            positionX = positionX * ratioKLMN + prvRawPos.x.getPrv(0) * (1 - ratioKLMN);
            positionY = positionY * ratioKLMN + prvRawPos.y.getPrv(0) * (1 - ratioKLMN);
        }
        
        //accele
        acceleX = 0;
        acceleY = 0;
        
    }
    else if(sensAry.get(0).cam.bAngle != None){    //更新されなかったが見えている
        positionX = prvPos.x.getPrv(0) + velocityX * sensAry.get(0).intervalus / 1000000;
        positionY = prvPos.y.getPrv(0) + velocityY * sensAry.get(0).intervalus / 1000000;
        
        const float ratioKLMN = 0.9;
        positionX = positionX * ratioKLMN + prvRawPos.x.getPrv(0) * (1 - ratioKLMN);
        positionY = positionY * ratioKLMN + prvRawPos.y.getPrv(0) * (1 - ratioKLMN);
    }
    else{   //見えていない
        positionX = prvPos.x.None;
        positionY = prvPos.y.None;
        velocityX = prvVel.x.None;
        velocityY = prvVel.y.None;
    }
    
    prvPos.x.update(positionX);
    prvPos.y.update(positionY);
    prvVel.x.update(velocityX);
    prvVel.y.update(velocityY);
    prvTime.update(0);
    for(int i=0; i<qtyOfPrevious; i++)
        prvTime.getPrv(i) += sensAry.get(0).intervalus;
    
    //--↓log用
    if(positionX < prvPos.x.None)
        prtPosX = positionX;
    if(positionY < prvPos.y.None)
        prtPosY = positionY;
    if(velocityX != prvVel.x.None)
        prtVelX = velocityX;
    else
        prtVelX = 0;
    if(velocityY != prvVel.y.None)
        prtVelY = velocityY;
    else
        prtVelY = 0;
    //--↑log用
    
    int comparison;
    for(comparison = 0; comparison<10; comparison++){
        if(prvPos.x.getPrv(comparison+1) == prvPos.x.None)
            break;
    }
    if(prvPos.x.getPrv(0) == prvPos.x.None || comparison == 0){
        roughVelX = 0;
        roughVelY = 0;
    }
    else{
        const float ratio = 0.5;
        roughVelX = (prvPos.x.getPrv(0) - prvPos.x.getPrv(comparison)) * 1000000  / (prvTime.getPrv(comparison) - prvTime.getPrv(0)) * ratio + roughVelX * (1-ratio);
        roughVelY = (prvPos.y.getPrv(0) - prvPos.y.getPrv(comparison)) * 1000000  / (prvTime.getPrv(comparison) - prvTime.getPrv(0)) * ratio + roughVelY * (1-ratio);
    }
}
