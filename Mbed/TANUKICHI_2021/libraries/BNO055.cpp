/*!
 * @file Adafruit_BNO055.cpp
 *
 *  @mainpage Adafruit BNO055 Orientation Sensor
 *
 *  @section intro_sec Introduction
 *
 *  This is a library for the BNO055 orientation sensor
 *
 *  Designed specifically to work with the Adafruit BNO055 9-DOF Breakout.
 *
 *  Pick one up today in the adafruit shop!
 *  ------> https://www.adafruit.com/product/2472
 *
 *  These sensors use I2C to communicate, 2 pins are required to interface.
 *
 *  Adafruit invests time and resources providing this open source code,
 *  please support Adafruit andopen-source hardware by purchasing products
 *  from Adafruit!
 *
 *  @section author Author
 *
 *  K.Townsend (Adafruit Industries)
 *
 *  @section license License
 *
 *  MIT license, all text above must be included in any redistribution
 */

#include "BNO055.h"

BNO055::BNO055(I2C &__i2c) : _i2c(__i2c) {
//    _i2c.frequency(400000);
}

bool BNO055::init(bno055_opmode_t mode) {
    wait_ms(5);
    uint8_t id = read8(BNO055_CHIP_ID_ADDR);
    if(id != BNO055_ID) {
        wait_ms(1000);
        id = read8(BNO055_CHIP_ID_ADDR);
        if(id != BNO055_ID){
            return false;
        }
    }
    
    setMode(OPERATION_MODE_CONFIG);
    
    //    write(BNO055_SYS_TRIGGER_ADDR, 0x20);
    //    wait_ms(50);
    
    /* Set to normal power mode */
    write(BNO055_PWR_MODE_ADDR, POWER_MODE_NORMAL);
    wait_ms(10);
    
    write(BNO055_PAGE_ID_ADDR, 0);
    
    write(BNO055_SYS_TRIGGER_ADDR, 0x0);
    wait_ms(10);
    /* Set the requested operating mode (see section 3.3) */
    setMode(mode);
    wait_ms(20);
    
    return true;
}

void BNO055::setMode(bno055_opmode_t mode) {
    _mode = mode;
    write(BNO055_OPR_MODE_ADDR, _mode);
    wait_ms(30);
}

void BNO055::setExtCrystalUse(bool usextal) {
    bno055_opmode_t modeback = _mode;
    
    setMode(OPERATION_MODE_CONFIG);
    wait_ms(25);
    write(BNO055_PAGE_ID_ADDR, 0);
    if (usextal) {
        write(BNO055_SYS_TRIGGER_ADDR, 0x80);
    } else {
        write(BNO055_SYS_TRIGGER_ADDR, 0x00);
    }
    wait_ms(10);
    setMode(modeback);
    wait_ms(20);
}

void BNO055::getVector(vector_type_t vector_type) {
    char buffer[6] = {0, 0, 0, 0, 0, 0};
    
    int16_t x, y, z;
    x = y = z = 0;
    
    /* Read vector data (6 bytes) */
    readLen((bno055_reg_t)vector_type, buffer, 6);
    
    x = ((int16_t)buffer[0]) | (((int16_t)buffer[1]) << 8);
    y = ((int16_t)buffer[2]) | (((int16_t)buffer[3]) << 8);
    z = ((int16_t)buffer[4]) | (((int16_t)buffer[5]) << 8);
    
    /* Convert the value to an appropriate range (section 3.6.4) */
    /* and assign the value to the Vector type */
    switch(vector_type)
    {
        case VECTOR_MAGNETOMETER:
            /* 1uT = 16 LSB */
            xyz[0] = ((double)x)/16.0;
            xyz[1] = ((double)y)/16.0;
            xyz[2] = ((double)z)/16.0;
            break;
        case VECTOR_GYROSCOPE:
            /* 1dps = 16 LSB */
            xyz[0] = ((double)x)/16.0;
            xyz[1] = ((double)y)/16.0;
            xyz[2] = ((double)z)/16.0;
            break;
        case VECTOR_EULER:
            /* 1 degree = 16 LSB */
            xyz[0] = ((double)x)/16.0;
            xyz[1] = ((double)y)/16.0;
            xyz[2] = ((double)z)/16.0;
            break;
        case VECTOR_ACCELEROMETER:
        case VECTOR_LINEARACCEL:
        case VECTOR_GRAVITY:
            /* 1m/s^2 = 100 LSB */
            xyz[0] = ((double)x)/100.0;
            xyz[1] = ((double)y)/100.0;
            xyz[2] = ((double)z)/100.0;
            break;
    }
    
    return;
}

bool BNO055::write(bno055_reg_t reg, char data) {
    char buffer[] = {reg, data};
    
    _i2c.write(0x28<<1, buffer, 2);
    
    return true;
}

int BNO055::read8(bno055_reg_t reg) {
    char buffer[] = {reg};
    _i2c.write(0x28<<1, buffer, 1, true);
    char Data;
    _i2c.read((0x28<<1) + 1, &Data, 1, false);
    return Data;
}

bool BNO055::readLen(bno055_reg_t reg, char *buf, int length) {
    char buffer[] = {reg};
    
    _i2c.write(0x28<<1, buffer, 1, true);
    _i2c.read((0x28<<1) + 1, buf, length, false);
    
    return true;
}
