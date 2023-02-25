/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#include "LineProcessing_aggressive.h"

LineProcessing_aggressive::LineProcessing_aggressive(MachinePosition &_machinePosition) :
    machinePosition(_machinePosition),
    passedXY(30)
{
    passedXY.x.update(0);
    passedXY.y.update(0);
    lineDissappeared_time.start();
}

Move LineProcessing_aggressive::main(Move move, SensorsArray &sensAry, Line &line){
    Sensors &sens = sensAry.get(0);
    
    if((passedXY.x.getPrv(0)-machinePosition.positionX)*(passedXY.x.getPrv(0)-machinePosition.positionX) + (passedXY.y.getPrv(0)-machinePosition.positionY)*(passedXY.y.getPrv(0)-machinePosition.positionY) > 2500){
        passedXY.x.update(machinePosition.positionX);
        passedXY.y.update(machinePosition.positionY);
    }
    
    int movex, movey;
    if (move.angle == DONTMOVE) {
        movex = 0;
        movey = 0;
    }
    else {
        movex = 100 * move.power * sin(move.angle);
        movey = 100 * move.power * cos(move.angle);
    }
    sens.mouse.apply(mouseX, mouseY);
//    recoverDetected(sensAry);
    getDetected(sensAry);
    getLineCoordinate(sensAry);
    switchDetected_for_penaltyArea(sensAry, line);
    clearDetected(sensAry, line);
    switchDetected_for_corner(sensAry, line);
    noiseCanceler(sensAry, line);

    if(sens.line.detected.allOR())
        move.power = constrain(move.power, 0, 0.7);

    if (sens.line.detected.r) {
        int var = lineCoordinate[x] / 20;
        int target = (firstDetected == x ? -280 : 100);
        int xMax = line.pid[RIGHT].getPID(var, target, sens.intervalus) * 100;
        movex = constrain(movex, -200, xMax);
        if(movex > 0)
            movex *= 0.5;

        if(abs(var - target) < 40 && range(0, sens.cam.bAngle, 110))
            movey = cos(sens.cam.bAngle) * 100 * 0.6;
    }
    if (sens.line.detected.l) {
        int var = lineCoordinate[x] / 20;
        int target = (firstDetected == x ? 280 : -100);
        int xMin = line.pid[LEFT].getPID(var, target, sens.intervalus) * 100;
        movex = constrain(movex, xMin, 200);
        if(movex < 0)
            movex *= 0.5;

        if(abs(var - target) < 40 && range(-110, sens.cam.bAngle, 0))
            movey = cos(sens.cam.bAngle) * 100 * 0.6;
    }
    if (sens.line.detected.f) {
        int var = lineCoordinate[y] / 20;
        int target = (firstDetected == y ? -200 : 100);
        int yMax = line.pid[FRONT].getPID(var, target, sens.intervalus) * 100;
        movey = constrain(movey, -200, yMax);
        if(movey > 0)
            movey *= 0.5;
    }
    if (sens.line.detected.b) {
        int var = lineCoordinate[y] / 20;
        int target = -200;
        int yMin = line.pid[BACK].getPID(var, target, sens.intervalus) * 100;
        movey = constrain(movey, yMin, 200);
        if(movey < 0)
            movey *= 0.5;
    }

    if ((sens.line.value[FRONT] | sens.line.value[RIGHT] | sens.line.value[BACK] | sens.line.value[LEFT]) != 0)
        lineDissappeared_time.reset();

    if (lineDissappeared_time.read_ms() > 2000 && (sens.line.detected.allOR()) &&
        !(sens.line.value[FRONT] || sens.line.value[RIGHT] || sens.line.value[BACK]  || sens.line.value[LEFT])) {
        sens.line.detected.clear();
    }
    else if (lineDissappeared_time.read_ms() > 1500 && (sens.line.detected.allOR()) &&
             !(sens.line.value[FRONT] | sens.line.value[RIGHT] | sens.line.value[BACK]  | sens.line.value[LEFT])) {
        movex = 0;
        movey = 0;
    }

    if((sens.line.detected.f | sens.line.detected.b) && !(sens.line.detected.r | sens.line.detected.l))
        movex = constrain(movex, -100 * 0.7, 100 * 0.7);
    if((sens.line.detected.r | sens.line.detected.l) && !(sens.line.detected.f | sens.line.detected.b))
        movey = constrain(movey, (sens.distance.value[BACK] > 550 ? -192 : -100 * 0.7), (sens.distance.value[FRONT] > 550 ? 192 : 100 * 0.7));

    move.angle = atan2(movex, movey);
    move.power = (float)sqrt(movex * movex + movey * movey) / 100;

    if(sens.line.detected.allOR()){
        if(!sensAry.get(1).line.detected.f && !sensAry.get(1).line.detected.r && !sensAry.get(1).line.detected.b && !sensAry.get(1).line.detected.l)
            lostLineDetected.clear();
        else
            lostLineDetected = sens.line.detected;
    }

    if(sens.line.detected.f)
        sound(3500);
    else if(sens.line.detected.r)
        sound(4000);
    else if(sens.line.detected.b)
        sound(4500);
    else if(sens.line.detected.l)
        sound(5000);
    else
        sound(0);

    return move ;
}

void LineProcessing_aggressive::noiseCanceler(SensorsArray &sensAry, Line &line){
    Sensors &sens = sensAry.get(0);
    if(!(sens.line.value[FRONT] || sens.line.value[RIGHT] || sens.line.value[BACK]  || sens.line.value[LEFT])){
        if(line.detectCounter(noiseCancelCounter.value[FRONT]) + line.detectCounter(noiseCancelCounter.value[RIGHT]) + line.detectCounter(noiseCancelCounter.value[BACK]) + line.detectCounter(noiseCancelCounter.value[LEFT]) < 20){
            sens.line.detected.clear();
            sensAry.get(1).line.detected.clear();
            line.pid[FRONT].reset();
            line.pid[RIGHT].reset();
            line.pid[BACK].reset();
            line.pid[LEFT].reset();
            noiseCancelCounter.value[FRONT] = 0;
            noiseCancelCounter.value[RIGHT] = 0;
            noiseCancelCounter.value[BACK] = 0;
            noiseCancelCounter.value[LEFT] = 0;
        }
    }
    else{
        noiseCancelCounter.value[FRONT] |= sens.line.value[FRONT];
        noiseCancelCounter.value[RIGHT] |= sens.line.value[RIGHT];
        noiseCancelCounter.value[BACK] |= sens.line.value[BACK];
        noiseCancelCounter.value[LEFT] |= sens.line.value[LEFT];
    }
    if(!sens.line.detected.allOR()){
        noiseCancelCounter.value[FRONT] = 0;
        noiseCancelCounter.value[RIGHT] = 0;
        noiseCancelCounter.value[BACK] = 0;
        noiseCancelCounter.value[LEFT] = 0;
    }
}

void LineProcessing_aggressive::recoverDetected(SensorsArray &sensAry){
    if(sensAry.get(0).line.value[FRONT] != 0 && !sensAry.get(1).line.value[FRONT] == 0){
        int rcvTar;
        for(rcvTar=2; rcvTar<=3; rcvTar++){
            if(sensAry.get(rcvTar).line.value[FRONT] != 0)
                break;
        }
        if(rcvTar < 4)
            sensAry.get(0).line.detected = sensAry.get(1).line.detected = sensAry.get(rcvTar).line.detected;
    }
    if(sensAry.get(0).line.value[RIGHT] != 0 && !sensAry.get(1).line.value[RIGHT] == 0){
        int rcvTar;
        for(rcvTar=2; rcvTar<=3; rcvTar++){
            if(sensAry.get(rcvTar).line.value[RIGHT] != 0)
                break;
        }
        if(rcvTar < 4)
            sensAry.get(0).line.detected = sensAry.get(1).line.detected = sensAry.get(rcvTar).line.detected;
    }
    if(sensAry.get(0).line.value[BACK] != 0 && !sensAry.get(1).line.value[BACK] == 0){
        int rcvTar;
        for(rcvTar=2; rcvTar<=3; rcvTar++){
            if(sensAry.get(rcvTar).line.value[BACK] != 0)
                break;
        }
        if(rcvTar < 4)
            sensAry.get(0).line.detected = sensAry.get(1).line.detected = sensAry.get(rcvTar).line.detected;
    }
    if(sensAry.get(0).line.value[LEFT] != 0 && !sensAry.get(1).line.value[LEFT] == 0){
        int rcvTar;
        for(rcvTar=2; rcvTar<=3; rcvTar++){
            if(sensAry.get(rcvTar).line.value[LEFT] != 0)
                break;
        }
        if(rcvTar < 4)
            sensAry.get(0).line.detected = sensAry.get(1).line.detected = sensAry.get(rcvTar).line.detected;
    }
}

void LineProcessing_aggressive::getDetected(SensorsArray &sensAry){
    Sensors &sens = sensAry.get(0);
    
    if(!sensAry.get(1).line.detected.f && !sensAry.get(1).line.detected.r && !sensAry.get(1).line.detected.b && !sensAry.get(1).line.detected.l){
        if (sens.line.value[FRONT] != 0 && sensAry.get(1).line.value[FRONT] == 0 && sens.line.detected.b == 0)
            sens.line.detected.f = 1;
        else if (sens.line.value[BACK] != 0  && sensAry.get(1).line.value[BACK]  == 0 && sens.line.detected.f == 0)
            sens.line.detected.b = 1;
        else if (sens.line.value[RIGHT] != 0 && sensAry.get(1).line.value[RIGHT] == 0 && sens.line.detected.l == 0)
            sens.line.detected.r = 1;
        else if (sens.line.value[LEFT] != 0  && sensAry.get(1).line.value[LEFT]  == 0 && sens.line.detected.r == 0)
            sens.line.detected.l = 1;
    }
    else{
        if(sens.line.detected.f && (sens.line.value[RIGHT] || sens.line.value[BACK]  || sens.line.value[LEFT])){
            if((sens.line.value[BACK] & 0b1111000000) && sens.line.coordinateY > -22 && mouseX > 2000  && !sens.line.detected.l
               && !(!sens.line.value[LEFT] && sens.line.value[RIGHT]))
                sens.line.detected.r = 1;
            if((sens.line.value[BACK] & 0b1111000000) && sens.line.coordinateY > -22 && mouseX < -2000 && !sens.line.detected.r
               && !(!sens.line.value[RIGHT] && sens.line.value[LEFT]))
                sens.line.detected.l = 1;
        }
        if(sens.line.detected.b && (sens.line.value[FRONT] || sens.line.value[RIGHT] || sens.line.value[LEFT])){
            if((sens.line.value[FRONT] & 0b1111000000) && sens.line.coordinateY < 22 && mouseX > 2000  && !sens.line.detected.l
               && !(!sens.line.value[LEFT] && sens.line.value[RIGHT]))
                sens.line.detected.r = 1;
            if((sens.line.value[FRONT] & 0b1111000000) && sens.line.coordinateY < 22 && mouseX < -2000 && !sens.line.detected.r
               && !(!sens.line.value[RIGHT] && sens.line.value[LEFT]))
                sens.line.detected.l = 1;
        }
        if(sens.line.detected.r && (sens.line.value[FRONT] || sens.line.value[BACK]  || sens.line.value[LEFT])){
            if((sens.line.value[LEFT] & 0b111100000000) && sens.line.coordinateX > -30 && mouseY > 2000  && !sens.line.detected.b
               && !(!sens.line.value[BACK] && sens.line.value[FRONT]))
                sens.line.detected.f = 1;
            if((sens.line.value[LEFT] & 0b111100000000) && sens.line.coordinateX > -30 && mouseY < -2000 && !sens.line.detected.f
               && !(!sens.line.value[FRONT] && sens.line.value[BACK]))
                sens.line.detected.b = 1;
        }
        if(sens.line.detected.l && (sens.line.value[FRONT] || sens.line.value[RIGHT] || sens.line.value[BACK])){
            if((sens.line.value[RIGHT] & 0b111100000000) && sens.line.coordinateX < 30 && mouseY > 2000  && !sens.line.detected.b
               && !(!sens.line.value[BACK] && sens.line.value[FRONT]))
                sens.line.detected.f = 1;
            if((sens.line.value[RIGHT] & 0b111100000000) && sens.line.coordinateX < 30 && mouseY < -2000 && !sens.line.detected.f
               && !(!sens.line.value[FRONT] && sens.line.value[BACK]))
                sens.line.detected.b = 1;
        }
    }
    
//    const int numOfPrvToUse = 10;
//    if(!sensAry.get(1).line.detected.f && sens.line.detected.f){
//        double posArrayY[numOfPrvToUse];
//        passedXY.y.getArray(posArrayY, passedXY.y.stock - numOfPrvToUse - 1, numOfPrvToUse);
//        if(statistics.calculate(posArrayY, numOfPrvToUse, passedXY.y.None).average > machinePosition.positionY + 150){
//            sens.line.detected.f = false;
//            sens.line.detected.b = true;
//        }
//    }
//    if(!sensAry.get(1).line.detected.b && sens.line.detected.b){
//        double posArrayY[numOfPrvToUse];
//        passedXY.y.getArray(posArrayY, passedXY.y.stock - numOfPrvToUse - 1, numOfPrvToUse);
//        if(statistics.calculate(posArrayY, numOfPrvToUse, passedXY.y.None).average < machinePosition.positionY - 150){
//            sens.line.detected.b = false;
//            sens.line.detected.f = true;
//        }
//    }
//    if(!sensAry.get(1).line.detected.r && sens.line.detected.r){
//        double posArrayX[numOfPrvToUse];
//        passedXY.x.getArray(posArrayX, passedXY.x.stock - numOfPrvToUse - 1, numOfPrvToUse);
//        if(statistics.calculate(posArrayX, numOfPrvToUse, passedXY.x.None).average > machinePosition.positionX + 150){
//            sens.line.detected.r = false;
//            sens.line.detected.l = true;
//        }
//    }
//    if(!sensAry.get(1).line.detected.l && sens.line.detected.l){
//        double posArrayX[numOfPrvToUse];
//        passedXY.x.getArray(posArrayX, passedXY.x.stock - numOfPrvToUse - 1, numOfPrvToUse);
//        if(statistics.calculate(posArrayX, numOfPrvToUse, passedXY.x.None).average < machinePosition.positionX - 150){
//            sens.line.detected.l = false;
//            sens.line.detected.r = true;
//        }
//    }

    if((sens.line.detected.f | sens.line.detected.b) && !(sens.line.detected.r | sens.line.detected.l))
        firstDetected = y;
    if((sens.line.detected.r | sens.line.detected.l) && !(sens.line.detected.f | sens.line.detected.b))
        firstDetected = x;
}

void LineProcessing_aggressive::switchDetected_for_penaltyArea(SensorsArray &sensAry, Line &line){
    Sensors &sens = sensAry.get(0);
    if(sens.line.detected.f == 1 && sens.line.detected.r == 0 &&
       sens.line.coordinateX != None && sens.line.coordinateX <= -26 && sens.line.coordinateY == None && mouseX >  2000){
        sens.line.detected.f = 0;
        sens.line.detected.l = 1;
        line.pid[FRONT].reset();
    }
    else if(sens.line.detected.f == 1 && sens.line.detected.l == 0 &&
            sens.line.coordinateX != None && sens.line.coordinateX >=  26 && sens.line.coordinateY == None && mouseX < -2000){
        sens.line.detected.f = 0;
        sens.line.detected.r = 1;
        line.pid[FRONT].reset();
    }
    if(sens.line.detected.r == 1 && sens.line.detected.b == 0 &&
       sens.line.coordinateY != None && sens.line.coordinateY >=  18 && sens.line.coordinateX == None && mouseY < -2000){
        sens.line.detected.r = 0;
        sens.line.detected.f = 1;
        line.pid[RIGHT].reset();
    }
    if(sens.line.detected.l == 1 && sens.line.detected.b == 0 &&
       sens.line.coordinateY != None && sens.line.coordinateY >=  18 && sens.line.coordinateX == None && mouseY < -2000){
        sens.line.detected.l = 0;
        sens.line.detected.f = 1;
        line.pid[LEFT].reset();
    }
}

void LineProcessing_aggressive::clearDetected(SensorsArray &sensAry, Line &line){
    Sensors &sens = sensAry.get(0);
    if ((sens.line.coordinateY == None && sensAry.get(1).line.coordinateY != None && lineCoordinate[y] >= 4000) ||
        (firstDetected == y && lineCoordinate[y] >= 5700) ||
        (firstDetected == x && lineCoordinate[y] >= 4000) ){
        sens.line.detected.f = 0;
        line.pid[FRONT].reset();
    }
    if ((sens.line.coordinateX == None && sensAry.get(1).line.coordinateX != None && lineCoordinate[x] >= 6500) ||
        (firstDetected == x && lineCoordinate[x] >= 7300) ||
        (firstDetected == y && lineCoordinate[x] >= 6500) ){
        sens.line.detected.r = 0;
        line.pid[RIGHT].reset();
    }
    if ((sens.line.coordinateY == None && sensAry.get(1).line.coordinateY != None && lineCoordinate[y] <= -4000) ||
        (firstDetected == y && lineCoordinate[y] <= -5700) ||
        (firstDetected == x && lineCoordinate[y] <= -4000) ){
        sens.line.detected.b = 0;
        line.pid[BACK].reset();
    }
    if ((sens.line.coordinateX == None && sensAry.get(1).line.coordinateX != None && lineCoordinate[x] <= -6500) ||
        (firstDetected == x && lineCoordinate[x] <= -7300) ||
        (firstDetected == y && lineCoordinate[x] <= -6500) ){
        sens.line.detected.l = 0;
        line.pid[LEFT].reset();
    }
}

void LineProcessing_aggressive::switchDetected_for_corner(SensorsArray &sensAry, Line &line){
    Sensors &sens = sensAry.get(0);
    if(sens.line.detected.f == 0 && sensAry.get(1).line.detected.f == 1){
        if(sens.line.coordinateX != None && sens.line.coordinateX > 0 && sens.line.detected.r == 0 && sens.line.detected.l == 0)
            sens.line.detected.r = 1;
        if(sens.line.coordinateX != None && sens.line.coordinateX < 0 && sens.line.detected.r == 0 && sens.line.detected.l == 0)
            sens.line.detected.l = 1;
    }
    if(sens.line.detected.r == 0 && sensAry.get(1).line.detected.r == 1){
        if(sens.line.coordinateY != None && sens.line.coordinateY > 0 && sens.line.detected.f == 0 && sens.line.detected.b == 0)
            sens.line.detected.f = 1;
        if(sens.line.coordinateY != None && sens.line.coordinateY < 0 && sens.line.detected.f == 0 && sens.line.detected.b == 0)
            sens.line.detected.b = 1;
    }
    if(sens.line.detected.b == 0 && sensAry.get(1).line.detected.b == 1){
        if(sens.line.coordinateX != None && sens.line.coordinateX > 0 && sens.line.detected.r == 0 && sens.line.detected.l == 0)
            sens.line.detected.r = 1;
        if(sens.line.coordinateX != None && sens.line.coordinateX < 0 && sens.line.detected.r == 0 && sens.line.detected.l == 0)
            sens.line.detected.l = 1;
    }
    if(sens.line.detected.l == 0 && sensAry.get(1).line.detected.l == 1){
        if(sens.line.coordinateY != None && sens.line.coordinateY > 0 && sens.line.detected.f == 0 && sens.line.detected.b == 0)
            sens.line.detected.f = 1;
        if(sens.line.coordinateY != None && sens.line.coordinateY < 0 && sens.line.detected.f == 0 && sens.line.detected.b == 0)
            sens.line.detected.b = 1;
    }
}

void LineProcessing_aggressive::getLineCoordinate(SensorsArray &sensAry){
    Sensors &sens = sensAry.get(0);
    if(!sensAry.get(1).line.detected.f &&!sensAry.get(1).line.detected.r && !sensAry.get(1).line.detected.b && !sensAry.get(1).line.detected.l){
        if(sens.line.detected.r | sens.line.detected.l){
            mouseX = -sens.line.coordinateX * 200;
            mouseY = 0;
        }
        else if(sens.line.detected.f | sens.line.detected.b){
            mouseX = 0;
            mouseY = -sens.line.coordinateY * 200;
        }
    }
    else{
        if((sensAry.get(1).line.detected.f | sensAry.get(1).line.detected.b) && !(sensAry.get(1).line.detected.r | sensAry.get(1).line.detected.l)){
            if(sens.line.detected.r | sens.line.detected.l)
                mouseX = 0;
        }
        if((sensAry.get(1).line.detected.r | sensAry.get(1).line.detected.l) && !(sensAry.get(1).line.detected.f | sensAry.get(1).line.detected.b)){
            if(sens.line.detected.f | sens.line.detected.b)
                mouseY = 0;
        }
    }

    if(sens.line.coordinateX != None && abs(sens.line.coordinateX * 200 + mouseX) > 100 &&
       !(sens.line.value[FRONT] || sens.line.value[BACK]) &&
       (sens.line.detected.r | sens.line.detected.l) && !(sens.line.detected.f | sens.line.detected.b) && firstDetected == x)
        mouseX = -((lineCoordinate[x] - sens.mouse.deltaX) * 0.5 + sens.line.coordinateX * 200 * 0.5);

    if(sens.line.coordinateY != None && abs(sens.line.coordinateY * 200 + mouseY) > 100 &&
       !(sens.line.value[RIGHT] || sens.line.value[LEFT]) &&
       (sens.line.detected.f | sens.line.detected.b) && !(sens.line.detected.r | sens.line.detected.l) && firstDetected == y &&
       abs(lineCoordinate[y] - sens.line.coordinateY * 200) < 10000)
        mouseY = -((lineCoordinate[y] - sens.mouse.deltaY) * 0.5 + sens.line.coordinateY * 200 * 0.5);

//    if(sens.line.coordinateX == None && sens.line.detected.r && !(sens.line.detected.f | sens.line.detected.b))
//        mouseX = -(-7200);
//    if(sens.line.coordinateX == None && sens.line.detected.l && !(sens.line.detected.f | sens.line.detected.b))
//        mouseX = -(7200);
//    if(sens.line.coordinateY == None && sens.line.detected.f && !(sens.line.detected.r | sens.line.detected.l))
//        mouseY = -(-5600);
//    if(sens.line.coordinateY == None && sens.line.detected.b && !(sens.line.detected.r | sens.line.detected.l))
//        mouseY = -(5600);

    lineCoordinate[x] = (sens.line.detected.r | sens.line.detected.l) ? -mouseX : None;
    lineCoordinate[y] = (sens.line.detected.f | sens.line.detected.b) ? -mouseY : None;
}
