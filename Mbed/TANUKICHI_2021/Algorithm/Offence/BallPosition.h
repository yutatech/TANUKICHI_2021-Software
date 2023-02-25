/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef BallPosition_h
#define BallPosition_h

#include "../../shared.h"
#include "../../Robot.h"
#include "../../libraries/PID.h"
#include "../../libraries/Math.h"
#include "../../libraries/Statistics.h"
#include "../../libraries/ReferencePrevious.h"
#include "MachinePosition.h"

class BallPosition {
//Unit : mm, s
    
//cam 0.2 -> 1mm
//cam 1   -> 5mm
private:
    int existsCnt = 0;
    double rawPosX = 0;
    double rawPosY = 0;
    double rawVelX = 0;
    double rawVelY = 0;
    int camInterval = 0;
    
    const int qtyOfPrevious = 30;
    
    class PrvXY{
    public:
        ReferencePrevious<double> x;
        ReferencePrevious<double> y;
        PrvXY(int qty) : x(qty, DBL_MAX), y(qty, DBL_MAX) {}
    };
    
    PrvXY prvRawPos;
    PrvXY prvRawVel;
    ReferencePrevious<long> prvTime;
    ReferencePrevious<long> prvCamTime;
    
    Statistics statistics;
    
    const double camUnit = 0.8421;
    const double camOrigin = 16.209;
    double convert(int dist){
        return dist * camUnit + camOrigin;
    }
    
public:
    double positionX = 0;
    double positionY = 0;
    double velocityX = 0;
    double velocityY = 0;
    double acceleX = 0;
    double acceleY = 0;
    bool exists = false;
    
    double prtPosX=0, prtPosY=0, prtVelX=0, prtVelY=0;
    double roughVelX = 0, roughVelY = 0;
    
    PrvXY prvPos;
    PrvXY prvVel;
    
    BallPosition();
    
    void calculate(SensorsArray &sensAry, MachinePosition &machinePosition);
};

/*
real(mm) ->  cam
    150  ->  30
    200  ->  37
    250  ->  46
    300  ->  56
    350  ->  67
    400  ->  76
    450  ->  87
    500  ->  96
    550  ->  107
    600  ->  120
 
 */

#endif /* BallPosition_h */
