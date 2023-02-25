/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef HOLD_App_h
#define HOLD_App_h

#include "ApplicationFramework.h"
#include "../bitmaps/HOLD_icon.h"

class HOLD_App : public ApplicationFramework {
private:
    Hold &hold;
    Hold_S data, lastData;
public:
    int backColor = tft.color(255,255,255);
    HOLD_App(ILI9341 &_tft, Hold &_hold) : ApplicationFramework(_tft, HOLD_icon), hold(_hold) {
        strcpy(AppName, "Hold");
        AppNameLen = 4;
    }
    
    void begin(){
        tft.fillRect(0,0,320,240,backColor);
        
        tft.fillRect(60,20,30,21,tft.color(0,140,0));
        tft.fillCircle(90,31,10,tft.color(0,140,0));
        tft.fillRect(231,20,30,21,tft.color(0,220,50));
        tft.fillCircle(231,31,10,tft.color(0,220,50));
        tft.fillRect(60,200,30,21,tft.color(0,220,50));
        tft.fillCircle(90,211,10,tft.color(0,220,50));
        tft.fillRect(231,200,30,21,tft.color(0,140,0));
        tft.fillCircle(231,211,10,tft.color(0,140,0));
        
        if(data.f)
            tft.fillCircle(160,31,40, tft.color(255,100,0));
        else
            tft.fillArc(231,31,110,80,-95,-85,tft.color(160,255,180));

        if(data.b)
            tft.fillCircle(160,211,40, tft.color(255,100,0));
        else
            tft.fillArc(90,211,110,80,85,95,tft.color(160,255,180));
    }
    
    void loop(TP tp){
        hold.getValue(data);
        if(lastData.f!=data.f){
            if(data.f){
                tft.fillArc(231,31,110,80,-95,-85,backColor);
                tft.fillCircle(160,31,40, tft.color(255,100,0));
            }
            else{
                tft.fillCircle(160,31,40,backColor);
                tft.fillArc(231,31,110,80,-95,-85,tft.color(160,255,180));
            }
        }
        
        if(lastData.b!=data.b){
            if(data.b){
                tft.fillArc(90,211,110,80,85,95,backColor);
                tft.fillCircle(160,211,40, tft.color(255,100,0));
            }
            else{
                tft.fillCircle(160,211,40, backColor);
                tft.fillArc(90,211,110,80,85,95,tft.color(160,255,180));
            }
        }
        lastData = data;
    }
};


#endif /* HOLD_App_h */
