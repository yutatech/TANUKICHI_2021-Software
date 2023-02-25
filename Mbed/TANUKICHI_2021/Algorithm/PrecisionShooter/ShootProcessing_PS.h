/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef ShootProcessing_PS_h
#define ShootProcessing_PS_h

#include "../../shared.h"
#include "../../Robot.h"
#include "../../libraries/PID.h"
#include "../../libraries/Math.h"
#include "../../libraries/statistics.h"
#include "../Offence/MachinePosition.h"
#include "../Offence/BallPosition.h"

class ShootProcessing_PS {
private:
    MachinePosition &machinePosition;
    BallPosition &ballPosition;
    Timer ballCatch;
    Timer frontLineDetected;
    
    int rotAngleWhenShot = 0;
    
    int gyOffset = 0;
    float gyTargetOffset = 0;
    int gyOffsetDir = 1;
    
    Statistics statistics;
public:
    ShootProcessing_PS(MachinePosition &_machinePosition, BallPosition &_ballPosition);
    Move moveToGoal(SensorsArray &sensAry, Dribbler &dribblerF);
    Move kick(SensorsArray &sensAry, Dribbler &dribblerF, Kicker &kicker, Gyro &gyro);
    
    int curveShotReady = 0;
};

#endif /* ShootProcessing_PS_h */
