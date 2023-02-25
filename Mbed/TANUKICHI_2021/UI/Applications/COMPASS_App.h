/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef COMPASS_App_h
#define COMPASS_App_h

#include "ApplicationFramework.h"
#include "../bitmaps/COMPASS_icon.h"

class COMPASS_App : public ApplicationFramework {
private:
    Gyro &gyro;
    EEPROM &eeprom;
    Motor &motor;
    Settings &settings;
    Gyro_S data, lastData;
    char &compassIs;
    void drawCheck(int x, int y, int interval, uint16_t color){
        tft.drawLine(x+0,y+5,x+0,y+5,color);
        wait_ms(interval);
        tft.drawLine(x+1,y+6,x+1,y+7,color);
        wait_ms(interval);
        tft.drawLine(x+2,y+7,x+2,y+9,color);
        wait_ms(interval);
        tft.drawLine(x+3,y+8,x+3,y+11,color);
        wait_ms(interval);
        tft.drawLine(x+4,y+9,x+4,y+11,color);
        wait_ms(interval);
        tft.drawLine(x+5,y+9,x+5,y+11,color);
        wait_ms(interval);
        tft.drawLine(x+6,y+8,x+6,y+10,color);
        wait_ms(interval);
        tft.drawLine(x+7,y+8,x+7,y+9,color);
        wait_ms(interval);
        tft.drawLine(x+8,y+7,x+8,y+9,color);
        wait_ms(interval);
        tft.drawLine(x+9,y+6,x+9,y+8,color);
        wait_ms(interval);
        tft.drawLine(x+10,y+6,x+10,y+7,color);
        wait_ms(interval);
        tft.drawLine(x+11,y+5,x+11,y+6,color);
        wait_ms(interval);
        tft.drawLine(x+12,y+4,x+12,y+6,color);
        wait_ms(interval);
        tft.drawLine(x+13,y+4,x+13,y+5,color);
        wait_ms(interval);
        tft.drawLine(x+14,y+3,x+14,y+4,color);
        wait_ms(interval);
        tft.drawLine(x+15,y+3,x+15,y+3,color);
        wait_ms(interval);
        tft.drawLine(x+16,y+2,x+16,y+3,color);
        wait_ms(interval);
        tft.drawLine(x+17,y+1,x+17,y+2,color);
        wait_ms(interval);
        tft.drawLine(x+18,y+1,x+18,y+1,color);
        wait_ms(interval);
        tft.drawLine(x+19,y,x+19,y,color);
    }
    
public:
    int backColor = tft.color(0,0,0);
    COMPASS_App(ILI9341 &_tft, Gyro &_gyro, EEPROM &_eeprom, Motor &_motor, Settings &_settings) : gyro(_gyro), eeprom(_eeprom), motor(_motor), settings( _settings), compassIs(settings.data[settings.addCompIs]), ApplicationFramework(_tft, COMPASS_icon) {
            strcpy(AppName, "Compass");
            AppNameLen = 7;
    }
    
    void drawIcon(int x, int y){
        uint16_t icon[2500];
        iconXY.x = popupRect.x = x;
        iconXY.y = popupRect.y = y;
        for(int i = 0; i < 2500; i++){
            icon[i] = COMPASS_icon[i];
            if(icon[i] == 0){
                icon[i] = BackGround[(x + i / 50) * 240 + 190 - y
                                     + i % 50];
            }
        }
        tft.drawimg(icon, x, y, 50, 50);
        tft.setFont(Arial_14, tft.Center, 1);
        tft.print(AppName, AppNameLen, x+25, y+55, tft.color(255,255,255));
        if(gyro.impossible)
            tft.drawRoundRect(x,y+1,50,50,10,tft.color(255,0,0));
    }
    
    void begin(){
        tft.fillRect(0,0,240,240,backColor);
        tft.fillRect(240,0,80,240,tft.color(60,60,60));
        
        tft.fillRoundRect(245,185,70,50,10,tft.color(255,255,255));
        tft.setFont(Arial_bold_14, tft.Center, 1);
        tft.print("Reset", 5, 280, 205, tft.color(0,0,0));
        
        
        tft.fillRoundRect(245,75,70,50,10,tft.color(255,255,255));
        tft.setFont(Arial_bold_14, tft.Center, 1);
        tft.print("Compass", 7, 280, 85, tft.color(0,0,0));
        if(compassIs){
            tft.print("ON", 2, 280, 105, tft.color(0,0,0));
            tft.fillRoundRect(245,130,70,50,10,tft.color(255,255,255));
            tft.setFont(Arial_bold_14, tft.Center, 1);
            tft.print("Calibration", 11, 280, 150, tft.color(0,0,0));
        }
        else
            tft.print("OFF", 3, 280, 105, tft.color(0,0,0));
        
        tft.setFont(Arial_bold_14, tft.Center, 2);
        gyro.getValue(lastData);
        tft.drawLine(120,120,120 - sin(lastData.value) * 100, 120 - cos(lastData.value) * 100, tft.color(255,0,0));
        tft.print(lastData.value, 280, 40, tft.color(255,255,255));
    }
    
    void drawPopup(){
        popupRect.width = 200;
        popupRect.hight = 150;
        if(popupRect.x < 15)
            popupRect.x = 15;
        else if(popupRect.x + popupRect.width - 1 > 304){
            popupRect.x = 305 - popupRect.width;
            if(popupRect.x < 15)
                popupRect.x = 15;
        }
        if(popupRect.y < 19)
            popupRect.y = 19;
        else if(popupRect.y + popupRect.hight - 1 > 217){
            popupRect.y = 218 - popupRect.hight;
            if(popupRect.y < 19)
                popupRect.y = 19;
        }
        tft.drawPopup(popupRect.x, popupRect.y, popupRect.width, popupRect.hight, iconXY.x + 25, iconXY.y + 25);
        tft.drawLine(popupRect.x + 2, popupRect.y + 50, popupRect.x + 197, popupRect.y + 50 , tft.color(100,100,100));
        tft.drawLine(popupRect.x + 2, popupRect.y + 100, popupRect.x + 197, popupRect.y + 100 , tft.color(100,100,100));

        tft.setFont(Arial_bold_14, tft.Center, 3);
        tft.print("Open", 4, popupRect.x+100, popupRect.y+18, tft.color(0,0,0));
        tft.print("Reset", 5, popupRect.x+100, popupRect.y+68, tft.color(0,0,0));
        tft.print("Cancel", 6, popupRect.x+100, popupRect.y+118, tft.color(0,0,0));
    }
    
    void loop(TP tp){
        gyro.getValue(data);
        if(lastData.value != data.value){
            tft.print(lastData.value, 280, 40, tft.color(60,60,60));
            tft.print(data.value, 280, 40, tft.color(255,255,255));
            tft.drawLine(120,120,120 - sin(lastData.value) * 100, 120 - cos(lastData.value) * 100, tft.color(0,0,0));
            tft.drawLine(120,120,120 - sin(data.value) * 100, 120 - cos(data.value) * 100, tft.color(255,0,0));
        }
        if(!tp.now.touches && tp.last.touches){
            if(range(245, tp.last.x, 315) && range(75, tp.last.y, 125)){   //compass ON/OFF
                compassIs = !compassIs;
                tft.setFont(Arial_bold_14, tft.Center, 1);
                if(compassIs){
                    tft.print("OFF", 3, 280, 105, tft.color(255,255,255));
                    tft.print("ON", 2, 280, 105, tft.color(0,0,0)); tft.fillRoundRect(245,130,70,50,10,tft.color(255,255,255));
                    tft.setFont(Arial_bold_14, tft.Center, 1);
                    tft.print("Calibration", 11, 280, 150, tft.color(0,0,0));
                }
                else{
                    tft.print("ON", 2, 280, 105, tft.color(255,255,255));
                    tft.print("OFF", 3, 280, 105, tft.color(0,0,0));
                    tft.fillRoundRect(245,130,70,50,10,tft.color(60,60,60));
                }
                gyro.compassIs = compassIs;
                eeprom.Write(settings.addRom, settings.data, settings.dataLen);
            }
            else if(range(245, tp.last.x, 315) && range(130, tp.last.y, 180) && compassIs){
                tft.print("Measuring", 9, 195, 148, tft.color(255,255,255));
                motor.EnablePID(false);
                motor.setValues(50,50,50,50);
                gyro.calibration();
                motor.setValues(0,0,0,0);
                wait_ms(100);
                motor.open();
                settings.data[settings.addCompCalibX] = gyro.calibX;
                settings.data[settings.addCompCalibY] = gyro.calibY;
                tft.print("Measuring", 9, 195, 148, tft.color(0,0,0));
                eeprom.Write(settings.addRom, settings.data, settings.dataLen);
            }
            else if(range(245, tp.last.x, 315) && range(185, tp.last.y, 235)){   //reset
                gyro.reset();
                drawCheck(215,202,6,tft.color(255,255,255));
                wait_ms(200);
                drawCheck(215,202,3,tft.color(0,0,0));
            }
        }
        lastData = data;
    }
    
    int loop_popup(TP tp){
        if(!tp.now.touches && tp.last.touches){
            if(range(popupRect.x, tp.last.x, popupRect.x + 200) && range(popupRect.y, tp.last.y, popupRect.y + 50)){    //Open
                return OPEN_App;
            }
            else if(range(popupRect.x, tp.last.x, popupRect.x + 200) && range(popupRect.y + 50, tp.last.y, popupRect.y + 100)){ //Reset
                gyro.reset();
                drawCheck(popupRect.x+170, popupRect.y+68,6,tft.color(0,0,0));
                wait_ms(200);
                return CANCEL_App;
            }
            else{   //Cancel
                return CANCEL_App;
            }
        }
        return OTHER_App;
    }
};

#endif /* COMPASS_App_h */
