/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef Gyro_h
#define Gyro_h

#include "mbed.h"
#include "../shared.h"
#include "../libraries/Math.h"
#include "../libraries/BNO055.h"

typedef struct {
    int value = 0;
    void print(){
        printf("Gyro:value=%d\t",value);
    }
} Gyro_S;

class Gyro {
private:
    BNO055 bno;
    Timer liftedCont;
    
    const float KP = 2.8;
    const float KI = 1.5;
    const float KD = 0.18;
    
    int diff[2];
    float integral;
    int resetValue = 0;
public:
    Gyro(I2C &_i2c);
    
    bool impossible = false;
    bool error = false;
    int compassIs;
    int calibX, calibY;
    bool lifted;
    unsigned long cycle_us;
    int x,y;
    int value;
    int offset;
    
    void init(void);
    void getValue(Gyro_S&);
    void reset(void);
    int getPID(unsigned long cycle);
    void calibration(void);
};

#endif /* Gyro_h */
