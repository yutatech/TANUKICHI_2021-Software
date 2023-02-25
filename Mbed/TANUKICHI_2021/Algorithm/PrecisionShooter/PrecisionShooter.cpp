/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#include "PrecisionShooter.h"

PrecisionShooter::PrecisionShooter(Devices &devices, Settings &settings) :
    cam(devices.cam),
    line(devices.line),
    distance(devices.distance),
    gyro(devices.gyro),
    mouse(devices.mouse),
    encoder(devices.encoder),
    hold(devices.hold),
    motor(devices.motor),
    kicker(devices.kicker),
    dribblerF(devices.dribblerF),
    dribblerB(devices.dribblerB),
    led(devices.led),
    speed(settings.data[settings.addSpeed]),
    ballTracking(machinePosition, ballPosition),
    shootProcessing(machinePosition, ballPosition)
{
    intervalTimer.start();
    sinceBallLost.start();
    sinceBallLiftedStart.start();
    sinceSwitchedTrackDirection.start();
    mouse.positionReset();
    encoder.resetf();
    encoder.resetb();
}

void PrecisionShooter::move() {
    Move move;
    Sensors &sens = sensAry.update();
    
    sens.intervalus = constrain(intervalTimer.read_us(), 6000, 50000);
    intervalTimer.reset();
    wireless.printf("a");
    
    cam.getValue(sens.cam);
    wireless.printf("b");
    mouse.getValue(sens.mouse, sens.gyro.value);
    wireless.printf("c");
    gyro.getValue(sens.gyro);
    wireless.printf("d");
    line.getValue(sens.line);
    wireless.printf("e");
    encoder.getValue(sens.encoder);
    wireless.printf("f");
    distance.getValue(sens.distance);
    wireless.printf("g");
    hold.getValue(sens.hold);
    wireless.printf("h");
    
    line.lifted = false;
    
    motor.EnablePID(true);
    motor.setPID(gyro.getPID(sens.intervalus));
    wireless.printf("i");
    
    if(cam.color == cam.Blue && sens.distance.value[FRONT] > 600){
        sens.cam.gAngleL = None;
        sens.cam.gAngleC = None;
        sens.cam.gAngleR = None;
        sens.cam.gDistance = None;
    }
    
    wireless.printf("j");
    machinePosition.calculate(sensAry);
    wireless.printf("k");
    ballPosition.calculate(sensAry, machinePosition);
    wireless.printf("l");
    
    move = mainProcessing();
    wireless.printf("m");
//    move = 

    int bytes[8];
    bytes[0] = machinePosition.positionX + 0.5;
    bytes[1] = machinePosition.positionY + 0.5;
    bytes[2] = ballPosition.prtPosX + 0.5;
    bytes[3] = ballPosition.prtPosY + 0.5;
    bytes[4] = machinePosition.velocityX + 0.5;
    bytes[5] = machinePosition.velocityY + 0.5;
    bytes[6] = ballPosition.prtVelX + 0.5;
    bytes[7] = ballPosition.prtVelY + 0.5;

    Log.setData(bytes);
    
    motor.setMove(move,speed);
    //    wireless.printf("o");
    //    printf("%d\t%d\n", sens.distance.lidar.angleC, sens.distance.lidar.distance);
    //    printf("%d\t%d\n", sens.hold.f, sens.hold.b);
    //    pc.putc(sens.distance.valueF[0]/5);
    //    pc.putc(sens.distance.valueF[1]/5);
    //    pc.putc(sens.distance.valueF[2]/5);
    //    pc.putc(sens.distance.valueF[3]/5);
    //    pc.putc(sens.distance.valueF[4]/5);
    //    pc.putc(254);
    //    printf("%ld\t%ld\t%d\n", sens.mouse.delta,sens.mouse.deltaY,sens.mouse.angle);
    //    printf("%d\n", int(cos(sens.cam.gAngleC)*sens.cam.gDistance));
    //    sens.distance.lidar.print();
    //    printf("%d\t%d\t%d\n",sens.gyro.value , sens.encoder.posf, sens.encoder.posb);
    //    printf("%d\n", sens.intervalus);
    //    wireless.printf("\n");
}

Move PrecisionShooter::mainProcessing(){
    Sensors &sens = sensAry.get(0);
    Move move;
    
    if(sens.cam.bAngle != None){
        ballLifted = 0;
        if(sens.cam.bDistance < 60)
            sinceBallLiftedStart.reset();
    }
    else if(sensAry.get(1).cam.bAngle != None && sinceBallLiftedStart.read_ms() < 500)
        ballLifted = 1;
    
    if(sens.cam.bAngle == None){
        if(sensAry.get(1).cam.bAngle != None){
            if(sens.line.detected.allOR())
                mainProcess = MOVETONEUTRAL;
            else if(ballLifted)
                mainProcess = MOVETONEUTRAL;
            else
                mainProcess = BACKTOHOME;
        }
    }
    else if(!sens.hold.f && !sens.hold.b)
        mainProcess = TRACK;
    
reSwitch:
    switch(mainProcess){
        case TRACK:
            gyro.offset = 0;
            encoder.resetf();
            encoder.resetb();
            
            dribblerF.set(abs(sens.cam.bAngle) < 45 && sens.cam.bDistance < 50 ? 40 : 0);
            dribblerB.set(abs(sens.cam.bAngle) > 135 && sens.cam.bDistance < 50 ? 30 : 0);
            {
                bool bckDistReady = false;
                if((sens.distance.value[BACK] < 700 && abs(sens.cam.bAngle) > 80) ||
                   (sens.distance.value[BACK] < 700 && abs(sens.cam.bAngle) > 120) ){
                    if(bckDistReady_for_multidefenceMeasure++ > 5)
                        bckDistReady = true;
                }
                else
                    bckDistReady_for_multidefenceMeasure = 0;
            }
            
            if(sinceSwitchedTrackDirection.read_ms() > 1000){
                //if(!(sens.cam.bDistance * abs(sin(sens.cam.bAngle)) < 50 && abs(sens.cam.bAngle) > 150) && !bckDistReady){
                if(sens.cam.bDistance * abs(sin(sens.cam.bAngle)) < 50 && abs(sens.cam.bAngle) > 150){
                    if(ForBtrack == f)
                        sinceSwitchedTrackDirection.reset();
                    ForBtrack = b;
                }
                else{
                    if(ForBtrack == b)
                        sinceSwitchedTrackDirection.reset();
                    ForBtrack = f;
                }
            }
            if(ForBtrack == f)
                move = ballTracking.frontSide(sens, speed);
            else
                move = ballTracking.backSide(sens, speed);
            
            if(sens.hold.b && abs(sens.cam.bAngle) > 174 && sens.cam.bDistance < 28)
                mainProcess = MOVETOCURVESHOTPOSITION;

            if(sens.hold.f && abs(sens.cam.bAngle) < 15 && sens.cam.bDistance < 28)
                mainProcess = MOVETOGOAL;
            
            
            if(mainProcess != TRACK)
                goto reSwitch;
            break;
        case MOVETOGOAL:
            gyro.offset = 0;
            dribblerB.set(0);
            
            move = shootProcessing.moveToGoal(sensAry, dribblerF);
            
            if(sens.distance.lidar.distance != None && sens.distance.lidar.distance < 300){
                if(sens.distance.lidar.angleR - sens.distance.lidar.angleL >= 40)
                    mainProcess = DODGEOPPONENT;
            }
            
            if(sens.cam.gAngleC == None && sens.line.detected.f){
                kicker.set(sens.distance.value[LEFT] > sens.distance.value[RIGHT] ? -45 : 45, 1);
                mainProcess = KICK;
            }
            else if(sens.cam.gAngleC != None && (sens.line.detected.f || sens.line.detected.r || sens.line.detected.l) && (sens.cam.gAngleL < 5 && sens.cam.gAngleR > -5)){
                kicker.set(sens.cam.gAngleC, 1);
                mainProcess = KICK;
            }
            else if(abs(sens.cam.gAngleC) < 40 && sens.cam.gDistance < 150 &&
                    !(range(sens.distance.lidar.angleL, 0, sens.distance.lidar.angleR) && sens.distance.lidar.distance < 300) &&
                    !(range(0, sens.distance.lidar.angleC, 180) && range(0, sens.cam.gAngleC, 180)) &&
                    !(range(-180, sens.distance.lidar.angleC, 0) && range(-180, sens.cam.gAngleC, 0)) ){
                if(sens.distance.lidar.angleC != None && sens.distance.lidar.distance < 650){
                    if(sens.distance.lidar.angleC > 0)
                        kicker.set((sens.cam.gAngleL*0.7+sens.cam.gAngleC*0.3), 1);
                    else
                        kicker.set((sens.cam.gAngleR*0.7+sens.cam.gAngleC*0.3), 1);
                }
                else{
                    kicker.set(sens.cam.gAngleC, 1);
                }
                mainProcess = KICK;
            }
            
            if(mainProcess != MOVETOGOAL)
                goto reSwitch;
            break;
        case KICK:
            dribblerB.set(0);
            
            if(sens.distance.lidar.distance != None && sens.distance.lidar.distance < 200){
                if(sens.distance.lidar.angleR - sens.distance.lidar.angleL >= 40)
                    mainProcess = DODGEOPPONENT;
            }
            
            move = shootProcessing.kick(sensAry, dribblerF, kicker, gyro);
            
            if(mainProcess != KICK)
                goto reSwitch;
            break;
    }
    return move;
}
