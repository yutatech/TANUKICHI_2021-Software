/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef Robot_h
#define Robot_h

#include "mbed.h"
#include "devices/Cam.h"
#include "devices/Line.h"
#include "devices/Gyro.h"
#include "devices/Distance.h"
#include "devices/Mouse.h"
#include "devices/Encoder.h"
#include "devices/Hold.h"
#include "devices/EEPROM.h"
#include "devices/Motor.h"
#include "devices/Dribbler.h"
#include "devices/Kicker.h"
#include "devices/LED.h"
#include "Shared.h"

class Devices{
public:
    SPI      spi;
    I2C      i2c;
    Timer    sysTime;
    Cam      cam;
    Line     line;
    Gyro     gyro;
    Distance distance;
    Mouse    mouse;
    Encoder  encoder;
    Hold     hold;
    EEPROM   eeprom;
    Motor    motor;
    Kicker   kicker;
    Dribbler dribblerF;
    Dribbler dribblerB;
    LED      led;

    Devices() :
    spi(MOSI, MISO, SCK),
    i2c(SDA, SCL),
    cam(TX_MV, RX_MV),
    line(spi, SS_LINE1, SS_LINE2, SS_LINE3, SS_LINE4),
    gyro(i2c),
    distance(spi, i2c, SS_UPPER),
    mouse(spi, SS_MOUSE),
    encoder(i2c),
    hold(HOLD_F, HOLD_B),
    eeprom(i2c),
    motor(MA, MB, MC, MD, MRST),
    kicker(KICKER_R, KICKER_L),
    dribblerF(DRIBBLER_F),
    dribblerB(DRIBBLER_B),
    led(spi)
    {}
};

typedef struct {
    Cam_S       cam;
    Line_S      line;
    Gyro_S      gyro;
    Distance_S  distance;
    Mouse_S     mouse;
    Encoder_S   encoder;
    Hold_S      hold;
    int         intervalus;
} Sensors;

class SensorsArray {
private:
    Sensors sensors[10];
    int index = 9;
public:
    Sensors& update(void){
        index++;
        index %= 10;
        sensors[index] = sensors[(index + 9) % 10];
        return sensors[index];
    }
    
    Sensors& get(int num){
        //current ------- previous
        // 0 ,1 , ------- , 8, 9
        if(num < 0 || 9 < num)
            return sensors[index];
        else
            return sensors[(index - num + 10) % 10];
    }
};

enum state {
    SaggressiveDodgeMotion  = 0b00000001,
    SaggressiveLineMotion    = 0b00000010,
    SdetectOpponent         = 0b00000100,
    SchaseBall_back         = 0b00001000,
    SdetectGoal             = 0b00010000,
    SwirelessLog            = 0b00100000,
    SilluminationEnabled    = 0b01000000
};

#define getBit(data, add) (((data) & (add)) != 0)

typedef struct {
    char data[8];
    int dataLen = 5;
    int addSpeed = 0;
    int addOGoal = 1;
    int addIspTarget = 2;
    int addKickPow = 3;
    int addState = 4;
    int addCompIs = 5;
    int addCompCalibX = 6;
    int addCompCalibY = 7;
    int addRom = 0;
} Settings;

#endif /* Robot_h */
