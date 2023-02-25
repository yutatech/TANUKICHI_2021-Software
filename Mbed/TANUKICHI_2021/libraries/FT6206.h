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

#ifndef FT6026_h
#define FT6026_h

#include "mbed.h"

#define FT6206_ADDR 0x70

#define FT6206_REG_MODE 0x00
#define FT6206_REG_CALIBRATE 0x02
#define FT6206_REG_WORKMODE 0x00
#define FT6206_REG_FACTORYMODE 0x40
#define FT6206_REG_THRESHHOLD 0x80
#define FT6206_REG_POINTRATE 0x88
#define FT6206_REG_FIRMVERS 0xA6
#define FT6206_REG_CHIPID 0xA3
#define FT6206_REG_VENDID 0xA8

#define swapxy(a, b) { int16_t t = a; a = 240 - b; b = t; }

class FT6206 {
private:
    I2C &i2c;
    void writeRegister8(uint8_t, uint8_t);
    uint8_t readRegister8(uint8_t);
public:
    uint8_t touches;
    uint16_t x[2], y[2], ID[2], weight[2];
    
    FT6206(I2C &_i2c);
    void readData();
};

#endif /* FT6026_h */
