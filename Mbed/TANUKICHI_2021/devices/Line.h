/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef Line_h
#define Line_h

#include "mbed.h"
#include "../Shared.h"
#include "../libraries/Math.h"
#include "../libraries/PID.h"

typedef struct{
    bool f = 0;
    bool r = 0;
    bool b = 0;
    bool l = 0;
    void clear(){
        f = 0; r = 0; b = 0; l = 0;
    }
    bool allOR(){
        return (f || r || b || l);
    }
} Line_Detected;

typedef struct {
    int value[4] = {0,0,0,0};
    int coordinateX = 0, coordinateY = 0;
    int subCoordinateX = 0, subCoordinateY = 0;
    int slope = 0, angle = 0, distance = 0;
    //slope
    // | : 0
    // \ : -1 ~ -89
    // / : +1 ~ +90
    Line_Detected detected;
} Line_S;

class Line {
private:
#if defined(POSITION_OF)
    float KP = 0.006;
    float KI = 0.004;
    float KD = 0.0006;
#elif defined(POSITION_DF)
    float KP = 0.01;
    float KI = 0;
    float KD = 0.06;
#endif
    SPI &spi;
    DigitalOut Line1, Line2, Line3, Line4;
    DigitalOut ss[4] = {Line1, Line2, Line3, Line4};
    int l[4][3];
    int Index = 0;
    int countX,countY;
    int coordinateX, coordinateY;
    void set_coordinateX(int dataX);
    void set_coordinateY(int dataY);
    void get_coordinate_OF(Line_S &sens);
    void get_coordinate_DF(Line_S &sens);
    
    int LS_detected[44][2];
    int LS_ave[2];
    int LS_num_detected = 0;
    void get_LS_OF(Line_S &sens);
    void set_data_LS_OF(int dataX, int dataY);
public:
    PID pid[4];
    Line(SPI &_spi, PinName _line1, PinName _line2, PinName _line3, PinName _line4);

    bool lifted;
    int value[4];
    void begin();
    void getValue(Line_S &sensors);
    void calibrationStart();
    void calibrationStop();
    
    Line_Detected lastDtected;
    
    int detectCounter(int data){
        int detectCount = 0;
        for(int i = 0; i < 12; i++){
            if(data & (0b1 << i))
                detectCount++;
        }
        return detectCount;
    }
};

#endif /* Line_h */
