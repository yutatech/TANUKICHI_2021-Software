/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#include "Mouse.h"

Mouse::Mouse(SPI &_spi, PinName _ss) : ss(_ss), spi(_spi) {}

uint8_t Mouse::read_reg(uint8_t reg_addr) {
    ss = 0;
    spi.write(reg_addr & 0x7f);
    wait_us(100);
    uint8_t _data = spi.write(0);
    wait_us(1);
    ss = 1;
    wait_us(19);
    return _data;
}

void Mouse::write_reg(uint8_t reg_addr, uint8_t _data) {
    ss = 0;
    
    spi.write(reg_addr | 0x80);
    spi.write(_data);
    
    wait_us(20);
    ss = 1;
    wait_us(100);
}

void Mouse::init() {
    spi.frequency(100000);
    spi.format(8, 3);
    ss = 1;
    ss = 0;
    ss = 1;
    write_reg(REG_Power_Up_Reset, 0x5a);
    wait_ms(50);
    read_reg(REG_Motion);
    read_reg(REG_Delta_X_L);
    read_reg(REG_Delta_X_H);
    read_reg(REG_Delta_Y_L);
    read_reg(REG_Delta_Y_H);
    upload_firmware();
    wait_ms(10);
    
    char laser_ctrl0 = read_reg(REG_LASER_CTRL0);
    write_reg(REG_LASER_CTRL0, laser_ctrl0 & 0xf0);
}

void Mouse::getValue(Mouse_S &sens, int angle) {
    spi.frequency(100000);
    spi.format(8, 3);
    
    read_reg(REG_Motion);
    long deltaX = read_reg(REG_Delta_X_L);
    deltaX += read_reg(REG_Delta_X_H) << 8;
    long deltaY = read_reg(REG_Delta_Y_L);
    deltaY += read_reg(REG_Delta_Y_H) << 8;
    
    if(deltaX >> 15 == 1)
        deltaX -= 65536;
    
    if(deltaY >> 15 == 1)
        deltaY -= 65536;
    
    deltaY = -deltaY;
    
    sens.angle = data.angle = atan2(deltaX, deltaY);
    sens.delta = data.delta = sqrt((deltaX * deltaX) + (deltaY * deltaY));
    sens.deltaX = data.deltaX = deltaX * cos(-angle) - deltaY * sin(-angle);
    sens.deltaY = data.deltaY = deltaX * sin(-angle) + deltaY * cos(-angle);
    data.positionX += data.deltaX;
    sens.positionX += data.deltaX;
    data.positionY += data.deltaY;
    sens.positionY += data.deltaY;
}

void Mouse::positionReset() {
    data.positionX = 0;
    data.positionY = 0;
}

void Mouse::positionSet(long x, long y) {
    data.positionX = x;
    data.positionY = y;
}

void Mouse::upload_firmware(){
    write_reg(REG_Configuration_IV, 0x02);
    write_reg(REG_SROM_Enable, 0x1d);
    wait_ms(10);
    write_reg(REG_SROM_Enable, 0x18);
    ss = 0;
    spi.write(REG_SROM_Load_Burst | 0x80);
    wait_us(15);
    unsigned char c;
    for(int i = 0; i < firmware_length; i++){
        c = firmware_data[i];
        spi.write(c);
        wait_us(15);
    }
    ss = 1;
}
