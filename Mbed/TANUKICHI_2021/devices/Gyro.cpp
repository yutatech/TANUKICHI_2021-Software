/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#include "Gyro.h"

Gyro::Gyro(I2C &_i2c) : bno(_i2c) {}

void Gyro::init() {
    impossible = !bno.init(OPERATION_MODE_IMUPLUS);
    bno.setExtCrystalUse(true);
    liftedCont.start();
}

void Gyro::getValue(Gyro_S &sens) {
    if (impossible){
        value = 0;
        x = 0;
        y = 0;
    }
    else {
        bno.getVector(VECTOR_EULER);
        value = bno.xyz[0];
        value = value > 180 ? (value - 360) : value;
        value -= + resetValue;
        
        value %= 360;
        if (value > 180)
            value -= 360;
        else if (value <= -180)
            value += 360;
        
        x = bno.xyz[1];
        y = bno.xyz[2];
        if(abs(x)+abs(y)<10)
            liftedCont.reset();
        if(liftedCont.read_ms() > 100)
            lifted = true;
        else
            lifted = false;
        
//        if(compassIs){
//            bno.getVector(VECTOR_MAGNETOMETER);
//            printf("%d\t%d\r\n", bno.xyz[0], bno.xyz[1]);
//            value = atan2(bno.xyz[0] - calibX, bno.xyz[1] - calibY);
//        }
    }
    sens.value = value;
}

void Gyro::reset() {
    bno.getVector(VECTOR_EULER);
    resetValue = bno.xyz[0];
    resetValue = resetValue > 180 ? (resetValue - 360) : resetValue;
}

int Gyro::getPID(unsigned long cycle) {
    cycle_us = cycle;
    if(cycle_us == 0)
        cycle_us = 1;
    int motorPlus = 0;
    int p = 0, i = 0, d = 0;
    int gap = value - offset;
    
    diff[0] = diff[1];
    diff[1] = gap > 180 ? gap - 360 : gap < -180 ? gap + 360 : gap;
    integral += (diff[1] + diff[0]) / 2 * ((float)cycle_us / 1000000);
    
    p = KP * diff[1] ;
    i = constrain(KI * integral, -50, 50);
    d = KD * (diff[1] - diff[0]) / ((float)cycle_us / 1000000);
    
    motorPlus = p + i + d;
    return motorPlus;
}

void Gyro::calibration(void){
    int maxX = 0, maxY = 0, minX = 0, minY = 0;
    for(int i = 0; i < 400; i++){
        bno.getVector(VECTOR_MAGNETOMETER);
        maxX = max(maxX, bno.xyz[0]);
        minX = min(minX, bno.xyz[0]);
        maxY = max(maxY, bno.xyz[1]);
        minY = min(minY, bno.xyz[1]);
        wait_ms(5);
    }
    calibX = (maxX + minX) / 2;
    calibY = (maxY + minY) / 2;
}
