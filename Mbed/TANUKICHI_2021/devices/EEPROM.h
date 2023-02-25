/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef EEPROM_h
#define EEPROM_h

#include "mbed.h"

#define EEPROMAddress  0xA0

class EEPROM{
private:
    I2C &i2c;
public:
    EEPROM(I2C &_i2c);
    void Read(uint16_t Address, char * Data, int length);
    void Write(uint16_t Address, char * Data, int length);
};

#endif /* EEPROM_h */
