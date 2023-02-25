/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef SETTINGS_App_h
#define SETTINGS_App_h

#include "ApplicationFramework.h"
#include "../bitmaps/SETTINGS_icon.h"

class SETTINGS_App : public ApplicationFramework {
private:
    EEPROM &eeprom;
    Settings &settings;
    void drawSwitch(int x, int y, bool state){
        if(state){
            tft.fillRoundRect(x, y, 40, 21, 10, tft.color(100,220,0));
            tft.fillCircle(x + 29, y + 11, 8, tft.color(255,255,255));
        }
        else{
            tft.fillRoundRect(x, y, 40, 21, 10, tft.color(220,220,220));
            tft.fillRoundRect(x + 2, y + 2, 36, 17, 8, tft.color(255,255,255));
            tft.drawCircle(x + 10, y + 11, 10, tft.color(220,220,220));
        }
    }
    
public:
    int backColor = tft.color(255,255,255);
    SETTINGS_App(ILI9341 &_tft, EEPROM &_eeprom, Settings &_settings) : eeprom(_eeprom), settings(_settings), ApplicationFramework(_tft, SETTINGS_icon) {
            strcpy(AppName, "Settings");
            AppNameLen = 8;
    }
    
    void begin(){
        tft.fillRect(0,0,320,240,backColor);
        
        tft.setFont(Arial_bold_14, tft.Left, 2);
        tft.print("Aggressive dodge motion", 23, 20, 20, tft.color(0,0,0));
        tft.print("Aggressive line motion ", 22, 20, 50, tft.color(0,0,0));
        tft.print("Use back dribbler"      , 17, 20, 80, tft.color(0,0,0));
        tft.print("Detect goal"            , 11, 20, 110, tft.color(0,0,0));
        tft.print("Detect opponent"        , 15, 20, 140, tft.color(0,0,0));
        tft.print("Wireless log"           , 12, 20, 170, tft.color(0,0,0));
        tft.print("Illumination"           , 12, 20, 200, tft.color(0,0,0));
        
        tft.drawLine(20, 11, 300, 11, tft.color(220,220,220));
        tft.drawLine(20, 41, 300, 41, tft.color(220,220,220));
        tft.drawLine(20, 71, 300, 71, tft.color(220,220,220));
        tft.drawLine(20, 101, 300, 101, tft.color(220,220,220));
        tft.drawLine(20, 131, 300, 131, tft.color(220,220,220));
        tft.drawLine(20, 161, 300, 161, tft.color(220,220,220));
        tft.drawLine(20, 191, 300, 191, tft.color(220,220,220));
        tft.drawLine(20, 221, 300, 221, tft.color(220,220,220));
        
        drawSwitch(261, 15, getBit(settings.data[settings.addState], SaggressiveDodgeMotion));
        drawSwitch(261, 45, getBit(settings.data[settings.addState], SaggressiveLineMotion));
        drawSwitch(261, 75, getBit(settings.data[settings.addState], SchaseBall_back));
        drawSwitch(261, 105, getBit(settings.data[settings.addState], SdetectGoal));
        drawSwitch(261, 135, getBit(settings.data[settings.addState], SdetectOpponent));
        drawSwitch(261, 165, getBit(settings.data[settings.addState], SwirelessLog));
        drawSwitch(261, 195, getBit(settings.data[settings.addState], SilluminationEnabled));
    }
    
    void loop(TP tp){
        if(!tp.now.touches && tp.last.touches){
            if(range(261, tp.last.x, 301)){
                if(range(15, tp.last.y, 45)){
                    settings.data[settings.addState] ^= SaggressiveDodgeMotion;
                    drawSwitch(261, 15, getBit(settings.data[settings.addState], SaggressiveDodgeMotion));
                    eeprom.Write(settings.addRom, settings.data, settings.dataLen);
                }
                if(range(15, tp.last.y, 75)){
                    settings.data[settings.addState] ^= SaggressiveLineMotion;
                    drawSwitch(261, 45, getBit(settings.data[settings.addState], SaggressiveLineMotion));
                    eeprom.Write(settings.addRom, settings.data, settings.dataLen);
                }
                else if(range(45, tp.last.y, 105)){
                    settings.data[settings.addState] ^= SchaseBall_back;
                    drawSwitch(261, 75, getBit(settings.data[settings.addState], SchaseBall_back));
                    eeprom.Write(settings.addRom, settings.data, settings.dataLen);
                }
                else if(range(75, tp.last.y, 135)){
                    settings.data[settings.addState] ^= SdetectGoal;
                    drawSwitch(261, 105, getBit(settings.data[settings.addState], SdetectGoal));
                    eeprom.Write(settings.addRom, settings.data, settings.dataLen);
                }
                else if(range(105, tp.last.y, 165)){
                    settings.data[settings.addState] ^= SdetectOpponent;
                    drawSwitch(261, 135, getBit(settings.data[settings.addState], SdetectOpponent));
                    eeprom.Write(settings.addRom, settings.data, settings.dataLen);
                }
                else if(range(135, tp.last.y, 195)){
                    settings.data[settings.addState] ^= SwirelessLog;
                    drawSwitch(261, 165, getBit(settings.data[settings.addState], SwirelessLog));
                    eeprom.Write(settings.addRom, settings.data, settings.dataLen);
                }
                else if(range(165, tp.last.y, 225)){
                    settings.data[settings.addState] ^= SilluminationEnabled;
                    drawSwitch(261, 195, getBit(settings.data[settings.addState], SilluminationEnabled));
                    eeprom.Write(settings.addRom, settings.data, settings.dataLen);
                }
            }
        }
    }
};

#endif /* SETTINGS_App_h */
