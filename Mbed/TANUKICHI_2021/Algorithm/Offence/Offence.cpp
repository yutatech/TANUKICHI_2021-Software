/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#include "Offence.h"
#if defined(POSITION_OF)

Algorithm::Algorithm(Devices &devices, Settings &settings) :
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
    aggressiveDodgeMotion(getBit(settings.data[settings.addState], SaggressiveDodgeMotion)),
    aggressiveLineMotion(getBit(settings.data[settings.addState], SaggressiveLineMotion)),
    useBackDribbler(getBit(settings.data[settings.addState], SchaseBall_back)),
    detectGoal(getBit(settings.data[settings.addState], SdetectGoal)),
    detectOpponent(getBit(settings.data[settings.addState], SdetectOpponent)),
    ballTracking(machinePosition, ballPosition),
    lineProcessing_aggressive(machinePosition),
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

void Algorithm::move() {
    Move move;
    Sensors &sens = sensAry.update();
    
    sens.intervalus = constrain(intervalTimer.read_us(), 6000, 50000);
    intervalTimer.reset();
//    wireless.printf("a");
    
    cam.getValue(sens.cam);
//    wireless.printf("b");
    mouse.getValue(sens.mouse, sens.gyro.value);
//    wireless.printf("c");
    gyro.getValue(sens.gyro);
//    wireless.printf("d");
    line.getValue(sens.line);
//    wireless.printf("e");
    encoder.getValue(sens.encoder);
//    wireless.printf("f");
    distance.getValue(sens.distance);
//    wireless.printf("g");
    hold.getValue(sens.hold);
//    wireless.printf("h");
    
    line.lifted = false;
    
    motor.EnablePID(true);
    motor.setPID(gyro.getPID(sens.intervalus));
//    wireless.printf("i");
    
    if(cam.color == cam.Blue && sens.distance.value[FRONT] > 600){
        sens.cam.gAngleL = None;
        sens.cam.gAngleC = None;
        sens.cam.gAngleR = None;
        sens.cam.gDistance = None;
    }
    
//    wireless.printf("j");
    machinePosition.calculate(sensAry);
//    wireless.printf("k");
    ballPosition.calculate(sensAry, machinePosition);
//    wireless.printf("l");
    
    if(sens.cam.bAngle>-15)
        go = true;
    if(go){
        move = mainProcessing();
    }
    else{
        move.set(DONTMOVE,0);
    }
    
    //    wireless.printf("m");
    if(aggressiveLineMotion)
        move = lineProcessing_aggressive.main(move, sensAry, line);
    else
        move = lineProcessing_defensive.main(move, sensAry, line);
//    wireless.printf("n\n");

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
    
//    printf("%lf\n", sens.cam.bDistance);
//    printf("%d, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf\n", count++, machinePosition.positionX, machinePosition.positionY, ballPosition.prtPosX, ballPosition.prtPosY, machinePosition.velocityX, machinePosition.velocityY, ballPosition.prtVelX, ballPosition.prtVelY);
    
    motor.setMove(move,speed);
}

Move Algorithm::mainProcessing(){
    Sensors &sens = sensAry.get(0);
    Move move;
    
    if(sens.cam.bAngle != None){
        ballLifted = 0;
        if(sens.cam.bDistance < 270)
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
            
            dribblerF.set(abs(sens.cam.bAngle) < 45 && sens.cam.bDistance < 260 ? 40 : 0);
            
            if(useBackDribbler){
                dribblerB.set(abs(sens.cam.bAngle) > 135 && sens.cam.bDistance < 260 ? 30 : 0);
                
                bool bckDistReady = false;
                if((sens.distance.value[BACK] < 700 && abs(sens.cam.bAngle) > 80) ||
                   (sens.distance.value[BACK] < 700 && abs(sens.cam.bAngle) > 120) ){
                    if(bckDistReady_for_multidefenceMeasure++ > 5)
                        bckDistReady = true;
                }
                else
                    bckDistReady_for_multidefenceMeasure = 0;
                
                if(sinceSwitchedTrackDirection.read_ms() > 1000){
                    //if(!(sens.cam.bDistance * abs(sin(sens.cam.bAngle)) < 50 && abs(sens.cam.bAngle) > 150) && !bckDistReady){
                    if(sens.cam.bDistance * abs(sin(sens.cam.bAngle)) < 260 && abs(sens.cam.bAngle) > 150){
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
            }
            else{
                dribblerB.set(0);
                move = ballTracking.frontSide(sens, speed);
            }
            
            if(useBackDribbler && sens.hold.b && abs(sens.cam.bAngle) > 174 && sens.cam.bDistance < 280)
                mainProcess = MOVETOCURVESHOTPOSITION;

            if(sens.hold.f && abs(sens.cam.bAngle) < 15 && sens.cam.bDistance < 280)
                mainProcess = detectGoal ? MOVETOGOAL : MOVETOGOALWITHOUTCAM;
            
            
            if(mainProcess != TRACK)
                goto reSwitch;
            break;
        case MOVETOGOAL:
            gyro.offset = 0;
            dribblerB.set(0);
            
            move = shootProcessing.moveToGoal(sensAry, dribblerF);
            
            if(detectOpponent && sens.distance.lidar.distance != None && sens.distance.lidar.distance < 300){
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
            else if(detectOpponent && abs(sens.cam.gAngleC) < 40 && sens.cam.gDistance < 700 &&
                    !(range(sens.distance.lidar.angleL, 0, sens.distance.lidar.angleR) && sens.distance.lidar.distance < 300) &&
                    !(range(0, sens.distance.lidar.angleC, 180) && range(0, sens.cam.gAngleC, 180)) &&
                    !(range(-180, sens.distance.lidar.angleC, 0) && range(-180, sens.cam.gAngleC, 0)) ){
                if(sens.distance.lidar.angleC != None && sens.distance.lidar.distance < 650){
                    if(sens.distance.lidar.angleC > 0)
                        kicker.set(sens.cam.gAngleC, 1);
                    else
                        kicker.set(sens.cam.gAngleC, 1);
                }
                else{
                    kicker.set(sens.cam.gAngleC, 1);
                }
                mainProcess = KICK;
            }
            else if(!detectOpponent && sens.cam.gDistance < 600 && range(-40, sens.cam.gAngleC, 40)){
                kicker.set(sens.cam.gAngleC, 1);
                mainProcess = KICK;
            }
            
            if(mainProcess != MOVETOGOAL)
                goto reSwitch;
            break;
        case MOVETOGOALWITHOUTCAM:
            gyro.offset = 0;
            dribblerB.set(0);

            if(detectOpponent && sens.distance.lidar.distance != None && sens.distance.lidar.distance < 300){
                if(sens.distance.lidar.angleR - sens.distance.lidar.angleL >= 40)
                    mainProcess = DODGEOPPONENT;
            }

            if (abs(sens.gyro.value) < 15) {
                int bx = -(sens.distance.value[LEFT] - sens.distance.value[RIGHT]) / 2 + 10;
                int by = sens.distance.value[0] / 10;
                if(sens.distance.value[0] >= 8189){
                    if(abs(bx) > 35)
                        by = 0;
                    else
                        by = 100;
                }
                move.power = 1.2;
                move.angle = atan2(bx, by);
                kicker.set(move.angle, 1);
                mainProcess = KICK;
            }
            else{
                move.angle = 0;
                move.power = 1;
            }

            if(mainProcess != MOVETOGOALWITHOUTCAM)
                goto reSwitch;
            break;
        case KICK:
            dribblerB.set(0);
            if(sens.cam.gAngleC != None)
                kicker.set(sens.cam.gAngleC, 1);
            
            if(sens.distance.lidar.distance != None && sens.distance.lidar.distance < 200){
                if(sens.distance.lidar.angleR - sens.distance.lidar.angleL >= 40)
                    mainProcess = DODGEOPPONENT;
            }
            
            move = shootProcessing.kick(sensAry, dribblerF, kicker, gyro);
            
            if(mainProcess != KICK)
                goto reSwitch;
            break;
        case MOVETOCURVESHOTPOSITION:
            dribblerF.set(0);
            move = shootProcessing.moveToCurveShotPosition(sensAry, dribblerB, gyro, hold, detectGoal, detectOpponent);
            if(shootProcessing.curveShotReady >= 10)
                mainProcess = CURVESHOT;
            
            if(mainProcess != MOVETOCURVESHOTPOSITION)
                goto reSwitch;
            break;
        case CURVESHOT:
            dribblerF.set(0);
            move = shootProcessing.curveShot(sensAry, dribblerB, kicker, gyro);
            
            if(mainProcess != CURVESHOT)
                goto reSwitch;
            break;
        case MOVETONEUTRAL:
            gyro.offset = 0;
            dribblerF.set(0);
            dribblerB.set(0);
            move = moveToFixed.neutral(sensAry);
            if(sens.cam.bAngle != None)
                mainProcess = TRACK;
            
            
            if(mainProcess != MOVETONEUTRAL)
                goto reSwitch;
            break;
        case DODGEOPPONENT:
            move = dodgeOpponent.motion1(sensAry, dribblerF);
            //            move = dodgeOpponent.motion2(sensAry);
            
            if(sens.distance.lidar.angleL > 30 || sens.distance.lidar.angleR < -30){
                kicker.set(0,1);
                mainProcess = KICK;
            }
            
            
            if(mainProcess != DODGEOPPONENT)
                goto reSwitch;
            break;
        case BACKTOHOME:
            if(sensAry.get(1).cam.bAngle != None){
                sinceBallLost.reset();
                ballLostAngle = sensAry.get(1).cam.bAngle;
            }
            if(sinceBallLost.read_ms() > 300){
                gyro.offset = 0;
                dribblerF.set(0);
                dribblerB.set(0);
            }
            
            if(sinceBallLost.read_ms() < 1000)
                move.set(ballLostAngle != None ? ballLostAngle : DONTMOVE, 0.5);
            else
                move = moveToFixed.home(sensAry);
            
            if(sens.cam.bAngle != None)
                mainProcess = TRACK;
            
            
            if(mainProcess != BACKTOHOME)
                goto reSwitch;
            break;
    }
    return move;
}

#endif
