/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#include "Encoder.h"

Encoder::Encoder(I2C &_i2c) : i2c(_i2c) {}

void Encoder::getValue(Encoder_S &sens) {
    char buffer[4];
    i2c.read((ADR_F << 1) + 1, buffer, 4, false);
    
    data.posf = buffer[0] + (buffer[1]<<8);
    data.rpmf = buffer[2] + (buffer[3]<<8);
    if(data.posf >> 15 == 1)  data.posf -= 65536;
    if(data.rpmf >> 15 == 1)  data.rpmf -= 65536;
    
    i2c.read((ADR_B << 1) + 1, buffer, 4, false);

    data.posb = buffer[0] + (buffer[1]<<8);
    data.rpmb = buffer[2] + (buffer[3]<<8);
    if(data.posb >> 15 == 1)  data.posb -= 65536;
    if(data.rpmb >> 15 == 1)  data.rpmb -= 65536;
    
    if(data.rpmf != 0)
        data.rpmf = prvData.rpmf * 0.9 + data.rpmf * 0.1;
    else
        prvData.rpmf = 0;
    if(data.rpmb != 0)
        data.rpmb = prvData.rpmb * 0.9 + data.rpmb * 0.1;
    else
        prvData.rpmb = 0;
    
    prvData = sens = data;
}

void Encoder::resetf(void){
    char buffer[1] = {1};
    i2c.write(ADR_F << 1, buffer, 1, false);
}

void Encoder::resetb(void){
    char buffer[1] = {1};
    i2c.write(ADR_B << 1, buffer, 1, false);
}
