/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef ISP_App_h
#define ISP_App_h

#include "ApplicationFramework.h"
#include "../bitmaps/ISP_icon.h"

class ISP_App : public ApplicationFramework {
private:
    EEPROM &eeprom;
    Settings &settings;
    ISP isp;
    bool lastIspState;
    
public:
    int backColor = tft.color(0,0,0);
    ISP_App(ILI9341 &_tft, SPI &spi, EEPROM &_eeprom, Settings &_settings) : isp(spi), eeprom(_eeprom), settings(_settings), ApplicationFramework(_tft, ISP_icon) {
            strcpy(AppName, "ISP");
            AppNameLen = 3;
    }
    
    void begin(){
        isp.target = settings.data[settings.addIspTarget];
        tft.fillRect(0,0,320,240,backColor);
        tft.setFont(Arial_14, tft.Left, 3);
        tft.print("Target", 6, 30, 35, tft.color(180,180,180));
        tft.setFont(Arial_bold_14, tft.Left, 3);
        tft.print("Front Line Sensor", 17, 30, 70, tft.color(255,255,255));
        tft.print("Right Line Sensor", 17, 30, 105, tft.color(255,255,255));
        tft.print("Back Line Sensor", 16, 30, 140, tft.color(255,255,255));
        tft.print("Left Line Sensor", 16, 30, 175, tft.color(255,255,255));
        tft.print("Upper Central Unit", 18, 30, 210, tft.color(255,255,255));
        tft.fillCircle(15, 76 + isp.target * 35, 2, tft.color(255,255,255));
        
        tft.print("ISPmode", 7, 230, 210, tft.color(255,255,255));
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
        tft.print("ISPmode", 7, popupRect.x+100, popupRect.y+68, tft.color(0,0,0));
        tft.print("Cancel", 6, popupRect.x+100, popupRect.y+118, tft.color(0,0,0));
    }
    
    void loop(TP tp){
        if(!tp.now.touches && tp.last.touches && (abs(tp.last.x -tp.first.x) < 5 && abs(tp.last.y -tp.first.y) < 5)){
            if(range(0,tp.last.x,200)){
                tft.fillCircle(15, 76 + isp.target * 35, 2, tft.color(0,0,0));
                if(range(60,tp.last.y,95))
                    isp.target = 0;
                else if(range(95,tp.last.y,130))
                    isp.target = 1;
                else if(range(130,tp.last.y,165))
                    isp.target = 2;
                else if(range(165,tp.last.y,190))
                    isp.target = 3;
                else if(range(190,tp.last.y,225))
                    isp.target = 4;
                
                tft.fillCircle(15, 76 + isp.target * 35, 2, tft.color(255,255,255));
                
                settings.data[settings.addIspTarget] = isp.target;
                eeprom.Write(settings.addRom, settings.data, settings.dataLen);
            }
            else if(range(230, tp.last.x, 300) && range(180, tp.last.y, 240)){
                isp.state = true;
                tft.fillRect(0,0,320,20,tft.color(255,0,0));
                tft.setFont(Arial_14, tft.Center, 3);
                tft.print("ISPmode", 7, 160, 5, tft.color(255,255,255));
                tft.setFont(Arial_14, tft.Left, 3);
                tft.print("Target", 6, 30, 35, tft.color(100,100,100));
                tft.setFont(Arial_bold_14, tft.Left, 3);
                tft.print("Front Line Sensor", 17, 30, 70, tft.color(100,100,100));
                tft.print("Right Line Sensor", 17, 30, 105, tft.color(100,100,100));
                tft.print("Back Line Sensor", 16, 30, 140, tft.color(100,100,100));
                tft.print("Left Line Sensor", 16, 30, 175, tft.color(100,100,100));
                tft.print("Upper Central Unit", 18, 30, 210, tft.color(100,100,100));
                tft.fillCircle(15, 76 + isp.target * 35, 2, tft.color(100,100,100));
                tft.print("ISPmode", 7, 230, 210, tft.color(100,100,100));
                READ_REG(SPIX->DR);
                isp.setup_ISP();
                while(isp.state && !Start.read()){
                    isp.loop_ISP();
                }
                while(Start.read());
                pc.baud(115200);
                tft.fillRect(0,0,320,20,tft.color(0,0,0));
                tft.setFont(Arial_14, tft.Left, 3);
                tft.print("Target", 6, 30, 35, tft.color(180,180,180));
                tft.setFont(Arial_bold_14, tft.Left, 3);
                tft.print("Front Line Sensor", 17, 30, 70, tft.color(255,255,255));
                tft.print("Right Line Sensor", 17, 30, 105, tft.color(255,255,255));
                tft.print("Back Line Sensor", 16, 30, 140, tft.color(255,255,255));
                tft.print("Left Line Sensor", 16, 30, 175, tft.color(255,255,255));
                tft.print("Upper Central Unit", 18, 30, 210, tft.color(255,255,255));
                tft.fillCircle(15, 76 + isp.target * 35, 2, tft.color(255,255,255));
                
                tft.print("ISPmode", 7, 230, 210, tft.color(255,255,255));
            }
        }
    }
    
    int loop_popup(TP tp){
        if(!tp.now.touches && tp.last.touches){
            if(range(popupRect.x, tp.last.x, popupRect.x + 200) && range(popupRect.y, tp.last.y, popupRect.y + 50)){    //Open
                return OPEN_App;
            }
            else if(range(popupRect.x, tp.last.x, popupRect.x + 200) && range(popupRect.y + 50, tp.last.y, popupRect.y + 100)){    //ISPmode
                isp.state = true;
                tft.fillRect(0,0,320,20,tft.color(255,0,0));
                tft.setFont(Arial_14, tft.Center, 3);
                tft.print("ISPmode", 7, 160, 5, tft.color(255,255,255));
                READ_REG(SPIX->DR);
                isp.setup_ISP();
                while(isp.state && !Start.read()){
                    isp.loop_ISP();
                }
                while(Start.read());
                pc.baud(115200);
                tft.cutImage(BackGround,0,0,0,0,320,20);
                return CANCEL_App;
            }
            else{   //Cancel
                return CANCEL_App;
            }
        }
        return OTHER_App;
    }
};

#endif /* ISP_App_h */
