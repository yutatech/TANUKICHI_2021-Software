/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef BallTracking_h
#define BallTracking_h

#include "../../shared.h"
#include "../../Robot.h"
#include "../../libraries/PID.h"
#include "../../libraries/Math.h"
#include "MachinePosition.h"
#include "BallPosition.h"

class BallTracking {
private:
    MachinePosition &machinePosition;
    BallPosition &ballPosition;
    PID ballX;
public:
    BallTracking(MachinePosition &_machinePosition, BallPosition &_ballPosition);
    Move frontSide(Sensors sensors, int speed);
    Move backSide(Sensors sensors, int speed);
};

#endif /* BallTracking_h */
