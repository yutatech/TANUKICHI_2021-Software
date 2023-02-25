/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#include "LineProcessing_defensive.h"

LineProcessing_defensive::LineProcessing_defensive() {
    lineDissappeared_time.start();
}

Move LineProcessing_defensive::main(Move move, SensorsArray &sensAry, Line &line){
    Sensors &sens = sensAry.get(0);
    
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
    getDetected(sensAry, line);
    getLineCoordinate(sensAry);
    noiseCanceler(sensAry, line);

    if(sens.line.detected.allOR())
        move.power = constrain(move.power, 0, 0.7);

    if (sens.line.detected.r) {
        int var = lineCoordinate[x] / 20;
        int target = 150;
        int xMax = line.pid[RIGHT].getPID(var, target, sens.intervalus) * 100;
        movex = constrain(movex, -200, xMax);
    }
    if (sens.line.detected.l) {
        int var = lineCoordinate[x] / 20;
        int target = -150;
        int xMin = line.pid[LEFT].getPID(var, target, sens.intervalus) * 100;
        movex = constrain(movex, xMin, 200);
    }
    if (sens.line.detected.f) {
        int var = lineCoordinate[y] / 20;
        int target = 150;
        int yMax = line.pid[FRONT].getPID(var, target, sens.intervalus) * 100;
        movey = constrain(movey, -200, yMax);
    }
    if (sens.line.detected.b) {
        int var = lineCoordinate[y] / 20;
        int target = -150;
        int yMin = line.pid[BACK].getPID(var, target, sens.intervalus) * 100;
        movey = constrain(movey, yMin, 200);
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
        movey = constrain(movey, (sens.distance.value[BACK] > 550 ? -200 : -100 * 0.7), (sens.distance.value[FRONT] > 550 ? 200 : 100 * 0.7));

    move.angle = atan2(movex, movey);
    move.power = (float)sqrt(movex * movex + movey * movey) / 100;

    if(sens.line.detected.allOR()){
        if(!sensAry.get(1).line.detected.f && !sensAry.get(1).line.detected.r && !sensAry.get(1).line.detected.b && !sensAry.get(1).line.detected.l)
            lostLineDetected.clear();
        else
            lostLineDetected = sens.line.detected;
    }

//    if(sens.line.detected.f)
//        sound(3500);
//    else if(sens.line.detected.r)
//        sound(4000);
//    else if(sens.line.detected.b)
//        sound(4500);
//    else if(sens.line.detected.l)
//        sound(5000);
//    else
//        sound(0);

    return move ;
}

void LineProcessing_defensive::noiseCanceler(SensorsArray &sensAry, Line &line){
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

void LineProcessing_defensive::getDetected(SensorsArray &sensAry, Line &line){
    Sensors &sens = sensAry.get(0);
    
    if (sens.line.coordinateY == None && sensAry.get(1).line.coordinateY != None && lineCoordinate[y] >= 4800){
        sens.line.detected.f = 0;
        line.pid[FRONT].reset();
    }
    if (sens.line.coordinateX == None && sensAry.get(1).line.coordinateX != None && lineCoordinate[x] >= 6400){
        sens.line.detected.r = 0;
        line.pid[RIGHT].reset();
    }
    if (sens.line.coordinateY == None && sensAry.get(1).line.coordinateY != None && lineCoordinate[y] <= -4800){
        sens.line.detected.b = 0;
        line.pid[BACK].reset();
    }
    if (sens.line.coordinateX == None && sensAry.get(1).line.coordinateX != None && lineCoordinate[x] <= -6400){
        sens.line.detected.l = 0;
        line.pid[LEFT].reset();
    }
    
    if (sens.line.value[FRONT] != 0 && sensAry.get(1).line.value[FRONT] == 0 && sens.line.detected.b == 0)
        sens.line.detected.f = 1;
    if (sens.line.value[RIGHT] != 0 && sensAry.get(1).line.value[RIGHT] == 0 && sens.line.detected.l == 0)
        sens.line.detected.r = 1;
    if (sens.line.value[BACK] != 0 && sensAry.get(1).line.value[BACK] == 0 && sens.line.detected.f == 0)
        sens.line.detected.b = 1;
    if (sens.line.value[LEFT] != 0 && sensAry.get(1).line.value[LEFT] == 0 && sens.line.detected.r == 0)
        sens.line.detected.l = 1;
    
    if((sens.line.detected.f | sens.line.detected.b) && !(sens.line.detected.r | sens.line.detected.l))
        firstDetected = y;
    if((sens.line.detected.r | sens.line.detected.l) && !(sens.line.detected.f | sens.line.detected.b))
        firstDetected = x;

    if (sens.line.value[FRONT] == 0 && sens.line.value[BACK] == 0 && (sens.line.detected.r | sens.line.detected.l) && firstDetected == x) {
        sens.line.detected.f = 0;
        sens.line.detected.b = 0;
    }
    if (sens.line.value[RIGHT] == 0 && sens.line.value[LEFT] == 0 && (sens.line.detected.f | sens.line.detected.b) && firstDetected == y) {
        sens.line.detected.r = 0;
        sens.line.detected.l = 0;
    }
}

void LineProcessing_defensive::getLineCoordinate(SensorsArray &sensAry){
    Sensors &sens = sensAry.get(0);
    if(!sensAry.get(1).line.detected.f && !sensAry.get(1).line.detected.r && !sensAry.get(1).line.detected.b && !sensAry.get(1).line.detected.l){
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
                mouseX = -sens.line.coordinateX * 200;
        }
        if((sensAry.get(1).line.detected.r | sensAry.get(1).line.detected.l) && !(sensAry.get(1).line.detected.f | sensAry.get(1).line.detected.b)){
            if(sens.line.detected.f | sens.line.detected.b)
                mouseY = -sens.line.coordinateY * 200;
        }
    }

    if(sens.line.coordinateX != None && abs(sens.line.coordinateX * 200 + mouseX) > 100 &&
       !(sens.line.value[FRONT] || sens.line.value[BACK]) &&
       (sens.line.detected.r | sens.line.detected.l) && !(sens.line.detected.f | sens.line.detected.b) && firstDetected == x)
        mouseX = -((lineCoordinate[x] - sens.mouse.deltaX) * 0.5 + sens.line.coordinateX * 200 * 0.5);

    if(sens.line.coordinateY != None && abs(sens.line.coordinateY * 200 + mouseY) > 100 &&
       !(sens.line.value[RIGHT] || sens.line.value[LEFT]) &&
       (sens.line.detected.f | sens.line.detected.b) && !(sens.line.detected.r | sens.line.detected.l) && firstDetected == y)
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
