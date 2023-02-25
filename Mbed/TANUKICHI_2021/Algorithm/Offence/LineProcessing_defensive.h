/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef LineProcessing_defensive_h
#define LineProcessing_defensive_h

#include "../../shared.h"
#include "../../Robot.h"
#include "../../libraries/PID.h"
#include "../../libraries/Math.h"
#include "../../libraries/Statistics.h"
#include "../../libraries/ReferencePrevious.h"

class LineProcessing_defensive {
private:
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
    
    Line_S noiseCancelCounter;
    void noiseCanceler(SensorsArray &sensAry, Line &line);
    void getDetected(SensorsArray &sensAry, Line &line);
    void getLineCoordinate(SensorsArray &sensAry);
public:
    LineProcessing_defensive();
    Move main(Move move, SensorsArray &sensAry, Line &line);
};

#endif /* LineProcessing_h */
