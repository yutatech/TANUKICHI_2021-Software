/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef Encoder_h
#define Encoder_h

#include "mbed.h"

#define ADR_F   0x08
#define ADR_B   0x07

typedef struct {
    int posf = 0;
    int posb = 0;
    int rpmf = 0;
    int rpmb = 0;
    void print(){
        printf("Encoder:posf=%d,rpmf=%d,posb=%d,rpmb=%d\t", posf, rpmf, posb, rpmb);
    }
} Encoder_S;

class Encoder {
private:
    I2C &i2c;
public:
    Encoder_S data;
    Encoder_S prvData;
    
    Encoder(I2C &_i2c);
    void getValue(Encoder_S&);
    void resetf(void);
    void resetb(void);
};

#endif /* Encoder_h */
