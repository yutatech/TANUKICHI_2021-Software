/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef Distance_h
#define Distance_h

#include "mbed.h"
#include "../shared.h"
#include "../libraries/Math.h"
#include "../libraries/MCP23017.h"
#include "../libraries/VL53L0X.h"

typedef struct {
    int value[5] = {0, 0, 0, 0, 0};
    int angleL = None;
    int angleC = None;
    int angleR = None;
    int distance = None;
    void print(){
        printf("Lidar:angleL=%d, angleC=%d, angleR=%d, distance=%d\t",angleL,angleC,angleR,distance);
    }
    int threshould = 0;
} Lidar_S;

typedef struct {
    int valueF[5] = {8190, 8190, 8190, 8190, 8190};
    int value[4] = {0, 0, 8200, 0};
    Lidar_S lidar;
    void print(){
        printf("Distance:F=%d,R=%d,B=%d,L=%d\t",value[0],value[1],value[2],value[3]);
    }
} Distance_S;

class Distance {
private:
    SPI &spi;
    I2C &i2c;
    DigitalOut ss;
    MCP23017 gpioExp;
    VL53L0X vl[6];
    int vldata[6];
    int lastVldata[6];
    int value[4];
public:
    Distance_S data;
    Distance(SPI &_spi, I2C &_i2c, PinName _ss);
    void init();
    void getValue(Distance_S &sens);
    void getValue(Lidar_S &sens);
};

#endif /* Sonic_h */
