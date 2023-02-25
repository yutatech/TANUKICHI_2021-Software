/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef KICKER_App_h
#define KICKER_App_h

#include "../Applications/ApplicationFramework.h"
#include "../bitmaps/KICKER_icon.h"

class KICKER_App : public ApplicationFramework {
private:
    Kicker &kicker;
    EEPROM &eeprom;
    Settings &settings;
    bool setL, setF, setR;
    bool onL, onF, onR;
    bool operating;
    int touchX, touchY, lineX, lineY, tr1X, tr1Y, tr2X, tr2Y, lastX, lastY, lastlineX, lastlineY, lasttr1X, lasttr1Y, lasttr2X, lasttr2Y;
    
    char maxTime;
    char lastmaxTime;
    bool sliding;
    
public:
    int backColor = tft.color(255,255,255);
    KICKER_App(ILI9341 &_tft, Kicker &_kicker, EEPROM &_eeprom, Settings &_settings) : kicker(_kicker), eeprom(_eeprom), settings(_settings), ApplicationFramework(_tft, KICKER_icon) {
            strcpy(AppName, "Kick");
            AppNameLen = 4;
    }
    
    void begin(){
#if defined(POSITION_OF)
        lastmaxTime = maxTime = 150 - settings.data[settings.addKickPow];
        tft.fillRect(0,0,320,240,tft.color(255,255,255));
        tft.drawCircle(160,120,10,tft.color(0,0,0));
        tft.fillCircle(160,120,2,tft.color(0,0,0));
        lastX = 0;
        lastY = 0;
        
        tft.drawCircle(301 , 45 + maxTime, 10, 0);
        if(maxTime > 11)
            tft.fillRect(300, 45, 3, maxTime - 11, 0x03E0);
        if(maxTime < 138)
            tft.fillRect(300, 45 + maxTime + 10, 3, 150 - maxTime - 10, tft.color(100,100,100));
        tft.setFont(Arial_bold_14, tft.Left, 3);
        tft.print(maxTime, 270, 114, tft.color(0,0,0));
        
        tft.drawRoundRect(45,185,50,30,10,tft.color(0,0,0));
        tft.drawRoundRect(135,185,50,30,10,tft.color(0,0,0));
        tft.drawRoundRect(225,185,50,30,10,tft.color(0,0,0));
        tft.setFont(Arial_bold_14, tft.Center, 3);
        tft.print("L", 1, 70, 194, tft.color(0,0,0));
        tft.print("C", 1, 160, 194, tft.color(0,0,0));
        tft.print("R", 1, 250, 194, tft.color(0,0,0));
        
        lastX = 0;
        lastY = 0;
        lastlineX = 0;
        lastlineY = 0;
        lasttr1X = 0;
        lasttr1Y = 0;
        lasttr2X = 0;
        lasttr2Y = 0;
        operating = false;
        sliding = false;
#elif defined(POSITION_DF)
        tft.fillRect(0,0,320,240,tft.color(255,255,255));
        tft.setFont(Arial_bold_14, tft.Center, 3);
        tft.print("No Kicker Connected", 19, 160, 114, tft.color(0,0,0));
#endif
    }
    
    void loop(TP tp){
#if defined(POSITION_OF)
        if(tp.now.touches && !tp.last.touches && range(140,tp.now.x,180) && range(100,tp.now.y,140)){
            operating = true;
            tft.drawRoundRect(45,185,50,30,10,tft.color(255,255,255));
            tft.drawRoundRect(135,185,50,30,10,tft.color(255,255,255));
            tft.drawRoundRect(225,185,50,30,10,tft.color(255,255,255));
            tft.setFont(Arial_bold_14, tft.Center, 3);
            tft.print("L", 1, 70, 194, tft.color(255,255,255));
            tft.print("C", 1, 160, 194, tft.color(255,255,255));
            tft.print("R", 1, 250, 194, tft.color(255,255,255));
        }
        
        if(tp.now.touches && (tp.now.x != tp.last.x || tp.now.y != tp.last.y) && operating){
            if(!tp.last.touches)
                tft.fillCircle(160,120,2,tft.color(255,255,255));
            
            touchX = 160 - tp.now.x;
            touchY = 120 - tp.now.y;
            int hypotenuse = sqrt(touchX * touchX + touchY * touchY);
            int angle = atan2(touchX, touchY);
            if(hypotenuse > 80){
                touchX = touchX * 80 / hypotenuse;
                touchY = touchY * 80 / hypotenuse;
            }

            if(45 < abs(angle) && abs(angle) < 135){
                if(45 < angle && angle < 90)
                    angle = 45;
                else if(90 <= angle && angle < 135)
                    angle = 135;
                else if(-135 < angle && angle < -90)
                    angle = -135;
                else if(-90 <= angle && angle < -45)
                    angle = -45;
                touchX = constrain(hypotenuse, 0, 80) * sin(angle);
                touchY = constrain(hypotenuse, 0, 80) * cos(angle);
            }
            
            lineX = touchX - 10 * sin(angle);
            lineY = touchY - 10 * cos(angle);
            tr1X = touchX - 10 * sin(angle - 20);
            tr1Y = touchY - 10 * cos(angle - 20);
            tr2X = touchX - 10 * sin(angle + 20);
            tr2Y = touchY - 10 * cos(angle + 20);
            

            tft.drawLine(160 - lastlineX, 120 - lastlineY, 160 + lastX, 120 + lastY, tft.color(255,255,255));
            tft.drawCircle(160 - lastX, 120 - lastY,10,tft.color(255,255,255));
            tft.fillTriangle(160 + lastX, 120 + lastY, 160 + lasttr1X, 120 + lasttr1Y, 160 + lasttr2X, 120 + lasttr2Y, tft.color(255,255,255));
            tft.drawLine(160 - lineX, 120 - lineY, 160 + touchX, 120 + touchY, tft.color(0,0,0));
            tft.drawCircle(160 - touchX, 120 - touchY,10,tft.color(0,0,0));
            tft.fillTriangle(160 + touchX, 120 + touchY, 160 + tr1X, 120 + tr1Y, 160 + tr2X, 120 + tr2Y, tft.color(0,0,0));
            lastX = touchX;
            lastY = touchY;
            lastlineX = lineX;
            lastlineY = lineY;
            lasttr1X = tr1X;
            lasttr1Y = tr1Y;
            lasttr2X = tr2X;
            lasttr2Y = tr2Y;
        }
        else if(!tp.now.touches && tp.last.touches && operating){
            tft.drawLine(160 - lastlineX, 120 - lastlineY, 160 + lastX, 120 + lastY, tft.color(255,255,255));
            tft.drawCircle(160 - lastX, 120 - lastY,10,tft.color(255,255,255));
            tft.fillTriangle(160 + lastX, 120 + lastY, 160 + lasttr1X, 120 + lasttr1Y, 160 + lasttr2X, 120 + lasttr2Y, tft.color(255,255,255));
            tft.drawCircle(160,120,10,tft.color(0,0,0));
            tft.fillCircle(160,120,2,tft.color(0,0,0));
            
            int angle = atan2(lastX, lastY);
            lastX = 0;
            lastY = 0;
            lastlineX = 0;
            lastlineY = 0;
            lasttr1X = 0;
            lasttr1Y = 0;
            lasttr2X = 0;
            lasttr2Y = 0;
            operating = false;
            kicker.kick(angle, 1);
            
            tft.drawRoundRect(45,185,50,30,10,tft.color(0,0,0));
            tft.drawRoundRect(135,185,50,30,10,tft.color(0,0,0));
            tft.drawRoundRect(225,185,50,30,10,tft.color(0,0,0));
            tft.setFont(Arial_bold_14, tft.Center, 3);
            tft.print("L", 1, 70, 194, tft.color(0,0,0));
            tft.print("C", 1, 160, 194, tft.color(0,0,0));
            tft.print("R", 1, 250, 194, tft.color(0,0,0));
        }
        else if(!tp.now.touches && tp.last.touches && !operating){
            if(range(180, tp.last.y, 220)){
                if(range(45, tp.last.x, 95))
                    kicker.kick(-45, 1);
                else if(range(135, tp.last.x, 185))
                    kicker.kick(0, 1);
                else if(range(225, tp.last.x, 275))
                    kicker.kick(45, 1);
            }
        }
        
        
        if(tp.now.touches && sliding){
            maxTime = constrain(tp.now.y - 45, 0, 150);
            if(maxTime != lastmaxTime){
                tft.drawCircle(301, 45 + lastmaxTime, 10, tft.color(255,255,255));
                tft.drawCircle(301, 45 + maxTime, 10, 0);
                if(maxTime > 11)
                    tft.fillRect(300, 45, 3, maxTime - 11, 0x03E0);
                if(maxTime < 138)
                    tft.fillRect(300, 45 + maxTime + 10, 3, 150 - maxTime - 10, tft.color(100,100,100));
                tft.fillRect(300, 45 + maxTime - 10, 3, 19, tft.color(255,255,255));
                tft.setFont(Arial_bold_14, tft.Left, 3);
                tft.print(lastmaxTime, 270, 114, tft.color(255,255,255));
                tft.print(maxTime, 270, 114, tft.color(0,0,0));
                lastmaxTime = maxTime;
            }
        }
        
        if(tp.now.touches && !tp.last.touches && range(275,tp.now.x,320) && range(30 + maxTime, tp.now.y, 60 + maxTime))
            sliding = true;
        
        if(!tp.now.touches && tp.last.touches && sliding == true){
            sliding = false;
            settings.data[settings.addKickPow] = 150 - maxTime;
            kicker.maxTime = (float)(150 - settings.data[settings.addKickPow]) / 10000;
            eeprom.Write(settings.addRom, settings.data, settings.dataLen);
        }
#endif
    }
};

#endif /* KICKER_App_h */
