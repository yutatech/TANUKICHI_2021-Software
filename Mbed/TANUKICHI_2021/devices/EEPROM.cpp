/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#include "EEPROM.h"

EEPROM::EEPROM(I2C &_i2c) : i2c(_i2c) {
//    i2c.frequency(100000);
}

void EEPROM::Read (uint16_t Address, char *Data, int Length) {
//    i2c.frequency(100000);
    char Buffer[2] = {(uint8_t)((Address >> 8) & 0xFF), (uint8_t)(Address & 0xFF)};
    
    while(i2c.write(EEPROMAddress, Buffer, 2, true));
    i2c.read(EEPROMAddress, Data, Length, false);
}


void EEPROM::Write(uint16_t Address, char *Data, int Length) {
//    i2c.frequency(100000);
    char Buffer[2 + Length] = {(uint8_t)((Address >> 8) & 0xFF), (uint8_t)(Address & 0xFF)};
    
    for (int i = 0; i < Length; i++)
        Buffer[2 + i] = Data[i];
    
    while(i2c.write(EEPROMAddress, Buffer, Length + 2, false));
}
