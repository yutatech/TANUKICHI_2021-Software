/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef BallTracking_PS_h
#define BallTracking_PS_h

#include "../../shared.h"
#include "../../Robot.h"
#include "../../libraries/PID.h"
#include "../../libraries/Math.h"
#include "../Offence/MachinePosition.h"
#include "../Offence/BallPosition.h"

class BallTracking_PS {
private:
    MachinePosition &machinePosition;
    BallPosition &ballPosition;
    PID ballX;
public:
    BallTracking_PS(MachinePosition &_machinePosition, BallPosition &_ballPosition);
    Move frontSide(Sensors sensors, int speed);
    Move backSide(Sensors sensors, int speed);
};

#endif /* BallTracking_PS_h */
