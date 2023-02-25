/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef MachinePosition_h
#define MachinePosition_h

#include "../../shared.h"
#include "../../Robot.h"
#include "../../libraries/PID.h"
#include "../../libraries/Math.h"
#include "../../libraries/ReferencePrevious.h"

class MachinePosition {
//Unit : mm, s

//mouse 20667 -> 300mm
//mouse 68.89 -> 1mm
//mouse 1     -> 0.014516mm
private:
    const int qtyOfPrevious = 30;
    
    class PrvXY{
    public:
        ReferencePrevious<double> x;
        ReferencePrevious<double> y;
        PrvXY(int qty) : x(qty, DBL_MAX), y(qty, DBL_MAX) {}
    };
public:
    const double mouseUnit = 0.013516;
    double positionX = 0;
    double positionY = 0;
    double velocityX = 0;
    double velocityY = 0;
    double velocity = 0;
    double acceleX = 0;
    double acceleY = 0;
    
    PrvXY prvPos;
    PrvXY prvVel;
    
    MachinePosition();
    
    void calculate(SensorsArray &sensAry);
};

#endif /* MachinePosition_h */
