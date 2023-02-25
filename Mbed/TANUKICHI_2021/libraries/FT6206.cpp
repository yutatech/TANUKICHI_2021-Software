/*!
 * @file Adafruit_FT6206.cpp
 *
 * @mainpage Adafruit FT2606 Library
 *
 * @section intro_sec Introduction
 *
 * This is a library for the Adafruit Capacitive Touch Screens
 *
 * ----> http://www.adafruit.com/products/1947
 *
 * Check out the links above for our tutorials and wiring diagrams
 * This chipset uses I2C to communicate
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * @section author Author
 *
 * Written by Limor Fried/Ladyada for Adafruit Industries.
 *
 * @section license License
 * MIT license, all text above must be included in any redistribution
 */

#include "FT6206.h"

FT6206::FT6206(I2C &_i2c) : i2c(_i2c){
//    i2c.frequency(100000);
}

void FT6206::readData(){
    char i2cdat[16];
    i2c.start();
    i2c.write(FT6206_ADDR);
    i2c.write(0);
    i2c.stop();
    
    i2c.read(FT6206_ADDR, i2cdat, 16);
    
    touches = i2cdat[0x02];
    
    if (touches > 2 || touches == 0) {
        touches = 0;
        x[0] = y[0] = x[1] = y[1] = 0;
        return;
    }
    
    for (uint8_t i=0; i<2; i++) {
        x[i] = i2cdat[0x03 + i*6] & 0x0F;
        x[i] <<= 8;
        x[i] |= i2cdat[0x04 + i*6];
        y[i] = i2cdat[0x05 + i*6] & 0x0F;
        y[i] <<= 8;
        y[i] |= i2cdat[0x06 + i*6];
        ID[i] = i2cdat[0x05 + i*6];
        weight[i] = i2cdat[0x07 + i*6];
        x[i] = 240 - x[i];
        y[i] = 320 - y[i];
        swapxy(y[i], x[i]);
    }
}

void FT6206::writeRegister8(uint8_t reg, uint8_t val) {
    i2c.start();
    i2c.write(FT6206_ADDR);
    i2c.write(reg);
    i2c.write(val);
    i2c.stop();
}

uint8_t FT6206::readRegister8(uint8_t reg) {
    char data[1];
    
    i2c.start();
    i2c.write(FT6206_ADDR);
    i2c.write(reg);
    i2c.stop();
    
    i2c.read(FT6206_ADDR, data, 2);
    
    return x[0];
}
