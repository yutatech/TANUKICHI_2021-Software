/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef MOTOR_App_h
#define MOTOR_App_h

#include "ApplicationFramework.h"
#include "../bitmaps/MOTOR_icon.h"

class MOTOR_App : public ApplicationFramework {
private:
    EEPROM &eeprom;
    Settings &settings;
    char &speed;
    char lastSpeed;
    bool sliding;
    
public:
    int backColor = tft.color(255,255,255);
    MOTOR_App(ILI9341 &_tft, EEPROM &_eeprom, Settings &_settings) : eeprom(_eeprom), settings(_settings), speed(settings.data[settings.addSpeed]), ApplicationFramework(_tft, MOTOR_icon) {
            strcpy(AppName, "Motor");
            AppNameLen = 5;
    }
    
    void begin(){
        tft.fillRect(0,0,320,240,backColor);
        tft.drawCircle(61 + speed, 122, 10, 0);
        if(speed > 11)
            tft.fillRect(61, 120, speed - 10, 3, 0x03E0);
        if(speed < 188)
            tft.fillRect(61 + speed + 11, 120, 200 - speed - 11, 3, tft.color(100,100,100));
        tft.setFont(Arial_bold_14, tft.Center, 3);
        tft.print(speed, 160, 90, tft.color(0,0,0));
        lastSpeed = speed;
        sliding = false;
    }
    
    void loop(TP tp){
        if(tp.now.touches && sliding){
            speed = constrain(tp.now.x - 60, 0, 200);
            if(speed != lastSpeed){
                tft.drawCircle(61 + lastSpeed, 122, 10, tft.color(255,255,255));
                tft.drawCircle(61 + speed, 122, 10, 0);
                if(speed > 11)
                    tft.fillRect(61, 120, speed - 10, 3, 0x03E0);
                if(speed < 188)
                    tft.fillRect(61 + speed + 11, 120, 200 - speed - 11, 3, tft.color(100,100,100));
                tft.fillRect(61 + speed - 9, 120, 19, 3, tft.color(255,255,255));
                tft.setFont(Arial_bold_14, tft.Center, 3);
                tft.print(lastSpeed, 160, 90, tft.color(255,255,255));
                tft.print(speed, 160, 90, tft.color(0,0,0));
                lastSpeed = speed;
            }
        }
        if(tp.now.touches && !tp.last.touches && range(95,tp.now.y,145) && range(50 + speed, tp.now.x, 80 + speed))
            sliding = true;
        
        if(!tp.now.touches && tp.last.touches){
            sliding = false;
            eeprom.Write(settings.addRom, settings.data, settings.dataLen);
        }
    }
};

#endif /* MOTOR_App_h */
