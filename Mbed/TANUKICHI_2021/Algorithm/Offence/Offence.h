/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef Algorithm_h
#define Algorithm_h

#include "../../shared.h"
#include "../../Robot.h"
#include "../../libraries/PID.h"
#include "../../Math.h"

#include "LineProcessing_aggressive.h"
#include "LineProcessing_defensive.h"
#include "BallTracking.h"
#include "ShootProcessing.h"
#include "DodgeOpponent.h"
#include "MoveToFixed.h"
#include "MachinePosition.h"
#include "BallPosition.h"

class Algorithm {
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
    
    BallTracking ballTracking;
    LineProcessing_aggressive lineProcessing_aggressive;
    LineProcessing_defensive lineProcessing_defensive;
    ShootProcessing shootProcessing;
    DodgeOpponent dodgeOpponent;
    MoveToFixed moveToFixed;
    MachinePosition machinePosition;
    BallPosition ballPosition;
    SensorsArray sensAry;
    
    Timer intervalTimer;
    char &speed;
    long interval = 0;
    
    Move mainProcessing();
    
    bool aggressiveDodgeMotion = false;
    bool aggressiveLineMotion = false;
    bool useBackDribbler = false;
    bool detectGoal = false;
    bool detectOpponent = false;
    
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
    
    enum ForB{  // front or back
        f = 0,
        b = 1
    };
    ForB ForBtrack = f;
    Timer sinceSwitchedTrackDirection;
    int bckDistReady_for_multidefenceMeasure = 0;
    bool go = false;
    
    int count=0;
public:
    Algorithm(Devices &devices, Settings &settings);
    void move();
};

#endif /* Algorithm */
