/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef PrecisionShooter_h
#define PrecisionShooter_h

#include "../../shared.h"
#include "../../Robot.h"
#include "../../libraries/PID.h"
#include "../../Math.h"

#include "AreaProcessing_PS.h"
#include "BallTracking_PS.h"
#include "ShootProcessing_PS.h"
#include "../Offence/MachinePosition.h"
#include "../Offence/BallPosition.h"

class PrecisionShooter {
private:
    Cam      &cam;
    Line     &line;
    Distance &distance;
    Gyro     &gyro;
    Mouse    &mouse;
    Encoder  &encoder;
    Hold     &hold;
    Motor    &motor;
    Kicker   &kicker;
    Dribbler &dribblerF;
    Dribbler &dribblerB;
    LED      &led;
    
    BallTracking_PS ballTracking;
    ShootProcessing_PS shootProcessing;
    MachinePosition machinePosition;
    BallPosition ballPosition;
    SensorsArray sensAry;
    
    Timer intervalTimer;
    char &speed;
    long interval = 0;
    
    Move mainProcessing();
    
    enum MainProcess{
        TRACK,
        MOVETOGOAL,
        MOVETOGOALWITHOUTCAM,
        KICK,
        MOVETOCURVESHOTPOSITION,
        CURVESHOT,
        MOVETONEUTRAL,
        DODGEOPPONENT,
        BACKTOHOME
    };
    MainProcess mainProcess = BACKTOHOME;
    
    Timer sinceBallLost;
    int ballLostAngle = None;
    
    Timer sinceBallLiftedStart;
    bool ballLifted = 0;
    
    enum ForB{
        f = 0,
        b = 1
    };
    ForB ForBtrack = f;
    Timer sinceSwitchedTrackDirection;
    int bckDistReady_for_multidefenceMeasure = 0;
public:
    PrecisionShooter(Devices &devices, Settings &settings);
    void move();
};

#endif /* PrecisionShooter_h */
