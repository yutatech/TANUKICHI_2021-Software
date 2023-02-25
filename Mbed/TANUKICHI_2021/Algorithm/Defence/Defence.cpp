/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#include "Defence.h"
#if defined(POSITION_DF)

Algorithm::Algorithm(Devices &devices, Settings &settings) :
    cam(devices.cam),
    line(devices.line),
    distance(devices.distance),
    gyro(devices.gyro),
    mouse(devices.mouse),
    hold(devices.hold),
    motor(devices.motor),
    kicker(devices.kicker),
    dribbler(devices.dribblerB),
    speed(settings.data[settings.addSpeed]),
    ballX(0.15, 0, 0.00, 0, -1.5, 1.5),
    goalL(0.09, 0, 0.02, 0, -1.5, 1.5),
    goalR(0.09, 0, 0.02, 0, -1.5, 1.5),
    goalC(0.05, 0, 0.01, 0, -1.2, 1.2),
    vsRiOneNano_CatBot(getBit(settings.data[settings.addState], SvsRiOneNano_CatBot)),
    detectGoal(getBit(settings.data[settings.addState], SdetectGoal)),
    useWireless(getBit(settings.data[settings.addState], Swireless))
{
    sysTime.start();
    intervalT.start();
}

void Algorithm::move() {
    Move move;
    Sensors sensors;
    interval = constrain(intervalT.read_us(), 10, 50000);
    intervalT.reset();
    gyro.cycle_us = interval;
    
    cam.getValue(sensors.cam);
    line.getValue(sensors.line);
    distance.getValue(sensors.distance);
    gyro.getValue(sensors.gyro);
    mouse.getValue(sensors.mouse, sensors.gyro.value);
    hold.getValue(sensors.hold);
    
    line.lifted = false;
    
    motor.EnablePID(true);
    motor.setPID(gyro.getPID());
    
    /*if(wirelessCount++ > 10 && useWireless){
     wireless.putc('a');
     wirelessCount = 0;
     }
     wireless.putc('a');*/
    
    bool isSpeed = false;
    if(speed == 0)
        isSpeed = speed = 1;
    
//    move.angle = a++;
//    move.power = 1.0;
    
    if(detectGoal){
        move = process_ball(sensors);
    } else {
        move = process_balldistance(sensors);
    }

    if(useWireless){
        move = process_line(move, sensors);
    }
    
    if(isSpeed){
        speed = 0;
        move.angle = DONTMOVE;
    }
    
    motor.setMove(move,speed);
    prevSensors = sensors;
}

Move Algorithm::process_ball(Sensors sensors){
    Move move;
    //gyro.offset = 0;
    if((abs(sensors.cam.bAngle) < 30 && sensors.cam.bDistance < 75) || (abs(sensors.cam.bAngle) < 10 && sensors.cam.bDistance < 85)){
        dribbler.set(60);
    }
    else{
        dribbler.set(0);
    }
    if(sensors.cam.gAngleC == None){
        move = process_balldistance(sensors);
    } else {
        if(sensors.cam.bAngle != None){
            if(abs(sensors.cam.bAngle) <= 110 || sensors.distance.value[BACK] < 300){
                int movex;
                int movey;
                int gR = sensors.cam.gAngleR + sensors.gyro.value;
                int gL = sensors.cam.gAngleL + sensors.gyro.value;
                int l = gR > 0 ? 180 - gR : -180 - gR;
                int r = gL > 0 ? 180 - gL : -180 - gL;
                
                if(abs(sensors.cam.bAngle) <= 110)
                    movex = ballX.getPID(sensors.cam.bAngle + sensors.gyro.value, 0, interval) * speed;
                else
                    movex = sensors.cam.bAngle < 0 ? ballX.getPID(sensors.cam.bAngle, -180, interval) * -speed * 0.8 : ballX.getPID(sensors.cam.bAngle, 180, interval) * -speed * 0.8;
                int Min,Max;
                if(0 < sensors.cam.bDistance * cos(sensors.cam.bAngle) && sensors.cam.bDistance * cos(sensors.cam.bAngle) < 65){
                    Min = goalL.getPID(l, 12, interval) * speed;
                    Max = goalR.getPID(r, -12, interval) * speed;
                }
                else{
                    Min = goalL.getPID(l, -18, interval) * speed;
                    Max = goalR.getPID(r, 18, interval) * speed;
                }
                movex = constrain(movex, Min, Max);
                
                if(l < 12 && r > -12)
                    movey = (200 - sensors.distance.value[BACK]) * speed * 0.003;
                else
                    movey = (500 - sensors.distance.value[BACK]) * speed * 0.003;
                if(sensors.distance.value[BACK] < 300 && 90 < abs(sensors.cam.bAngle)){
                    if(30 < abs(l) && 30 < abs(r))
                        movey = (145 - sensors.distance.value[BACK] + sensors.cam.bDistance * sin(180 - abs(sensors.cam.bAngle)) + 30) * speed * 0.005;
                }
                
                move.angle = atan2(movex, movey);
                move.power = (float)sqrt(movex * movex + movey * movey) / (float)speed;
            } else {
                move.angle = sensors.cam.bAngle * 1.4;
                move.power = 1.2;
            }
            
            if(sensors.distance.value[BACK] <= 800 && abs(sensors.cam.bDistance * sin(sensors.cam.bAngle)) < 25){
                if(sensors.cam.bDistance < 55){
                    if(abs(sensors.cam.gAngleC) < 30)
                        move.angle = constrain(sensors.cam.bAngle, -45, 45);
                    else if(0 <= sensors.cam.gAngleC)
                        move.angle = constrain(sensors.cam.bAngle, -45, 0);
                    else if(0 >= sensors.cam.gAngleC)
                        move.angle = constrain(sensors.cam.bAngle, 0, 45);
                    else
                        move.angle = 0;
                        
                    move.power = 1.4;
                    dribbler.set(80);
                } else if(sensors.cam.bDistance < 70){
                    if(150 < ballcount++){
                        if(abs(sensors.cam.gAngleC) < 30)
                            move.angle = constrain(sensors.cam.bAngle, -45, 45);
                        else if(0 <= sensors.cam.gAngleC)
                            move.angle = constrain(sensors.cam.bAngle, -45, 0);
                        else if(0 >= sensors.cam.gAngleC)
                            move.angle = constrain(sensors.cam.bAngle, 0, 45);
                        else
                            move.angle = 0;
                        
                        move.power = 1.4;
                        dribbler.set(80);
                    }
                } else if(sensors.cam.bDistance < 85){
                    if(450 < ballcount++){
                        if(abs(sensors.cam.gAngleC) < 30)
                            move.angle = constrain(sensors.cam.bAngle, -45, 45);
                        else if(0 <= sensors.cam.gAngleC)
                            move.angle = constrain(sensors.cam.bAngle, -45, 0);
                        else if(0 >= sensors.cam.gAngleC)
                            move.angle = constrain(sensors.cam.bAngle, 0, 45);
                        else
                            move.angle = 0;
                        
                        move.power = 1.4;
                        dribbler.set(80);
                    }
                } else {
                    ballcount = 0;
                }
            } else {
                ballcount = 0;
            }
        } else {
            int c = sensors.cam.gAngleC > 0 ? 180 - sensors.cam.gAngleC : -180 - sensors.cam.gAngleC;
            int movex = goalC.getPID(c, 0, interval) * speed;
            int movey = (250 - sensors.distance.value[BACK]) * speed * 0.005;
            move.angle = atan2(movex, movey);
            move.power = sensors.distance.value[BACK] < 500 ? (float)sqrt(movex * movex + movey * movey) / (float)speed : 1.2;
            if(abs(sensors.cam.gAngleC) < 140){
                move.power = 0.8;
                if(sensors.cam.gAngleC > 0)
                    move.angle = 45;
                else
                    move.angle = -45;
            }
//            if(500 < sensors.distance.value[BACK]){
//                move.angle = (sensors.cam.gAngleC != None && 100 < abs(sensors.cam.gAngleC) && sensors.distance.value[BACK] < 800) ? sensors.cam.gAngleC * 1.5 : 180;
//            }
            if(abs(sensors.cam.gAngleC) < 140){
                move.power = 0.8;
                if(sensors.cam.gAngleC > 0)
                    move.angle = 45;
                else
                    move.angle = -45;
            }
            
        }
    }
    return move;
}

Move Algorithm::process_balldistance(Sensors sensors){
    Move move;
    //gyro.offset = 0;
    if(sensors.cam.bAngle != None){
        if(abs(sensors.cam.bAngle) <= 90 || sensors.distance.value[BACK] < 300){
            if(abs(sensors.cam.bAngle) < 45){
                dribbler.set(60);
            } else {
                dribbler.set(0);
            }
            int movex;
            int movey;
            if(abs(sensors.cam.bAngle) <= 90)
                movex = ballX.getPID(sensors.cam.bAngle * 1.5, 0, interval) * speed;
            else
                movex = sensors.cam.bAngle < 0 ?  ballX.getPID(sensors.cam.bAngle, -180, interval) * -speed * 0.8 : ballX.getPID(sensors.cam.bAngle, 180, interval) * -speed * 0.8;
            movey = (350 - sensors.distance.value[BACK]) * speed * 0.003;
            if(sensors.distance.value[BACK] < 300 && 90 < abs(sensors.cam.bAngle)){
                movey = (350  - sensors.distance.value[BACK] + sensors.cam.bDistance * sin(abs(sensors.cam.bAngle) - 90) + 30) * speed * 0.006;
            }
            move.angle = atan2(movex, movey);
            move.power = sensors.cam.bDistance == None ? (float)sqrt(movex * movex + movey * movey) / (float)speed : (float)sqrt(movex * movex + movey * movey) / (float)speed * 80 / (sensors.cam.bDistance);
        } else {
            move.angle = sensors.cam.bAngle * 1.4;
            move.power = 1.2;
        }
    } else {
        int movex = (sensors.distance.value[RIGHT] - sensors.distance.value[LEFT]) * speed * 0.1;
        int movey = (350 - sensors.distance.value[BACK]) * speed * 0.005;
        move.angle = atan2(movex, movey);
        move.angle = (move.angle * 0.6) + (lastmove * 0.4);
        lastmove = move.angle;
        move.power = (float)sqrt(movex * movex + movey * movey) / (float)speed;
    }
    if(sensors.distance.value[BACK] <= 450 && abs(sensors.cam.bAngle) < 20){
        if(sensors.cam.bDistance < 35){
            move.angle = 0;
            move.power = 1.4;
        } else {
            if(sensors.cam.bDistance < 60){
                ballcount += 1;
                if(750 < ballcount){
                    //gyro.offset = sensors.cam.gAngleC > 0 ? (180 - sensors.cam.gAngleC) * -0.7 : (180 + sensors.cam.gAngleC) * -0.7;
                    move.angle = 0;
                    move.power = 1.4;
                }
            } else {
                ballcount = 0;
            }
        }
    }
    if(450 < sensors.distance.value[BACK])
        gyro.offset = 0;
    return move;
}

Move Algorithm::process_line(Move move, Sensors sensors){
    getDetected(sensors);
    if(lineAppeared_time == UINT32_MAX)
        lineAppeared_time = sysTime.read_ms();
    else if(detected[FRONT] | detected[RIGHT] | detected[BACK] | detected[LEFT])
        lineAppeared_time = UINT32_MAX;
    
    int movex, movey;
    if (move.angle == DONTMOVE) {
        movex = 0;
        movey = 0;
    }
    else {
        movex = speed * move.power * sin(move.angle);
        movey = speed * move.power * cos(move.angle);
    }
    
    if (detected[FRONT] == 1) {
        int distance = sensors.line.coordinateY;
        if (distance == None)
            distance = -36;
        int yMax = line.pid[FRONT].getPID(distance, 20, interval) * speed;
        movey = constrain(movey, -192, yMax);
    }
    if (detected[RIGHT] == 1) {
        int distance = sensors.line.coordinateX;
        if (distance == None)
            distance = -36;
        int xMax = line.pid[RIGHT].getPID(distance, 15, interval) * speed;
        movex = constrain(movex, -192, xMax);
    }
    if (detected[BACK] == 1) {
        int distance = sensors.line.coordinateY;
        if (distance == None)
            distance = 36;
        int yMin = line.pid[BACK].getPID(distance, -30, interval) * speed;
        movey = constrain(movey, yMin, 192);
    }
    if (detected[LEFT] == 1) {
        int distance = sensors.line.coordinateX;
        if (distance == None)
            distance = 36;
        int xMin = line.pid[LEFT].getPID(distance, -15, interval) * speed;
        movex = constrain(movex, xMin, 192);
    }
    
    if (lineDissappeared_time + 100 < sysTime.read_ms()) {
        movex = 0;
        movey = 0;
    }
    if (lineDissappeared_time + 500 < sysTime.read_ms()) {
        detected[FRONT] = 0;
        detected[RIGHT] = 0;
        detected[BACK] = 0;
        detected[LEFT] = 0;
        lineDissappeared_time = UINT32_MAX - 2000;
    }
    
    move.angle = atan2(movex, movey);
    move.power = (float)sqrt(movex * movex + movey * movey) / (float)speed;
    
    lastlineval[FRONT] = sensors.line.value[FRONT];
    lastlineval[RIGHT] = sensors.line.value[RIGHT];
    lastlineval[LEFT] = sensors.line.value[LEFT];
    lastlineval[BACK] = sensors.line.value[BACK];
    
    //    printf("%d\r\n", move.angle);
    return move;
}

void Algorithm::getDetected(Sensors sensors){
    if (lastlineval[FRONT] >= 0b10000 && sensors.line.value[FRONT] == 0){
        detected[FRONT] = 0;
        line.pid[FRONT].reset();
    }
    if (lastlineval[RIGHT] >= 0b1000000 && sensors.line.value[RIGHT] == 0){
        detected[RIGHT] = 0;
        line.pid[RIGHT].reset();
    }
    if (lastlineval[BACK] >= 0b10000 && sensors.line.value[BACK] == 0){
        detected[BACK] = 0;
        line.pid[BACK].reset();
    }
    if (lastlineval[LEFT] >= 0b1000000 && sensors.line.value[LEFT] == 0){
        detected[LEFT] = 0;
        line.pid[LEFT].reset();
    }
    if ((detected[FRONT] | detected[RIGHT] | detected[BACK] | detected[LEFT]) == 0) {
    }
    
    if ((sensors.line.value[FRONT] | sensors.line.value[RIGHT] | sensors.line.value[BACK] | sensors.line.value[LEFT]) != 0)
        lineDissappeared_time = UINT32_MAX - 2000;
    else if ((lastlineval[FRONT] | lastlineval[RIGHT] | lastlineval[BACK] | lastlineval[LEFT]) != 0 && (detected[FRONT] | detected[RIGHT] | detected[BACK] | detected[LEFT]) != 0)
        lineDissappeared_time = sysTime.read_ms();
    
    if (sensors.line.value[FRONT] != 0 && lastlineval[FRONT] == 0 && detected[BACK] == 0) {
        detected[FRONT] = 1;
    }
    if (sensors.line.value[RIGHT] != 0 && lastlineval[RIGHT] == 0 && detected[LEFT] == 0) {
        detected[RIGHT] = 1;
    }
    if (sensors.line.value[BACK] != 0 && lastlineval[BACK] == 0 && detected[FRONT] == 0) {
        detected[BACK] = 1;
    }
    if (sensors.line.value[LEFT] != 0 && lastlineval[LEFT] == 0 && detected[RIGHT] == 0) {
        detected[LEFT] = 1;
    }
    
    if (sensors.line.value[BACK] == 0 && sensors.line.value[FRONT] == 0 && (sensors.line.value[RIGHT] & 0b00000011) == 0 && (sensors.line.value[LEFT] & 0b00000011) == 0) {
        detected[FRONT] = 0;
        detected[BACK] = 0;
    }
    if (sensors.line.value[RIGHT] == 0 && sensors.line.value[LEFT] == 0 && (sensors.line.value[FRONT] & 0b00000011) == 0 && (sensors.line.value[BACK] & 0b00000011) == 0) {
        detected[RIGHT] = 0;
        detected[LEFT] = 0;
    }
    
    //    printf("%d\t", move.angle);
    //    printf("%d\t%d\t%d\t%d\r\n", detected[0], detected[1], detected[2], detected[3]);
    //    printf("detected0= %d\tdetected1= %d\tdetected2= %d\tdetected3= %d\t", detected[0], detected[1], detected[2], detected[3]);
    //    printf("sens0= %d\tsens1= %d\tsens2= %d\tsens3= %d\t", sensors.line.value[0], sensors.line.value[1], sensors.line.value[2], sensors.line.value[3]);
}

#endif
