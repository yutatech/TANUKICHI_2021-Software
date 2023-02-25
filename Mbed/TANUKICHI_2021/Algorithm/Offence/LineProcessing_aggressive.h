/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef LineProcessing_aggressive_h
#define LineProcessing_aggressive_h

#include "../../shared.h"
#include "../../Robot.h"
#include "../../libraries/PID.h"
#include "../../libraries/Math.h"
#include "../../libraries/Statistics.h"
#include "../../libraries/ReferencePrevious.h"
#include "MachinePosition.h"

class LineProcessing_aggressive {
private:
    MachinePosition &machinePosition;
    
    enum XY{
        x = 0,
        y = 1
    };
    
    XY firstDetected;
    Line_Detected lostLineDetected;
    Timer lineDissappeared_time;
    int lineCoordinate[2] = {0,0};
    
    long mouseX = 0;
    long mouseY = 0;
    
    class PassedXY{
    public:
        ReferencePrevious<double> x;
        ReferencePrevious<double> y;
        PassedXY(int qty) : x(qty, DBL_MAX), y(qty, DBL_MAX) {}
    };
    PassedXY passedXY;
    Statistics statistics;
    
    Line_S noiseCancelCounter;
    void noiseCanceler(SensorsArray &sensAry, Line &line);
    void recoverDetected(SensorsArray &sensAry);
    void getDetected(SensorsArray &sensAry);
    void switchDetected_for_penaltyArea(SensorsArray &sensAry, Line &line);
    void clearDetected(SensorsArray &sensAry, Line &line);
    void switchDetected_for_corner(SensorsArray &sensAry, Line &line);
    void getLineCoordinate(SensorsArray &sensAry);
public:
    LineProcessing_aggressive(MachinePosition &_machinePosition);
    Move main(Move move, SensorsArray &sensAry, Line &line);
};

#endif /* LineProcessing_h */
