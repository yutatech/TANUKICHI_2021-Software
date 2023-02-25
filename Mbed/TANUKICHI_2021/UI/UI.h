/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef UI_h
#define UI_h

#include "mbed.h"
#include "../Shared.h"
#include "Applications.h"
#include "../Robot.h"
#include "CUI.h"

class UI{
private:
    ILI9341 tft;
    FT6206 touch;
    DigitalOut SSUPPER;
    SPI &spi;
    Devices &devices;
    Settings &settings;
    
    CUI cui;
    
    ApplicationFramework *Apps[12] = {
        new KICKER_App(tft, devices.kicker, devices.eeprom, settings),
        new LINE_App(tft, devices.line),
        new CAM_App(tft, devices.cam, devices.eeprom, settings),
        new COMPASS_App(tft, devices.gyro, devices.eeprom, devices.motor, settings),
        new SETTINGS_App(tft, devices.eeprom, settings),
        new MOTOR_App(tft, devices.eeprom, settings),
        new DISTANCE_App(tft, devices.distance),
        new ISP_App(tft, devices.spi, devices.eeprom, settings),
        new HOLD_App(tft, devices.hold),
        new LIDAR_App(tft, devices.distance),
        new SHOOTER_App(tft, devices, settings),
        new BLANK_App(tft)
    };
    
    enum PageName{
        HOME_PAGE       = 12,
        KICKER_PAGE     = 0,
        LINE_PAGE       = 1,
        CAM_PAGE        = 2,
        COMPASS_PAGE    = 3,
        SETTINGS_PAGE   = 4,
        MOTOR_PAGE      = 5,
        DISTANCE_PAGE   = 6,
        ISP_PAGE        = 7,
        HOLD_PAGE       = 8
    };
    
    uint16_t readPixel(uint16_t x, uint16_t y);
    int loadingState = 0;
    TP tp;
    PageName page = HOME_PAGE;
    bool pushed = false;
    bool pushing = false;
    PageName popup_page = HOME_PAGE;
    void Home();
    void Home_Popup();
    int identify();
    void drawNextPage(PageName nextPage);
    bool started = false;
    
public:
    int batt = -1;
    int battVol = -1;
    int force = 150;
    
    UI(PinName _cs, PinName _dc, PinName _ssupper, Devices &_devices, Settings &_settings);
    void begin();
    void loading(int state);
    void showHome();
    void loop();
    void drawIcons();
    int getBattery();
    void backLightCommand(char comm);
    int getForce();
    void drawBattery(int val);
    void drawPopup(int sx, int sy, int width, int hight, int refX, int refY);
    
    void LED(int num, int color);
    void LEDdefault();
    int a=0;
};

#endif /* UI_h */
